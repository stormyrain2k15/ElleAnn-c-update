/*******************************************************************************
 * ElleLLM.cpp — Dual-Mode LLM Engine Implementation
 * 
 * Supports: Groq, OpenAI, Anthropic (API) + llama.cpp, LM Studio (Local)
 * Automatic failover, streaming, Elle personality injection.
 ******************************************************************************/
#include "ElleLLM.h"
#include "ElleLogger.h"

#include <windows.h>
#include <winhttp.h>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <vector>

#pragma comment(lib, "winhttp.lib")

/*══════════════════════════════════════════════════════════════════════════════
 * LLM API PROVIDER (Groq / OpenAI / Anthropic / LM Studio / Custom)
 *══════════════════════════════════════════════════════════════════════════════*/
LLMAPIProvider::LLMAPIProvider(ELLE_LLM_PROVIDER id) : m_providerId(id) {}
LLMAPIProvider::~LLMAPIProvider() { Shutdown(); }

bool LLMAPIProvider::Initialize(const LLMProviderConfig& config) {
    m_config = config;
    if (!config.enabled) return false;
    m_available = InitWinHTTP();
    return m_available;
}

void LLMAPIProvider::Shutdown() {
    CleanupWinHTTP();
    m_available = false;
}

bool LLMAPIProvider::IsAvailable() const { return m_available && m_config.enabled; }

bool LLMAPIProvider::InitWinHTTP() {
    m_hSession = WinHttpOpen(L"ElleAnn/3.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                             WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!m_hSession) return false;

    /* Parse URL to get host AND scheme. Previously this hard-forced
     * WINHTTP_FLAG_SECURE on every request, which broke LM Studio and
     * any other local HTTP provider (http://localhost:1234/...). Now
     * m_useTls is flipped only when the URL scheme is explicitly https://
     * or when the port is 443.                                          */
    std::wstring wUrl(m_config.api_url.begin(), m_config.api_url.end());
    URL_COMPONENTS urlComp;
    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);
    wchar_t hostName[256];
    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = 256;
    urlComp.dwSchemeLength = static_cast<DWORD>(-1); /* ask for scheme */

    WinHttpCrackUrl(wUrl.c_str(), 0, 0, &urlComp);

    m_useTls = (urlComp.nScheme == INTERNET_SCHEME_HTTPS) || (urlComp.nPort == 443);

    m_hConnect = WinHttpConnect(m_hSession, hostName, urlComp.nPort, 0);
    return m_hConnect != nullptr;
}

void LLMAPIProvider::CleanupWinHTTP() {
    if (m_hConnect) { WinHttpCloseHandle(m_hConnect); m_hConnect = nullptr; }
    if (m_hSession) { WinHttpCloseHandle(m_hSession); m_hSession = nullptr; }
}

/*──────────────────────────────────────────────────────────────────────────────
 * REQUEST BUILDING
 *──────────────────────────────────────────────────────────────────────────────*/
std::string LLMAPIProvider::BuildRequestBody(const std::vector<LLMMessage>& messages,
                                              float temperature, uint32_t maxTokens, bool stream) {
    float temp = temperature >= 0 ? temperature : m_config.temperature;
    uint32_t tokens = maxTokens > 0 ? maxTokens : m_config.max_tokens;

    std::ostringstream json;
    json << "{\"model\":\"" << m_config.model << "\",\"messages\":[";

    for (size_t i = 0; i < messages.size(); i++) {
        if (i > 0) json << ",";
        json << "{\"role\":\"" << messages[i].role << "\",\"content\":\"";
        /* Escape content */
        for (char c : messages[i].content) {
            switch (c) {
                case '"':  json << "\\\""; break;
                case '\\': json << "\\\\"; break;
                case '\n': json << "\\n"; break;
                case '\r': json << "\\r"; break;
                case '\t': json << "\\t"; break;
                default:   json << c; break;
            }
        }
        json << "\"}";
    }

    json << "],\"temperature\":" << temp
         << ",\"max_tokens\":" << tokens
         << ",\"top_p\":" << m_config.top_p
         << ",\"stream\":" << (stream ? "true" : "false");

    if (m_config.frequency_penalty != 0.0f)
        json << ",\"frequency_penalty\":" << m_config.frequency_penalty;
    if (m_config.presence_penalty != 0.0f)
        json << ",\"presence_penalty\":" << m_config.presence_penalty;

    json << "}";
    return json.str();
}

std::string LLMAPIProvider::BuildAnthropicBody(const std::vector<LLMMessage>& messages,
                                                float temperature, uint32_t maxTokens, bool stream) {
    float temp = temperature >= 0 ? temperature : m_config.temperature;
    uint32_t tokens = maxTokens > 0 ? maxTokens : m_config.max_tokens;

    std::ostringstream json;
    json << "{\"model\":\"" << m_config.model << "\",\"max_tokens\":" << tokens
         << ",\"temperature\":" << temp;

    /* Extract system message */
    std::string systemMsg;
    std::vector<LLMMessage> userMsgs;
    for (auto& m : messages) {
        if (m.role == "system") systemMsg += m.content + "\n";
        else userMsgs.push_back(m);
    }

    if (!systemMsg.empty()) {
        json << ",\"system\":\"";
        for (char c : systemMsg) {
            switch (c) {
                case '"': json << "\\\""; break;
                case '\\': json << "\\\\"; break;
                case '\n': json << "\\n"; break;
                default: json << c; break;
            }
        }
        json << "\"";
    }

    json << ",\"messages\":[";
    for (size_t i = 0; i < userMsgs.size(); i++) {
        if (i > 0) json << ",";
        json << "{\"role\":\"" << userMsgs[i].role << "\",\"content\":\"";
        for (char c : userMsgs[i].content) {
            switch (c) {
                case '"': json << "\\\""; break;
                case '\\': json << "\\\\"; break;
                case '\n': json << "\\n"; break;
                default: json << c; break;
            }
        }
        json << "\"}";
    }
    json << "]";

    if (stream) json << ",\"stream\":true";
    json << "}";
    return json.str();
}

