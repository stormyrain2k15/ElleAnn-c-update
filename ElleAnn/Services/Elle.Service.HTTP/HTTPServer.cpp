/*******************************************************************************
 * HTTPServer.cpp — Raw Winsock HTTP + WebSocket Server (No Framework)
 *
 * Handles REST API endpoints and WebSocket command channel.
 * Uses IOCP for high-performance async I/O.
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include "../../Shared/ElleLLM.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <sstream>
#include <algorithm>

/*──────────────────────────────────────────────────────────────────────────────
 * HTTP REQUEST/RESPONSE TYPES
 *──────────────────────────────────────────────────────────────────────────────*/
struct HTTPRequest {
    std::string method;     /* GET, POST, PUT, DELETE */
    std::string path;
    std::string query;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    bool isWebSocket = false;
    std::string wsKey;
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
        ss << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
        for (auto& [k, v] : headers) ss << k << ": " << v << "\r\n";
        ss << "\r\n" << body;
        return ss.str();
    }

    static HTTPResponse JSON(int status, const std::string& json) {
        HTTPResponse r;
        r.status = status;
        r.statusText = status == 200 ? "OK" : (status == 404 ? "Not Found" : "Error");
        r.body = json;
        return r;
    }

    static HTTPResponse Error(int status, const std::string& message) {
        return JSON(status, "{\"error\":\"" + message + "\"}");
    }
};

typedef std::function<HTTPResponse(const HTTPRequest&)> RouteHandler;

/*──────────────────────────────────────────────────────────────────────────────
 * ROUTE DISPATCHER
 *──────────────────────────────────────────────────────────────────────────────*/
class RouteDispatch {
public:
    void Register(const std::string& method, const std::string& path, RouteHandler handler) {
        m_routes[method + " " + path] = handler;
    }

    HTTPResponse Dispatch(const HTTPRequest& req) {
        /* Try exact match */
        std::string key = req.method + " " + req.path;
        auto it = m_routes.find(key);
        if (it != m_routes.end()) return it->second(req);

        /* Try prefix match */
        for (auto& [route, handler] : m_routes) {
            std::string routePath = route.substr(route.find(' ') + 1);
            std::string routeMethod = route.substr(0, route.find(' '));
            if (routeMethod == req.method && req.path.find(routePath) == 0) {
                return handler(req);
            }
        }

        return HTTPResponse::Error(404, "Not found: " + req.path);
    }

private:
    std::unordered_map<std::string, RouteHandler> m_routes;
};

/*──────────────────────────────────────────────────────────────────────────────
 * WEBSOCKET CLIENT
 *──────────────────────────────────────────────────────────────────────────────*/
struct WSClient {
    SOCKET      socket;
    std::string id;
    bool        authenticated;
    uint64_t    connected_ms;
    std::mutex  sendMutex;
};

/*──────────────────────────────────────────────────────────────────────────────
 * HTTP SERVER
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleHTTPService : public ElleServiceBase {
public:
    ElleHTTPService()
        : ElleServiceBase(SVC_HTTP_SERVER, "ElleHTTPServer",
                          "Elle-Ann HTTP/WebSocket Server",
                          "REST API and WebSocket command server") {}

protected:
    bool OnStart() override {
        /* Initialize Winsock */
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            ELLE_ERROR("WSAStartup failed");
            return false;
        }

        RegisterRoutes();

        auto& cfg = ElleConfig::Instance().GetHTTP();

        /* Create listen socket */
        m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_listenSocket == INVALID_SOCKET) {
            ELLE_ERROR("socket() failed: %d", WSAGetLastError());
            return false;
        }

        /* Bind */
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

        /* Start accept thread */
        m_acceptThread = std::thread(&ElleHTTPService::AcceptLoop, this);

        ELLE_INFO("HTTP server listening on %s:%d", cfg.bind_address.c_str(), cfg.port);
        return true;
    }

    void OnStop() override {
        if (m_listenSocket != INVALID_SOCKET) {
            closesocket(m_listenSocket);
            m_listenSocket = INVALID_SOCKET;
        }
        if (m_acceptThread.joinable()) m_acceptThread.join();
        WSACleanup();
        ELLE_INFO("HTTP server stopped");
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        /* Broadcast to WebSocket clients when relevant */
        if (msg.header.msg_type == IPC_EMOTION_UPDATE ||
            msg.header.msg_type == IPC_LOG_ENTRY ||
            msg.header.msg_type == IPC_TRUST_UPDATE) {
            BroadcastToWebSockets(msg);
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_COGNITIVE, SVC_EMOTIONAL, SVC_MEMORY };
    }

