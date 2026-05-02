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
#include <chrono>
#include <functional>
#include <thread>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <type_traits>

/*──────────────────────────────────────────────────────────────────────────────
 * IPC MESSAGE WRAPPER
 *──────────────────────────────────────────────────────────────────────────────*/
struct ElleIPCMessage {
    ELLE_IPC_HEADER header;
    std::vector<uint8_t> payload;

    /* Shared checksum helper implemented in ElleQueueIPC.cpp. */
    static uint32_t ComputeChecksum(const ELLE_IPC_HEADER& header,
                                   const uint8_t* payload,
                                   uint32_t payloadSize);

    static ElleIPCMessage Create(ELLE_IPC_MSG_TYPE type, 
                                  ELLE_SERVICE_ID src, 
                                  ELLE_SERVICE_ID dst,
                                  const void* data = nullptr, 
                                  uint32_t dataSize = 0);

    /* Serialize to wire format */
    std::vector<uint8_t> Serialize() const;
    /* Deserialize from wire bytes */
    static bool Deserialize(const uint8_t* data, uint32_t len, ElleIPCMessage& out);

    /* Typed payload helpers.
     * These are only safe for trivially-copyable POD types — enforce at
     * compile time so someone can't accidentally SetPayload(std::string)
     * or any type with non-trivial constructors.                         */
    template<typename T>
    void SetPayload(const T& obj) {
        static_assert(std::is_trivially_copyable<T>::value,
            "SetPayload requires a trivially-copyable type — "
            "use SetStringPayload for strings or nlohmann::json.dump().");
        payload.resize(sizeof(T));
        memcpy(payload.data(), &obj, sizeof(T));
        header.payload_size = sizeof(T);

        /* Keep checksum consistent when payload bytes change after Create(). */
        header.checksum = ComputeChecksum(header,
                                          payload.empty() ? nullptr : payload.data(),
                                          header.payload_size);
    }

    template<typename T>
    bool GetPayload(T& obj) const {
        static_assert(std::is_trivially_copyable<T>::value,
            "GetPayload requires a trivially-copyable type.");
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

    /* Offsets used by the IOCP dispatcher to classify a completion ptr. */
    static size_t ReadOvlOffset()  { return offsetof(EllePipeConnection, m_readOvl);  }
    static size_t WriteOvlOffset() { return offsetof(EllePipeConnection, m_writeOvl); }

private:
    friend class ElleIPCServer;
    friend class ElleIPCClient;
    friend class ElleIPCHub;

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
    std::vector<uint8_t> m_readBuffer;    /* Reassembly buffer for msgs > ELLE_PIPE_BUFFER_SIZE */
    /* NOTE: per-connection write buffer was removed in Wave 5 — every async
     * send now owns its wire bytes inside PendingWrite (heap-allocated,
     * freed on IOCP completion). Keeping a dead m_writeBuffer invited
     * future callers to reintroduce the use-after-free the audit killed. */

    /* Outstanding async-write payloads — ownership lives on the connection,
     * NOT the stack, so the buffer survives until the IOCP write completion
     * fires. Key = the overlapped pointer handed to WriteFile.             */
    struct PendingWrite {
        ELLE_IOCP_OVERLAPPED ovl{};
        std::vector<uint8_t> data;
    };
    std::vector<std::unique_ptr<PendingWrite>> m_pendingWrites;
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
    ELLE_SERVICE_ID m_serviceId = (ELLE_SERVICE_ID)0;
    HANDLE          m_hIOCP = nullptr;
    std::string     m_pipeName;
    std::atomic<bool> m_running{false};
    uint32_t        m_maxInstances = 0;

    IPCMessageHandler m_handler;

    std::vector<std::unique_ptr<EllePipeConnection>> m_connections;
    std::mutex m_connMutex;

    friend class ElleIPCHub;

    bool CreatePipeInstance(EllePipeConnection* conn);
    void ProcessReadComplete(EllePipeConnection* conn, DWORD bytes);
    void ProcessReadComplete(EllePipeConnection* conn, DWORD bytes, bool partial);
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
    ELLE_SERVICE_ID m_myService = (ELLE_SERVICE_ID)0;
    ELLE_SERVICE_ID m_targetService = (ELLE_SERVICE_ID)0;
    HANDLE          m_hIOCP = nullptr;
    std::unique_ptr<EllePipeConnection> m_conn;
    IPCMessageHandler m_handler;

    friend class ElleIPCHub;
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

    /** Cheap probe — does this hub have a live client pipe to `target`?
     *  Returns false for a dead-but-not-yet-cleaned connection so the
     *  reconnector can rebuild it. Mutex-protected. */
    bool IsConnectedTo(ELLE_SERVICE_ID target) const;

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

    /* Per-service liveness stamps for /api/diag/wires.
     *
     * Updated on every successful Send (target stamped) and every
     * incoming message (sender stamped). Empty until any traffic flows
     * — a service that never appeared at all will be reported "unknown"
     * by the dev panel rather than "down" so we don't spuriously red-
     * flag services that are simply not started yet (lazy services).  */
    std::unordered_map<ELLE_SERVICE_ID, uint64_t> LastSeenPerService() const {
        std::lock_guard<std::mutex> lk(m_lastSeenMutex);
        return m_lastSeen;
    }

private:
    ELLE_SERVICE_ID m_serviceId = (ELLE_SERVICE_ID)0;
    HANDLE          m_hIOCP = nullptr;
    bool            m_initialized = false;
    std::atomic<bool> m_running{false};

    ElleIPCServer   m_server;
    std::unordered_map<ELLE_SERVICE_ID, std::unique_ptr<ElleIPCClient>> m_clients;
    std::mutex      m_clientMutex;

    /* IOCP worker threads */
    std::vector<std::thread> m_workers;
    void WorkerThread();
    void DispatchIOComplete(ELLE_IOCP_OVERLAPPED* ovl, DWORD bytes, DWORD err,
                            EllePipeConnection* conn);

    /* Message queue */
    std::queue<ElleIPCMessage> m_incomingQueue;
    std::mutex                  m_queueMutex;
    std::condition_variable     m_queueCV;
    IPCMessageHandler           m_handler;

    void DispatchMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender);

    std::atomic<uint64_t> m_sent{0};
    std::atomic<uint64_t> m_received{0};

    /* Per-service last-seen ms epoch — populated on Send/Recv. Mutex-
     * guarded because /api/diag/wires reads from the HTTP thread while
     * the IOCP workers write. */
    mutable std::mutex                              m_lastSeenMutex;
    std::unordered_map<ELLE_SERVICE_ID, uint64_t>   m_lastSeen;
    void StampLastSeen(ELLE_SERVICE_ID id) {
        const uint64_t now = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        std::lock_guard<std::mutex> lk(m_lastSeenMutex);
        m_lastSeen[id] = now;
    }
};

/*──────────────────────────────────────────────────────────────────────────────
 * PIPE NAME HELPER
 *──────────────────────────────────────────────────────────────────────────────*/
namespace ElleIPC {
    std::string GetPipeName(ELLE_SERVICE_ID svc);
    const char* GetServiceName(ELLE_SERVICE_ID svc);
}

#endif /* ELLE_QUEUE_IPC_H */