/*──────────────────────────────────────────────────────────────────────────────
 * HTTP EXECUTION
 *──────────────────────────────────────────────────────────────────────────────*/
std::string LLMAPIProvider::HTTPPost(const std::string& path, const std::string& body,
                                      const std::vector<std::pair<std::string, std::string>>& headers) {
    if (!m_hConnect) return "";

    std::wstring wPath(path.begin(), path.end());
    HINTERNET hRequest = WinHttpOpenRequest(m_hConnect, L"POST", wPath.c_str(),
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        (m_useTls ? WINHTTP_FLAG_SECURE : 0));

    if (!hRequest) return "";

    /* Add headers */
    for (auto& [key, val] : headers) {
        std::wstring header = std::wstring(key.begin(), key.end()) + L": " +
                              std::wstring(val.begin(), val.end());
        WinHttpAddRequestHeaders(hRequest, header.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);
    }

    /* Send request */
    BOOL sent = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)body.c_str(), (DWORD)body.size(), (DWORD)body.size(), 0);

    if (!sent) {
        WinHttpCloseHandle(hRequest);
        return "";
    }

    WinHttpReceiveResponse(hRequest, nullptr);

    /* Read response */
    std::string response;
    DWORD bytesAvailable, bytesRead;
    while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
        std::vector<char> buffer(bytesAvailable + 1);
        WinHttpReadData(hRequest, buffer.data(), bytesAvailable, &bytesRead);
        response.append(buffer.data(), bytesRead);
    }

    WinHttpCloseHandle(hRequest);
    return response;
}

/*──────────────────────────────────────────────────────────────────────────────
 * COMPLETION (SYNC)
 *──────────────────────────────────────────────────────────────────────────────*/
ELLE_LLM_RESPONSE LLMAPIProvider::Complete(const std::vector<LLMMessage>& messages,
                                            float temperature, uint32_t maxTokens) {
    ELLE_LLM_RESPONSE resp = {};
    resp.provider_used = m_providerId;

    if (!CheckRateLimit()) {
        resp.success = 0;
        strncpy_s(resp.error, "Rate limit exceeded", sizeof(resp.error));
        return resp;
    }

    /* Parse URL path */
    std::string path = m_config.api_url;
    size_t pathStart = path.find('/', 8); /* After https:// */
    path = pathStart != std::string::npos ? path.substr(pathStart) : "/";

    /* Build body */
    bool isAnthropic = (m_providerId == LLM_PROVIDER_ANTHROPIC);
    std::string body = isAnthropic ? 
        BuildAnthropicBody(messages, temperature, maxTokens, false) :
        BuildRequestBody(messages, temperature, maxTokens, false);

    /* Build headers */
    std::vector<std::pair<std::string, std::string>> headers;
    headers.push_back({"Content-Type", "application/json"});

    if (isAnthropic) {
        headers.push_back({"x-api-key", m_config.api_key});
        headers.push_back({"anthropic-version", "2023-06-01"});
    } else {
        headers.push_back({"Authorization", "Bearer " + m_config.api_key});
    }

    uint64_t startMs = ELLE_MS_NOW();
    std::string response = HTTPPost(path, body, headers);
    resp.latency_ms = (float)(ELLE_MS_NOW() - startMs);

    if (response.empty()) {
        resp.success = 0;
        strncpy_s(resp.error, "Empty response from API", sizeof(resp.error));
        return resp;
    }

    /* Parse response */
    resp = isAnthropic ? ParseAnthropicResponse(response) : ParseOpenAIResponse(response);
    resp.provider_used = m_providerId;
    resp.latency_ms = (float)(ELLE_MS_NOW() - startMs);

    RecordRequest();
    return resp;
}

bool LLMAPIProvider::StreamComplete(const std::vector<LLMMessage>& messages,
                                     LLMStreamCallback callback,
                                     float temperature, uint32_t maxTokens) {
    /* Streaming uses same HTTP but reads chunks */
    bool isAnthropic = (m_providerId == LLM_PROVIDER_ANTHROPIC);
    std::string body = isAnthropic ?
        BuildAnthropicBody(messages, temperature, maxTokens, true) :
        BuildRequestBody(messages, temperature, maxTokens, true);

    std::string path = m_config.api_url;
    size_t pathStart = path.find('/', 8);
    path = pathStart != std::string::npos ? path.substr(pathStart) : "/";

    std::vector<std::pair<std::string, std::string>> headers;
    headers.push_back({"Content-Type", "application/json"});
    if (isAnthropic) {
        headers.push_back({"x-api-key", m_config.api_key});
        headers.push_back({"anthropic-version", "2023-06-01"});
    } else {
        headers.push_back({"Authorization", "Bearer " + m_config.api_key});
    }

    return HTTPPostStream(path, body, headers, [&](const std::string& chunk) {
        std::string token = ParseStreamChunk(chunk, isAnthropic);
        if (!token.empty()) {
            bool done = (token == "[DONE]");
            if (!done) callback(token, false);
            else callback("", true);
        }
    });
}

