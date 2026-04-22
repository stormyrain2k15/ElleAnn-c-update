/*******************************************************************************
 * ElleQueueIPC.h — IOCP-based Inter-Process Communication
 * 
 * Named pipe server/client using Windows IOCP for high-performance async I/O.
 * Each service runs a pipe server and connects to others as clients.
 ******************************************************************************/
#pragma once
#ifndef ELLE_QUEUE_IPC_H
#define ELLE_QUEUE_IPC_H

#include "ElleTypes.h"
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <unordered_map>
#include <memory>
#include <atomic>

/*──────────────────────────────────────────────────────────────────────────────
 * IPC MESSAGE WRAPPER
 *──────────────────────────────────────────────────────────────────────────────*/
struct ElleIPCMessage {
    ELLE_IPC_HEADER header;
    std::vector<uint8_t> payload;

    static ElleIPCMessage Create(ELLE_IPC_MSG_TYPE type, 
                                  ELLE_SERVICE_ID src, 
                                  ELLE_SERVICE_ID dst,
                                  const void* data = nullptr, 
                                  uint32_t dataSize = 0);

    /* Serialize to wire format */
    std::vector<uint8_t> Serialize() const;
    /* Deserialize from wire bytes */
    static bool Deserialize(const uint8_t* data, uint32_t len, ElleIPCMessage& out);

    /* Typed payload helpers */
    template<typename T>
    void SetPayload(const T& obj) {
        payload.resize(sizeof(T));
        memcpy(payload.data(), &obj, sizeof(T));
        header.payload_size = sizeof(T);
    }

    template<typename T>
    bool GetPayload(T& obj) const {
        if (payload.size() < sizeof(T)) return false;
        memcpy(&obj, payload.data(), sizeof(T));
        return true;
    }

    void SetStringPayload(const std::string& s);
    std::string GetStringPayload() const;
};

/*──────────────────────────────────────────────────────────────────────────────
 * PIPE CONNECTION (represents one connected pipe instance)
 *──────────────────────────────────────────────────────────────────────────────*/
class EllePipeConnection {
public:
    EllePipeConnection();
    ~EllePipeConnection();

    bool    IsConnected() const { return m_connected; }
    HANDLE  GetHandle() const { return m_hPipe; }
    ELLE_SERVICE_ID GetRemoteService() const { return m_remoteService; }

    ELLE_IOCP_OVERLAPPED* GetReadOverlapped() { return &m_readOvl; }
    ELLE_IOCP_OVERLAPPED* GetWriteOverlapped() { return &m_writeOvl; }

private:
    friend class ElleIPCServer;
    friend class ElleIPCClient;

    HANDLE              m_hPipe = INVALID_HANDLE_VALUE;
    bool                m_connected = false;
    ELLE_SERVICE_ID     m_remoteService = (ELLE_SERVICE_ID)0;
    /* Owner discriminator — set to the ElleIPCClient* that owns this conn
     * (nullptr for server-owned connections). The IOCP worker uses this to
     * route read completions to the right handler. Without it the client
     * side of a conn silently drops all inbound messages.                 */
    void*               m_clientOwner = nullptr;
    ELLE_IOCP_OVERLAPPED m_readOvl;
    ELLE_IOCP_OVERLAPPED m_writeOvl;
    std::vector<uint8_t> m_readBuffer;
    std::vector<uint8_t> m_writeBuffer;
    std::mutex           m_writeMutex;
};

/*──────────────────────────────────────────────────────────────────────────────
 * IPC SERVER (listens on a named pipe for incoming connections)
 *──────────────────────────────────────────────────────────────────────────────*/
typedef std::function<void(const ElleIPCMessage&, ELLE_SERVICE_ID sender)> IPCMessageHandler;

class ElleIPCServer {
public:
    ElleIPCServer();
    ~ElleIPCServer();

    bool Start(ELLE_SERVICE_ID myService, HANDLE hIOCP, uint32_t maxInstances = 8);
    void Stop();
    bool IsRunning() const { return m_running; }

    void SetMessageHandler(IPCMessageHandler handler) { m_handler = handler; }

    /* Send to a specific connected client */
    bool Send(ELLE_SERVICE_ID target, const ElleIPCMessage& msg);
    /* Broadcast to all connected clients */
    void Broadcast(const ElleIPCMessage& msg);

