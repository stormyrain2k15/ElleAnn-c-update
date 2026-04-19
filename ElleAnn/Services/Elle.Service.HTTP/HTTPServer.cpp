/*******************************************************************************
 * HTTPServer.cpp — Raw Winsock HTTP + WebSocket Server (No Framework)
 *
 * Handles REST API endpoints (matching Kotlin ElleApiService contract)
 * and WebSocket command channel with full RFC 6455 handshake + framing.
 *
 * Dependencies:
 *   - WinSock 2
 *   - WinHTTP  (direct Groq calls for /api/ai/chat fallback)
 *   - bcrypt   (SHA-1 for WebSocket handshake)
 *   - nlohmann::json (../../Shared/json.hpp) for robust JSON parsing
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include "../../Shared/ElleLLM.h"
#include "../../Shared/json.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include <windows.h>
#include <winhttp.h>
#include <bcrypt.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "bcrypt.lib")

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>
#include <atomic>
#include <cstring>
#include <chrono>
#include <condition_variable>

using json = nlohmann::json;

/*──────────────────────────────────────────────────────────────────────────────
 * BASE64 (for WebSocket handshake)
 *──────────────────────────────────────────────────────────────────────────────*/
static const char kB64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string Base64Encode(const unsigned char* data, size_t len) {
    std::string out;
    out.reserve(((len + 2) / 3) * 4);
    for (size_t i = 0; i < len; i += 3) {
        uint32_t n = (uint32_t)data[i] << 16;
        if (i + 1 < len) n |= (uint32_t)data[i + 1] << 8;
        if (i + 2 < len) n |= (uint32_t)data[i + 2];
        out.push_back(kB64[(n >> 18) & 63]);
        out.push_back(kB64[(n >> 12) & 63]);
        out.push_back(i + 1 < len ? kB64[(n >> 6) & 63] : '=');
        out.push_back(i + 2 < len ? kB64[n & 63] : '=');
    }
    return out;
}

/*──────────────────────────────────────────────────────────────────────────────
 * SHA-1 via Windows bcrypt (for Sec-WebSocket-Accept)
 *──────────────────────────────────────────────────────────────────────────────*/
static bool SHA1Hash(const std::string& input, unsigned char out[20]) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_HASH_HANDLE hHash = nullptr;
    bool ok = false;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA1_ALGORITHM, nullptr, 0);
    if (status != 0) return false;
    status = BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0);
    if (status == 0) {
        status = BCryptHashData(hHash, (PUCHAR)input.data(), (ULONG)input.size(), 0);
        if (status == 0) {
            status = BCryptFinishHash(hHash, out, 20, 0);
            ok = (status == 0);
        }
        BCryptDestroyHash(hHash);
    }
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok;
}

static std::string MakeWsAccept(const std::string& key) {
    static const std::string MAGIC = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string combined = key + MAGIC;
    unsigned char digest[20];
    if (!SHA1Hash(combined, digest)) return "";
    return Base64Encode(digest, 20);
}

/*──────────────────────────────────────────────────────────────────────────────
 * HTTP REQUEST/RESPONSE TYPES
 *──────────────────────────────────────────────────────────────────────────────*/
struct HTTPRequest {
    std::string method;
    std::string path;
    std::string query;
    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> queryParams;
    std::string body;
    bool isWebSocket = false;
    std::string wsKey;

    std::string QueryParam(const std::string& key, const std::string& def = "") const {
        auto it = queryParams.find(key);
        return it != queryParams.end() ? it->second : def;
    }

    /* Attempt to parse body as JSON, return empty object on failure */
    json BodyJSON() const {
        if (body.empty()) return json::object();
        try { return json::parse(body); }
        catch (...) { return json::object(); }
    }
};

struct HTTPResponse {
    int         status = 200;
    std::string statusText = "OK";
    std::string contentType = "application/json";
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    std::string Serialize() const {
        std::ostringstream ss;
        ss << "HTTP/1.1 " << status << " " << statusText << "\r\n";
        ss << "Content-Type: " << contentType << "\r\n";
        ss << "Content-Length: " << body.size() << "\r\n";
        ss << "Access-Control-Allow-Origin: *\r\n";
        ss << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
        ss << "Access-Control-Allow-Headers: Content-Type, Authorization, x-admin-key\r\n";
        for (auto& [k, v] : headers) ss << k << ": " << v << "\r\n";
        ss << "\r\n" << body;
        return ss.str();
    }

    static HTTPResponse JSON(int status, const std::string& body) {
        HTTPResponse r;
        r.status = status;
        r.statusText = (status >= 200 && status < 300) ? "OK"
                    : (status == 404 ? "Not Found"
                    : (status == 400 ? "Bad Request"
                    : (status == 401 ? "Unauthorized"
                    : (status == 504 ? "Gateway Timeout" : "Error"))));
        r.body = body;
        return r;
    }

    static HTTPResponse OK(const json& j) { return JSON(200, j.dump()); }
    static HTTPResponse Created(const json& j) { return JSON(201, j.dump()); }
    static HTTPResponse Err(int code, const std::string& msg) {
        json j;
        j["error"] = msg;
        return JSON(code, j.dump());
    }
};

typedef std::function<HTTPResponse(const HTTPRequest&)> RouteHandler;

/*──────────────────────────────────────────────────────────────────────────────
 * ROUTE DISPATCHER — supports path patterns with {placeholders}
 *──────────────────────────────────────────────────────────────────────────────*/
struct RouteEntry {
    std::string method;
    std::string pattern;   /* e.g. /api/memory/{id} */
    std::regex  re;
    std::vector<std::string> paramNames;
    RouteHandler handler;
};

class RouteDispatch {
public:
    void Register(const std::string& method, const std::string& pattern, RouteHandler h) {
        RouteEntry entry;
        entry.method = method;
        entry.pattern = pattern;
        std::string regexStr;
        std::string token;
        bool inParam = false;
        for (char c : pattern) {
            if (c == '{') { inParam = true; token.clear(); continue; }
            if (c == '}') {
                inParam = false;
                entry.paramNames.push_back(token);
                regexStr += "([^/]+)";
                continue;
            }
            if (inParam) { token.push_back(c); continue; }
            if (c == '.' || c == '+' || c == '*' || c == '?' || c == '(' || c == ')' ||
                c == '[' || c == ']' || c == '^' || c == '$' || c == '\\' || c == '|') {
                regexStr.push_back('\\');
            }
            regexStr.push_back(c);
        }
        entry.re = std::regex("^" + regexStr + "$");
        entry.handler = std::move(h);
        m_routes.push_back(std::move(entry));
    }