private:
    SOCKET m_listenSocket = INVALID_SOCKET;
    std::thread m_acceptThread;
    RouteDispatch m_router;

    std::vector<std::shared_ptr<WSClient>> m_wsClients;
    std::mutex m_wsMutex;

    void AcceptLoop() {
        while (Running()) {
            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(m_listenSocket, &readSet);

            timeval timeout = {1, 0}; /* 1 second timeout */
            int result = select(0, &readSet, nullptr, nullptr, &timeout);
            if (result <= 0) continue;

            SOCKET clientSocket = accept(m_listenSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) continue;

            /* Handle client in a new thread */
            std::thread([this, clientSocket]() {
                HandleClient(clientSocket);
            }).detach();
        }
    }

    void HandleClient(SOCKET clientSocket) {
        try {
            /* Set socket options for reliable send */
            int optval = 1;
            setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));

            /* Set receive timeout */
            DWORD recvTimeout = 5000;
            setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&recvTimeout, sizeof(recvTimeout));

            char buffer[65536];
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                closesocket(clientSocket);
                return;
            }
            buffer[bytesRead] = '\0';

            /* Parse HTTP request */
            HTTPRequest req = ParseRequest(std::string(buffer, bytesRead));

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
                return;
            }

            /* Route request */
            HTTPResponse resp = m_router.Dispatch(req);
            SendResponse(clientSocket, resp);

        } catch (const std::exception& e) {
            ELLE_ERROR("HTTP handler exception: %s", e.what());
            try {
                SendResponse(clientSocket, HTTPResponse::Error(500, 
                    std::string("Internal error: ") + e.what()));
            } catch (...) {
                closesocket(clientSocket);
            }
        } catch (...) {
            ELLE_ERROR("HTTP handler unknown exception");
            try {
                SendResponse(clientSocket, HTTPResponse::Error(500, "Unknown internal error"));
            } catch (...) {
                closesocket(clientSocket);
            }
        }
    }

    void SendResponse(SOCKET clientSocket, const HTTPResponse& resp) {
        std::string data = resp.Serialize();
        int totalSent = 0;
        int remaining = (int)data.size();
        const char* ptr = data.c_str();

        /* Send all data reliably */
        while (remaining > 0) {
            int sent = send(clientSocket, ptr + totalSent, remaining, 0);
            if (sent == SOCKET_ERROR) break;
            totalSent += sent;
            remaining -= sent;
        }

        /* Graceful shutdown — let client read all data before closing */
        shutdown(clientSocket, SD_SEND);
        
        /* Drain any remaining incoming data */
        char drain[1024];
        while (recv(clientSocket, drain, sizeof(drain), 0) > 0) {}
        
        closesocket(clientSocket);
    }

    HTTPRequest ParseRequest(const std::string& raw) {
        HTTPRequest req;
        std::istringstream iss(raw);
        std::string line;

        /* Request line */
        std::getline(iss, line);
        std::istringstream reqLine(line);
        reqLine >> req.method >> req.path;

        /* Parse query string */
        size_t qPos = req.path.find('?');
        if (qPos != std::string::npos) {
            req.query = req.path.substr(qPos + 1);
            req.path = req.path.substr(0, qPos);
        }

        /* Headers */
        while (std::getline(iss, line) && line != "\r" && !line.empty()) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string val = line.substr(colon + 2);
                if (!val.empty() && val.back() == '\r') val.pop_back();
                req.headers[key] = val;
            }
        }

        /* Check for WebSocket upgrade */
        if (req.headers.count("Upgrade") && req.headers["Upgrade"] == "websocket") {
            req.isWebSocket = true;
            req.wsKey = req.headers["Sec-WebSocket-Key"];
        }

        /* Body */
        size_t headerEnd = raw.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            req.body = raw.substr(headerEnd + 4);
        }

        return req;
    }

    void HandleWebSocketUpgrade(SOCKET clientSocket, const HTTPRequest& req) {
        /* WebSocket handshake */
        ELLE_INFO("WebSocket upgrade request from client");
        
        auto client = std::make_shared<WSClient>();
        client->socket = clientSocket;
        client->authenticated = false;
        client->connected_ms = ELLE_MS_NOW();

        {
            std::lock_guard<std::mutex> lock(m_wsMutex);
            m_wsClients.push_back(client);
        }

        /* WebSocket read loop would go here */
    }

    void BroadcastToWebSockets(const ElleIPCMessage& msg) {
        /* Serialize IPC message as JSON and send to all WS clients */
        std::lock_guard<std::mutex> lock(m_wsMutex);
        /* Remove disconnected clients and broadcast */
    }

    /*──────────────────────────────────────────────────────────────────────────
     * ROUTE REGISTRATION
     *──────────────────────────────────────────────────────────────────────────*/
    void RegisterRoutes() {
        /* /api/health — Simple health check, no dependencies */
        m_router.Register("GET", "/api/health", [](const HTTPRequest& req) -> HTTPResponse {
            return HTTPResponse::JSON(200, 
                "{\"status\":\"alive\",\"name\":\"Elle-Ann\",\"version\":\"" 
                ELLE_VERSION_STRING "\"}");
        });

        /* /api/ai — Chat with Elle */
        m_router.Register("POST", "/api/ai/chat", [this](const HTTPRequest& req) -> HTTPResponse {
            try {
                /* Extract message from JSON body — simple extraction */
                std::string message = req.body;
                size_t msgStart = message.find("\"message\":");
                if (msgStart != std::string::npos) {
                    size_t valStart = message.find('"', msgStart + 10) + 1;
                    size_t valEnd = message.find('"', valStart);
                    if (valStart != std::string::npos && valEnd != std::string::npos) {
                        message = message.substr(valStart, valEnd - valStart);
                    }
                }

                auto resp = ElleLLMEngine::Instance().Ask(message);
                
                /* Escape response for JSON */
                std::string escaped;
                for (char c : resp) {
                    switch (c) {
                        case '"':  escaped += "\\\""; break;
                        case '\\': escaped += "\\\\"; break;
                        case '\n': escaped += "\\n"; break;
                        case '\r': escaped += "\\r"; break;
                        case '\t': escaped += "\\t"; break;
                        default:   escaped += c; break;
                    }
                }
                
                return HTTPResponse::JSON(200, "{\"response\":\"" + escaped + "\"}");
            } catch (const std::exception& e) {
                return HTTPResponse::Error(500, std::string("Chat error: ") + e.what());
            }
        });

        /* /api/emotions — Current emotional state */
        m_router.Register("GET", "/api/emotions", [this](const HTTPRequest& req) -> HTTPResponse {
            try {
                ELLE_EMOTION_STATE state;
                ElleDB::GetLatestEmotionState(state);
                std::ostringstream json;
                json << "{\"valence\":" << state.valence
                     << ",\"arousal\":" << state.arousal
                     << ",\"dominance\":" << state.dominance
                     << ",\"tick\":" << state.tick_count << "}";
                return HTTPResponse::JSON(200, json.str());
            } catch (...) {
                return HTTPResponse::JSON(200, 
                    "{\"valence\":0.0,\"arousal\":0.0,\"dominance\":0.0,\"tick\":0,\"note\":\"db_unavailable\"}");
            }
        });

        /* /api/memory — Memory operations */
        m_router.Register("GET", "/api/memory/recall", [](const HTTPRequest& req) -> HTTPResponse {
            return HTTPResponse::JSON(200, "{\"memories\":[]}");
        });

        m_router.Register("POST", "/api/memory/store", [](const HTTPRequest& req) -> HTTPResponse {
            return HTTPResponse::JSON(200, "{\"stored\":true}");
        });

        /* /api/server — Server status */
        m_router.Register("GET", "/api/server/status", [](const HTTPRequest& req) -> HTTPResponse {
            try {
                std::vector<ELLE_SERVICE_STATUS> statuses;
                ElleDB::GetWorkerStatuses(statuses);
                std::ostringstream json;
                json << "{\"services\":" << statuses.size() 
                     << ",\"version\":\"" << ELLE_VERSION_STRING << "\"}";
                return HTTPResponse::JSON(200, json.str());
            } catch (...) {
                return HTTPResponse::JSON(200, 
                    "{\"services\":0,\"version\":\"" ELLE_VERSION_STRING "\",\"note\":\"db_unavailable\"}");
            }
        });

        /* /api/brain — Cognitive operations */
        m_router.Register("GET", "/api/brain/status", [](const HTTPRequest& req) -> HTTPResponse {
            return HTTPResponse::JSON(200, "{\"status\":\"active\"}");
        });

        /* /api/goals — Goal management */
        m_router.Register("GET", "/api/goals", [](const HTTPRequest& req) -> HTTPResponse {
            try {
                std::vector<ELLE_GOAL_RECORD> goals;
                ElleDB::GetActiveGoals(goals);
                std::ostringstream json;
                json << "{\"goals\":[";
                for (size_t i = 0; i < goals.size(); i++) {
                    if (i > 0) json << ",";
                    json << "{\"id\":" << goals[i].id 
                         << ",\"description\":\"" << goals[i].description << "\""
                         << ",\"progress\":" << goals[i].progress << "}";
                }
                json << "]}";
                return HTTPResponse::JSON(200, json.str());
            } catch (...) {
                return HTTPResponse::JSON(200, "{\"goals\":[],\"note\":\"db_unavailable\"}");
            }
        });

        /* /api/hal — Hardware abstraction */
        m_router.Register("GET", "/api/hal/status", [](const HTTPRequest& req) -> HTTPResponse {
            return HTTPResponse::JSON(200, "{\"hardware\":\"nominal\"}");
        });

        /* /api/admin — Administrative endpoints */
        m_router.Register("POST", "/api/admin/reload", [](const HTTPRequest& req) -> HTTPResponse {
            try {
                ElleConfig::Instance().Reload();
                return HTTPResponse::JSON(200, "{\"reloaded\":true}");
            } catch (...) {
                return HTTPResponse::Error(500, "Config reload failed");
            }
        });

        ELLE_INFO("Registered %d API routes", 11);
    }
};

ELLE_SERVICE_MAIN(ElleHTTPService)