    /* Called by IOCP worker threads when I/O completes */
    void OnIOComplete(ELLE_IOCP_OVERLAPPED* ovl, DWORD bytesTransferred, DWORD error);

    uint32_t ConnectedClients() const;

private:
    ELLE_SERVICE_ID m_serviceId;
    HANDLE          m_hIOCP = nullptr;
    std::string     m_pipeName;
    std::atomic<bool> m_running{false};
    uint32_t        m_maxInstances;

    IPCMessageHandler m_handler;

    std::vector<std::unique_ptr<EllePipeConnection>> m_connections;
    std::mutex m_connMutex;

    bool CreatePipeInstance(EllePipeConnection* conn);
    void ProcessReadComplete(EllePipeConnection* conn, DWORD bytes);
    void IssueRead(EllePipeConnection* conn);
};

/*──────────────────────────────────────────────────────────────────────────────
 * IPC CLIENT (connects to another service's named pipe)
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleIPCClient {
public:
    ElleIPCClient();
    ~ElleIPCClient();

    bool Connect(ELLE_SERVICE_ID myService, ELLE_SERVICE_ID targetService, 
                 HANDLE hIOCP, uint32_t timeoutMs = 5000);
    void Disconnect();
    bool IsConnected() const;

    bool Send(const ElleIPCMessage& msg);

    void SetMessageHandler(IPCMessageHandler handler) { m_handler = handler; }
    void OnIOComplete(ELLE_IOCP_OVERLAPPED* ovl, DWORD bytesTransferred, DWORD error);

private:
    ELLE_SERVICE_ID m_myService;
    ELLE_SERVICE_ID m_targetService;
    HANDLE          m_hIOCP = nullptr;
    std::unique_ptr<EllePipeConnection> m_conn;
    IPCMessageHandler m_handler;
};

/*──────────────────────────────────────────────────────────────────────────────
 * IPC HUB (manages all connections for a service)
 * Each service creates one hub that handles server + all client connections.
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleIPCHub {
public:
    ElleIPCHub();
    ~ElleIPCHub();

    /* Initialize: creates IOCP, starts pipe server, launches worker threads */
    bool Initialize(ELLE_SERVICE_ID myService, uint32_t workerThreads = ELLE_IOCP_THREADS);
    void Shutdown();

    /* Connect to another service */
    bool ConnectTo(ELLE_SERVICE_ID target, uint32_t timeoutMs = 5000);

    /* Send message to specific service or broadcast */
    bool Send(ELLE_SERVICE_ID target, const ElleIPCMessage& msg);
    void Broadcast(const ElleIPCMessage& msg);

    /* Register handler for incoming messages */
    void SetMessageHandler(IPCMessageHandler handler);

    /* Message queue for pull-based consumption */
    bool HasPendingMessage() const;
    bool PopMessage(ElleIPCMessage& out, uint32_t timeoutMs = 0);

    /* Stats */
    uint64_t MessagesSent() const { return m_sent; }
    uint64_t MessagesReceived() const { return m_received; }

private:
    ELLE_SERVICE_ID m_serviceId;
    HANDLE          m_hIOCP = nullptr;
    bool            m_initialized = false;
    std::atomic<bool> m_running{false};

    ElleIPCServer   m_server;
    std::unordered_map<ELLE_SERVICE_ID, std::unique_ptr<ElleIPCClient>> m_clients;
    std::mutex      m_clientMutex;

    /* IOCP worker threads */
    std::vector<std::thread> m_workers;
    void WorkerThread();
    void DispatchIOComplete(ELLE_IOCP_OVERLAPPED* ovl, DWORD bytes, DWORD err);

    /* Message queue */
    std::queue<ElleIPCMessage> m_incomingQueue;
    std::mutex                  m_queueMutex;
    std::condition_variable     m_queueCV;
    IPCMessageHandler           m_handler;

    void DispatchMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender);

    std::atomic<uint64_t> m_sent{0};
    std::atomic<uint64_t> m_received{0};
};

/*──────────────────────────────────────────────────────────────────────────────
 * PIPE NAME HELPER
 *──────────────────────────────────────────────────────────────────────────────*/
namespace ElleIPC {
    std::string GetPipeName(ELLE_SERVICE_ID svc);
    const char* GetServiceName(ELLE_SERVICE_ID svc);
}

#endif /* ELLE_QUEUE_IPC_H */