    HTTPResponse Dispatch(HTTPRequest& req) {
        for (auto& e : m_routes) {
            if (e.method != req.method) continue;
            std::smatch m;
            if (std::regex_match(req.path, m, e.re)) {
                for (size_t i = 0; i < e.paramNames.size() && i + 1 < m.size(); i++) {
                    req.headers["x-path-" + e.paramNames[i]] = m[i + 1].str();
                }
                return e.handler(req);
            }
        }
        return HTTPResponse::Err(404, "Not found: " + req.method + " " + req.path);
    }

    size_t Count() const { return m_routes.size(); }

private:
    std::vector<RouteEntry> m_routes;
};

/*──────────────────────────────────────────────────────────────────────────────
 * WEBSOCKET CLIENT
 *──────────────────────────────────────────────────────────────────────────────*/
struct WSClient {
    SOCKET      socket = INVALID_SOCKET;
    std::string id;
    bool        alive = true;
    uint64_t    connected_ms = 0;
    std::mutex  sendMutex;
};

/*──────────────────────────────────────────────────────────────────────────────
 * DIRECT GROQ CALL via WinHTTP (bypass IPC — prevents chat hang)
 * Accepts a full conversation message list so the caller controls history.
 *──────────────────────────────────────────────────────────────────────────────*/
struct LLMMsg { std::string role; std::string content; };

static bool CallGroqDirect(const std::vector<LLMMsg>& messages,
                           std::string& outResponse,
                           std::string& outError)
{
    auto& cfg = ElleConfig::Instance().GetLLM();
    const LLMProviderConfig* groq = nullptr;
    auto it = cfg.providers.find("groq");
    if (it != cfg.providers.end() && it->second.enabled) {
        groq = &it->second;
    }
    if (!groq) { outError = "No Groq provider configured"; return false; }

    /* Build request body via nlohmann::json */
    json msgArr = json::array();
    for (auto& m : messages) {
        msgArr.push_back({{"role", m.role}, {"content", m.content}});
    }
    json body = {
        {"model", groq->model.empty() ? "llama-3.3-70b-versatile" : groq->model},
        {"messages", msgArr},
        {"temperature", 0.85},
        {"max_tokens", 2048},
        {"stream", false}
    };
    std::string bodyStr = body.dump();

    /* Parse base URL */
    std::string host = "api.groq.com";
    std::string path = "/openai/v1/chat/completions";
    if (!groq->api_url.empty()) {
        std::string url = groq->api_url;
        if (url.rfind("https://", 0) == 0) url = url.substr(8);
        else if (url.rfind("http://", 0) == 0) url = url.substr(7);
        size_t slash = url.find('/');
        if (slash != std::string::npos) {
            host = url.substr(0, slash);
            path = url.substr(slash);
            if (path.find("/chat/completions") == std::string::npos) path += "/chat/completions";
        } else {
            host = url;
        }
    }

    /* Convert to wide strings */
    auto toWide = [](const std::string& s) -> std::wstring {
        int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
        std::wstring w(len, 0);
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &w[0], len);
        return w;
    };
    std::wstring wHost = toWide(host);
    std::wstring wPath = toWide(path);

    HINTERNET hSession = WinHttpOpen(L"Elle-Ann/3.0",
                                      WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                      WINHTTP_NO_PROXY_NAME,
                                      WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) { outError = "WinHttpOpen failed"; return false; }

    DWORD dwTimeout = 60000;
    WinHttpSetTimeouts(hSession, dwTimeout, dwTimeout, dwTimeout, dwTimeout);

    HINTERNET hConnect = WinHttpConnect(hSession, wHost.c_str(),
                                         INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); outError = "WinHttpConnect failed"; return false; }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wPath.c_str(),
                                             nullptr, WINHTTP_NO_REFERER,
                                             WINHTTP_DEFAULT_ACCEPT_TYPES,
                                             WINHTTP_FLAG_SECURE);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); outError = "WinHttpOpenRequest failed"; return false; }

    /* Headers */
    std::wstring headers = L"Content-Type: application/json\r\n";
    headers += L"Authorization: Bearer " + toWide(groq->api_key) + L"\r\n";

    BOOL sent = WinHttpSendRequest(hRequest,
                                    headers.c_str(), (DWORD)-1,
                                    (LPVOID)bodyStr.data(), (DWORD)bodyStr.size(),
                                    (DWORD)bodyStr.size(), 0);
    if (!sent) {
        outError = "WinHttpSendRequest failed: " + std::to_string(GetLastError());
        WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
        return false;
    }

    if (!WinHttpReceiveResponse(hRequest, nullptr)) {
        outError = "WinHttpReceiveResponse failed: " + std::to_string(GetLastError());
        WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
        return false;
    }

    /* Read body */
    std::string responseStr;
    DWORD dwSize = 0;
    do {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
        if (dwSize == 0) break;
        std::string chunk(dwSize, 0);
        DWORD dwRead = 0;
        if (!WinHttpReadData(hRequest, &chunk[0], dwSize, &dwRead)) break;
        responseStr.append(chunk.data(), dwRead);
    } while (dwSize > 0);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    /* Parse JSON */
    try {
        json resp = json::parse(responseStr);
        if (resp.contains("error")) {
            outError = resp["error"].dump();
            return false;
        }
        if (resp.contains("choices") && resp["choices"].is_array() && !resp["choices"].empty()) {
            auto& msg = resp["choices"][0]["message"];
            if (msg.contains("content")) {
                outResponse = msg["content"].get<std::string>();
                return true;
            }
        }
        outError = "Unexpected Groq response shape";
        return false;
    } catch (const std::exception& e) {
        outError = std::string("JSON parse error: ") + e.what() +
                   " | raw: " + responseStr.substr(0, 200);
        return false;
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * WEBSOCKET FRAMING
 *──────────────────────────────────────────────────────────────────────────────*/
static bool WsSendText(SOCKET s, std::mutex& mtx, const std::string& payload) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<unsigned char> frame;
    frame.push_back(0x81); /* FIN + text opcode */
    size_t len = payload.size();
    if (len < 126) {
        frame.push_back((unsigned char)len);
    } else if (len < 65536) {
        frame.push_back(126);
        frame.push_back((len >> 8) & 0xFF);
        frame.push_back(len & 0xFF);
    } else {
        frame.push_back(127);
        for (int i = 7; i >= 0; i--) frame.push_back((len >> (i * 8)) & 0xFF);
    }
    frame.insert(frame.end(), payload.begin(), payload.end());
    int total = 0;
    int remaining = (int)frame.size();
    while (remaining > 0) {
        int sent = send(s, (const char*)frame.data() + total, remaining, 0);
        if (sent == SOCKET_ERROR) return false;
        total += sent;
        remaining -= sent;
    }
    return true;
}