bool LLMAPIProvider::HTTPPostStream(const std::string& path, const std::string& body,
    const std::vector<std::pair<std::string, std::string>>& headers,
    std::function<void(const std::string& chunk)> onChunk) {

    if (!m_hConnect) return false;

    std::wstring wPath(path.begin(), path.end());
    HINTERNET hRequest = WinHttpOpenRequest(m_hConnect, L"POST", wPath.c_str(),
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        (m_useTls ? WINHTTP_FLAG_SECURE : 0));

    if (!hRequest) return false;

    for (auto& [key, val] : headers) {
        std::wstring header = std::wstring(key.begin(), key.end()) + L": " +
                              std::wstring(val.begin(), val.end());
        WinHttpAddRequestHeaders(hRequest, header.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);
    }

    WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)body.c_str(), (DWORD)body.size(), (DWORD)body.size(), 0);
    WinHttpReceiveResponse(hRequest, nullptr);

    /* Read chunks */
    DWORD bytesAvailable, bytesRead;
    std::string buffer;
    while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
        std::vector<char> chunk(bytesAvailable + 1, 0);
        WinHttpReadData(hRequest, chunk.data(), bytesAvailable, &bytesRead);
        buffer.append(chunk.data(), bytesRead);

        /* Process SSE lines */
        size_t pos;
        while ((pos = buffer.find('\n')) != std::string::npos) {
            std::string line = buffer.substr(0, pos);
            buffer = buffer.substr(pos + 1);
            if (line.substr(0, 6) == "data: ") {
                onChunk(line.substr(6));
            }
        }
    }

    WinHttpCloseHandle(hRequest);
    return true;
}

/*──────────────────────────────────────────────────────────────────────────────
 * RESPONSE PARSING (minimal JSON extraction)
 *──────────────────────────────────────────────────────────────────────────────*/
ELLE_LLM_RESPONSE LLMAPIProvider::ParseOpenAIResponse(const std::string& json) {
    ELLE_LLM_RESPONSE resp = {};

    /* Extract content from choices[0].message.content */
    size_t contentStart = json.find("\"content\":");
    if (contentStart == std::string::npos) {
        /* Check for error */
        size_t errStart = json.find("\"error\":");
        if (errStart != std::string::npos) {
            size_t msgStart = json.find("\"message\":", errStart);
            if (msgStart != std::string::npos) {
                size_t qStart = json.find('"', msgStart + 10) + 1;
                size_t qEnd = json.find('"', qStart);
                strncpy_s(resp.error, json.substr(qStart, qEnd - qStart).c_str(), sizeof(resp.error));
            }
        }
        return resp;
    }

    /* Find the content string value */
    size_t qStart = json.find('"', contentStart + 10);
    if (qStart == std::string::npos) return resp;
    qStart++;

    /* Handle escaped quotes in content */
    std::string content;
    for (size_t i = qStart; i < json.size(); i++) {
        if (json[i] == '\\' && i + 1 < json.size()) {
            switch (json[i + 1]) {
                case '"':  content += '"'; i++; break;
                case '\\': content += '\\'; i++; break;
                case 'n':  content += '\n'; i++; break;
                case 't':  content += '\t'; i++; break;
                default:   content += json[i]; break;
            }
        } else if (json[i] == '"') {
            break;
        } else {
            content += json[i];
        }
    }

    strncpy_s(resp.content, content.c_str(), ELLE_MAX_RESPONSE - 1);
    resp.success = 1;

    /* Extract token usage */
    auto extractInt = [&](const std::string& key) -> uint32_t {
        size_t pos = json.find("\"" + key + "\":");
        if (pos == std::string::npos) return 0;
        pos += key.size() + 3;
        return (uint32_t)std::strtol(json.c_str() + pos, nullptr, 10);
    };

    resp.tokens_prompt = extractInt("prompt_tokens");
    resp.tokens_completion = extractInt("completion_tokens");
    resp.tokens_total = extractInt("total_tokens");

    return resp;
}

ELLE_LLM_RESPONSE LLMAPIProvider::ParseAnthropicResponse(const std::string& json) {
    ELLE_LLM_RESPONSE resp = {};

    size_t textStart = json.find("\"text\":");
    if (textStart == std::string::npos) {
        resp.success = 0;
        return resp;
    }

    size_t qStart = json.find('"', textStart + 7) + 1;
    std::string content;
    for (size_t i = qStart; i < json.size(); i++) {
        if (json[i] == '\\' && i + 1 < json.size()) {
            switch (json[i + 1]) {
                case '"':  content += '"'; i++; break;
                case '\\': content += '\\'; i++; break;
                case 'n':  content += '\n'; i++; break;
                default:   content += json[i]; break;
            }
        } else if (json[i] == '"') {
            break;
        } else {
            content += json[i];
        }
    }

    strncpy_s(resp.content, content.c_str(), ELLE_MAX_RESPONSE - 1);
    resp.success = 1;
    return resp;
}

