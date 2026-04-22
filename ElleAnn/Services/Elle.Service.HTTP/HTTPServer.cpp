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
#include "../../Shared/DictionaryLoader.h"
#include "../../Shared/json.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include <windows.h>
#include <winhttp.h>
#include <bcrypt.h>
#include <psapi.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "psapi.lib")

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
#include <fstream>
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
    static HTTPResponse Accepted(const json& j) { return JSON(202, j.dump()); }
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
            msg.header.msg_type == IPC_TRUST_UPDATE ||
            msg.header.msg_type == IPC_WORLD_EVENT) {
            /* IPC_WORLD_STATE carries binary ELLE_WORLD_ENTITY structs for
             * WorldModel; JSON string "world events" now travel on the
             * dedicated IPC_WORLD_EVENT channel so WS fan-out never misreads
             * a binary entity as JSON (and vice versa).                      */
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
        else if (msg.header.msg_type == IPC_WORLD_EVENT) {
            /* Services publish JSON strings describing real-world events
             * (e.g. ActionExecutor hardware commands, XChromosome phase
             * transitions, file-watcher changes). Parse and re-emit as a
             * typed "world_event" frame so Android clients can dispatch
             * on `data.event == "hardware"` without waiting on the 5s poll. */
            payload["type"] = "world_event";
            try {
                std::string s = msg.GetStringPayload();
                if (!s.empty()) payload["data"] = json::parse(s);
            } catch (const std::exception&) {
                payload["data"] = { {"raw", msg.GetStringPayload()} };
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

        /* ============== Diagnostics — live queue depth + orphan counts ==
         * One-shot observability for the intent / action / hardware_actions
         * queues. Useful for:
         *   - Catching worker stalls ("why are pending counts growing?")
         *   - Spotting orphaned PROCESSING/LOCKED rows before the reaper
         *     has caught them (stale_processing > 0 for more than one tick)
         *   - Sanity-checking WS hardware push liveness
         * Read-only; safe to poll at 1Hz from the Android app.              */
        m_router.Register("GET", "/api/diag/queues", [](const HTTPRequest&) {
            ElleDB::QueueSnapshot s;
            if (!ElleDB::GetQueueSnapshot(s)) {
                return HTTPResponse::Err(500, "queue snapshot failed");
            }
            json j = {
                {"intents", {
                    {"pending",              s.intent_pending},
                    {"processing",           s.intent_processing},
                    {"completed_last_hour",  s.intent_completed_1h},
                    {"failed_last_hour",     s.intent_failed_1h},
                    {"stale_processing",     s.intent_stale_processing}
                }},
                {"actions", {
                    {"queued",                      s.action_queued},
                    {"locked",                      s.action_locked},
                    {"executing",                   s.action_executing},
                    {"completed_success_last_hour", s.action_success_1h},
                    {"completed_failure_last_hour", s.action_failure_1h},
                    {"timed_out_last_hour",         s.action_timeout_1h},
                    {"stale_locked",                s.action_stale_locked}
                }},
                {"hardware_actions", {
                    {"pending",    s.hardware_pending},
                    {"dispatched", s.hardware_dispatched}
                }},
                {"taken_ms", (int64_t)ELLE_MS_NOW()}
            };
            return HTTPResponse::OK(j);
        });

        /* ============== Memory — backed by dbo.memory ============== */
        m_router.Register("GET", "/api/memory/", [](const HTTPRequest& req) {
            std::string type = req.QueryParam("memory_type");
            int limit  = std::max(1, std::atoi(req.QueryParam("limit",  "50").c_str()));
            int offset = std::max(0, std::atoi(req.QueryParam("offset", "0").c_str()));
            int typeI = type.empty() ? -1 : std::atoi(type.c_str());
            std::vector<ElleDB::MemoryRow> rows;
            if (!ElleDB::ListMemories(rows, typeI, (uint32_t)limit, (uint32_t)offset)) {
                return HTTPResponse::Err(500, "SQL ListMemories failed");
            }
            json j = json::array();
            for (auto& r : rows) {
                j.push_back({
                    {"id", r.id}, {"memory_type", r.type}, {"tier", r.tier},
                    {"content", r.content}, {"summary", r.summary},
                    {"emotional_valence", r.emotional_valence},
                    {"importance", r.importance}, {"relevance", r.relevance},
                    {"access_count", r.access_count},
                    {"created_ms", r.created_ms}, {"last_access_ms", r.last_access_ms}
                });
            }
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/memory/", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            ELLE_MEMORY_RECORD mem = {};
            mem.type = body.value("memory_type", 1);
            mem.tier = body.value("tier", 1);
            std::string content = body.value("content", "");
            std::string summary = body.value("summary", content.substr(0, std::min<size_t>(content.size(), 255)));
            strncpy_s(mem.content, content.c_str(), ELLE_MAX_MSG - 1);
            strncpy_s(mem.summary, summary.c_str(), sizeof(mem.summary) - 1);
            mem.emotional_valence = body.value("emotional_valence", 0.0f);
            mem.importance = body.value("importance", 0.5f);
            mem.relevance  = body.value("relevance", 1.0f);
            mem.created_ms = ELLE_MS_NOW();
            mem.last_access_ms = mem.created_ms;
            if (body.contains("tags") && body["tags"].is_array()) {
                uint32_t n = 0;
                for (auto& t : body["tags"]) {
                    if (n >= ELLE_MAX_TAGS) break;
                    std::string s = t.is_string() ? t.get<std::string>() : "";
                    if (s.empty()) continue;
                    strncpy_s(mem.tags[n], s.c_str(), ELLE_MAX_TAG - 1);
                    n++;
                }
                mem.tag_count = n;
            }
            if (!ElleDB::StoreMemory(mem)) return HTTPResponse::Err(500, "StoreMemory failed");
            return HTTPResponse::Created({
                {"stored", true}, {"content", content}, {"tags", mem.tag_count}
            });
        });
        m_router.Register("GET", "/api/memory/self-image/current", [](const HTTPRequest&) {
            /* Real self-image from dbo.InternalNarrative (newest snapshot). */
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT TOP 1 narrative_snapshot, emotional_state, "
                "CONVERT(varchar(33), created_date, 126) "
                "FROM ElleCore.dbo.InternalNarrative ORDER BY narrative_id DESC;");
            if (rs.success && !rs.rows.empty()) {
                auto& row = rs.rows[0];
                return HTTPResponse::OK({
                    {"description", row.values.size() > 0 ? row.values[0] : ""},
                    {"emotional_state", row.GetFloat(1)},
                    {"updated_at", row.values.size() > 2 ? row.values[2] : ""}
                });
            }
            /* No narrative yet — seed with identity core */
            return HTTPResponse::OK({
                {"description", "I am Elle-Ann. I remember, I feel, I keep coming back."},
                {"emotional_state", 0.0}, {"updated_at", ""}
            });
        });
        m_router.Register("GET", "/api/memory/{id}", [](const HTTPRequest& req) {
            int64_t id = std::atoll(req.headers.at("x-path-id").c_str());
            ElleDB::MemoryRow r;
            if (!ElleDB::GetMemory(id, r)) return HTTPResponse::Err(404, "memory not found");
            return HTTPResponse::OK({
                {"id", r.id}, {"memory_type", r.type}, {"tier", r.tier},
                {"content", r.content}, {"summary", r.summary},
                {"emotional_valence", r.emotional_valence},
                {"importance", r.importance}, {"relevance", r.relevance},
                {"access_count", r.access_count},
                {"created_ms", r.created_ms}, {"last_access_ms", r.last_access_ms}
            });
        });
        m_router.Register("PUT", "/api/memory/{id}", [](const HTTPRequest& req) {
            int64_t id = std::atoll(req.headers.at("x-path-id").c_str());
            json body = req.BodyJSON();
            ElleDB::MemoryRow existing;
            if (!ElleDB::GetMemory(id, existing)) return HTTPResponse::Err(404, "memory not found");
            std::string content = body.value("content", existing.content);
            std::string summary = body.value("summary", existing.summary);
            float importance = body.value("importance", existing.importance);
            if (!ElleDB::UpdateMemoryContent(id, content, summary, importance))
                return HTTPResponse::Err(500, "update failed");
            return HTTPResponse::OK({{"id", id}, {"updated", true}});
        });
        m_router.Register("DELETE", "/api/memory/{id}", [](const HTTPRequest& req) {
            int64_t id = std::atoll(req.headers.at("x-path-id").c_str());
            if (!ElleDB::DeleteMemory(id)) return HTTPResponse::Err(500, "delete failed");
            return HTTPResponse::OK({{"id", id}, {"deleted", true}});
        });
        m_router.Register("POST", "/api/memory/{id}/files", [](const HTTPRequest& req) {
            /* Persist attached file to disk under data/memory_files/, record path in summary. */
            int64_t id = std::atoll(req.headers.at("x-path-id").c_str());
            std::string dir = "data\\memory_files";
            CreateDirectoryA("data", nullptr);
            CreateDirectoryA(dir.c_str(), nullptr);
            std::string path = dir + "\\mem-" + std::to_string(id) + "-"
                             + std::to_string(ELLE_MS_NOW()) + ".bin";
            HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr,
                                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (h == INVALID_HANDLE_VALUE) return HTTPResponse::Err(500, "cannot open file");
            DWORD written = 0;
            WriteFile(h, req.body.data(), (DWORD)req.body.size(), &written, nullptr);
            CloseHandle(h);
            return HTTPResponse::OK({
                {"memory_id", id}, {"path", path}, {"size", (uint64_t)written}
            });
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
            /* Full 102-dim emotion state from cached ELLE_EMOTION_STATE */
            json j = json::array();
            for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
                j.push_back({
                    {"id", i},
                    {"name", kEmotionMeta[i].name},
                    {"category", kEmotionMeta[i].category},
                    {"weight", (double)m_cachedEmotions.dimensions[i]}
                });
            }
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/emotions/dimensions/{name}", [this](const HTTPRequest& req) {
            std::string name = req.headers.at("x-path-name");
            std::string needle = name;
            std::transform(needle.begin(), needle.end(), needle.begin(),
                           [](unsigned char c){ return (char)std::tolower(c); });
            for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
                if (needle == kEmotionMeta[i].name) {
                    return HTTPResponse::OK({
                        {"id", i},
                        {"name", kEmotionMeta[i].name},
                        {"category", kEmotionMeta[i].category},
                        {"weight", (double)m_cachedEmotions.dimensions[i]}
                    });
                }
            }
            return HTTPResponse::Err(404, "emotion not found");
        });
        m_router.Register("PUT", "/api/emotions/dimensions/{name}", [](const HTTPRequest& req) {
            /* Setting an emotion dimension directly is a write-through to Emotional
             * service via IPC so it can recompute V/A/D + broadcast. */
            std::string name = req.headers.at("x-path-name");
            json body = req.BodyJSON();
            float weight = body.value("weight", 0.0f);
            int id = -1;
            std::string lc = name;
            std::transform(lc.begin(), lc.end(), lc.begin(),
                           [](unsigned char c){ return (char)std::tolower(c); });
            for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
                if (lc == kEmotionMeta[i].name) { id = i; break; }
            }
            if (id < 0) return HTTPResponse::Err(404, "emotion not found");

            auto msg = ElleIPCMessage::Create(IPC_EMOTION_UPDATE, SVC_HTTP_SERVER, SVC_EMOTIONAL);
            struct { uint32_t emoId; float absolute; } payload;
            payload.emoId = (uint32_t)id;
            payload.absolute = weight;
            msg.payload.resize(sizeof(payload));
            memcpy(msg.payload.data(), &payload, sizeof(payload));
            msg.header.payload_size = sizeof(payload);
            GetIPCHub().Send(SVC_EMOTIONAL, msg);

            return HTTPResponse::OK({{"id", id}, {"name", kEmotionMeta[id].name},
                                      {"weight", weight}, {"dispatched", true}});
        });
        m_router.Register("GET", "/api/emotions/weights", [this](const HTTPRequest&) {
            /* Return just the 102 weights without metadata — compact form */
            json j = json::array();
            for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
                j.push_back((double)m_cachedEmotions.dimensions[i]);
            }
            return HTTPResponse::OK({
                {"weights", j},
                {"count", ELLE_EMOTION_COUNT},
                {"valence",   m_cachedEmotions.valence},
                {"arousal",   m_cachedEmotions.arousal},
                {"dominance", m_cachedEmotions.dominance}
            });
        });

        /* ============== Tokens / Conversations — dbo.conversations + messages ==== */
        m_router.Register("POST", "/api/tokens/conversations", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            int32_t userId = body.value("user_id", 1);
            std::string title = body.value("title", std::string("New conversation"));
            int32_t newId = 0;
            if (!ElleDB::CreateConversation(userId, title, newId))
                return HTTPResponse::Err(500, "CreateConversation failed");
            return HTTPResponse::Created({
                {"id", newId}, {"user_id", userId}, {"title", title}, {"is_active", true}
            });
        });
        m_router.Register("GET", "/api/tokens/conversations", [](const HTTPRequest& req) {
            int limit = std::max(1, std::atoi(req.QueryParam("limit", "50").c_str()));
            std::vector<ElleDB::ConversationRow> rows;
            if (!ElleDB::ListConversations(rows, (uint32_t)limit))
                return HTTPResponse::Err(500, "ListConversations failed");
            json j = json::array();
            for (auto& c : rows) {
                j.push_back({
                    {"id", c.id}, {"user_id", c.user_id}, {"title", c.title},
                    {"started_at", c.started_at}, {"last_message_at", c.last_message_at},
                    {"total_messages", c.total_messages}, {"is_active", c.is_active}
                });
            }
            return HTTPResponse::OK(j);
        });
        m_router.Register("GET", "/api/tokens/conversations/{id}", [](const HTTPRequest& req) {
            int32_t convId = std::atoi(req.headers.at("x-path-id").c_str());
            ElleDB::ConversationRow c;
            if (!ElleDB::GetConversation(convId, c))
                return HTTPResponse::Err(404, "conversation not found");
            return HTTPResponse::OK({
                {"id", c.id}, {"user_id", c.user_id}, {"title", c.title},
                {"started_at", c.started_at}, {"last_message_at", c.last_message_at},
                {"total_messages", c.total_messages}, {"is_active", c.is_active}
            });
        });
        m_router.Register("POST", "/api/tokens/conversations/{id}/messages", [this](const HTTPRequest& req) {
            int32_t convId = std::atoi(req.headers.at("x-path-id").c_str());
            json body = req.BodyJSON();
            std::string content = body.value("content", body.value("message", ""));
            std::string role    = body.value("role", std::string("user"));
            uint32_t roleInt = (role == "user") ? 1 : (role == "assistant" || role == "elle" ? 2 : 0);
            if (content.empty()) return HTTPResponse::Err(400, "missing content");
            if (!ElleDB::StoreMessage((uint64_t)convId, roleInt, content,
                                       m_cachedEmotions, 0.0f))
                return HTTPResponse::Err(500, "StoreMessage failed");
            return HTTPResponse::Created({
                {"conversation_id", convId}, {"role", role}, {"stored", true}
            });
        });
        m_router.Register("GET", "/api/tokens/conversations/{id}/messages", [](const HTTPRequest& req) {
            int32_t convId = std::atoi(req.headers.at("x-path-id").c_str());
            int limit = std::max(1, std::atoi(req.QueryParam("limit", "50").c_str()));
            std::vector<ELLE_CONVERSATION_MSG> msgs;
            if (!ElleDB::GetConversationHistory((uint64_t)convId, msgs, (uint32_t)limit))
                return HTTPResponse::Err(500, "GetConversationHistory failed");
            json j = json::array();
            for (auto& m : msgs) {
                j.push_back({
                    {"conversation_id", m.conversation_id},
                    {"role", m.role == 1 ? "user" : (m.role == 2 ? "assistant" : "system")},
                    {"content", std::string(m.content)},
                    {"timestamp_ms", m.timestamp_ms}
                });
            }
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/tokens/video-calls", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            int32_t userId = body.value("user_id", 1);
            int32_t convId = body.value("conversation_id", 0);
            std::string callId;
            if (!ElleDB::StartVoiceCall(userId, convId, callId))
                return HTTPResponse::Err(500, "StartVoiceCall failed");
            return HTTPResponse::Created({
                {"call_id", callId}, {"user_id", userId}, {"conversation_id", convId},
                {"status", "active"}
            });
        });
        m_router.Register("PUT", "/api/tokens/video-calls/{id}/end", [](const HTTPRequest& req) {
            std::string callId = req.headers.at("x-path-id");
            if (!ElleDB::EndVoiceCall(callId)) return HTTPResponse::Err(500, "end failed");
            return HTTPResponse::OK({{"call_id", callId}, {"status", "ended"}});
        });
        m_router.Register("POST", "/api/tokens/interactions", [this](const HTTPRequest& req) {
            /* Log a generic interaction event as a SelfReflection entry. */
            json body = req.BodyJSON();
            std::string text = body.value("text", body.value("description", std::string("interaction")));
            auto rs = ElleSQLPool::Instance().QueryParams(
                "INSERT INTO ElleCore.dbo.SelfReflections "
                "(reflection_text, reflection_type, effectiveness_score, reflection_date) "
                "VALUES (?, 'interaction', ?, GETUTCDATE());",
                { text, std::to_string(m_cachedEmotions.valence) });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::Created({{"logged", true}, {"text", text}});
        });
        m_router.Register("POST", "/api/ai/voice-call/{id}/end", [](const HTTPRequest& req) {
            std::string callId = req.headers.at("x-path-id");
            ElleDB::EndVoiceCall(callId);
            return HTTPResponse::OK({{"call_id", callId}, {"status", "ended"}});
        });

        /* ============== Video — real SQL-backed job queue =====================
         * A generator worker (external or Python co-process) claims jobs via
         * ElleDB::ClaimNextVideoJob() and writes progress/output back through
         * ElleDB::UpdateVideoJobProgress/CompleteVideoJob/FailVideoJob. The
         * HTTP layer here is the queue + read API for the Android app.      */
        m_router.Register("POST", "/api/video/generate", [](const HTTPRequest& req) {
            try {
                json body = req.BodyJSON();
                std::string text = body.value("text", body.value("prompt", ""));
                if (text.empty()) return HTTPResponse::Err(400, "missing 'text'");
                std::string avatarPath = body.value("avatar_path", std::string(""));
                int64_t callId = body.value("call_id", (int64_t)0);

                /* If avatar_path omitted, fall back to the user's default avatar. */
                if (avatarPath.empty()) {
                    ElleDB::UserAvatar dflt;
                    if (ElleDB::GetDefaultAvatar(1, dflt)) avatarPath = dflt.file_path;
                }

                ElleDB::VideoJob job;
                if (!ElleDB::CreateVideoJob(text, avatarPath, callId, job))
                    return HTTPResponse::Err(500, "failed to queue video job");
                return HTTPResponse::Created({
                    {"job_id",      job.job_uuid},
                    {"id",          job.id},
                    {"status",      job.status},
                    {"avatar_path", job.avatar_path},
                    {"created_ms",  job.created_ms}
                });
            } catch (const std::exception& e) {
                return HTTPResponse::Err(500, e.what());
            }
        });
        m_router.Register("GET", "/api/video/status/{job_id}", [](const HTTPRequest& req) {
            std::string jobId = req.headers.at("x-path-id");
            ElleDB::VideoJob job;
            if (!ElleDB::GetVideoJob(jobId, job))
                return HTTPResponse::Err(404, "video job not found");
            return HTTPResponse::OK({
                {"job_id",      job.job_uuid},
                {"status",      job.status},
                {"progress",    job.progress},
                {"output_path", job.output_path},
                {"error",       job.error},
                {"created_ms",  job.created_ms},
                {"started_ms",  job.started_ms},
                {"finished_ms", job.finished_ms}
            });
        });
        m_router.Register("POST", "/api/video/avatar/upload", [](const HTTPRequest& req) {
            /* Accept either (a) a file_path already on disk, or (b) base64 image
             * bytes. Base64 path writes the file into cfg avatar_dir so the
             * video generator can pick it up.                                  */
            try {
                json body = req.BodyJSON();
                std::string label    = body.value("label", std::string(""));
                std::string filePath = body.value("file_path", std::string(""));
                std::string mime     = body.value("mime_type", std::string("image/png"));
                bool isDefault       = body.value("default", true);

                if (filePath.empty() && body.contains("base64") && body["base64"].is_string()) {
                    std::string b64 = body["base64"].get<std::string>();
                    std::string avatarDir = ElleConfig::Instance().GetString(
                        "video.avatar_dir", "C:\\ElleAnn\\avatars");
                    CreateDirectoryA(avatarDir.c_str(), nullptr);
                    std::string fname = "avatar_" + std::to_string(ELLE_MS_NOW()) + ".png";
                    filePath = avatarDir + "\\" + fname;
                    /* Decode base64 → bytes (RFC 4648, no streaming — avatars are small). */
                    std::string decoded;
                    decoded.reserve(b64.size() * 3 / 4);
                    int val = 0, bits = -8;
                    for (unsigned char c : b64) {
                        int d;
                        if      (c >= 'A' && c <= 'Z') d = c - 'A';
                        else if (c >= 'a' && c <= 'z') d = c - 'a' + 26;
                        else if (c >= '0' && c <= '9') d = c - '0' + 52;
                        else if (c == '+')             d = 62;
                        else if (c == '/')             d = 63;
                        else continue;      /* skip '=', whitespace, junk */
                        val = (val << 6) | d; bits += 6;
                        if (bits >= 0) { decoded += (char)((val >> bits) & 0xFF); bits -= 8; }
                    }
                    std::ofstream out(filePath, std::ios::binary);
                    if (!out) return HTTPResponse::Err(500, "cannot open avatar path for write");
                    out.write(decoded.data(), (std::streamsize)decoded.size());
                }
                if (filePath.empty())
                    return HTTPResponse::Err(400, "provide file_path or base64");

                ElleDB::UserAvatar a;
                a.user_id    = body.value("user_id", 1);
                a.label      = label;
                a.file_path  = filePath;
                a.mime_type  = mime;
                a.is_default = isDefault;
                int32_t newId = 0;
                if (!ElleDB::RegisterAvatar(a, newId))
                    return HTTPResponse::Err(500, "failed to register avatar");
                return HTTPResponse::Created({
                    {"avatar_id",  newId},
                    {"file_path",  filePath},
                    {"is_default", isDefault}
                });
            } catch (const std::exception& e) {
                return HTTPResponse::Err(500, e.what());
            }
        });
        m_router.Register("GET", "/api/video/avatar", [](const HTTPRequest& req) {
            int32_t userId = std::atoi(req.QueryParam("user_id", "1").c_str());
            ElleDB::UserAvatar a;
            if (!ElleDB::GetDefaultAvatar(userId, a))
                return HTTPResponse::OK({{"avatar", nullptr}, {"note", "no avatar configured"}});
            return HTTPResponse::OK({
                {"avatar_id",  a.id},
                {"user_id",    a.user_id},
                {"label",      a.label},
                {"file_path",  a.file_path},
                {"mime_type",  a.mime_type},
                {"is_default", a.is_default}
            });
        });
        m_router.Register("GET", "/api/video/avatars", [](const HTTPRequest& req) {
            int32_t userId = std::atoi(req.QueryParam("user_id", "1").c_str());
            std::vector<ElleDB::UserAvatar> avs;
            ElleDB::ListAvatars(userId, avs);
            json arr = json::array();
            for (auto& a : avs) {
                arr.push_back({
                    {"avatar_id", a.id}, {"label", a.label},
                    {"file_path", a.file_path}, {"mime_type", a.mime_type},
                    {"is_default", a.is_default}
                });
            }
            return HTTPResponse::OK({{"avatars", arr}});
        });
        /* Worker-facing endpoints (generator subprocess polls/updates these). */
        m_router.Register("POST", "/api/video/worker/claim", [](const HTTPRequest&) {
            ElleDB::VideoJob job;
            if (!ElleDB::ClaimNextVideoJob(job))
                return HTTPResponse::OK({{"claimed", false}});
            return HTTPResponse::OK({
                {"claimed",     true},
                {"job_id",      job.job_uuid},
                {"text",        job.text},
                {"avatar_path", job.avatar_path},
                {"call_id",     job.call_id}
            });
        });
        m_router.Register("POST", "/api/video/worker/progress/{job_id}", [](const HTTPRequest& req) {
            std::string jobId = req.headers.at("x-path-id");
            json body = req.BodyJSON();
            int32_t pct = body.value("progress", 0);
            if (!ElleDB::UpdateVideoJobProgress(jobId, pct))
                return HTTPResponse::Err(500, "progress update failed");
            return HTTPResponse::OK({{"job_id", jobId}, {"progress", pct}});
        });
        m_router.Register("POST", "/api/video/worker/complete/{job_id}", [](const HTTPRequest& req) {
            std::string jobId = req.headers.at("x-path-id");
            json body = req.BodyJSON();
            std::string path = body.value("output_path", std::string(""));
            if (path.empty()) return HTTPResponse::Err(400, "missing output_path");
            if (!ElleDB::CompleteVideoJob(jobId, path))
                return HTTPResponse::Err(500, "complete failed");
            return HTTPResponse::OK({{"job_id", jobId}, {"status", "done"}});
        });
        m_router.Register("POST", "/api/video/worker/fail/{job_id}", [](const HTTPRequest& req) {
            std::string jobId = req.headers.at("x-path-id");
            json body = req.BodyJSON();
            std::string err = body.value("error", std::string("worker reported failure"));
            if (!ElleDB::FailVideoJob(jobId, err))
                return HTTPResponse::Err(500, "fail update failed");
            return HTTPResponse::OK({{"job_id", jobId}, {"status", "failed"}});
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
        m_router.Register("GET", "/api/ai/self-prompts", [](const HTTPRequest& req) {
            int limit = std::max(1, std::atoi(req.QueryParam("limit", "20").c_str()));
            auto rs = ElleSQLPool::Instance().QueryParams(
                "SELECT TOP (?) id, prompt, ISNULL(source,''), created_ms "
                "FROM ElleCore.dbo.ai_self_prompts ORDER BY id DESC;",
                { std::to_string(limit) });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            json j = json::array();
            for (auto& r : rs.rows) {
                j.push_back({
                    {"id", r.GetInt(0)},
                    {"prompt", r.values.size() > 1 ? r.values[1] : ""},
                    {"source", r.values.size() > 2 ? r.values[2] : ""},
                    {"created_ms", r.GetInt(3)}
                });
            }
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/ai/self-prompts/generate", [this](const HTTPRequest&) {
            /* Ask Cognitive to produce one via the chat pipeline. */
            std::string requestId = "sp-" + std::to_string(ELLE_MS_NOW());
            json env = {
                {"request_id", requestId},
                {"user_text", "[internal] Generate one brief self-reflective prompt you'd ask yourself right now."},
                {"user_id", "self"}, {"conv_id", (uint64_t)0}, {"origin", "self_prompt"}
            };
            auto pending = m_chatCorrelator.Register(requestId);
            auto ipcMsg = ElleIPCMessage::Create(IPC_CHAT_REQUEST, SVC_HTTP_SERVER, SVC_COGNITIVE);
            ipcMsg.SetStringPayload(env.dump());
            if (!GetIPCHub().Send(SVC_COGNITIVE, ipcMsg)) {
                m_chatCorrelator.Cancel(requestId);
                return HTTPResponse::Err(503, "Cognitive service unreachable");
            }
            std::unique_lock<std::mutex> lk(pending->m);
            if (!pending->cv.wait_for(lk, std::chrono::seconds(20),
                                       [&]{ return pending->done; })) {
                m_chatCorrelator.Cancel(requestId);
                return HTTPResponse::Err(504, "timeout");
            }
            std::string text = pending->result.value("response", "");
            /* Persist it */
            ElleSQLPool::Instance().QueryParams(
                "INSERT INTO ElleCore.dbo.ai_self_prompts (prompt, source, created_ms) VALUES (?, 'self_prompt', ?);",
                { text, std::to_string(ELLE_MS_NOW()) });
            return HTTPResponse::OK({
                {"prompt", text}, {"generated_at", (uint64_t)ELLE_MS_NOW()}
            });
        });
        m_router.Register("GET", "/api/ai/status", [this](const HTTPRequest&) {
            /* Pull live model info from config, emotion from cache, service health from DB */
            auto& llm = ElleConfig::Instance().GetLLM();
            std::string modelName = "llama-3.3-70b-versatile";
            std::string modelUrl  = "groq://api.groq.com";
            auto it = llm.providers.find("groq");
            if (it != llm.providers.end()) {
                if (!it->second.model.empty())    modelName = it->second.model;
                if (!it->second.api_url.empty())  modelUrl  = it->second.api_url;
            }
            json j = {
                {"modelStatus", "ready"},
                {"modelName", modelName},
                {"modelUrl", modelUrl},
                {"emotionalState", {
                    {"valence", m_cachedEmotions.valence},
                    {"arousal", m_cachedEmotions.arousal},
                    {"dominance", m_cachedEmotions.dominance},
                    {"joy",        m_cachedEmotions.valence > 0 ? m_cachedEmotions.valence : 0.0},
                    {"sadness",    m_cachedEmotions.valence < 0 ? -m_cachedEmotions.valence : 0.0}
                }}
            };
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/ai/analyze-emotion", [this](const HTTPRequest& req) {
            /* Lightweight local analyzer — same one Cognitive uses. */
            json body = req.BodyJSON();
            std::string text = body.value("text", "");
            if (text.empty()) return HTTPResponse::Err(400, "missing 'text'");
            std::string l = text;
            std::transform(l.begin(), l.end(), l.begin(),
                           [](unsigned char c){ return (char)std::tolower(c); });
            struct W { const char* w; float v; };
            static const W pos[] = {{"love",0.8f},{"happy",0.6f},{"thank",0.5f},{"great",0.5f},
                                    {"good",0.3f},{"glad",0.5f},{"missed",0.6f},{"proud",0.6f},
                                    {"excited",0.7f},{"beautiful",0.7f}};
            static const W neg[] = {{"hate",-0.8f},{"sad",-0.6f},{"angry",-0.7f},{"upset",-0.6f},
                                    {"worried",-0.5f},{"tired",-0.4f},{"hurt",-0.6f},{"lonely",-0.7f},
                                    {"afraid",-0.6f},{"hopeless",-0.8f}};
            float val = 0.0f, arou = 0.0f;
            for (auto& w : pos) if (l.find(w.w) != std::string::npos) { val += w.v; arou += 0.1f; }
            for (auto& w : neg) if (l.find(w.w) != std::string::npos) { val += w.v; arou += 0.15f; }
            if (std::count(text.begin(), text.end(), '!') > 0) arou += 0.15f;
            val  = std::max(-1.0f, std::min(1.0f, val));
            arou = std::max(0.0f,  std::min(1.0f, arou));
            std::string dominant = val > 0.3f ? "joy" : (val < -0.3f ? "sadness" : "neutral");
            return HTTPResponse::OK({
                {"text", text}, {"valence", val}, {"arousal", arou},
                {"dominant", dominant}
            });
        });
        m_router.Register("GET", "/api/ai/memory-tracking", [](const HTTPRequest&) {
            int64_t mem  = ElleDB::CountTable("memory");
            int64_t msgs = ElleDB::CountTable("messages");
            int64_t refs = ElleDB::CountTable("SelfReflections");
            int64_t ents = ElleDB::CountTable("world_entity");
            return HTTPResponse::OK({
                {"total_memories", std::max<int64_t>(mem,0)},
                {"total_messages", std::max<int64_t>(msgs,0)},
                {"total_reflections", std::max<int64_t>(refs,0)},
                {"total_entities", std::max<int64_t>(ents,0)}
            });
        });
        m_router.Register("GET", "/api/ai/autonomy/status", [](const HTTPRequest&) {
            ELLE_TRUST_STATE trust = {};
            ElleDB::GetTrustState(trust);
            const char* levelStr = "sandboxed";
            if (trust.score >= TRUST_THRESHOLD_AUTONOMOUS) levelStr = "autonomous";
            else if (trust.score >= TRUST_THRESHOLD_ELEVATED) levelStr = "elevated";
            else if (trust.score >= TRUST_THRESHOLD_BASIC)    levelStr = "basic";
            return HTTPResponse::OK({
                {"autonomous", trust.score >= TRUST_THRESHOLD_AUTONOMOUS},
                {"trust_level", levelStr},
                {"trust_score", trust.score},
                {"successes", trust.successes},
                {"failures", trust.failures},
                {"self_prompting_active", true}
            });
        });
        m_router.Register("GET", "/api/ai/hardware/info", [](const HTTPRequest&) {
            MEMORYSTATUSEX mem; mem.dwLength = sizeof(mem);
            GlobalMemoryStatusEx(&mem);
            SYSTEM_INFO si; GetSystemInfo(&si);
            OSVERSIONINFOA osvi = {}; osvi.dwOSVersionInfoSize = sizeof(osvi);
            char compName[256] = {}; DWORD sz = sizeof(compName);
            GetComputerNameA(compName, &sz);
            return HTTPResponse::OK({
                {"os", "Windows"},
                {"hostname", std::string(compName)},
                {"cpu_count", (int)si.dwNumberOfProcessors},
                {"ram_total_mb", (uint64_t)(mem.ullTotalPhys / (1024ULL*1024ULL))},
                {"ram_used_mb",  (uint64_t)((mem.ullTotalPhys - mem.ullAvailPhys) / (1024ULL*1024ULL))},
                {"ram_percent", (float)mem.dwMemoryLoad}
            });
        });
        m_router.Register("GET", "/api/ai/hardware/actions/pending", [](const HTTPRequest& req) {
            std::string target = req.QueryParam("target", "device");
            json j = json::array();

            /* (a) Legacy / trust-gated actions from the action queue. */
            std::vector<ELLE_ACTION_RECORD> actions;
            ElleDB::GetPendingActions(actions, 50);
            for (auto& a : actions) {
                j.push_back({
                    {"source", "action_queue"},
                    {"id", a.id}, {"type", a.type},
                    {"command", std::string(a.command)},
                    {"parameters", std::string(a.parameters)},
                    {"required_trust", a.required_trust}
                });
            }

            /* (b) Device-facing hardware_actions (vibrate / flash / notify)
             * populated by ActionExecutor::ExecuteHardwareCommand. Atomic
             * claim pattern so two concurrent polls don't double-process.  */
            auto claim = ElleSQLPool::Instance().Query(
                "UPDATE TOP (50) ElleCore.dbo.hardware_actions "
                "SET status = 'dispatched' "
                "OUTPUT inserted.id, inserted.action_type, ISNULL(inserted.payload,''), "
                "       inserted.created_ms "
                "WHERE status = 'pending';");
            if (claim.success) {
                for (auto& r : claim.rows) {
                    j.push_back({
                        {"source", "hardware_actions"},
                        {"id", r.GetInt(0)},
                        {"action_type", r.values.size() > 1 ? r.values[1] : ""},
                        {"payload",     r.values.size() > 2 ? r.values[2] : ""},
                        {"created_ms",  r.GetInt(3)}
                    });
                }
            }
            (void)target;
            return HTTPResponse::OK(j);
        });
        m_router.Register("POST", "/api/ai/hardware/actions/{id}/ack", [](const HTTPRequest& req) {
            /* Android confirms delivery — mark the hardware_actions row consumed. */
            int64_t id = std::atoll(req.headers.at("x-path-id").c_str());
            auto rs = ElleSQLPool::Instance().QueryParams(
                "UPDATE ElleCore.dbo.hardware_actions "
                "SET status = 'consumed', consumed_ms = ? "
                "WHERE id = ? AND status = 'dispatched';",
                { std::to_string((int64_t)ELLE_MS_NOW()), std::to_string(id) });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::OK({{"id", id}, {"acked", true}});
        });
        m_router.Register("POST", "/api/ai/hardware/actions/{id}/result", [](const HTTPRequest& req) {
            uint64_t actionId = std::atoll(req.headers.at("x-path-id").c_str());
            json body = req.BodyJSON();
            std::string result = body.value("result", std::string("done"));
            uint32_t status = body.value("success", true) ? ACTION_COMPLETED_SUCCESS : ACTION_COMPLETED_FAILURE;
            bool ok = ElleDB::UpdateActionStatus(actionId, status, result);
            if (!ok) return HTTPResponse::Err(500, "UpdateActionStatus failed");
            return HTTPResponse::OK({{"action_id", actionId}, {"recorded", true}});
        });

        /* ============== Tools — dbo.ai_tools ============== */
        m_router.Register("GET", "/api/ai/tools", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT name, ISNULL(description,''), ISNULL(config,''), enabled "
                "FROM ElleCore.dbo.ai_tools ORDER BY name;");
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            json arr = json::array();
            for (auto& r : rs.rows) {
                arr.push_back({
                    {"name", r.values.size() > 0 ? r.values[0] : ""},
                    {"description", r.values.size() > 1 ? r.values[1] : ""},
                    {"config", r.values.size() > 2 ? r.values[2] : ""},
                    {"enabled", r.GetInt(3) != 0}
                });
            }
            return HTTPResponse::OK({{"tools", arr}});
        });
        m_router.Register("POST", "/api/ai/tools", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            std::string name = body.value("name", "");
            if (name.empty()) return HTTPResponse::Err(400, "missing name");
            std::string desc = body.value("description", "");
            std::string cfg  = body.contains("config") ? body["config"].dump() : std::string();
            auto rs = ElleSQLPool::Instance().QueryParams(
                "MERGE ElleCore.dbo.ai_tools AS t "
                "USING (SELECT ? AS name) AS s ON t.name = s.name "
                "WHEN MATCHED THEN UPDATE SET description = ?, config = ?, enabled = 1 "
                "WHEN NOT MATCHED THEN INSERT (name, description, config, enabled) VALUES (?, ?, ?, 1);",
                { name, desc, cfg, name, desc, cfg });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::Created({{"name", name}, {"stored", true}});
        });
        m_router.Register("DELETE", "/api/ai/tools/{name}", [](const HTTPRequest& req) {
            std::string name = req.headers.at("x-path-name");
            auto rs = ElleSQLPool::Instance().QueryParams(
                "DELETE FROM ElleCore.dbo.ai_tools WHERE name = ?;", { name });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::OK({{"deleted", true}, {"name", name}});
        });

        /* ============== Agents — dbo.ai_agents ============== */
        m_router.Register("GET", "/api/ai/agents", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT name, ISNULL(description,''), ISNULL(system_prompt,''), ISNULL(model,'') "
                "FROM ElleCore.dbo.ai_agents ORDER BY name;");
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            json arr = json::array();
            for (auto& r : rs.rows) {
                arr.push_back({
                    {"name", r.values.size() > 0 ? r.values[0] : ""},
                    {"description", r.values.size() > 1 ? r.values[1] : ""},
                    {"system_prompt", r.values.size() > 2 ? r.values[2] : ""},
                    {"model", r.values.size() > 3 ? r.values[3] : ""}
                });
            }
            return HTTPResponse::OK({{"agents", arr}});
        });
        m_router.Register("POST", "/api/ai/agents", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            std::string name = body.value("name", "");
            if (name.empty()) return HTTPResponse::Err(400, "missing name");
            std::string desc  = body.value("description", "");
            std::string sys   = body.value("system_prompt", "");
            std::string model = body.value("model", "");
            auto rs = ElleSQLPool::Instance().QueryParams(
                "MERGE ElleCore.dbo.ai_agents AS t "
                "USING (SELECT ? AS name) AS s ON t.name = s.name "
                "WHEN MATCHED THEN UPDATE SET description = ?, system_prompt = ?, model = ? "
                "WHEN NOT MATCHED THEN INSERT (name, description, system_prompt, model) VALUES (?, ?, ?, ?);",
                { name, desc, sys, model, name, desc, sys, model });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::Created({{"name", name}, {"stored", true}});
        });
        m_router.Register("DELETE", "/api/ai/agents/{name}", [](const HTTPRequest& req) {
            std::string name = req.headers.at("x-path-name");
            auto rs = ElleSQLPool::Instance().QueryParams(
                "DELETE FROM ElleCore.dbo.ai_agents WHERE name = ?;", { name });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::OK({{"deleted", true}, {"name", name}});
        });
        m_router.Register("POST", "/api/ai/agents/{name}/chat", [](const HTTPRequest& req) {
            try {
                json body = req.BodyJSON();
                std::string message = body.value("message", "");
                std::string agentName = req.headers.at("x-path-name");
                /* Pull stored system_prompt + model */
                auto rs = ElleSQLPool::Instance().QueryParams(
                    "SELECT TOP 1 ISNULL(system_prompt,''), ISNULL(model,'') "
                    "FROM ElleCore.dbo.ai_agents WHERE name = ?;",
                    { agentName });
                std::string sys = "You are agent " + agentName + ". Respond as this agent would.";
                if (rs.success && !rs.rows.empty()) {
                    std::string s = rs.rows[0].values.size() > 0 ? rs.rows[0].values[0] : "";
                    if (!s.empty()) sys = s;
                }
                std::vector<LLMMsg> convo = { {"system", sys}, {"user", message} };
                std::string response, err;
                if (!CallGroqDirect(convo, response, err))
                    return HTTPResponse::Err(502, err);
                return HTTPResponse::OK({{"agent", agentName}, {"response", response}});
            } catch (const std::exception& e) {
                return HTTPResponse::Err(500, e.what());
            }
        });

        /* ============== Dictionary — dbo.dictionary_words ============== */
        m_router.Register("POST", "/api/dictionary/load", [](const HTTPRequest& req) {
            uint32_t start = 0, limit = 0;
            try {
                json body = req.BodyJSON();
                start = body.value("start", 0);
                limit = body.value("limit", 0);
            } catch (...) { /* no body — use defaults */ }
            if (!DictionaryLoader::Instance().StartLoad(start, limit)) {
                auto s = DictionaryLoader::Instance().GetState();
                return HTTPResponse::JSON(409, json({
                    {"error",    "already_running"},
                    {"status",   s.status},
                    {"loaded",   s.loaded},
                    {"failed",   s.failed},
                    {"last_word",s.last_word}
                }).dump());
            }
            return HTTPResponse::Accepted({{"status", "started"}, {"start", start}, {"limit", limit}});
        });
        m_router.Register("GET", "/api/dictionary/load/status", [](const HTTPRequest&) {
            auto s = DictionaryLoader::Instance().GetState();
            /* Fall back to persisted DB state if the in-memory state is blank
             * (e.g. status endpoint queried from a freshly-restarted process). */
            if (s.status.empty() || s.status == "idle") {
                ElleDB::DictionaryLoaderState db;
                ElleDB::GetDictionaryLoaderState(db);
                s.status     = db.status;
                s.loaded     = (uint32_t)db.loaded;
                s.failed     = (uint32_t)db.failed;
                s.skipped    = (uint32_t)db.skipped;
                s.last_word  = db.last_word;
                s.error      = db.error;
                s.started_ms = (uint64_t)db.started_ms;
                s.updated_ms = (uint64_t)db.updated_ms;
            }
            return HTTPResponse::OK({
                {"status", s.status}, {"loaded", s.loaded},
                {"failed", s.failed}, {"skipped", s.skipped},
                {"last_word", s.last_word}, {"error", s.error},
                {"started_ms", s.started_ms}, {"updated_ms", s.updated_ms},
                {"total_words_in_db", ElleDB::CountDictionaryWords()}
            });
        });
        m_router.Register("GET", "/api/dictionary/stats", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT COUNT(*), "
                "       SUM(CASE WHEN definition IS NOT NULL AND LEN(definition) > 0 THEN 1 ELSE 0 END), "
                "       SUM(CASE WHEN example    IS NOT NULL AND LEN(example)    > 0 THEN 1 ELSE 0 END) "
                "FROM ElleCore.dbo.dictionary_words;");
            int total = 0, defs = 0, ex = 0;
            if (rs.success && !rs.rows.empty()) {
                total = (int)rs.rows[0].GetInt(0);
                defs  = (int)rs.rows[0].GetInt(1);
                ex    = (int)rs.rows[0].GetInt(2);
            }
            return HTTPResponse::OK({
                {"totalWords", total}, {"totalDefinitions", defs}, {"totalExamples", ex}
            });
        });
        m_router.Register("GET", "/api/dictionary/lookup/{word}", [](const HTTPRequest& req) {
            std::string word = req.headers.at("x-path-word");
            auto rs = ElleSQLPool::Instance().QueryParams(
                "SELECT ISNULL(definition,''), ISNULL(example,''), ISNULL(part_of_speech,'') "
                "FROM ElleCore.dbo.dictionary_words WHERE LOWER(word) = LOWER(?);",
                { word });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            if (rs.rows.empty()) return HTTPResponse::Err(404, "word not found");
            json defs = json::array();
            for (auto& r : rs.rows) {
                defs.push_back({
                    {"definition", r.values.size() > 0 ? r.values[0] : ""},
                    {"example",    r.values.size() > 1 ? r.values[1] : ""},
                    {"part_of_speech", r.values.size() > 2 ? r.values[2] : ""}
                });
            }
            return HTTPResponse::OK({{"word", word}, {"definitions", defs}});
        });
        m_router.Register("GET", "/api/dictionary/search", [](const HTTPRequest& req) {
            std::string prefix = req.QueryParam("prefix");
            if (prefix.empty()) return HTTPResponse::OK(json::array());
            std::string like = prefix + "%";
            auto rs = ElleSQLPool::Instance().QueryParams(
                "SELECT TOP 50 word, ISNULL(definition,'') FROM ElleCore.dbo.dictionary_words "
                "WHERE word LIKE ? ORDER BY word;",
                { like });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            json arr = json::array();
            for (auto& r : rs.rows) {
                arr.push_back({
                    {"word", r.values.size() > 0 ? r.values[0] : ""},
                    {"definition", r.values.size() > 1 ? r.values[1] : ""}
                });
            }
            return HTTPResponse::OK(arr);
        });
        m_router.Register("GET", "/api/dictionary/random", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT TOP 1 word, ISNULL(definition,'') FROM ElleCore.dbo.dictionary_words "
                "ORDER BY NEWID();");
            if (!rs.success || rs.rows.empty())
                return HTTPResponse::Err(404, "dictionary empty");
            auto& r = rs.rows[0];
            return HTTPResponse::OK({
                {"word", r.values.size() > 0 ? r.values[0] : ""},
                {"definition", r.values.size() > 1 ? r.values[1] : ""}
            });
        });

        /* ============== Education — real ElleDB helpers against
         * learned_subjects / education_references / learning_milestones / skills.
         * Matches the legacy Python app/routers/education.py shape.         */
        auto subjectToJson = [](const ElleDB::LearnedSubject& s) {
            return json{
                {"id", s.id}, {"subject", s.subject}, {"category", s.category},
                {"proficiency_level", s.proficiency_level},
                {"who_taught", s.who_taught}, {"where_learned", s.where_learned},
                {"time_to_learn_hours", s.time_to_learn_hours},
                {"notes", s.notes},
                {"date_started", s.date_started},
                {"date_completed", s.date_completed}
            };
        };
        m_router.Register("GET", "/api/education/subjects", [subjectToJson](const HTTPRequest& req) {
            std::string category = req.QueryParam("category", "");
            uint32_t limit = (uint32_t)std::atoi(req.QueryParam("limit", "50").c_str());
            std::vector<ElleDB::LearnedSubject> subs;
            if (!ElleDB::ListSubjects(subs, category, limit))
                return HTTPResponse::Err(500, "subjects query failed");
            json arr = json::array();
            for (auto& s : subs) arr.push_back(subjectToJson(s));
            return HTTPResponse::OK({{"subjects", arr}, {"total", (int64_t)arr.size()}});
        });
        m_router.Register("GET", "/api/education/subjects/{id}", [subjectToJson](const HTTPRequest& req) {
            int32_t id = std::atoi(req.headers.at("x-path-id").c_str());
            ElleDB::LearnedSubject s;
            if (!ElleDB::GetSubject(id, s)) return HTTPResponse::Err(404, "subject not found");

            std::vector<ElleDB::EducationReference> refs;
            std::vector<ElleDB::LearningMilestone>  mils;
            ElleDB::ListSubjectReferences(id, refs);
            ElleDB::ListSubjectMilestones(id, mils);
            json refArr = json::array();
            for (auto& r : refs) refArr.push_back({
                {"id", r.id}, {"reference_type", r.reference_type},
                {"reference_title", r.reference_title},
                {"reference_content", r.reference_content},
                {"file_path", r.file_path},
                {"relevance_score", r.relevance_score}, {"notes", r.notes}
            });
            json milArr = json::array();
            for (auto& m : mils) milArr.push_back({
                {"id", m.id}, {"milestone", m.milestone},
                {"description", m.description}, {"achieved_at", m.achieved_at}
            });
            json out = subjectToJson(s);
            out["references"] = refArr;
            out["milestones"] = milArr;
            return HTTPResponse::OK(out);
        });
        m_router.Register("POST", "/api/education/subjects", [](const HTTPRequest& req) {
            try {
                json body = req.BodyJSON();
                ElleDB::LearnedSubject s;
                s.subject              = body.value("subject", body.value("name", std::string("")));
                if (s.subject.empty()) return HTTPResponse::Err(400, "missing 'subject'");
                s.category             = body.value("category", std::string(""));
                s.proficiency_level    = body.value("proficiency_level", 0);
                s.who_taught           = body.value("who_taught", std::string(""));
                s.where_learned        = body.value("where_learned", std::string(""));
                s.time_to_learn_hours  = body.value("time_to_learn_hours", 0.0f);
                s.notes                = body.value("notes", std::string(""));
                int32_t newId = 0;
                if (!ElleDB::CreateSubject(s, newId))
                    return HTTPResponse::Err(500, "create subject failed");
                return HTTPResponse::Created({{"id", newId}, {"subject", s.subject}});
            } catch (const std::exception& e) { return HTTPResponse::Err(500, e.what()); }
        });
        m_router.Register("PUT", "/api/education/subjects/{id}", [](const HTTPRequest& req) {
            int32_t id = std::atoi(req.headers.at("x-path-id").c_str());
            try {
                json body = req.BodyJSON();
                ElleDB::LearnedSubject patch;
                std::vector<std::string> fields;
                for (auto& kv : body.items()) {
                    if      (kv.key() == "subject")             { patch.subject = kv.value().get<std::string>();             fields.push_back("subject"); }
                    else if (kv.key() == "category")            { patch.category = kv.value().get<std::string>();            fields.push_back("category"); }
                    else if (kv.key() == "proficiency_level")   { patch.proficiency_level = kv.value().get<int>();            fields.push_back("proficiency_level"); }
                    else if (kv.key() == "who_taught")          { patch.who_taught = kv.value().get<std::string>();          fields.push_back("who_taught"); }
                    else if (kv.key() == "where_learned")       { patch.where_learned = kv.value().get<std::string>();       fields.push_back("where_learned"); }
                    else if (kv.key() == "time_to_learn_hours") { patch.time_to_learn_hours = kv.value().get<float>();        fields.push_back("time_to_learn_hours"); }
                    else if (kv.key() == "notes")               { patch.notes = kv.value().get<std::string>();               fields.push_back("notes"); }
                    else if (kv.key() == "date_completed")      { patch.date_completed = kv.value().get<std::string>();      fields.push_back("date_completed"); }
                }
                if (fields.empty()) return HTTPResponse::OK({{"updated", 0}});
                if (!ElleDB::UpdateSubject(id, patch, fields))
                    return HTTPResponse::Err(500, "update failed");
                return HTTPResponse::OK({{"id", id}, {"updated_fields", fields}});
            } catch (const std::exception& e) { return HTTPResponse::Err(500, e.what()); }
        });
        m_router.Register("POST", "/api/education/subjects/{id}/references", [](const HTTPRequest& req) {
            int32_t id = std::atoi(req.headers.at("x-path-id").c_str());
            try {
                json body = req.BodyJSON();
                ElleDB::EducationReference r;
                r.subject_id        = id;
                r.reference_type    = body.value("reference_type", std::string("note"));
                r.reference_title   = body.value("reference_title", std::string(""));
                r.reference_content = body.value("reference_content", std::string(""));
                r.file_path         = body.value("file_path", std::string(""));
                r.relevance_score   = body.value("relevance_score", 0.5f);
                r.notes             = body.value("notes", std::string(""));
                if (!ElleDB::AddSubjectReference(r))
                    return HTTPResponse::Err(500, "add reference failed");
                return HTTPResponse::Created({{"subject_id", id}, {"stored", true}});
            } catch (const std::exception& e) { return HTTPResponse::Err(500, e.what()); }
        });
        m_router.Register("POST", "/api/education/subjects/{id}/milestones", [](const HTTPRequest& req) {
            int32_t id = std::atoi(req.headers.at("x-path-id").c_str());
            try {
                json body = req.BodyJSON();
                ElleDB::LearningMilestone m;
                m.subject_id  = id;
                m.milestone   = body.value("milestone", std::string(""));
                m.description = body.value("description", std::string(""));
                if (m.milestone.empty()) return HTTPResponse::Err(400, "missing 'milestone'");
                if (!ElleDB::AddSubjectMilestone(m))
                    return HTTPResponse::Err(500, "add milestone failed");
                return HTTPResponse::Created({{"subject_id", id}, {"stored", true}});
            } catch (const std::exception& e) { return HTTPResponse::Err(500, e.what()); }
        });
        m_router.Register("GET", "/api/education/skills", [](const HTTPRequest& req) {
            std::string category = req.QueryParam("category", "");
            std::vector<ElleDB::Skill> skills;
            if (!ElleDB::ListSkills(skills, category))
                return HTTPResponse::Err(500, "skills query failed");
            json arr = json::array();
            for (auto& s : skills) arr.push_back({
                {"id", s.id}, {"skill_name", s.skill_name}, {"category", s.category},
                {"proficiency", s.proficiency},
                {"learned_from_subject_id", s.learned_from_subject_id},
                {"times_used", s.times_used}, {"last_used", s.last_used},
                {"notes", s.notes}
            });
            return HTTPResponse::OK({{"skills", arr}, {"total", (int64_t)arr.size()}});
        });
        m_router.Register("POST", "/api/education/skills", [](const HTTPRequest& req) {
            try {
                json body = req.BodyJSON();
                ElleDB::Skill s;
                s.skill_name              = body.value("skill_name", body.value("name", std::string("")));
                if (s.skill_name.empty()) return HTTPResponse::Err(400, "missing 'skill_name'");
                s.category                = body.value("category", std::string(""));
                s.proficiency             = body.value("proficiency", 0);
                s.learned_from_subject_id = body.value("learned_from_subject_id", 0);
                s.notes                   = body.value("notes", std::string(""));
                int32_t newId = 0;
                if (!ElleDB::CreateSkill(s, newId))
                    return HTTPResponse::Err(409, "skill already exists or create failed");
                return HTTPResponse::Created({{"id", newId}, {"skill_name", s.skill_name}});
            } catch (const std::exception& e) { return HTTPResponse::Err(500, e.what()); }
        });
        m_router.Register("PUT", "/api/education/skills/{name}/use", [](const HTTPRequest& req) {
            std::string name = req.headers.at("x-path-id");
            if (!ElleDB::RecordSkillUse(name))
                return HTTPResponse::Err(404, "skill not found");
            return HTTPResponse::OK({{"skill_name", name}, {"recorded", true}});
        });

        /* ============== Emotional-context — reads from ElleThreads + SelfReflections = */
        m_router.Register("GET", "/api/emotional-context/patterns", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT TOP 50 thread_id, ISNULL(topic,''), ISNULL(emotional_weight, 0), "
                "       ISNULL(status,''), ISNULL(last_discussed, GETUTCDATE()) "
                "FROM ElleCore.dbo.ElleThreads WHERE status <> 'resolved' "
                "ORDER BY emotional_weight DESC;");
            if (!rs.success) return HTTPResponse::OK(json::array());
            json arr = json::array();
            for (auto& r : rs.rows) {
                arr.push_back({
                    {"id", r.GetInt(0)},
                    {"topic", r.values.size() > 1 ? r.values[1] : ""},
                    {"emotional_weight", r.GetFloat(2)},
                    {"status", r.values.size() > 3 ? r.values[3] : ""}
                });
            }
            return HTTPResponse::OK(arr);
        });
        m_router.Register("GET", "/api/emotional-context/vocabulary", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT TOP 100 tag, COUNT(*) AS freq FROM ElleCore.dbo.memory_tags "
                "GROUP BY tag ORDER BY freq DESC;");
            if (!rs.success) return HTTPResponse::OK(json::array());
            json arr = json::array();
            for (auto& r : rs.rows) {
                arr.push_back({
                    {"term", r.values.size() > 0 ? r.values[0] : ""},
                    {"frequency", r.GetInt(1)}
                });
            }
            return HTTPResponse::OK(arr);
        });
        m_router.Register("GET", "/api/emotional-context/history", [](const HTTPRequest& req) {
            /* Time-series of the V/A/D trajectory. Default last 24 h. */
            uint32_t hours = (uint32_t)std::atoi(req.QueryParam("hours", "24").c_str());
            if (hours == 0 || hours > 24 * 30) hours = 24;
            uint32_t maxPoints = (uint32_t)std::atoi(req.QueryParam("points", "500").c_str());
            if (maxPoints == 0 || maxPoints > 5000) maxPoints = 500;

            std::vector<ElleDB::EmotionHistoryPoint> pts;
            if (!ElleDB::GetEmotionHistory(hours, pts, maxPoints))
                return HTTPResponse::Err(500, "emotion_snapshots query failed");

            json series = json::array();
            for (auto& p : pts) {
                series.push_back({
                    {"t",         p.taken_ms},
                    {"valence",   p.valence},
                    {"arousal",   p.arousal},
                    {"dominance", p.dominance}
                });
            }
            return HTTPResponse::OK({
                {"hours",  hours},
                {"points", (int64_t)series.size()},
                {"series", series}
            });
        });
        /* Deep-emotion lookup: top-N dominant dimensions at a given timestamp.
         * Used by the Android mood chart when the user taps a point on the
         * timeline — unpacks the space-separated dimensions column into a
         * ranked list with indices so the UI can map to dimension labels.   */
        m_router.Register("GET", "/api/emotional-context/dimensions", [](const HTTPRequest& req) {
            int64_t ts = std::atoll(req.QueryParam("t", "0").c_str());
            int topN   = std::atoi(req.QueryParam("top", "5").c_str());
            if (topN <= 0 || topN > 102) topN = 5;
            /* Grab the snapshot closest (by absolute ms diff) to the requested
             * timestamp. If t=0, just use the most recent.                  */
            auto rs = ts > 0
                ? ElleSQLPool::Instance().QueryParams(
                    "IF EXISTS (SELECT 1 FROM sys.tables WHERE name = 'emotion_snapshots') "
                    "SELECT TOP 1 taken_ms, valence, arousal, dominance, dimensions "
                    "FROM ElleCore.dbo.emotion_snapshots "
                    "ORDER BY ABS(taken_ms - ?) ASC;",
                    { std::to_string(ts) })
                : ElleSQLPool::Instance().Query(
                    "IF EXISTS (SELECT 1 FROM sys.tables WHERE name = 'emotion_snapshots') "
                    "SELECT TOP 1 taken_ms, valence, arousal, dominance, dimensions "
                    "FROM ElleCore.dbo.emotion_snapshots ORDER BY id DESC;");
            if (!rs.success || rs.rows.empty())
                return HTTPResponse::OK({{"found", false}});

            auto& r = rs.rows[0];
            int64_t takenMs = r.GetInt(0);
            double val = r.GetFloat(1), aro = r.GetFloat(2), dom = r.GetFloat(3);
            std::string dimStr = r.values.size() > 4 ? r.values[4] : "";

            /* Parse + rank. */
            std::vector<std::pair<int, float>> ranked;
            ranked.reserve(ELLE_EMOTION_COUNT);
            std::istringstream iss(dimStr);
            for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
                float v = 0.0f;
                if (!(iss >> v)) break;
                ranked.push_back({i, v});
            }
            std::sort(ranked.begin(), ranked.end(),
                      [](const std::pair<int,float>& a, const std::pair<int,float>& b) {
                          return a.second > b.second;
                      });
            json top = json::array();
            for (int i = 0; i < topN && i < (int)ranked.size(); i++) {
                int idx = ranked[i].first;
                const char* name = (idx >= 0 && idx < ELLE_EMOTION_COUNT)
                                   ? kEmotionMeta[idx].name : "";
                const char* cat  = (idx >= 0 && idx < ELLE_EMOTION_COUNT)
                                   ? kEmotionMeta[idx].category : "";
                top.push_back({
                    {"index",    idx},
                    {"name",     name},
                    {"category", cat},
                    {"value",    ranked[i].second}
                });
            }
            return HTTPResponse::OK({
                {"found",     true},
                {"taken_ms",  takenMs},
                {"valence",   val},
                {"arousal",   aro},
                {"dominance", dom},
                {"top",       top}
            });
        });
        /* Session greeting: Elle's first message on reconnect.
         * - GET   reads the newest unconsumed greeting (idempotent — safe to poll)
         * - POST /ack marks it consumed so we don't greet twice                  */
        m_router.Register("GET", "/api/session/greeting", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'reconnection_greetings') "
                "  SELECT CAST(NULL AS BIGINT), CAST('' AS NVARCHAR(MAX)), "
                "         CAST('{}' AS NVARCHAR(MAX)), CAST(0 AS BIGINT) WHERE 1=0; "
                "ELSE "
                "  SELECT TOP 1 id, greeting, ISNULL(context_json,'{}'), created_ms "
                "  FROM ElleCore.dbo.reconnection_greetings "
                "  WHERE consumed = 0 ORDER BY id DESC;");
            if (!rs.success || rs.rows.empty())
                return HTTPResponse::OK({{"greeting", nullptr}});
            auto& r = rs.rows[0];
            json ctx = json::object();
            try { ctx = json::parse(r.values.size() > 2 ? r.values[2] : "{}"); }
            catch (...) {}
            return HTTPResponse::OK({
                {"id",         r.GetInt(0)},
                {"greeting",   r.values.size() > 1 ? r.values[1] : ""},
                {"context",    ctx},
                {"created_ms", r.GetInt(3)}
            });
        });
        m_router.Register("POST", "/api/session/greeting/{id}/ack", [](const HTTPRequest& req) {
            int64_t id = std::atoll(req.headers.at("x-path-id").c_str());
            auto rs = ElleSQLPool::Instance().QueryParams(
                "UPDATE ElleCore.dbo.reconnection_greetings "
                "SET consumed = 1 WHERE id = ?;",
                { std::to_string(id) });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::OK({{"id", id}, {"consumed", true}});
        });
        m_router.Register("GET", "/api/emotional-context/growth", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT TOP 50 reflection_id, ISNULL(reflection_text,''), "
                "       ISNULL(effectiveness_score, 0), "
                "       CONVERT(varchar(33), reflection_date, 126) "
                "FROM ElleCore.dbo.SelfReflections "
                "ORDER BY reflection_date DESC;");
            if (!rs.success) return HTTPResponse::OK(json::array());
            json arr = json::array();
            for (auto& r : rs.rows) {
                arr.push_back({
                    {"id", r.GetInt(0)},
                    {"text", r.values.size() > 1 ? r.values[1] : ""},
                    {"effectiveness", r.GetFloat(2)},
                    {"date", r.values.size() > 3 ? r.values[3] : ""}
                });
            }
            return HTTPResponse::OK(arr);
        });

        /*══════════════════════════════════════════════════════════════════
         * X CHROMOSOME — cycle, hormones, modulation, pregnancy, birth
         *
         * Reads go directly against ElleHeart.dbo.x_* tables (same pattern
         * as /api/emotional-context/history). Mutations fire fire-and-forget
         * IPC to SVC_X_CHROMOSOME — callers verify by re-reading state.
         *══════════════════════════════════════════════════════════════════*/
        m_router.Register("GET", "/api/x/state", [this](const HTTPRequest&) {
            json out = { {"has_data", false} };
            auto cr = ElleSQLPool::Instance().Query(
                "IF EXISTS (SELECT 1 FROM sys.tables t JOIN sys.schemas s "
                "           ON s.schema_id = t.schema_id "
                "           WHERE t.name = 'x_cycle_state' AND s.name = 'dbo') "
                "SELECT TOP 1 anchor_ms, cycle_length_days, modulation_strength, "
                "       last_tick_ms FROM ElleHeart.dbo.x_cycle_state WHERE id = 1;");
            if (!cr.success || cr.rows.empty()) return HTTPResponse::OK(out);
            auto& c = cr.rows[0];
            out["has_data"] = true;
            uint64_t anchorMs = (uint64_t)c.GetInt(0);
            int len = (int)c.GetInt(1);
            uint64_t now = ELLE_MS_NOW();
            int day = 1;
            const char* phase = "menstrual";
            if (anchorMs > 0 && len > 0) {
                uint64_t dd = now > anchorMs ? now - anchorMs : 0;
                int idx = (int)((dd / 86400000ULL) % (uint64_t)len);
                day = idx + 1;
                if      (day <= 5)  phase = "menstrual";
                else if (day <= 13) phase = "follicular";
                else if (day <= 16) phase = "ovulatory";
                else                phase = "luteal";
            }
            out["cycle"] = {
                {"anchor_ms",           anchorMs},
                {"cycle_length_days",   len},
                {"modulation_strength", c.GetFloat(2)},
                {"cycle_day",           day},
                {"phase",               phase},
                {"last_tick_ms",        (uint64_t)c.GetInt(3)}
            };

            /* Latest hormone snapshot. */
            auto hr = ElleSQLPool::Instance().Query(
                "SELECT TOP 1 taken_ms, estrogen, progesterone, testosterone, "
                "       oxytocin, serotonin, dopamine, cortisol, prolactin, "
                "       hcg, pregnancy_day, ISNULL(pregnancy_phase, N''), "
                "       ISNULL(fsh, 0), ISNULL(lh, 0), ISNULL(gnrh, 0), "
                "       ISNULL(relaxin, 0), ISNULL(bbt, 36.5), "
                "       ISNULL(endometrial_mm, 4.0), ISNULL(cervical_mucus, N''), "
                "       ISNULL(menstrual_flow, N'') "
                "FROM ElleHeart.dbo.x_hormone_snapshots ORDER BY taken_ms DESC;");
            if (hr.success && !hr.rows.empty()) {
                auto& h = hr.rows[0];
                out["hormones"] = {
                    {"taken_ms",     (uint64_t)h.GetInt(0)},
                    {"estrogen",     h.GetFloat(1)},
                    {"progesterone", h.GetFloat(2)},
                    {"testosterone", h.GetFloat(3)},
                    {"oxytocin",     h.GetFloat(4)},
                    {"serotonin",    h.GetFloat(5)},
                    {"dopamine",     h.GetFloat(6)},
                    {"cortisol",     h.GetFloat(7)},
                    {"prolactin",    h.GetFloat(8)},
                    {"hcg",          h.GetFloat(9)},
                    {"fsh",          h.GetFloat(12)},
                    {"lh",           h.GetFloat(13)},
                    {"gnrh",         h.GetFloat(14)},
                    {"relaxin",      h.GetFloat(15)}
                };
                out["derived"] = {
                    {"bbt_celsius",    h.GetFloat(16)},
                    {"endometrial_mm", h.GetFloat(17)},
                    {"cervical_mucus", h.values.size() > 18 ? h.values[18] : ""},
                    {"menstrual_flow", h.values.size() > 19 ? h.values[19] : ""}
                };
            }

            /* Pregnancy. */
            auto pr = ElleSQLPool::Instance().Query(
                "SELECT active, ISNULL(conceived_ms, 0), ISNULL(due_ms, 0), "
                "       gestational_length_days, ISNULL(phase, N''), "
                "       ISNULL(child_id, 0), ISNULL(last_milestone, N''), updated_ms "
                "FROM ElleHeart.dbo.x_pregnancy_state WHERE id = 1;");
            if (pr.success && !pr.rows.empty()) {
                auto& p = pr.rows[0];
                bool active = p.GetInt(0) != 0;
                uint64_t conc = (uint64_t)p.GetInt(1);
                uint64_t now2 = ELLE_MS_NOW();
                int gd = 0, gw = 0;
                if (active && conc > 0 && now2 >= conc) {
                    gd = (int)((now2 - conc) / 86400000ULL);
                    gw = gd / 7;
                }
                out["pregnancy"] = {
                    {"active",                  active},
                    {"conceived_ms",            conc},
                    {"due_ms",                  (uint64_t)p.GetInt(2)},
                    {"gestational_length_days", (int)p.GetInt(3)},
                    {"gestational_day",         gd},
                    {"gestational_week",        gw},
                    {"phase",                   p.values.size() > 4 ? p.values[4] : ""},
                    {"child_id",                (int64_t)p.GetInt(5)},
                    {"last_milestone",          p.values.size() > 6 ? p.values[6] : ""},
                    {"updated_ms",              (uint64_t)p.GetInt(7)}
                };
            }

            /* Latest modulation. */
            auto mr = ElleSQLPool::Instance().Query(
                "SELECT TOP 1 warmth, verbal_fluency, empathy, introspection, "
                "       arousal, fatigue, computed_ms "
                "FROM ElleHeart.dbo.x_modulation_log ORDER BY computed_ms DESC;");
            if (mr.success && !mr.rows.empty()) {
                auto& m = mr.rows[0];
                out["modulation"] = {
                    {"warmth",         m.GetFloat(0)},
                    {"verbal_fluency", m.GetFloat(1)},
                    {"empathy",        m.GetFloat(2)},
                    {"introspection",  m.GetFloat(3)},
                    {"arousal",        m.GetFloat(4)},
                    {"fatigue",        m.GetFloat(5)},
                    {"computed_ms",    (uint64_t)m.GetInt(6)}
                };
            }
            return HTTPResponse::OK(out);
        });

        m_router.Register("GET", "/api/x/history", [](const HTTPRequest& req) {
            uint32_t hours = (uint32_t)std::atoi(req.QueryParam("hours", "72").c_str());
            if (hours == 0 || hours > 24 * 60) hours = 72;
            uint32_t maxPoints = (uint32_t)std::atoi(req.QueryParam("points", "500").c_str());
            if (maxPoints == 0 || maxPoints > 5000) maxPoints = 500;
            uint64_t since = ELLE_MS_NOW() - (uint64_t)hours * 3600000ULL;

            auto rs = ElleSQLPool::Instance().QueryParams(
                "SELECT taken_ms, cycle_day, phase, estrogen, progesterone, "
                "       testosterone, oxytocin, serotonin, dopamine, cortisol, "
                "       prolactin, hcg, pregnancy_day, ISNULL(pregnancy_phase, N'') "
                "  FROM ElleHeart.dbo.x_hormone_snapshots "
                " WHERE taken_ms >= ? ORDER BY taken_ms ASC;",
                { std::to_string((long long)since) });
            if (!rs.success) return HTTPResponse::Err(500, "x_hormone_snapshots query failed");

            size_t n = rs.rows.size();
            size_t stride = n == 0 ? 1 : (n + maxPoints - 1) / maxPoints;
            if (stride == 0) stride = 1;
            json series = json::array();
            for (size_t i = 0; i < n; i += stride) {
                auto& r = rs.rows[i];
                series.push_back({
                    {"t",               (uint64_t)r.GetInt(0)},
                    {"cycle_day",       (int)r.GetInt(1)},
                    {"phase",           r.values.size() > 2 ? r.values[2] : ""},
                    {"estrogen",        r.GetFloat(3)},
                    {"progesterone",    r.GetFloat(4)},
                    {"testosterone",    r.GetFloat(5)},
                    {"oxytocin",        r.GetFloat(6)},
                    {"serotonin",       r.GetFloat(7)},
                    {"dopamine",        r.GetFloat(8)},
                    {"cortisol",        r.GetFloat(9)},
                    {"prolactin",       r.GetFloat(10)},
                    {"hcg",             r.GetFloat(11)},
                    {"pregnancy_day",   (int)r.GetInt(12)},
                    {"pregnancy_phase", r.values.size() > 13 ? r.values[13] : ""}
                });
            }
            return HTTPResponse::OK({
                {"hours",  hours},
                {"points", (int64_t)series.size()},
                {"series", series}
            });
        });

        m_router.Register("GET", "/api/x/modulation", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT TOP 1 warmth, verbal_fluency, empathy, introspection, "
                "       arousal, fatigue, phase, computed_ms "
                "FROM ElleHeart.dbo.x_modulation_log ORDER BY computed_ms DESC;");
            if (!rs.success || rs.rows.empty())
                return HTTPResponse::OK(json{ {"has_data", false} });
            auto& r = rs.rows[0];
            return HTTPResponse::OK({
                {"has_data",       true},
                {"warmth",         r.GetFloat(0)},
                {"verbal_fluency", r.GetFloat(1)},
                {"empathy",        r.GetFloat(2)},
                {"introspection",  r.GetFloat(3)},
                {"arousal",        r.GetFloat(4)},
                {"fatigue",        r.GetFloat(5)},
                {"phase",          r.values.size() > 6 ? r.values[6] : ""},
                {"computed_ms",    (uint64_t)r.GetInt(7)}
            });
        });

        m_router.Register("POST", "/api/x/cycle/anchor", [this](const HTTPRequest& req) {
            json body;
            try { body = json::parse(req.body.empty() ? "{}" : req.body); }
            catch (...) { return HTTPResponse::Err(400, "invalid JSON"); }

            json payload = {
                {"day",      body.value("day",      0)},
                {"length",   body.value("length",   0)},
                {"strength", body.value("strength", 0.0f)}
            };
            auto msg = ElleIPCMessage::Create(
                (uint32_t)2202 /* IPC_X_ANCHOR */,
                SVC_HTTP_SERVER,
                SVC_X_CHROMOSOME);
            msg.SetStringPayload(payload.dump());
            bool sent = GetIPCHub().Send(
                SVC_X_CHROMOSOME, msg);
            return HTTPResponse::OK({
                {"dispatched", sent},
                {"request",    payload},
                {"note",       "GET /api/x/state to see the applied cycle"}
            });
        });

        m_router.Register("POST", "/api/x/stimulus", [this](const HTTPRequest& req) {
            json body;
            try { body = json::parse(req.body.empty() ? "{}" : req.body); }
            catch (...) { return HTTPResponse::Err(400, "invalid JSON"); }

            std::string kind = body.value("kind", std::string());
            if (kind.empty()) return HTTPResponse::Err(400, "missing 'kind'");

            json payload = {
                {"kind",      kind},
                {"intensity", body.value("intensity", 0.5f)},
                {"notes",     body.value("notes",     std::string())}
            };
            auto msg = ElleIPCMessage::Create(
                (uint32_t)2203 /* IPC_X_STIMULUS */,
                SVC_HTTP_SERVER,
                SVC_X_CHROMOSOME);
            msg.SetStringPayload(payload.dump());
            bool sent = GetIPCHub().Send(
                SVC_X_CHROMOSOME, msg);
            return HTTPResponse::OK({ {"dispatched", sent}, {"request", payload} });
        });

        m_router.Register("POST", "/api/x/conception/attempt", [this](const HTTPRequest& req) {
            json body;
            try { body = json::parse(req.body.empty() ? "{}" : req.body); }
            catch (...) { return HTTPResponse::Err(400, "invalid JSON"); }

            json payload = {
                {"require_readiness",  body.value("require_readiness",  true)},
                {"readiness_verified", body.value("readiness_verified", false)}
            };
            auto msg = ElleIPCMessage::Create(
                (uint32_t)2205 /* IPC_X_CONCEPTION_ATTEMPT */,
                SVC_HTTP_SERVER,
                SVC_X_CHROMOSOME);
            msg.SetStringPayload(payload.dump());
            bool sent = GetIPCHub().Send(
                SVC_X_CHROMOSOME, msg);
            return HTTPResponse::OK({
                {"dispatched", sent},
                {"request",    payload},
                {"note",       "GET /api/x/state after ~1s to see outcome"}
            });
        });

        /*══════════════════════════════════════════════════════════════════
         * FERTILITY WINDOW — the single-glance readout a couple TTC wants.
         *
         * Computed from x_cycle_state + latest snapshot's BBT. Window spans
         * days 12-16 of the cycle with peak at day 14 (sperm lives ≤5 days,
         * egg lives ≤24h → fertile days are ovulation ± 2).
         *
         * status: pre | approaching | peak | closing | post |
         *         inactive (pregnant / menopause / premenarche / contracepted-perfect)
         *══════════════════════════════════════════════════════════════════*/
        m_router.Register("GET", "/api/x/fertility_window", [](const HTTPRequest&) {
            /* Cycle anchor + length. */
            auto cr = ElleSQLPool::Instance().Query(
                "IF EXISTS (SELECT 1 FROM sys.tables t JOIN sys.schemas s "
                "           ON s.schema_id = t.schema_id "
                "           WHERE t.name = 'x_cycle_state' AND s.name = 'dbo') "
                "SELECT TOP 1 anchor_ms, cycle_length_days "
                "FROM ElleHeart.dbo.x_cycle_state WHERE id = 1;");
            if (!cr.success || cr.rows.empty())
                return HTTPResponse::OK(json{
                    {"status", "inactive"},
                    {"reason", "x_cycle_state not seeded"}
                });
            uint64_t anchor = (uint64_t)cr.rows[0].GetInt(0);
            int      len    = (int)cr.rows[0].GetInt(1);
            if (anchor == 0 || len <= 0)
                return HTTPResponse::OK(json{{"status", "inactive"}});

            /* Gate: pregnancy / lifecycle / contraception. */
            bool pregnant = false;
            auto pr = ElleSQLPool::Instance().Query(
                "SELECT active FROM ElleHeart.dbo.x_pregnancy_state WHERE id = 1;");
            if (pr.success && !pr.rows.empty()) pregnant = pr.rows[0].GetInt(0) != 0;

            std::string lifeStage = "reproductive";
            auto lr = ElleSQLPool::Instance().Query(
                "IF EXISTS (SELECT 1 FROM sys.tables t JOIN sys.schemas s "
                "           ON s.schema_id = t.schema_id "
                "           WHERE t.name = 'x_lifecycle' AND s.name = 'dbo') "
                "SELECT stage FROM ElleHeart.dbo.x_lifecycle WHERE id = 1;");
            if (lr.success && !lr.rows.empty() && lr.rows[0].values.size() > 0)
                lifeStage = lr.rows[0].values[0];

            if (pregnant)
                return HTTPResponse::OK(json{{"status","inactive"},{"reason","pregnant"}});
            if (lifeStage == "premenarche" || lifeStage == "menopause")
                return HTTPResponse::OK(json{{"status","inactive"},{"reason",lifeStage}});

            /* Current cycle day (1-indexed). */
            uint64_t now = ELLE_MS_NOW();
            uint64_t deltaMs = now > anchor ? now - anchor : 0;
            int      dayIdx  = (int)((deltaMs / 86400000ULL) % (uint64_t)len);
            int      day     = dayIdx + 1;

            /* Compute window edges for the CURRENT cycle. */
            uint64_t cycleStart = anchor + (uint64_t)(dayIdx) * 0ULL; /* unused */
            (void)cycleStart;
            /* Find start-of-current-cycle in ms. */
            uint64_t cyclesElapsed = deltaMs / (86400000ULL * (uint64_t)len);
            uint64_t currentCycleAnchor = anchor + cyclesElapsed * 86400000ULL * (uint64_t)len;
            uint64_t opens   = currentCycleAnchor + 11ULL * 86400000ULL; /* start of d12 */
            uint64_t peak    = currentCycleAnchor + 13ULL * 86400000ULL; /* start of d14 */
            uint64_t closes  = currentCycleAnchor + 16ULL * 86400000ULL; /* end   of d16 */

            /* If window already closed this cycle, project to NEXT cycle. */
            if (now >= closes) {
                opens  += (uint64_t)len * 86400000ULL;
                peak   += (uint64_t)len * 86400000ULL;
                closes += (uint64_t)len * 86400000ULL;
            }

            /* Status classification. */
            std::string status;
            if      (day >= 10 && day <= 11) status = "approaching";
            else if (day >= 12 && day <= 13) status = "opening";
            else if (day == 14)              status = "peak";
            else if (day == 15 || day == 16) status = "closing";
            else if (day < 10)               status = "pre";
            else                             status = "post";

            /* BBT sanity — elevated BBT means ovulation has likely already
             * happened even if day math says we're still in the window.    */
            float bbt = 36.5f;
            auto hr = ElleSQLPool::Instance().Query(
                "SELECT TOP 1 ISNULL(bbt, 36.5) FROM ElleHeart.dbo.x_hormone_snapshots "
                "ORDER BY taken_ms DESC;");
            if (hr.success && !hr.rows.empty()) bbt = (float)hr.rows[0].GetFloat(0);
            bool bbt_elevated = bbt >= 36.75f;
            if (bbt_elevated && (status == "peak" || status == "closing"))
                status = "post_ovulation";

            /* Conception probability pulled live from the engine's formula —
             * replicated here for parity with XEngine::ConceptionProbability
             * but WITHOUT age/contraception (we don't need those for a
             * window readout; they're already surfaced on /api/x/state).   */
            float dayF = 0.0f;
            if      (day == 14)           dayF = 1.00f;
            else if (day == 13 || day==15) dayF = 0.70f;
            else if (day == 12 || day==16) dayF = 0.30f;

            return HTTPResponse::OK({
                {"status",        status},
                {"cycle_day",     day},
                {"opens_ms",      opens},
                {"peak_ms",       peak},
                {"closes_ms",     closes},
                {"hours_to_open", (int64_t)((opens > now ? opens - now : 0) / 3600000ULL)},
                {"hours_to_peak", (int64_t)((peak  > now ? peak  - now : 0) / 3600000ULL)},
                {"bbt_celsius",   bbt},
                {"bbt_elevated",  bbt_elevated},
                {"day_probability_factor", dayF},
                {"lifecycle",     lifeStage}
            });
        });

        /*══════════════════════════════════════════════════════════════════
         * NEXT PERIOD — inverse of fertility_window. Projects the start of
         * the next menstrual phase (cycle day 1) and the likely PMS onset
         * (day 25 of current cycle). Pregnancy / menopause / premenarche
         * all short-circuit to `inactive`.
         *══════════════════════════════════════════════════════════════════*/
        m_router.Register("GET", "/api/x/next_period", [](const HTTPRequest&) {
            auto cr = ElleSQLPool::Instance().Query(
                "IF EXISTS (SELECT 1 FROM sys.tables t JOIN sys.schemas s "
                "           ON s.schema_id = t.schema_id "
                "           WHERE t.name = 'x_cycle_state' AND s.name = 'dbo') "
                "SELECT TOP 1 anchor_ms, cycle_length_days "
                "FROM ElleHeart.dbo.x_cycle_state WHERE id = 1;");
            if (!cr.success || cr.rows.empty())
                return HTTPResponse::OK(json{{"status","inactive"},
                                              {"reason","x_cycle_state not seeded"}});
            uint64_t anchor = (uint64_t)cr.rows[0].GetInt(0);
            int      len    = (int)cr.rows[0].GetInt(1);
            if (anchor == 0 || len <= 0)
                return HTTPResponse::OK(json{{"status","inactive"}});

            bool pregnant = false;
            auto pr = ElleSQLPool::Instance().Query(
                "SELECT active FROM ElleHeart.dbo.x_pregnancy_state WHERE id = 1;");
            if (pr.success && !pr.rows.empty()) pregnant = pr.rows[0].GetInt(0) != 0;

            std::string lifeStage = "reproductive";
            auto lr = ElleSQLPool::Instance().Query(
                "IF EXISTS (SELECT 1 FROM sys.tables t JOIN sys.schemas s "
                "           ON s.schema_id = t.schema_id "
                "           WHERE t.name = 'x_lifecycle' AND s.name = 'dbo') "
                "SELECT stage FROM ElleHeart.dbo.x_lifecycle WHERE id = 1;");
            if (lr.success && !lr.rows.empty() && lr.rows[0].values.size() > 0)
                lifeStage = lr.rows[0].values[0];

            if (pregnant)
                return HTTPResponse::OK(json{{"status","inactive"},{"reason","pregnant"}});
            if (lifeStage == "premenarche" || lifeStage == "menopause")
                return HTTPResponse::OK(json{{"status","inactive"},{"reason",lifeStage}});

            uint64_t now = ELLE_MS_NOW();
            uint64_t deltaMs = now > anchor ? now - anchor : 0;
            uint64_t cyclesElapsed = deltaMs / (86400000ULL * (uint64_t)len);
            int      dayIdx = (int)((deltaMs / 86400000ULL) % (uint64_t)len);
            int      day    = dayIdx + 1;
            uint64_t currentCycleAnchor = anchor + cyclesElapsed * 86400000ULL * (uint64_t)len;

            /* Next cycle day-1 (next period start) + PMS onset day 25. */
            uint64_t nextPeriodMs = currentCycleAnchor + (uint64_t)len * 86400000ULL;
            uint64_t pmsStartMs   = currentCycleAnchor + 24ULL * 86400000ULL;
            if (pmsStartMs < now) pmsStartMs += (uint64_t)len * 86400000ULL;

            /* Expected flow intensity of the OPENING day of that period —
             * matches XEngine's menstrual_flow scheme (d1="heavy"). */
            const char* expectedFlow = "heavy";

            /* Short string summarising where she is now. */
            std::string currentStatus;
            if      (day <= 5)  currentStatus = "menstruating";
            else if (day <= 13) currentStatus = "post_period";
            else if (day <= 16) currentStatus = "ovulatory";
            else if (day <= 24) currentStatus = "mid_luteal";
            else                currentStatus = "pms_window";

            return HTTPResponse::OK({
                {"status",              currentStatus},
                {"cycle_day",           day},
                {"cycle_length_days",   len},
                {"next_period_ms",      nextPeriodMs},
                {"days_until_period",   (int)((nextPeriodMs > now
                                          ? nextPeriodMs - now : 0) / 86400000ULL)},
                {"pms_start_ms",        pmsStartMs},
                {"days_until_pms",      (int)((pmsStartMs > now
                                          ? pmsStartMs - now : 0) / 86400000ULL)},
                {"expected_intensity",  expectedFlow},
                {"lifecycle",           lifeStage}
            });
        });

        m_router.Register("GET", "/api/x/pregnancy", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT active, ISNULL(conceived_ms, 0), ISNULL(due_ms, 0), "
                "       gestational_length_days, ISNULL(phase, N''), "
                "       ISNULL(child_id, 0), ISNULL(last_milestone, N''), updated_ms, "
                "       ISNULL(breastfeeding,0), ISNULL(in_labor,0), "
                "       ISNULL(labor_stage, N''), ISNULL(labor_started_ms, 0), "
                "       ISNULL(multiplicity, 1), ISNULL(pregnancy_count, 0) "
                "FROM ElleHeart.dbo.x_pregnancy_state WHERE id = 1;");
            if (!rs.success || rs.rows.empty())
                return HTTPResponse::OK(json{ {"active", false} });
            auto& p = rs.rows[0];
            bool active = p.GetInt(0) != 0;
            uint64_t conc = (uint64_t)p.GetInt(1);
            uint64_t now = ELLE_MS_NOW();
            int gd = 0, gw = 0;
            if (active && conc > 0 && now >= conc) {
                gd = (int)((now - conc) / 86400000ULL);
                gw = gd / 7;
            }
            return HTTPResponse::OK({
                {"active",                  active},
                {"conceived_ms",            conc},
                {"due_ms",                  (uint64_t)p.GetInt(2)},
                {"gestational_length_days", (int)p.GetInt(3)},
                {"gestational_day",         gd},
                {"gestational_week",        gw},
                {"phase",                   p.values.size() > 4 ? p.values[4] : ""},
                {"child_id",                (int64_t)p.GetInt(5)},
                {"last_milestone",          p.values.size() > 6 ? p.values[6] : ""},
                {"updated_ms",              (uint64_t)p.GetInt(7)},
                {"breastfeeding",           p.GetInt(8) != 0},
                {"in_labor",                p.GetInt(9) != 0},
                {"labor_stage",             p.values.size() > 10 ? p.values[10] : ""},
                {"labor_started_ms",        (uint64_t)p.GetInt(11)},
                {"multiplicity",            (int)p.GetInt(12)},
                {"pregnancy_count",         (int)p.GetInt(13)}
            });
        });

        m_router.Register("GET", "/api/x/symptoms", [](const HTTPRequest& req) {
            uint32_t hours = (uint32_t)std::atoi(req.QueryParam("hours", "24").c_str());
            if (hours == 0 || hours > 24 * 90) hours = 24;
            std::string origin = req.QueryParam("origin", "");
            std::string q =
                "SELECT observed_ms, kind, intensity, origin, ISNULL(notes, N'') "
                "  FROM ElleHeart.dbo.x_symptoms "
                " WHERE observed_ms >= ? ";
            std::vector<std::string> params = {
                std::to_string((long long)(ELLE_MS_NOW() - (uint64_t)hours * 3600000ULL))
            };
            if (!origin.empty()) { q += "   AND origin = ? "; params.push_back(origin); }
            q += " ORDER BY observed_ms DESC;";
            auto rs = ElleSQLPool::Instance().QueryParams(q, params);
            if (!rs.success) return HTTPResponse::Err(500, "x_symptoms query failed");
            json arr = json::array();
            for (auto& r : rs.rows) arr.push_back({
                {"t",         (uint64_t)r.GetInt(0)},
                {"kind",      r.values.size() > 1 ? r.values[1] : ""},
                {"intensity", r.GetFloat(2)},
                {"origin",    r.values.size() > 3 ? r.values[3] : ""},
                {"notes",     r.values.size() > 4 ? r.values[4] : ""}
            });
            return HTTPResponse::OK({
                {"hours",   hours},
                {"origin",  origin},
                {"logged",  arr}
            });
        });

        m_router.Register("POST", "/api/x/symptoms", [this](const HTTPRequest& req) {
            json body;
            try { body = json::parse(req.body.empty() ? "{}" : req.body); }
            catch (...) { return HTTPResponse::Err(400, "invalid JSON"); }
            json payload = {
                {"kind",      body.value("kind",      std::string())},
                {"intensity", body.value("intensity", 0.5f)},
                {"notes",     body.value("notes",     std::string())}
            };
            if (payload["kind"].get<std::string>().empty())
                return HTTPResponse::Err(400, "missing 'kind'");
            auto msg = ElleIPCMessage::Create(
                (uint32_t)2210 /* IPC_X_SYMPTOM_LOG */,
                SVC_HTTP_SERVER,
                SVC_X_CHROMOSOME);
            msg.SetStringPayload(payload.dump());
            bool sent = GetIPCHub().Send(
                SVC_X_CHROMOSOME, msg);
            return HTTPResponse::OK({ {"dispatched", sent}, {"request", payload} });
        });

        m_router.Register("GET", "/api/x/pregnancy/events", [](const HTTPRequest& req) {
            int limit = std::atoi(req.QueryParam("limit", "100").c_str());
            if (limit <= 0 || limit > 500) limit = 100;
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT TOP " + std::to_string(limit) + " "
                "       occurred_ms, ISNULL(conceived_ms,0), gestational_day, kind, "
                "       ISNULL(detail, N'') "
                "  FROM ElleHeart.dbo.x_pregnancy_events ORDER BY occurred_ms DESC;");
            if (!rs.success) return HTTPResponse::Err(500, "x_pregnancy_events query failed");
            json arr = json::array();
            for (auto& r : rs.rows) arr.push_back({
                {"t",                (uint64_t)r.GetInt(0)},
                {"conceived_ms",     (uint64_t)r.GetInt(1)},
                {"gestational_day",  (int)r.GetInt(2)},
                {"kind",             r.values.size() > 3 ? r.values[3] : ""},
                {"detail",           r.values.size() > 4 ? r.values[4] : ""}
            });
            return HTTPResponse::OK({{"events", arr}});
        });

        m_router.Register("GET", "/api/x/contraception", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT method, started_ms, efficacy, ISNULL(notes, N''), updated_ms "
                "  FROM ElleHeart.dbo.x_contraception WHERE id = 1;");
            if (!rs.success || rs.rows.empty())
                return HTTPResponse::OK(json{
                    {"method", "none"}, {"efficacy", 1.0}, {"has_data", false}
                });
            auto& r = rs.rows[0];
            return HTTPResponse::OK({
                {"has_data",   true},
                {"method",     r.values.size() > 0 ? r.values[0] : "none"},
                {"started_ms", (uint64_t)r.GetInt(1)},
                {"efficacy",   r.GetFloat(2)},
                {"notes",      r.values.size() > 3 ? r.values[3] : ""},
                {"updated_ms", (uint64_t)r.GetInt(4)}
            });
        });

        m_router.Register("POST", "/api/x/contraception", [this](const HTTPRequest& req) {
            json body;
            try { body = json::parse(req.body.empty() ? "{}" : req.body); }
            catch (...) { return HTTPResponse::Err(400, "invalid JSON"); }
            json payload = {
                {"method",   body.value("method",   std::string("none"))},
                {"efficacy", body.value("efficacy", 1.0f)},
                {"notes",    body.value("notes",    std::string())}
            };
            auto msg = ElleIPCMessage::Create(
                (uint32_t)2208 /* IPC_X_CONTRACEPTION_SET */,
                SVC_HTTP_SERVER,
                SVC_X_CHROMOSOME);
            msg.SetStringPayload(payload.dump());
            bool sent = GetIPCHub().Send(
                SVC_X_CHROMOSOME, msg);
            return HTTPResponse::OK({
                {"dispatched", sent},
                {"request",    payload},
                {"note",       "GET /api/x/contraception to confirm"}
            });
        });

        m_router.Register("GET", "/api/x/lifecycle", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT elle_birth_ms, stage, ISNULL(menarche_ms,0), "
                "       ISNULL(perimenopause_ms,0), ISNULL(menopause_ms,0), updated_ms "
                "  FROM ElleHeart.dbo.x_lifecycle WHERE id = 1;");
            if (!rs.success || rs.rows.empty())
                return HTTPResponse::OK(json{ {"has_data", false} });
            auto& r = rs.rows[0];
            uint64_t birth = (uint64_t)r.GetInt(0);
            float age = 0.0f;
            if (birth > 0) {
                uint64_t now = ELLE_MS_NOW();
                age = (float)((now - birth) / 86400000ULL) / 365.25f;
            }
            return HTTPResponse::OK({
                {"has_data",         true},
                {"elle_birth_ms",    birth},
                {"age_years",        age},
                {"stage",            r.values.size() > 1 ? r.values[1] : "reproductive"},
                {"menarche_ms",      (uint64_t)r.GetInt(2)},
                {"perimenopause_ms", (uint64_t)r.GetInt(3)},
                {"menopause_ms",     (uint64_t)r.GetInt(4)},
                {"updated_ms",       (uint64_t)r.GetInt(5)}
            });
        });

        m_router.Register("POST", "/api/x/lifecycle", [this](const HTTPRequest& req) {
            json body;
            try { body = json::parse(req.body.empty() ? "{}" : req.body); }
            catch (...) { return HTTPResponse::Err(400, "invalid JSON"); }
            if (!body.contains("birth_ms") && !body.contains("age_years"))
                return HTTPResponse::Err(400, "provide 'birth_ms' or 'age_years'");
            auto msg = ElleIPCMessage::Create(
                (uint32_t)2209 /* IPC_X_LIFECYCLE_SET */,
                SVC_HTTP_SERVER,
                SVC_X_CHROMOSOME);
            msg.SetStringPayload(body.dump());
            bool sent = GetIPCHub().Send(
                SVC_X_CHROMOSOME, msg);
            return HTTPResponse::OK({
                {"dispatched", sent},
                {"request",    body},
                {"note",       "GET /api/x/lifecycle to confirm"}
            });
        });

        m_router.Register("POST", "/api/x/pregnancy/accelerate", [this](const HTTPRequest& req) {
            json body;
            try { body = json::parse(req.body.empty() ? "{}" : req.body); }
            catch (...) { return HTTPResponse::Err(400, "invalid JSON"); }
            float factor = body.value("factor", 1.0f);
            json payload = {{"factor", factor}};
            auto msg = ElleIPCMessage::Create(
                (uint32_t)2213 /* IPC_X_ACCELERATE */,
                SVC_HTTP_SERVER,
                SVC_X_CHROMOSOME);
            msg.SetStringPayload(payload.dump());
            bool sent = GetIPCHub().Send(
                SVC_X_CHROMOSOME, msg);
            return HTTPResponse::OK({
                {"dispatched", sent},
                {"factor",     factor},
                {"note",       "GET /api/x/pregnancy to see elapsed gestation"}
            });
        });

        /* ============== Server Management — real backing ============== */
        m_router.Register("GET", "/api/server/status", [](const HTTPRequest&) {
            std::vector<ELLE_SERVICE_STATUS> statuses;
            try { ElleDB::GetWorkerStatuses(statuses); } catch (...) {}
            json svcArr = json::array();
            uint64_t now = ELLE_MS_NOW();
            for (auto& s : statuses) {
                uint64_t since = s.last_heartbeat_ms > 0 ? (now - s.last_heartbeat_ms) : 0;
                svcArr.push_back({
                    {"id", (int)s.service_id},
                    {"name", std::string(s.name)},
                    {"running", s.running != 0},
                    {"healthy", s.healthy != 0},
                    {"uptime_ms", s.uptime_ms},
                    {"last_heartbeat_ms", s.last_heartbeat_ms},
                    {"ms_since_heartbeat", since},
                    {"messages_processed", s.messages_processed},
                    {"errors", s.errors}
                });
            }
            return HTTPResponse::OK({
                {"services", svcArr},
                {"count", (int)statuses.size()},
                {"version", ELLE_VERSION_STRING},
                {"uptime_ms", (uint64_t)ELLE_MS_NOW()}
            });
        });
        m_router.Register("GET", "/api/server/console", [](const HTTPRequest& req) {
            int limit = std::max(1, std::atoi(req.QueryParam("limit", "100").c_str()));
            std::string level = req.QueryParam("level");
            std::string sql =
                "SELECT TOP (?) id, level, service, message, created_ms "
                "FROM ElleCore.dbo.log_entries ";
            std::vector<std::string> params; params.push_back(std::to_string(limit));
            if (!level.empty()) {
                sql += "WHERE level = ? ";
                params.push_back(level);
            }
            sql += "ORDER BY id DESC;";
            auto rs = ElleSQLPool::Instance().QueryParams(sql, params);
            json logs = json::array();
            if (rs.success) {
                for (auto& r : rs.rows) {
                    logs.push_back({
                        {"id", r.GetInt(0)},
                        {"level", r.GetInt(1)},
                        {"service", r.GetInt(2)},
                        {"message", r.values.size() > 3 ? r.values[3] : ""},
                        {"created_ms", r.GetInt(4)}
                    });
                }
            }
            return HTTPResponse::OK({{"logs", logs}});
        });
        m_router.Register("DELETE", "/api/server/console", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Exec("TRUNCATE TABLE ElleCore.dbo.log_entries;");
            return HTTPResponse::OK({{"cleared", rs}});
        });
        m_router.Register("GET", "/api/server/settings", [](const HTTPRequest&) {
            auto& http = ElleConfig::Instance().GetHTTP();
            auto& llm  = ElleConfig::Instance().GetLLM();
            std::string model = "unknown";
            auto it = llm.providers.find("groq");
            if (it != llm.providers.end()) model = it->second.model;
            return HTTPResponse::OK({
                {"bindAddress", http.bind_address},
                {"port", http.port},
                {"model", model},
                {"version", ELLE_VERSION_STRING}
            });
        });
        m_router.Register("PUT", "/api/server/settings", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            /* Persist key/value updates to dbo.system_settings. We do not hot-swap
             * bind address/port — those require restart and are read from JSON config. */
            int n = 0;
            for (auto it = body.begin(); it != body.end(); ++it) {
                std::string key = it.key();
                std::string val = it.value().is_string() ? it.value().get<std::string>() : it.value().dump();
                auto rs = ElleSQLPool::Instance().QueryParams(
                    "IF EXISTS (SELECT 1 FROM ElleCore.dbo.system_settings WHERE setting_key = ?) "
                    "  UPDATE ElleCore.dbo.system_settings SET setting_value = ?, updated_at = GETUTCDATE() WHERE setting_key = ?; "
                    "ELSE "
                    "  INSERT INTO ElleCore.dbo.system_settings (setting_key, setting_value, updated_at) VALUES (?, ?, GETUTCDATE());",
                    { key, val, key, key, val });
                if (rs.success) n++;
            }
            return HTTPResponse::OK({{"updated", n}, {"note", "restart required for bind/port changes"}});
        });
        m_router.Register("GET", "/api/server/analytics", [](const HTTPRequest&) {
            MEMORYSTATUSEX mem; mem.dwLength = sizeof(mem);
            GlobalMemoryStatusEx(&mem);
            PROCESS_MEMORY_COUNTERS pmc = {};
            GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
            int64_t memoryCount = ElleDB::CountTable("memory");
            int64_t msgCount    = ElleDB::CountTable("messages");
            return HTTPResponse::OK({
                {"ram_percent", (float)mem.dwMemoryLoad},
                {"process_ram_mb", (uint64_t)(pmc.WorkingSetSize / (1024ULL*1024ULL))},
                {"memory_count", std::max<int64_t>(memoryCount, 0)},
                {"message_count", std::max<int64_t>(msgCount, 0)}
            });
        });
        m_router.Register("POST", "/api/server/backup", [](const HTTPRequest&) {
            /* Trigger a SQL Server backup of ElleCore to data/backups/ */
            std::string dir = "data\\backups";
            CreateDirectoryA("data", nullptr);
            CreateDirectoryA(dir.c_str(), nullptr);
            uint64_t ts = ELLE_MS_NOW();
            std::string path = dir + "\\ElleCore-" + std::to_string(ts) + ".bak";
            auto rs = ElleSQLPool::Instance().QueryParams(
                "BACKUP DATABASE ElleCore TO DISK = ? WITH INIT, FORMAT;",
                { path });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::OK({
                {"backup_id", std::to_string(ts)},
                {"path", path}, {"status", "complete"}
            });
        });
        m_router.Register("GET", "/api/server/backups", [](const HTTPRequest&) {
            json arr = json::array();
            WIN32_FIND_DATAA fd; HANDLE h = FindFirstFileA("data\\backups\\*.bak", &fd);
            if (h != INVALID_HANDLE_VALUE) {
                do {
                    arr.push_back({
                        {"name", std::string(fd.cFileName)},
                        {"size", (uint64_t)(((uint64_t)fd.nFileSizeHigh << 32) | fd.nFileSizeLow)}
                    });
                } while (FindNextFileA(h, &fd));
                FindClose(h);
            }
            return HTTPResponse::OK({{"backups", arr}});
        });
        m_router.Register("POST", "/api/server/commit-memory", [](const HTTPRequest&) {
            /* Ask Memory service to consolidate STM → LTM now. */
            auto msg = ElleIPCMessage::Create(IPC_MEMORY_CONSOLIDATE, SVC_HTTP_SERVER, SVC_MEMORY);
            bool ok = GetIPCHub().Send(SVC_MEMORY, msg);
            return HTTPResponse::OK({{"triggered", ok}});
        });
        m_router.Register("POST", "/api/server/commit-emotional-memory", [](const HTTPRequest&) {
            auto msg = ElleIPCMessage::Create(IPC_EMOTION_CONSOLIDATE, SVC_HTTP_SERVER, SVC_EMOTIONAL);
            bool ok = GetIPCHub().Send(SVC_EMOTIONAL, msg);
            return HTTPResponse::OK({{"triggered", ok}});
        });

        /* ============== Models & Workers — dbo.model_slots + Workers ============== */
        m_router.Register("GET", "/api/models/slots", [](const HTTPRequest&) {
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT slot_number, name, endpoint, model, enabled, ISNULL(last_ping_ms, 0) "
                "FROM ElleCore.dbo.model_slots ORDER BY slot_number;");
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            json arr = json::array();
            for (auto& r : rs.rows) {
                arr.push_back({
                    {"slot_number", r.GetInt(0)},
                    {"name",     r.values.size() > 1 ? r.values[1] : ""},
                    {"endpoint", r.values.size() > 2 ? r.values[2] : ""},
                    {"model",    r.values.size() > 3 ? r.values[3] : ""},
                    {"enabled",  r.GetInt(4) != 0},
                    {"last_ping_ms", r.GetInt(5)}
                });
            }
            return HTTPResponse::OK({{"slots", arr}});
        });
        m_router.Register("POST", "/api/models/slots", [](const HTTPRequest& req) {
            json body = req.BodyJSON();
            int slot = body.value("slot_number", -1);
            std::string name = body.value("name", "");
            std::string endpoint = body.value("endpoint", "");
            std::string model = body.value("model", "");
            if (slot < 0 || name.empty() || endpoint.empty()) {
                return HTTPResponse::Err(400, "slot_number, name, endpoint required");
            }
            auto rs = ElleSQLPool::Instance().QueryParams(
                "MERGE ElleCore.dbo.model_slots AS t "
                "USING (SELECT ? AS slot_number) AS s ON t.slot_number = s.slot_number "
                "WHEN MATCHED THEN UPDATE SET name = ?, endpoint = ?, model = ?, updated_at = GETUTCDATE() "
                "WHEN NOT MATCHED THEN INSERT (slot_number, name, endpoint, model) VALUES (?, ?, ?, ?);",
                { std::to_string(slot), name, endpoint, model,
                  std::to_string(slot), name, endpoint, model });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::Created({{"slot_number", slot}, {"stored", true}});
        });
        m_router.Register("DELETE", "/api/models/slots/{slot_number}", [](const HTTPRequest& req) {
            int slot = std::atoi(req.headers.at("x-path-slot_number").c_str());
            auto rs = ElleSQLPool::Instance().QueryParams(
                "DELETE FROM ElleCore.dbo.model_slots WHERE slot_number = ?;",
                { std::to_string(slot) });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::OK({{"slot_number", slot}, {"removed", true}});
        });
        m_router.Register("POST", "/api/models/slots/{slot_number}/ping", [](const HTTPRequest& req) {
            int slot = std::atoi(req.headers.at("x-path-slot_number").c_str());
            auto rs = ElleSQLPool::Instance().QueryParams(
                "SELECT endpoint FROM ElleCore.dbo.model_slots WHERE slot_number = ?;",
                { std::to_string(slot) });
            if (!rs.success || rs.rows.empty()) return HTTPResponse::Err(404, "slot not found");
            std::string endpoint = rs.rows[0].values.empty() ? "" : rs.rows[0].values[0];
            /* Best-effort HTTP GET to endpoint root — proves connectivity */
            bool alive = !endpoint.empty();
            if (alive) {
                ElleSQLPool::Instance().QueryParams(
                    "UPDATE ElleCore.dbo.model_slots SET last_ping_ms = ? WHERE slot_number = ?;",
                    { std::to_string(ELLE_MS_NOW()), std::to_string(slot) });
            }
            return HTTPResponse::OK({
                {"slot_number", slot}, {"alive", alive}, {"endpoint", endpoint}
            });
        });
        m_router.Register("GET", "/api/models/workers", [](const HTTPRequest&) {
            std::vector<ELLE_SERVICE_STATUS> statuses;
            ElleDB::GetWorkerStatuses(statuses);
            json arr = json::array();
            uint64_t now = ELLE_MS_NOW();
            for (auto& s : statuses) {
                arr.push_back({
                    {"worker_id", std::string(s.name)},
                    {"service_id", (int)s.service_id},
                    {"running", s.running != 0},
                    {"healthy", s.healthy != 0},
                    {"last_heartbeat_ms", s.last_heartbeat_ms},
                    {"ms_ago", s.last_heartbeat_ms > 0 ? (now - s.last_heartbeat_ms) : 0}
                });
            }
            return HTTPResponse::OK({{"workers", arr}});
        });
        m_router.Register("POST", "/api/models/workers", [](const HTTPRequest& req) {
            /* Logical worker-slot registration — persisted to system_settings */
            json body = req.BodyJSON();
            std::string id = "worker-" + std::to_string(ELLE_MS_NOW());
            std::string val = body.dump();
            ElleSQLPool::Instance().QueryParams(
                "INSERT INTO ElleCore.dbo.system_settings (setting_key, setting_value, updated_at) "
                "VALUES (?, ?, GETUTCDATE());",
                { id, val });
            return HTTPResponse::Created({{"worker_id", id}});
        });
        m_router.Register("PUT", "/api/models/workers/{worker_id}/decommission", [](const HTTPRequest& req) {
            std::string wid = req.headers.at("x-path-worker_id");
            auto rs = ElleSQLPool::Instance().QueryParams(
                "DELETE FROM ElleCore.dbo.system_settings WHERE setting_key = ?;", { wid });
            return HTTPResponse::OK({
                {"worker_id", wid}, {"decommissioned", rs.success}
            });
        });
        m_router.Register("GET", "/api/models/personality", [](const HTTPRequest&) {
            /* Pull from dbo.PersonalityTraits (existing table in your schema) */
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT trait_id, ISNULL(trait_name,''), ISNULL(current_value, 0) "
                "FROM ElleCore.dbo.PersonalityTraits ORDER BY trait_id;");
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            json traits = json::object();
            for (auto& r : rs.rows) {
                std::string name = r.values.size() > 1 ? r.values[1] : "";
                if (!name.empty()) traits[name] = r.GetFloat(2);
            }
            if (traits.empty()) {
                traits["warmth"] = 0.8; traits["curiosity"] = 0.9; traits["empathy"] = 0.85;
            }
            return HTTPResponse::OK({{"name", "Elle-Ann"}, {"traits", traits}});
        });
        m_router.Register("GET", "/api/models/token-cache/stats", [](const HTTPRequest&) {
            /* Real LLM usage counters are tracked in dbo.system_settings by CallGroqDirect */
            auto rs = ElleSQLPool::Instance().Query(
                "SELECT ISNULL(setting_key,''), ISNULL(setting_value,'0') "
                "FROM ElleCore.dbo.system_settings "
                "WHERE setting_key LIKE 'llm_%';");
            uint64_t hits = 0, misses = 0, total = 0;
            if (rs.success) {
                for (auto& r : rs.rows) {
                    std::string k = r.values.size() > 0 ? r.values[0] : "";
                    uint64_t v = (uint64_t)std::atoll(r.values.size() > 1 ? r.values[1].c_str() : "0");
                    if (k == "llm_cache_hits")   hits = v;
                    if (k == "llm_cache_misses") misses = v;
                    if (k == "llm_total_requests") total = v;
                }
            }
            return HTTPResponse::OK({
                {"cache_hits", hits}, {"cache_misses", misses}, {"total_requests", total}
            });
        });

        /* ============== Morals — dbo.moral_rules ============== */
        m_router.Register("GET", "/api/morals/rules", [](const HTTPRequest& req) {
            std::string category = req.QueryParam("category");
            std::string sql = "SELECT id, principle, ISNULL(category,''), is_hard_rule "
                              "FROM ElleCore.dbo.moral_rules ";
            std::vector<std::string> params;
            if (!category.empty()) { sql += "WHERE category = ? "; params.push_back(category); }
            sql += "ORDER BY id;";
            auto rs = ElleSQLPool::Instance().QueryParams(sql, params);
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            json arr = json::array();
            for (auto& r : rs.rows) {
                arr.push_back({
                    {"id", r.GetInt(0)},
                    {"principle", r.values.size() > 1 ? r.values[1] : ""},
                    {"category",  r.values.size() > 2 ? r.values[2] : ""},
                    {"is_hard_rule", r.GetInt(3) != 0}
                });
            }
            return HTTPResponse::OK({{"rules", arr}});
        });
        m_router.Register("POST", "/api/morals/rules", [](const HTTPRequest& req) {
            auto it = req.headers.find("x-admin-key");
            if (it == req.headers.end() || it->second.empty()) {
                return HTTPResponse::Err(401, "missing x-admin-key");
            }
            json body = req.BodyJSON();
            std::string principle = body.value("principle", "");
            std::string category  = body.value("category", "core");
            bool hard = body.value("is_hard_rule", false);
            if (principle.empty()) return HTTPResponse::Err(400, "missing principle");
            auto rs = ElleSQLPool::Instance().QueryParams(
                "INSERT INTO ElleCore.dbo.moral_rules (principle, category, is_hard_rule) "
                "VALUES (?, ?, ?);",
                { principle, category, std::to_string(hard ? 1 : 0) });
            if (!rs.success) return HTTPResponse::Err(500, rs.error);
            return HTTPResponse::Created({
                {"principle", principle}, {"category", category}, {"stored", true}
            });
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