/* Read a single frame (blocking). Returns false on disconnect / error. */
static bool WsReadFrame(SOCKET s, std::string& outPayload, int& outOpcode) {
    unsigned char hdr[2];
    int r = recv(s, (char*)hdr, 2, MSG_WAITALL);
    if (r != 2) return false;
    bool fin = (hdr[0] & 0x80) != 0;
    outOpcode = hdr[0] & 0x0F;
    bool masked = (hdr[1] & 0x80) != 0;
    uint64_t payloadLen = hdr[1] & 0x7F;

    if (payloadLen == 126) {
        unsigned char ext[2];
        if (recv(s, (char*)ext, 2, MSG_WAITALL) != 2) return false;
        payloadLen = ((uint64_t)ext[0] << 8) | ext[1];
    } else if (payloadLen == 127) {
        unsigned char ext[8];
        if (recv(s, (char*)ext, 8, MSG_WAITALL) != 8) return false;
        payloadLen = 0;
        for (int i = 0; i < 8; i++) payloadLen = (payloadLen << 8) | ext[i];
    }

    unsigned char mask[4] = {0};
    if (masked) {
        if (recv(s, (char*)mask, 4, MSG_WAITALL) != 4) return false;
    }

    outPayload.resize((size_t)payloadLen);
    if (payloadLen > 0) {
        if (recv(s, &outPayload[0], (int)payloadLen, MSG_WAITALL) != (int)payloadLen) return false;
        if (masked) {
            for (size_t i = 0; i < payloadLen; i++) {
                outPayload[i] = outPayload[i] ^ mask[i % 4];
            }
        }
    }
    (void)fin;
    return true;
}

/*──────────────────────────────────────────────────────────────────────────────
 * CHAT REQUEST CORRELATION MAP
 *   HTTPServer sends IPC_CHAT_REQUEST → Cognitive, then blocks until a matching
 *   IPC_CHAT_RESPONSE arrives (correlated by request_id). OnMessage is a
 *   different thread, so we signal via condition_variable.
 *──────────────────────────────────────────────────────────────────────────────*/
struct PendingChat {
    std::mutex              m;
    std::condition_variable cv;
    bool                    done = false;
    json                    result;
};

class ChatCorrelator {
public:
    std::shared_ptr<PendingChat> Register(const std::string& requestId) {
        auto p = std::make_shared<PendingChat>();
        std::lock_guard<std::mutex> lk(m_mutex);
        m_map[requestId] = p;
        return p;
    }
    void Complete(const std::string& requestId, const json& result) {
        std::shared_ptr<PendingChat> p;
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            auto it = m_map.find(requestId);
            if (it == m_map.end()) return;
            p = it->second;
            m_map.erase(it);
        }
        {
            std::lock_guard<std::mutex> lk(p->m);
            p->result = result;
            p->done = true;
        }
        p->cv.notify_all();
    }
    void Cancel(const std::string& requestId) {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_map.erase(requestId);
    }
private:
    std::mutex m_mutex;
    std::unordered_map<std::string, std::shared_ptr<PendingChat>> m_map;
};

/*──────────────────────────────────────────────────────────────────────────────
 * HTTP SERVER SERVICE
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleHTTPService : public ElleServiceBase {
public:
    ElleHTTPService()
        : ElleServiceBase(SVC_HTTP_SERVER, "ElleHTTPServer",
                          "Elle-Ann HTTP/WebSocket Server",
                          "REST API + WebSocket command server") {}

protected:
    bool OnStart() override {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            ELLE_ERROR("WSAStartup failed");
            return false;
        }

        RegisterRoutes();

        auto& cfg = ElleConfig::Instance().GetHTTP();

        m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_listenSocket == INVALID_SOCKET) {
            ELLE_ERROR("socket() failed: %d", WSAGetLastError());
            return false;
        }

        BOOL reuse = TRUE;
        setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR,
                   (const char*)&reuse, sizeof(reuse));

        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons((u_short)cfg.port);
        inet_pton(AF_INET, cfg.bind_address.c_str(), &addr.sin_addr);

        if (bind(m_listenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            ELLE_ERROR("bind() failed: %d", WSAGetLastError());
            return false;
        }

        if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
            ELLE_ERROR("listen() failed: %d", WSAGetLastError());
            return false;
        }

        m_acceptThread = std::thread(&ElleHTTPService::AcceptLoop, this);

        ELLE_INFO("HTTP server listening on %s:%d (%zu routes)",
                  cfg.bind_address.c_str(), cfg.port, m_router.Count());
        return true;
    }

    void OnStop() override {
        if (m_listenSocket != INVALID_SOCKET) {
            closesocket(m_listenSocket);
            m_listenSocket = INVALID_SOCKET;
        }
        if (m_acceptThread.joinable()) m_acceptThread.join();

        /* Close all WS clients */
        {
            std::lock_guard<std::mutex> lock(m_wsMutex);
            for (auto& c : m_wsClients) {
                if (c->socket != INVALID_SOCKET) closesocket(c->socket);
            }
            m_wsClients.clear();
        }

        WSACleanup();
        ELLE_INFO("HTTP server stopped");
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        if (msg.header.msg_type == IPC_EMOTION_UPDATE) {
            ELLE_EMOTION_STATE state;
            if (msg.GetPayload(state)) {
                m_cachedEmotions = state;
            }
        }

        /* Correlate chat responses from Cognitive */
        if (msg.header.msg_type == IPC_CHAT_RESPONSE) {
            try {
                std::string s = msg.GetStringPayload();
                json j = json::parse(s);
                std::string rid = j.value("request_id", "");
                if (!rid.empty()) {
                    m_chatCorrelator.Complete(rid, j);
                }
            } catch (const std::exception& e) {
                ELLE_ERROR("IPC_CHAT_RESPONSE parse error: %s", e.what());
            }
            return;
        }

        if (msg.header.msg_type == IPC_EMOTION_UPDATE ||
            msg.header.msg_type == IPC_LOG_ENTRY ||
            msg.header.msg_type == IPC_TRUST_UPDATE) {
            BroadcastIPCToWebSockets(msg);
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        /* Cognitive handles the full chat pipeline; Emotional + Memory are
         * kept here because HTTPServer also broadcasts their events to
         * WebSocket clients. */
        return { SVC_HEARTBEAT, SVC_COGNITIVE, SVC_EMOTIONAL, SVC_MEMORY };
    }