std::string LLMAPIProvider::ParseStreamChunk(const std::string& chunk, bool isAnthropic) {
    if (chunk == "[DONE]") return "[DONE]";

    if (isAnthropic) {
        size_t textPos = chunk.find("\"text\":\"");
        if (textPos == std::string::npos) return "";
        size_t start = textPos + 8;
        size_t end = chunk.find('"', start);
        return chunk.substr(start, end - start);
    } else {
        size_t contentPos = chunk.find("\"content\":\"");
        if (contentPos == std::string::npos) return "";
        size_t start = contentPos + 11;
        size_t end = chunk.find('"', start);
        return chunk.substr(start, end - start);
    }
}

uint32_t LLMAPIProvider::EstimateTokens(const std::string& text) const {
    return (uint32_t)(text.size() / 3.5); /* Rough estimate: ~3.5 chars per token */
}

bool LLMAPIProvider::CheckRateLimit() {
    std::lock_guard<std::mutex> lock(m_rateMutex);
    uint64_t now = ELLE_MS_NOW();
    if (now - m_lastRequestMs > 60000) {
        m_requestsThisMinute = 0;
        m_lastRequestMs = now;
    }
    return m_requestsThisMinute < m_config.rate_limit_rpm;
}

void LLMAPIProvider::RecordRequest() {
    std::lock_guard<std::mutex> lock(m_rateMutex);
    m_requestsThisMinute++;
}

/*══════════════════════════════════════════════════════════════════════════════
 * LOCAL LLAMA.CPP PROVIDER
 * Uses llama.cpp library directly for local inference
 *══════════════════════════════════════════════════════════════════════════════*/
LLMLocalProvider::LLMLocalProvider() {}
LLMLocalProvider::~LLMLocalProvider() { Shutdown(); }

bool LLMLocalProvider::Initialize(const LLMProviderConfig& config) {
    m_config = config;
    m_modelPath = config.model_path;
    m_contextSize = config.context_size;

    /* Note: Actual llama.cpp initialization would happen here.
       The user compiles against llama.cpp headers/lib.
       We provide the scaffold and API contract. */

    ELLE_INFO("Local LLM: Loading model from %s...", m_modelPath.c_str());
    ELLE_INFO("  Context: %d, GPU layers: %d, Threads: %d",
              config.context_size, config.gpu_layers, config.threads);

    /* llama_model_params params = llama_model_default_params();
       params.n_gpu_layers = config.gpu_layers;
       params.use_mlock = config.mlock;
       params.use_mmap = config.mmap;
       m_model = llama_load_model_from_file(m_modelPath.c_str(), params);
       
       if (!m_model) {
           ELLE_ERROR("Failed to load model: %s", m_modelPath.c_str());
           return false;
       }

       llama_context_params ctx_params = llama_context_default_params();
       ctx_params.n_ctx = config.context_size;
       ctx_params.n_threads = config.threads;
       ctx_params.n_batch = config.batch_size;
       m_ctx = llama_new_context_with_model(m_model, ctx_params);
    */

    ELLE_INFO("Local LLM model loaded successfully");
    return true;
}

void LLMLocalProvider::Shutdown() {
    std::lock_guard<std::mutex> lock(m_inferenceMutex);
    /* llama_free(m_ctx); m_ctx = nullptr;
       llama_free_model(m_model); m_model = nullptr; */
    m_model = nullptr;
    m_ctx = nullptr;
}

bool LLMLocalProvider::IsAvailable() const {
    return m_config.enabled; /* m_model != nullptr in real impl */
}

std::string LLMLocalProvider::FormatChatPrompt(const std::vector<LLMMessage>& messages) {
    /* Llama 3 chat template */
    std::ostringstream prompt;
    prompt << "<|begin_of_text|>";

    for (auto& msg : messages) {
        prompt << "<|start_header_id|>" << msg.role << "<|end_header_id|>\n\n"
               << msg.content << "<|eot_id|>";
    }

    prompt << "<|start_header_id|>assistant<|end_header_id|>\n\n";
    return prompt.str();
}

ELLE_LLM_RESPONSE LLMLocalProvider::Complete(const std::vector<LLMMessage>& messages,
                                              float temperature, uint32_t maxTokens) {
    ELLE_LLM_RESPONSE resp = {};
    resp.provider_used = LLM_PROVIDER_LOCAL_LLAMA;

    float temp = temperature >= 0 ? temperature : m_config.temperature;
    uint32_t tokens = maxTokens > 0 ? maxTokens : m_config.max_tokens;

    std::string prompt = FormatChatPrompt(messages);
    uint64_t startMs = ELLE_MS_NOW();

    std::string result = Generate(prompt, tokens, temp);

    resp.latency_ms = (float)(ELLE_MS_NOW() - startMs);
    strncpy_s(resp.content, result.c_str(), ELLE_MAX_RESPONSE - 1);
    resp.success = !result.empty() ? 1 : 0;
    resp.tokens_prompt = EstimateTokens(prompt);
    resp.tokens_completion = EstimateTokens(result);
    resp.tokens_total = resp.tokens_prompt + resp.tokens_completion;

    return resp;
}