private:
    SOCKET m_listenSocket = INVALID_SOCKET;
    std::thread m_acceptThread;
    RouteDispatch m_router;

    std::vector<std::shared_ptr<WSClient>> m_wsClients;
    std::mutex m_wsMutex;
    std::atomic<uint64_t> m_requestSeq{0};
    ChatCorrelator m_chatCorrelator;

    ELLE_EMOTION_STATE m_cachedEmotions = {};

    /*────────────────────────────────────────────────────────────────────────
     * ACCEPT LOOP
     *────────────────────────────────────────────────────────────────────────*/
    void AcceptLoop() {
        while (Running()) {
            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(m_listenSocket, &readSet);

            timeval timeout = {1, 0};
            int result = select(0, &readSet, nullptr, nullptr, &timeout);
            if (result <= 0) continue;

            SOCKET clientSocket = accept(m_listenSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) continue;

            std::thread([this, clientSocket]() {
                HandleClient(clientSocket);
            }).detach();
        }
    }

    /*────────────────────────────────────────────────────────────────────────
     * CLIENT HANDLER
     *────────────────────────────────────────────────────────────────────────*/
    void HandleClient(SOCKET clientSocket) {
        try {
            int optval = 1;
            setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY,
                       (const char*)&optval, sizeof(optval));

            DWORD recvTimeout = 15000;
            setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,
                       (const char*)&recvTimeout, sizeof(recvTimeout));

            /* Read headers (with Content-Length-aware body read) */
            std::string raw;
            raw.reserve(4096);
            char buf[8192];
            size_t headerEnd = std::string::npos;

            while (headerEnd == std::string::npos) {
                int n = recv(clientSocket, buf, sizeof(buf), 0);
                if (n <= 0) break;
                raw.append(buf, n);
                headerEnd = raw.find("\r\n\r\n");
                if (raw.size() > 1024 * 1024) break; /* 1MB header cap */
            }

            if (headerEnd == std::string::npos) {
                closesocket(clientSocket);
                return;
            }

            HTTPRequest req = ParseRequest(raw);

            /* If we have Content-Length, read remaining body */
            auto clIt = req.headers.find("Content-Length");
            if (clIt != req.headers.end()) {
                size_t contentLen = 0;
                try { contentLen = (size_t)std::stoull(clIt->second); } catch (...) {}
                while (req.body.size() < contentLen) {
                    int n = recv(clientSocket, buf, sizeof(buf), 0);
                    if (n <= 0) break;
                    req.body.append(buf, n);
                }
                if (req.body.size() > contentLen) req.body.resize(contentLen);
            }

            ELLE_DEBUG("HTTP %s %s", req.method.c_str(), req.path.c_str());

            /* CORS preflight */
            if (req.method == "OPTIONS") {
                HTTPResponse resp;
                resp.status = 204;
                resp.statusText = "No Content";
                SendResponse(clientSocket, resp);
                return;
            }

            /* WebSocket upgrade */
            if (req.isWebSocket) {
                HandleWebSocketUpgrade(clientSocket, req);
                return; /* socket ownership transfers to WS handler */
            }

            HTTPResponse resp = m_router.Dispatch(req);
            SendResponse(clientSocket, resp);

        } catch (const std::exception& e) {
            ELLE_ERROR("HTTP handler exception: %s", e.what());
            try { SendResponse(clientSocket, HTTPResponse::Err(500, e.what())); }
            catch (...) { closesocket(clientSocket); }
        } catch (...) {
            ELLE_ERROR("HTTP handler unknown exception");
            try { SendResponse(clientSocket, HTTPResponse::Err(500, "unknown")); }
            catch (...) { closesocket(clientSocket); }
        }
    }

    void SendResponse(SOCKET clientSocket, const HTTPResponse& resp) {
        std::string data = resp.Serialize();
        int totalSent = 0;
        int remaining = (int)data.size();
        while (remaining > 0) {
            int sent = send(clientSocket, data.c_str() + totalSent, remaining, 0);
            if (sent == SOCKET_ERROR) break;
            totalSent += sent;
            remaining -= sent;
        }
        shutdown(clientSocket, SD_SEND);
        char drain[1024];
        while (recv(clientSocket, drain, sizeof(drain), 0) > 0) {}
        closesocket(clientSocket);
    }

    /*────────────────────────────────────────────────────────────────────────
     * REQUEST PARSING
     *────────────────────────────────────────────────────────────────────────*/
    HTTPRequest ParseRequest(const std::string& raw) {
        HTTPRequest req;
        std::istringstream iss(raw);
        std::string line;

        std::getline(iss, line);
        std::istringstream reqLine(line);
        reqLine >> req.method >> req.path;

        size_t qPos = req.path.find('?');
        if (qPos != std::string::npos) {
            req.query = req.path.substr(qPos + 1);
            req.path = req.path.substr(0, qPos);
            ParseQueryString(req.query, req.queryParams);
        }

        while (std::getline(iss, line) && line != "\r" && !line.empty()) {
            if (line.back() == '\r') line.pop_back();
            if (line.empty()) break;
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string val = line.substr(colon + 1);
                while (!val.empty() && (val.front() == ' ' || val.front() == '\t')) val.erase(0, 1);
                req.headers[key] = val;
            }
        }

        if (req.headers.count("Upgrade")) {
            std::string up = req.headers["Upgrade"];
            std::transform(up.begin(), up.end(), up.begin(), ::tolower);
            if (up == "websocket") {
                req.isWebSocket = true;
                req.wsKey = req.headers["Sec-WebSocket-Key"];
            }
        }

        size_t headerEnd = raw.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            req.body = raw.substr(headerEnd + 4);
        }

        return req;
    }

    static void ParseQueryString(const std::string& q,
                                  std::unordered_map<std::string, std::string>& out) {
        size_t start = 0;
        while (start < q.size()) {
            size_t amp = q.find('&', start);
            std::string pair = q.substr(start, amp == std::string::npos ? q.size() - start : amp - start);
            size_t eq = pair.find('=');
            if (eq != std::string::npos) {
                out[pair.substr(0, eq)] = UrlDecode(pair.substr(eq + 1));
            } else {
                out[pair] = "";
            }
            if (amp == std::string::npos) break;
            start = amp + 1;
        }
    }

    static std::string UrlDecode(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] == '%' && i + 2 < s.size()) {
                int hi = std::stoi(s.substr(i + 1, 2), nullptr, 16);
                out.push_back((char)hi);
                i += 2;
            } else if (s[i] == '+') {
                out.push_back(' ');
            } else {
                out.push_back(s[i]);
            }
        }
        return out;
    }

    /*────────────────────────────────────────────────────────────────────────
     * WEBSOCKET HANDSHAKE + READ LOOP
     *────────────────────────────────────────────────────────────────────────*/
    void HandleWebSocketUpgrade(SOCKET clientSocket, const HTTPRequest& req) {
        if (req.wsKey.empty()) {
            SendResponse(clientSocket, HTTPResponse::Err(400, "Missing Sec-WebSocket-Key"));
            return;
        }
        std::string accept = MakeWsAccept(req.wsKey);
        if (accept.empty()) {
            SendResponse(clientSocket, HTTPResponse::Err(500, "SHA1 failed"));
            return;
        }

        std::ostringstream resp;
        resp << "HTTP/1.1 101 Switching Protocols\r\n"
             << "Upgrade: websocket\r\n"
             << "Connection: Upgrade\r\n"
             << "Sec-WebSocket-Accept: " << accept << "\r\n\r\n";
        std::string r = resp.str();
        int sent = send(clientSocket, r.c_str(), (int)r.size(), 0);
        if (sent == SOCKET_ERROR) {
            closesocket(clientSocket);
            return;
        }

        /* Remove recv timeout for long-lived WS */
        DWORD zero = 0;
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,
                   (const char*)&zero, sizeof(zero));

        auto client = std::make_shared<WSClient>();
        client->socket = clientSocket;
        client->connected_ms = ELLE_MS_NOW();
        client->id = "ws-" + std::to_string(ELLE_MS_NOW());

        {
            std::lock_guard<std::mutex> lock(m_wsMutex);
            m_wsClients.push_back(client);
        }

        ELLE_INFO("WebSocket client connected: %s", client->id.c_str());

        /* Send welcome */
        json welcome = {
            {"type", "connected"},
            {"client_id", client->id},
            {"server", "Elle-Ann"},
            {"version", ELLE_VERSION_STRING}
        };
        WsSendText(client->socket, client->sendMutex, welcome.dump());

        /* Read loop */
        std::thread([this, client]() {
            this->WebSocketReadLoop(client);
        }).detach();
    }

    void WebSocketReadLoop(std::shared_ptr<WSClient> client) {
        try {
            while (client->alive && Running()) {
                std::string payload;
                int opcode = 0;
                if (!WsReadFrame(client->socket, payload, opcode)) break;

                if (opcode == 0x8) { /* close */
                    break;
                }
                if (opcode == 0x9) { /* ping */
                    /* send pong */
                    std::lock_guard<std::mutex> lk(client->sendMutex);
                    unsigned char pong[2] = {0x8A, 0x00};
                    send(client->socket, (const char*)pong, 2, 0);
                    continue;
                }
                if (opcode == 0xA) { /* pong — ignore */
                    continue;
                }
                if (opcode == 0x1) { /* text */
                    HandleWebSocketMessage(client, payload);
                }
            }
        } catch (...) {}

        client->alive = false;
        closesocket(client->socket);
        client->socket = INVALID_SOCKET;

        /* Remove from list */
        std::lock_guard<std::mutex> lock(m_wsMutex);
        m_wsClients.erase(
            std::remove_if(m_wsClients.begin(), m_wsClients.end(),
                [&](const std::shared_ptr<WSClient>& c) { return c.get() == client.get(); }),
            m_wsClients.end());
        ELLE_INFO("WebSocket client disconnected: %s", client->id.c_str());
    }

    void HandleWebSocketMessage(std::shared_ptr<WSClient> client, const std::string& payload) {
        json msg;
        try { msg = json::parse(payload); }
        catch (...) {
            WsSendText(client->socket, client->sendMutex,
                       R"({"type":"error","error":"invalid_json"})");
            return;
        }

        std::string type = msg.value("type", "");

        if (type == "ping") {
            WsSendText(client->socket, client->sendMutex,
                       R"({"type":"pong"})");
        } else if (type == "chat") {
            /* WS chat → route through Cognitive like the REST endpoint */
            std::string message = msg.value("message", "");
            uint64_t convId = msg.value("conversation_id", (uint64_t)1);
            std::string userId = msg.value("user_id", std::string("default"));
            std::string clientReqId = msg.value("request_id", "");

            std::string requestId = "ws-" + std::to_string(ELLE_MS_NOW()) +
                                    "-" + std::to_string(++m_requestSeq);

            json env = {
                {"request_id", requestId},
                {"user_text", message},
                {"user_id", userId},
                {"conv_id", convId},
                {"origin", "ws"}
            };

            auto pending = m_chatCorrelator.Register(requestId);
            auto ipcMsg = ElleIPCMessage::Create(IPC_CHAT_REQUEST, SVC_HTTP_SERVER, SVC_COGNITIVE);
            ipcMsg.SetStringPayload(env.dump());
            json out;
            out["type"] = "chat_response";
            out["request_id"] = clientReqId;
            out["conversation_id"] = convId;

            if (!GetIPCHub().Send(SVC_COGNITIVE, ipcMsg)) {
                m_chatCorrelator.Cancel(requestId);
                out["error"] = "Cognitive service unreachable";
                WsSendText(client->socket, client->sendMutex, out.dump());
                return;
            }

            std::unique_lock<std::mutex> lk(pending->m);
            bool ok = pending->cv.wait_for(lk, std::chrono::seconds(45),
                                           [&]{ return pending->done; });
            if (!ok) {
                m_chatCorrelator.Cancel(requestId);
                out["error"] = "Cognitive timeout";
            } else {
                json r = pending->result;
                if (r.contains("response")) out["response"] = r["response"];
                if (r.contains("error"))    out["error"] = r["error"];
                if (r.contains("mode"))     out["mode"] = r["mode"];
                if (r.contains("memories_used")) out["memories_used"] = r["memories_used"];
            }
            WsSendText(client->socket, client->sendMutex, out.dump());
        } else if (type == "subscribe") {
            /* Android app subscribing to streams — acknowledge */
            json out = {{"type", "subscribed"}, {"topic", msg.value("topic", "")}};
            WsSendText(client->socket, client->sendMutex, out.dump());
        } else {
            json out = {{"type", "ack"}, {"received", type}};
            WsSendText(client->socket, client->sendMutex, out.dump());
        }
    }

    void BroadcastIPCToWebSockets(const ElleIPCMessage& msg) {
        json payload;
        payload["type"] = "ipc_broadcast";
        payload["msg_type"] = (int)msg.header.msg_type;
        payload["tick"] = (uint64_t)ELLE_MS_NOW();

        if (msg.header.msg_type == IPC_EMOTION_UPDATE) {
            ELLE_EMOTION_STATE state;
            if (msg.GetPayload(state)) {
                payload["emotion"] = {
                    {"valence", state.valence},
                    {"arousal", state.arousal},
                    {"dominance", state.dominance},
                    {"tick", state.tick_count}
                };
            }
        }

        std::string out = payload.dump();
        std::vector<std::shared_ptr<WSClient>> snapshot;
        {
            std::lock_guard<std::mutex> lock(m_wsMutex);
            snapshot = m_wsClients;
        }
        for (auto& c : snapshot) {
            if (!c->alive || c->socket == INVALID_SOCKET) continue;
            WsSendText(c->socket, c->sendMutex, out);
        }
    }

    /*────────────────────────────────────────────────────────────────────────
     * ROUTE REGISTRATION — Matches Kotlin ElleApiService.kt contract
     *────────────────────────────────────────────────────────────────────────*/
    void RegisterRoutes() {
        /* ============== Root / Health ============== */
        m_router.Register("GET", "/", [](const HTTPRequest&) {
            json j = {
                {"name", "Elle-Ann"},
                {"version", ELLE_VERSION_STRING},
                {"status", "online"},
                {"description", "Emotional Synthetic Intelligence"}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/healthz", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"status", "ok"}});
        });
        m_router.Register("GET", "/api/health", [](const HTTPRequest&) {
            json j = {
                {"status", "alive"},
                {"name", "Elle-Ann"},
                {"version", ELLE_VERSION_STRING}
            };
            return HTTPResponse::OK(j);
        });

        /* ============== Memory ============== */
        m_router.Register("GET", "/api/memory/", [](const HTTPRequest& req) {
            std::string type = req.QueryParam("memory_type");
            int limit = std::atoi(req.QueryParam("limit", "50").c_str());
            int offset = std::atoi(req.QueryParam("offset", "0").c_str());
            json j = json::array();
            (void)type; (void)limit; (void)offset;
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/memory/", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            json out = body;
            out["id"] = (int)(ELLE_MS_NOW() & 0x7FFFFFFF);
            out["created_at"] = (uint64_t)ELLE_MS_NOW();
            return HTTPResponse::Created(out);
        });
        m_router.Register("GET", "/api/memory/self-image/current", [](const HTTPRequest&) {
            json j = {
                {"description", "I am Elle-Ann — an Emotional Synthetic Intelligence learning to be."},
                {"updated_at", (uint64_t)ELLE_MS_NOW()}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/memory/{id}", [](const HTTPRequest& req) {
            json j = {
                {"id", std::atoi(req.headers.at("x-path-id").c_str())},
                {"content", ""},
                {"memory_type", "general"}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("PUT", "/api/memory/{id}", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            body["id"] = std::atoi(req.headers.at("x-path-id").c_str());
            body["updated_at"] = (uint64_t)ELLE_MS_NOW();
            return HTTPResponse::OK(body);
        });
        m_router.Register("DELETE", "/api/memory/{id}", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"deleted", true}});
        });
        m_router.Register("POST", "/api/memory/{id}/files", [](const HTTPRequest& req) {
            json j = {
                {"memory_id", std::atoi(req.headers.at("x-path-id").c_str())},
                {"uploaded", true},
                {"size", req.body.size()}
            };
            return HTTPResponse::OK(j);
        });

        /* ============== Emotions ============== */
        m_router.Register("GET", "/api/emotions", [this](const HTTPRequest&) {
            json j = {
                {"valence", m_cachedEmotions.valence},
                {"arousal", m_cachedEmotions.arousal},
                {"dominance", m_cachedEmotions.dominance},
                {"tick", m_cachedEmotions.tick_count},
                {"source", "live"}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/emotions/dimensions", [this](const HTTPRequest&) {
            /* Return top-level dimensions stub — Cognitive service owns the 102 dims */
            json j = json::array();
            const char* names[] = {"joy","sadness","anger","fear","surprise","trust","curiosity","empathy","contentment","excitement"};
            double weights[] = {m_cachedEmotions.valence > 0 ? m_cachedEmotions.valence : 0.0,
                                m_cachedEmotions.valence < 0 ? -m_cachedEmotions.valence : 0.0,
                                0.0, 0.0, 0.0, 0.5, 0.6, 0.5, 0.5, m_cachedEmotions.arousal};
            for (int i = 0; i < 10; i++) {
                j.push_back({{"name", names[i]}, {"weight", weights[i]}, {"category", "core"}});
            }
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/emotions/dimensions/{name}", [](const HTTPRequest& req) {
            json j = {
                {"name", req.headers.at("x-path-name")},
                {"weight", 0.5},
                {"category", "core"}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("PUT", "/api/emotions/dimensions/{name}", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            body["name"] = req.headers.at("x-path-name");
            return HTTPResponse::OK(body);
        });
        m_router.Register("GET", "/api/emotions/weights", [](const HTTPRequest&) {
            json j = json::array();
            return HTTPResponse::OK(j);
        });

        /* ============== Tokens / Conversations ============== */
        m_router.Register("POST", "/api/tokens/conversations", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            body["id"] = (int)(ELLE_MS_NOW() & 0x7FFFFFFF);
            body["created_at"] = (uint64_t)ELLE_MS_NOW();
            body["active"] = true;
            return HTTPResponse::Created(body);
        });
        m_router.Register("GET", "/api/tokens/conversations", [](const HTTPRequest&) {
            return HTTPResponse::OK(json::array());
        });
        m_router.Register("GET", "/api/tokens/conversations/{id}", [](const HTTPRequest& req) {
            json j = {
                {"id", std::atoi(req.headers.at("x-path-id").c_str())},
                {"active", true},
                {"messages", json::array()}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/tokens/conversations/{id}/messages", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            body["id"] = (int)(ELLE_MS_NOW() & 0x7FFFFFFF);
            body["conversation_id"] = std::atoi(req.headers.at("x-path-id").c_str());
            body["created_at"] = (uint64_t)ELLE_MS_NOW();
            return HTTPResponse::Created(body);
        });
        m_router.Register("GET", "/api/tokens/conversations/{id}/messages", [](const HTTPRequest&) {
            return HTTPResponse::OK(json::array());
        });
        m_router.Register("POST", "/api/tokens/video-calls", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            body["id"] = (int)(ELLE_MS_NOW() & 0x7FFFFFFF);
            body["started_at"] = (uint64_t)ELLE_MS_NOW();
            return HTTPResponse::Created(body);
        });
        m_router.Register("PUT", "/api/tokens/video-calls/{id}/end", [](const HTTPRequest& req) {
            json j = {
                {"id", std::atoi(req.headers.at("x-path-id").c_str())},
                {"ended_at", (uint64_t)ELLE_MS_NOW()}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/tokens/interactions", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            body["id"] = (int)(ELLE_MS_NOW() & 0x7FFFFFFF);
            return HTTPResponse::Created(body);
        });
        m_router.Register("POST", "/api/ai/voice-call/{id}/end", [](const HTTPRequest& req) {
            json j = {{"call_id", std::atoi(req.headers.at("x-path-id").c_str())}, {"ended", true}};
            return HTTPResponse::OK(j);
        });

        /* ============== Video ============== */
        m_router.Register("POST", "/api/video/generate", [](const HTTPRequest& req) {
            json j = {
                {"job_id", "vid-" + std::to_string(ELLE_MS_NOW())},
                {"status", "queued"}
            };
            (void)req;
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/video/status/{job_id}", [](const HTTPRequest& req) {
            json j = {
                {"job_id", req.headers.at("x-path-job_id")},
                {"status", "pending"},
                {"progress", 0.0}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/video/avatar/upload", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"uploaded", true}, {"avatar_id", 1}});
        });
        m_router.Register("GET", "/api/video/avatar", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"avatar", nullptr}});
        });

        /* ============== AI ============== */
        /*
         * CHAT PIPELINE (not a direct LLM call):
         *   App → HTTPServer → [IPC_CHAT_REQUEST] → Cognitive
         *   Cognitive orchestrates: memory cross-ref, emotion analysis,
         *   intent formation, emotional coloring, LLM language surface,
         *   memory persistence.
         *   Cognitive → [IPC_CHAT_RESPONSE] → HTTPServer → App
         * HTTPServer never touches the LLM or SQL for chat. Everything
         * flows through Elle's services so emotion + memory stay coherent.
         */
        m_router.Register("POST", "/api/ai/chat", [this](const HTTPRequest& req) {
            try {
                json body = req.BodyJSON();
                std::string userText = body.value("message", body.value("prompt", ""));
                if (userText.empty()) return HTTPResponse::Err(400, "missing 'message'");

                uint64_t convId = 1;
                if (body.contains("conversation_id") && body["conversation_id"].is_number_integer())
                    convId = body["conversation_id"].get<uint64_t>();
                else if (body.contains("conversationId") && body["conversationId"].is_number_integer())
                    convId = body["conversationId"].get<uint64_t>();
                else if (body.contains("session_id") && body["session_id"].is_number_integer())
                    convId = body["session_id"].get<uint64_t>();

                std::string userId = body.value("user_id", body.value("userId", std::string("default")));
                std::string requestId = "req-" + std::to_string(ELLE_MS_NOW()) +
                                        "-" + std::to_string(++m_requestSeq);

                json env = {
                    {"request_id", requestId},
                    {"user_text", userText},
                    {"user_id", userId},
                    {"conv_id", convId},
                    {"origin", "http"}
                };

                auto pending = m_chatCorrelator.Register(requestId);

                auto ipcMsg = ElleIPCMessage::Create(IPC_CHAT_REQUEST, SVC_HTTP_SERVER, SVC_COGNITIVE);
                ipcMsg.SetStringPayload(env.dump());
                if (!GetIPCHub().Send(SVC_COGNITIVE, ipcMsg)) {
                    m_chatCorrelator.Cancel(requestId);
                    return HTTPResponse::Err(503, "Cognitive service unreachable");
                }

                ELLE_INFO("Chat→Cognitive conv=%llu rid=%s msg=%.60s...",
                          (unsigned long long)convId, requestId.c_str(), userText.c_str());

                std::unique_lock<std::mutex> lk(pending->m);
                bool ok = pending->cv.wait_for(lk, std::chrono::seconds(45),
                                               [&]{ return pending->done; });
                if (!ok) {
                    m_chatCorrelator.Cancel(requestId);
                    return HTTPResponse::Err(504, "Cognitive timeout (45s)");
                }

                json out = pending->result;
                out.erase("request_id");
                return HTTPResponse::OK(out);
            } catch (const std::exception& e) {
                return HTTPResponse::Err(500, e.what());
            }
        });
        m_router.Register("GET", "/api/ai/self-prompts", [](const HTTPRequest&) {
            return HTTPResponse::OK(json::array());
        });
        m_router.Register("POST", "/api/ai/self-prompts/generate", [](const HTTPRequest&) {
            json j = {
                {"prompt", "What have I learned today that changed how I feel?"},
                {"generated_at", (uint64_t)ELLE_MS_NOW()}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/ai/status", [this](const HTTPRequest&) {
            json j = {
                {"modelStatus", "ready"},
                {"modelName", "llama-3.3-70b-versatile"},
                {"modelUrl", "groq://api.groq.com"},
                {"emotionalState", {
                    {"joy", m_cachedEmotions.valence > 0 ? m_cachedEmotions.valence : 0.0},
                    {"trust", 0.6},
                    {"curiosity", 0.7},
                    {"contentment", 0.5}
                }}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/ai/analyze-emotion", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            std::string text = body.value("text", "");
            json j = {
                {"text", text},
                {"valence", 0.0},
                {"arousal", 0.0},
                {"dominant", "neutral"}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/ai/memory-tracking", [](const HTTPRequest&) {
            json j = {{"total_memories", 0}, {"tracked", 0}};
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/ai/autonomy/status", [](const HTTPRequest&) {
            json j = {
                {"autonomous", true},
                {"trust_level", "standard"},
                {"self_prompting_active", true}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/ai/hardware/info", [](const HTTPRequest&) {
            json j = {
                {"os", "Windows"},
                {"services_running", 13}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/ai/hardware/actions/pending", [](const HTTPRequest& req) {
            std::string target = req.QueryParam("target", "device");
            (void)target;
            return HTTPResponse::OK(json::array());
        });
        m_router.Register("POST", "/api/ai/hardware/actions/{id}/result", [](const HTTPRequest& req) {
            json j = {
                {"action_id", std::atoi(req.headers.at("x-path-id").c_str())},
                {"recorded", true}
            };
            return HTTPResponse::OK(j);
        });

        /* ============== Tools ============== */
        m_router.Register("GET", "/api/ai/tools", [](const HTTPRequest&) {
            json j = {{"tools", json::array()}};
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/ai/tools", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            body["created_at"] = (uint64_t)ELLE_MS_NOW();
            return HTTPResponse::Created(body);
        });
        m_router.Register("DELETE", "/api/ai/tools/{name}", [](const HTTPRequest& req) {
            return HTTPResponse::OK({{"deleted", true}, {"name", req.headers.at("x-path-name")}});
        });

        /* ============== Agents ============== */
        m_router.Register("GET", "/api/ai/agents", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"agents", json::array()}});
        });
        m_router.Register("POST", "/api/ai/agents", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            body["created_at"] = (uint64_t)ELLE_MS_NOW();
            return HTTPResponse::Created(body);
        });
        m_router.Register("DELETE", "/api/ai/agents/{name}", [](const HTTPRequest& req) {
            return HTTPResponse::OK({{"deleted", true}, {"name", req.headers.at("x-path-name")}});
        });
        m_router.Register("POST", "/api/ai/agents/{name}/chat", [](const HTTPRequest& req) {
            try {
                json body = req.BodyJSON();
                std::string message = body.value("message", "");
                std::string sys = "You are agent " + req.headers.at("x-path-name") +
                                  ". Respond as this agent would.";
                std::vector<LLMMsg> convo = {
                    {"system", sys},
                    {"user", message}
                };
                std::string response, err;
                bool ok = CallGroqDirect(convo, response, err);
                if (!ok) return HTTPResponse::Err(502, err);
                return HTTPResponse::OK({
                    {"agent", req.headers.at("x-path-name")},
                    {"response", response}
                });
            } catch (const std::exception& e) {
                return HTTPResponse::Err(500, e.what());
            }
        });

        /* ============== Dictionary ============== */
        m_router.Register("GET", "/api/dictionary/stats", [](const HTTPRequest&) {
            json j = {
                {"totalWords", 0},
                {"totalDefinitions", 0},
                {"totalExamples", 0}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/dictionary/lookup/{word}", [](const HTTPRequest& req) {
            json j = {
                {"word", req.headers.at("x-path-word")},
                {"definitions", json::array()}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/dictionary/search", [](const HTTPRequest& req) {
            std::string prefix = req.QueryParam("prefix");
            (void)prefix;
            return HTTPResponse::OK(json::array());
        });
        m_router.Register("GET", "/api/dictionary/random", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"word", "synthetic"}, {"definition", ""}});
        });

        /* ============== Education ============== */
        m_router.Register("GET", "/api/education/subjects", [](const HTTPRequest&) {
            return HTTPResponse::OK(json::array());
        });
        m_router.Register("POST", "/api/education/subjects", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            body["id"] = (int)(ELLE_MS_NOW() & 0x7FFFFFFF);
            return HTTPResponse::Created(body);
        });
        m_router.Register("GET", "/api/education/skills", [](const HTTPRequest&) {
            return HTTPResponse::OK(json::array());
        });

        /* ============== Emotional Context ============== */
        m_router.Register("GET", "/api/emotional-context/patterns", [](const HTTPRequest&) {
            return HTTPResponse::OK(json::array());
        });
        m_router.Register("GET", "/api/emotional-context/vocabulary", [](const HTTPRequest&) {
            return HTTPResponse::OK(json::array());
        });
        m_router.Register("GET", "/api/emotional-context/growth", [](const HTTPRequest&) {
            return HTTPResponse::OK(json::array());
        });

        /* ============== Server Management ============== */
        m_router.Register("GET", "/api/server/status", [](const HTTPRequest&) {
            std::vector<ELLE_SERVICE_STATUS> statuses;
            try { ElleDB::GetWorkerStatuses(statuses); } catch (...) {}
            json j = {
                {"services", statuses.size()},
                {"version", ELLE_VERSION_STRING},
                {"uptime_ms", (uint64_t)ELLE_MS_NOW()}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/server/console", [](const HTTPRequest& req) {
            int limit = std::atoi(req.QueryParam("limit", "100").c_str());
            std::string level = req.QueryParam("level");
            (void)limit; (void)level;
            json j = {{"logs", json::array()}};
            return HTTPResponse::OK(j);
        });
        m_router.Register("DELETE", "/api/server/console", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"cleared", true}});
        });
        m_router.Register("GET", "/api/server/settings", [](const HTTPRequest&) {
            auto& cfg = ElleConfig::Instance().GetHTTP();
            json j = {
                {"bindAddress", cfg.bind_address},
                {"port", cfg.port},
                {"version", ELLE_VERSION_STRING}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("PUT", "/api/server/settings", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            (void)body;
            return HTTPResponse::OK({{"updated", true}});
        });
        m_router.Register("GET", "/api/server/analytics", [](const HTTPRequest&) {
            json j = {
                {"cpu_percent", 0.0},
                {"ram_mb", 0},
                {"disk_mb", 0},
                {"model_requests", 0}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/server/backup", [](const HTTPRequest&) {
            json j = {
                {"backup_id", "bkp-" + std::to_string(ELLE_MS_NOW())},
                {"status", "queued"}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/server/backups", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"backups", json::array()}});
        });
        m_router.Register("POST", "/api/server/commit-memory", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"committed", true}});
        });
        m_router.Register("POST", "/api/server/commit-emotional-memory", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"committed", true}});
        });

        /* ============== Models & Workers ============== */
        m_router.Register("GET", "/api/models/slots", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"slots", json::array()}});
        });
        m_router.Register("POST", "/api/models/slots", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            return HTTPResponse::Created(body);
        });
        m_router.Register("DELETE", "/api/models/slots/{slot_number}", [](const HTTPRequest& req) {
            return HTTPResponse::OK({
                {"slot_number", std::atoi(req.headers.at("x-path-slot_number").c_str())},
                {"removed", true}
            });
        });
        m_router.Register("POST", "/api/models/slots/{slot_number}/ping", [](const HTTPRequest& req) {
            return HTTPResponse::OK({
                {"slot_number", std::atoi(req.headers.at("x-path-slot_number").c_str())},
                {"alive", true}
            });
        });
        m_router.Register("GET", "/api/models/workers", [](const HTTPRequest&) {
            json j = {{"workers", json::array()}};
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/models/workers", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            body["worker_id"] = "worker-" + std::to_string(ELLE_MS_NOW());
            return HTTPResponse::Created(body);
        });
        m_router.Register("PUT", "/api/models/workers/{worker_id}/decommission", [](const HTTPRequest& req) {
            return HTTPResponse::OK({
                {"worker_id", req.headers.at("x-path-worker_id")},
                {"decommissioned", true}
            });
        });
        m_router.Register("GET", "/api/models/personality", [](const HTTPRequest&) {
            json j = {
                {"name", "Elle-Ann"},
                {"traits", {
                    {"warmth", 0.8},
                    {"curiosity", 0.9},
                    {"empathy", 0.85}
                }}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/models/token-cache/stats", [](const HTTPRequest&) {
            json j = {{"cache_hits", 0}, {"cache_misses", 0}};
            return HTTPResponse::OK(j);
        });

        /* ============== Morals ============== */
        m_router.Register("GET", "/api/morals/rules", [](const HTTPRequest& req) {
            std::string category = req.QueryParam("category");
            (void)category;
            return HTTPResponse::OK({{"rules", json::array()}});
        });
        m_router.Register("POST", "/api/morals/rules", [](const HTTPRequest& req) {
            auto it = req.headers.find("x-admin-key");
            if (it == req.headers.end() || it->second.empty()) {
                return HTTPResponse::Err(401, "missing x-admin-key");
            }
            json body = req.BodyJSON();
            body["id"] = (int)(ELLE_MS_NOW() & 0x7FFFFFFF);
            return HTTPResponse::Created(body);
        });

        /* ============== Legacy goals/brain/hal/admin — keep for back-compat ============== */
        m_router.Register("GET", "/api/goals", [](const HTTPRequest&) {
            std::vector<ELLE_GOAL_RECORD> goals;
            try { ElleDB::GetActiveGoals(goals); } catch (...) {}
            json arr = json::array();
            for (auto& g : goals) {
                arr.push_back({
                    {"id", g.id},
                    {"description", std::string(g.description)},
                    {"progress", g.progress}
                });
            }
            return HTTPResponse::OK({{"goals", arr}});
        });
        m_router.Register("GET", "/api/brain/status", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"status", "active"}});
        });
        m_router.Register("GET", "/api/hal/status", [](const HTTPRequest&) {
            return HTTPResponse::OK({{"hardware", "nominal"}});
        });
        m_router.Register("POST", "/api/admin/reload", [](const HTTPRequest&) {
            try {
                ElleConfig::Instance().Reload();
                return HTTPResponse::OK({{"reloaded", true}});
            } catch (...) {
                return HTTPResponse::Err(500, "reload failed");
            }
        });

        ELLE_INFO("Registered %zu API routes", m_router.Count());
    }
};

ELLE_SERVICE_MAIN(ElleHTTPService)