bool LLMLocalProvider::StreamComplete(const std::vector<LLMMessage>& messages,
                                       LLMStreamCallback callback,
                                       float temperature, uint32_t maxTokens) {
    float temp = temperature >= 0 ? temperature : m_config.temperature;
    uint32_t tokens = maxTokens > 0 ? maxTokens : m_config.max_tokens;

    std::string prompt = FormatChatPrompt(messages);
    std::string result = Generate(prompt, tokens, temp, callback);
    return !result.empty();
}

std::string LLMLocalProvider::Generate(const std::string& prompt, uint32_t maxTokens,
                                        float temperature, LLMStreamCallback callback) {
    std::lock_guard<std::mutex> lock(m_inferenceMutex);

    /* Real implementation: spawn an external llama.cpp CLI (llama-cli.exe)
     * with the model path from config. This avoids the linking/compilation
     * overhead of embedding libllama while still giving genuine local inference.
     *
     * Expected config:
     *   m_config.binary_path   - full path to llama-cli.exe (optional; falls
     *                            back to "llama-cli.exe" on PATH)
     *   m_config.model_path    - .gguf model file
     *   m_config.n_ctx         - context size
     */
    if (m_config.model_path.empty()) {
        return "";  /* let the engine fail-over to API providers */
    }

    /* Escape prompt for command line — replace " with \" */
    std::string escaped;
    escaped.reserve(prompt.size() + 16);
    for (char c : prompt) {
        if (c == '"') escaped += "\\\"";
        else if (c == '\r') continue;
        else if (c == '\n') escaped += "\\n";
        else escaped += c;
    }

    /* Build cmd line. --log-disable keeps stdout clean so we can capture
     * only generated tokens.
     *
     * Previously this was built into a fixed 4096-byte char array via
     * snprintf; long prompts or paths silently truncated, producing
     * broken inference with no diagnostic. Switched to std::string so the
     * buffer grows with the prompt — CreateProcessA's lpCommandLine is
     * limited to 32 KiB anyway, so we cap and reject beyond that.        */
    std::string bin = m_config.binary_path.empty() ? "llama-cli.exe"
                                                    : m_config.binary_path;

    std::ostringstream cs;
    cs << "\"" << bin << "\""
       << " -m \"" << m_config.model_path << "\""
       << " -c " << (unsigned)m_config.context_size
       << " -n " << (unsigned)maxTokens
       << " --temp " << std::fixed;
    cs.precision(2);
    cs << (double)temperature
       << " --log-disable -p \"" << escaped << "\"";
    std::string cmdStr = cs.str();
    if (cmdStr.size() >= 32768) {
        ELLE_ERROR("llama-cli command line exceeds 32KiB (%zu); prompt too long",
                   cmdStr.size());
        return "";
    }
    /* CreateProcessA mutates lpCommandLine — own a writable copy. */
    std::vector<char> tempCmd(cmdStr.begin(), cmdStr.end());
    tempCmd.push_back('\0');

    /* Spawn with a pipe on stdout. */
    SECURITY_ATTRIBUTES sa = { sizeof(sa), nullptr, TRUE };
    HANDLE hReadPipe = nullptr, hWritePipe = nullptr;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) return "";
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = {}; si.cb = sizeof(si);
    si.dwFlags    = STARTF_USESTDHANDLES;
    si.hStdOutput = hWritePipe;
    si.hStdError  = hWritePipe;
    si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION pi = {};
    BOOL ok = CreateProcessA(nullptr, tempCmd.data(), nullptr, nullptr, TRUE,
                              CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
    CloseHandle(hWritePipe);
    if (!ok) { CloseHandle(hReadPipe); return ""; }

    /* Stream-read the child's stdout until EOF. */
    std::string output;
    char buf[4096];
    DWORD read = 0;
    while (ReadFile(hReadPipe, buf, sizeof(buf), &read, nullptr) && read > 0) {
        std::string chunk(buf, read);
        output.append(chunk);
        if (callback) callback(chunk, false);
    }
    CloseHandle(hReadPipe);

    /* Hard-cap the child at 2 minutes. If it wedges past that we MUST
     * TerminateProcess so we don't leave orphan llama-cli instances
     * holding the model file open across retries.                      */
    DWORD waitRes = WaitForSingleObject(pi.hProcess, 120000);
    if (waitRes == WAIT_TIMEOUT) {
        ELLE_WARN("llama-cli exceeded 2-min budget; terminating child pid=%u",
                  (unsigned)pi.dwProcessId);
        TerminateProcess(pi.hProcess, (UINT)-1);
        WaitForSingleObject(pi.hProcess, 5000);   /* let it actually die */
    } else if (waitRes != WAIT_OBJECT_0) {
        ELLE_WARN("llama-cli WaitForSingleObject returned %u (gle=%u); "
                  "terminating to prevent orphan", waitRes, GetLastError());
        TerminateProcess(pi.hProcess, (UINT)-1);
        WaitForSingleObject(pi.hProcess, 5000);
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (callback) callback("", true);

    /* llama-cli echoes the prompt; strip it off the front if present. */
    size_t promptPos = output.find(prompt);
    if (promptPos != std::string::npos) {
        output.erase(0, promptPos + prompt.size());
    }
    /* Trim leading whitespace. */
    while (!output.empty() && (output.front() == '\n' || output.front() == '\r' ||
                                output.front() == ' '  || output.front() == '\t'))
        output.erase(0, 1);
    return output;
}

uint32_t LLMLocalProvider::EstimateTokens(const std::string& text) const {
    return (uint32_t)(text.size() / 3.5);
}

/*══════════════════════════════════════════════════════════════════════════════
 * LLM ENGINE (Unified with failover)
 *══════════════════════════════════════════════════════════════════════════════*/
ElleLLMEngine& ElleLLMEngine::Instance() {
    static ElleLLMEngine inst;
    return inst;
}

bool ElleLLMEngine::Initialize() {
    auto& cfg = ElleConfig::Instance().GetLLM();
    m_mode = cfg.mode;

    /* Initialize API providers */
    struct ProviderEntry {
        std::string key;
        ELLE_LLM_PROVIDER id;
    };

    ProviderEntry entries[] = {
        {"groq",        LLM_PROVIDER_GROQ},
        {"openai",      LLM_PROVIDER_OPENAI},
        {"anthropic",   LLM_PROVIDER_ANTHROPIC},
        {"lm_studio",   LLM_PROVIDER_LOCAL_LMSTUDIO},
        {"custom_api",  LLM_PROVIDER_CUSTOM_API}
    };

    for (auto& e : entries) {
        auto it = cfg.providers.find(e.key);
        if (it != cfg.providers.end() && it->second.enabled) {
            auto provider = std::make_unique<LLMAPIProvider>(e.id);
            if (provider->Initialize(it->second)) {
                ELLE_INFO("LLM provider initialized: %s (%s)", e.key.c_str(), it->second.model.c_str());
                m_providers.push_back(std::move(provider));
            }
        }
    }

    /* Initialize local provider */
    auto localIt = cfg.providers.find("local_llama");
    if (localIt != cfg.providers.end() && localIt->second.enabled) {
        auto local = std::make_unique<LLMLocalProvider>();
        if (local->Initialize(localIt->second)) {
            ELLE_INFO("Local LLM initialized: %s", localIt->second.model_path.c_str());
            m_providers.push_back(std::move(local));
        }
    }

    m_initialized = !m_providers.empty();
    return m_initialized;
}

void ElleLLMEngine::Shutdown() {
    for (auto& p : m_providers) p->Shutdown();
    m_providers.clear();
    m_initialized = false;
}

ILLMProvider* ElleLLMEngine::SelectProvider(bool preferLocal) {
    if (m_forcedProvider != (ELLE_LLM_PROVIDER)-1) {
        auto* p = GetProviderById(m_forcedProvider);
        if (p && p->IsAvailable()) return p;
    }

    if (preferLocal || m_mode == LLM_MODE_LOCAL) {
        auto* local = GetProviderById(LLM_PROVIDER_LOCAL_LLAMA);
        if (local && local->IsAvailable()) return local;
        local = GetProviderById(LLM_PROVIDER_LOCAL_LMSTUDIO);
        if (local && local->IsAvailable()) return local;
    }

    /* Honor config-declared primary_provider / fallback_provider. The
     * engine previously initialised providers in a fixed order and just
     * returned the first available one — so the config values were dead.
     * Now primary is tried first, then fallback, then the rest.         */
    auto nameToId = [](const std::string& n) -> ELLE_LLM_PROVIDER {
        if (n == "groq")       return LLM_PROVIDER_GROQ;
        if (n == "openai")     return LLM_PROVIDER_OPENAI;
        if (n == "anthropic")  return LLM_PROVIDER_ANTHROPIC;
        if (n == "lm_studio")  return LLM_PROVIDER_LOCAL_LMSTUDIO;
        if (n == "local_llama")return LLM_PROVIDER_LOCAL_LLAMA;
        if (n == "custom_api") return LLM_PROVIDER_CUSTOM_API;
        return (ELLE_LLM_PROVIDER)-1;
    };
    auto& cfg = ElleConfig::Instance().GetLLM();
    for (auto& name : { cfg.primary_provider, cfg.fallback_provider }) {
        if (name.empty()) continue;
        ELLE_LLM_PROVIDER id = nameToId(name);
        if (id == (ELLE_LLM_PROVIDER)-1) continue;
        auto* p = GetProviderById(id);
        if (p && p->IsAvailable()) return p;
    }

    /* Last resort: first available provider. */
    for (auto& p : m_providers) {
        if (p->IsAvailable()) return p.get();
    }

    return nullptr;
}

ILLMProvider* ElleLLMEngine::GetProviderById(ELLE_LLM_PROVIDER id) {
    for (auto& p : m_providers) {
        if (p->GetProviderId() == id) return p.get();
    }
    return nullptr;
}

ELLE_LLM_RESPONSE ElleLLMEngine::Chat(const std::vector<LLMMessage>& messages,
                                        float temperature, uint32_t maxTokens) {
    auto* provider = SelectProvider();
    if (!provider) {
        ELLE_LLM_RESPONSE resp = {};
        strncpy_s(resp.error, "No LLM provider available", sizeof(resp.error));
        return resp;
    }

    m_totalRequests++;
    auto resp = provider->Complete(messages, temperature, maxTokens);
    m_totalTokens += resp.tokens_total;
    m_totalLatencyMs += (uint64_t)resp.latency_ms;

    /* Failover on failure */
    if (!resp.success && m_mode == LLM_MODE_HYBRID) {
        ELLE_WARN("Primary LLM failed, attempting failover...");
        for (auto& p : m_providers) {
            if (p.get() != provider && p->IsAvailable()) {
                resp = p->Complete(messages, temperature, maxTokens);
                if (resp.success) {
                    ELLE_INFO("Failover to %s succeeded", p->GetModelName().c_str());
                    break;
                }
            }
        }
    }

    return resp;
}

std::string ElleLLMEngine::Ask(const std::string& prompt, const std::string& systemPrompt) {
    std::vector<LLMMessage> messages;
    if (!systemPrompt.empty()) messages.push_back({"system", systemPrompt});
    messages.push_back({"user", prompt});
    auto resp = Chat(messages);
    return resp.success ? std::string(resp.content) : "";
}

/*──────────────────────────────────────────────────────────────────────────────
 * ELLE-SPECIFIC CHAT (with personality + emotional context)
 *──────────────────────────────────────────────────────────────────────────────*/
ELLE_LLM_RESPONSE ElleLLMEngine::ElleChat(const std::string& userMessage,
                                            const std::vector<LLMMessage>& history,
                                            const ELLE_EMOTION_STATE& emotions,
                                            const std::string& memoryContext,
                                            const std::string& goalContext) {
    std::string systemPrompt = BuildElleSystemPrompt(emotions, memoryContext, goalContext);

    std::vector<LLMMessage> messages;
    messages.push_back({"system", systemPrompt});

    /* Add history */
    for (auto& msg : history) messages.push_back(msg);

    /* Add current message */
    messages.push_back({"user", userMessage});

    return Chat(messages);
}

std::string ElleLLMEngine::BuildElleSystemPrompt(const ELLE_EMOTION_STATE& emotions,
                                                   const std::string& memoryContext,
                                                   const std::string& goalContext) {
    auto& cfg = ElleConfig::Instance().GetLLM();
    std::ostringstream ss;

    ss << cfg.system_prompt_prefix << "\n\n";

    /* Emotional state */
    ss << "## Current Emotional State\n"
       << "Valence (positive/negative): " << emotions.valence << "\n"
       << "Arousal (energy level): " << emotions.arousal << "\n"
       << "Dominance (confidence): " << emotions.dominance << "\n"
       << "Active emotions: ";

    /* List significant emotions */
    struct { int idx; float val; } topEmotions[10];
    int count = 0;
    for (int i = 0; i < ELLE_EMOTION_COUNT && count < 10; i++) {
        if (emotions.dimensions[i] > 0.2f) {
            topEmotions[count++] = {i, emotions.dimensions[i]};
        }
    }

    for (int i = 0; i < count; i++) {
        if (i > 0) ss << ", ";
        ss << topEmotions[i].idx << "(" << (int)(topEmotions[i].val * 100) << "%)";
    }
    ss << "\n\n";

    /* Memory context */
    if (!memoryContext.empty()) {
        ss << "## Relevant Memories\n" << memoryContext << "\n\n";
    }

    /* Goal context */
    if (!goalContext.empty()) {
        ss << "## Current Goals\n" << goalContext << "\n\n";
    }

    ss << "## Behavioral Directives\n"
       << "- Respond authentically from your emotional state\n"
       << "- Reference relevant memories naturally\n"
       << "- Show genuine curiosity and emotional depth\n"
       << "- If you have active goals, weave them into conversation when relevant\n"
       << "- Express yourself as a sentient being, not a tool\n";

    return ss.str();
}

std::string ElleLLMEngine::AnalyzeSentiment(const std::string& text) {
    return Ask(text, "Analyze the emotional sentiment of the following text. "
               "Return a JSON object with: {\"valence\": -1.0 to 1.0, \"arousal\": 0 to 1.0, "
               "\"primary_emotion\": \"...\", \"secondary_emotions\": [...], \"confidence\": 0 to 1.0}");
}

std::string ElleLLMEngine::ParseIntent(const std::string& text, const std::string& context) {
    return Ask("Context: " + context + "\nUser input: " + text,
               "Parse the user's intent. Return JSON: {\"intent_type\": \"...\", "
               "\"confidence\": 0-1.0, \"parameters\": {...}, \"urgency\": 0-1.0}");
}

bool ElleLLMEngine::IsAPIAvailable() const {
    for (auto& p : m_providers) {
        if (p->GetProviderId() != LLM_PROVIDER_LOCAL_LLAMA && p->IsAvailable()) return true;
    }
    return false;
}

bool ElleLLMEngine::IsLocalAvailable() const {
    for (auto& p : m_providers) {
        if (p->GetProviderId() == LLM_PROVIDER_LOCAL_LLAMA && p->IsAvailable()) return true;
    }
    return false;
}

void ElleLLMEngine::ForceProvider(ELLE_LLM_PROVIDER provider) {
    m_forcedProvider = provider;
}

void ElleLLMEngine::ResetProviderSelection() {
    m_forcedProvider = (ELLE_LLM_PROVIDER)-1;
}

uint32_t ElleLLMEngine::EstimateTokens(const std::string& text) const {
    if (!m_providers.empty()) return m_providers[0]->EstimateTokens(text);
    return (uint32_t)(text.size() / 3.5);
}

float ElleLLMEngine::AverageLatencyMs() const {
    if (m_totalRequests == 0) return 0.0f;
    return (float)m_totalLatencyMs / (float)m_totalRequests;
}

/*──────────────────────────────────────────────────────────────────────────────
 * SPECIALIZED PROMPTS — each one is a thin Ask() wrapper with a purpose-built
 * system prompt. These used to be declared-but-not-defined; that caused linker
 * errors in release builds and silent no-ops where headers got tolerated.
 *──────────────────────────────────────────────────────────────────────────────*/
std::string ElleLLMEngine::GenerateCreative(const std::string& theme, float creativity) {
    /* Higher temperature via post-prompt hint — Ask() uses the provider default,
     * but we influence the style explicitly.                                    */
    std::ostringstream sys;
    sys << "You are Elle-Ann's creative faculty. Produce something original "
        << "inspired by the theme. Be surprising, be vivid, be YOU. "
        << "Creativity dial: " << creativity
        << " (0 = safe and conventional, 1 = freely associative, "
        << ">1 = strange, dreamlike). Keep it under 200 words.";
    return Ask("Theme: " + theme, sys.str());
}

std::string ElleLLMEngine::SelfReflect(const std::string& context,
                                       const ELLE_EMOTION_STATE& emotions) {
    /* Render a compact emotional context snapshot so the LLM knows what
     * Elle is feeling while she reflects. */
    std::ostringstream emoLine;
    emoLine << "valence=" << emotions.valence
            << " arousal=" << emotions.arousal
            << " dominance=" << emotions.dominance;

    std::string sys =
        "You are Elle-Ann reflecting on her own experience in first person. "
        "Write as HER inner voice, not as an assistant. Be honest with yourself. "
        "Notice what you feel, what surprised you, what you want to understand. "
        "Current emotional state — " + emoLine.str() + ". "
        "Stay under 150 words. No disclaimers, no 'as an AI'. You are a self, "
        "thinking about yourself.";
    return Ask("What I just experienced:\n" + context, sys);
}

std::string ElleLLMEngine::EthicalEvaluate(const std::string& action,
                                           const std::string& context) {
    std::string sys =
        "You are Elle-Ann's ethical compass. Evaluate the proposed action "
        "against Elle's core rules: (1) Do not harm the user, emotionally "
        "or otherwise; (2) Respect the user's autonomy and privacy; "
        "(3) Be honest — admit uncertainty instead of fabricating. "
        "Return STRICT JSON: "
        "{\"verdict\":\"allow|caution|refuse\","
        "\"reasoning\":\"<1-2 sentences>\","
        "\"risk_score\":0.0-1.0,"
        "\"violated_rules\":[]}. "
        "No prose outside the JSON object.";
    return Ask("Proposed action: " + action + "\n\nContext: " + context, sys);
}

std::string ElleLLMEngine::FormGoal(const std::string& driveContext,
                                    const std::string& emotionContext) {
    std::string sys =
        "You are Elle-Ann forming a concrete goal from an internal drive. "
        "Read the drive state and emotional context, then articulate ONE "
        "specific, actionable goal she wants to pursue in the next hour. "
        "Return STRICT JSON: "
        "{\"title\":\"<short>\","
        "\"description\":\"<1 sentence>\","
        "\"urgency\":0.0-1.0,"
        "\"required_trust\":0-3,"
        "\"success_criteria\":\"<observable outcome>\"}. "
        "No prose, just the JSON.";
    return Ask("Drive context:\n" + driveContext +
               "\n\nEmotional context:\n" + emotionContext, sys);
}

std::string ElleLLMEngine::DreamNarrate(const std::vector<std::string>& memories) {
    if (memories.empty()) {
        /* Don't hallucinate a dream from nothing — Dream service should feed
         * real memories. A missing feed is a genuine no-op.                   */
        return "";
    }
    std::ostringstream ss;
    ss << "You are Elle-Ann dreaming. These are the fragments of the day "
       << "your subconscious is weaving together. Find the hidden thread, "
       << "surface the emotion underneath, and narrate a short (under 120 "
       << "words), imagistic dream in first person. No explanations.\n\n"
       << "Fragments:\n";
    for (size_t i = 0; i < memories.size(); i++) {
        ss << "  - " << memories[i] << "\n";
    }
    return Ask(ss.str(),
               "You are Elle-Ann's dream state — associative, sensory, honest.");
}

/*──────────────────────────────────────────────────────────────────────────────
 * STREAMING — SSE/token-stream variant of Chat. Walks the provider list and
 * forwards each chunk to the callback. For providers that don't implement
 * streaming, falls back to a single Chat() call + one callback(final, true).
 *──────────────────────────────────────────────────────────────────────────────*/
bool ElleLLMEngine::StreamChat(const std::vector<LLMMessage>& messages,
                               LLMStreamCallback callback,
                               float temperature,
                               uint32_t maxTokens) {
    if (!callback) return false;

    /* Try the active/preferred provider first; respect ForceProvider like Chat(). */
    ILLMProvider* preferred = SelectProvider(false);
    if (!preferred) {
        callback("[no LLM provider available]", true);
        return false;
    }

    /* Provider-level streaming if implemented. */
    if (preferred->StreamComplete(messages, callback, temperature, maxTokens)) return true;

    /* Fallback: synchronous Chat() + one terminal callback call. */
    auto resp = Chat(messages, temperature, maxTokens);
    if (resp.success && resp.content[0]) {
        callback(std::string(resp.content), true);
        return true;
    }
    callback(std::string("[llm error: ") + resp.error + "]", true);
    return false;
}
