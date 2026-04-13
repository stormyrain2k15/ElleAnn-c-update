/*******************************************************************************
 * ElleQueueIPC.cpp — IOCP Named Pipe IPC Implementation
 ******************************************************************************/
#include "ElleQueueIPC.h"
#include "ElleLogger.h"
#include "ElleConfig.h"

/*──────────────────────────────────────────────────────────────────────────────
 * PIPE NAME HELPERS
 *──────────────────────────────────────────────────────────────────────────────*/
static const char* g_serviceNames[] = {
    "QueueWorker", "HTTPServer", "Emotional", "Memory", "Cognitive",
    "Action", "Identity", "Heartbeat", "SelfPrompt", "Dream",
    "GoalEngine", "WorldModel", "LuaBehavioral"
};

namespace ElleIPC {
    std::string GetPipeName(ELLE_SERVICE_ID svc) {
        auto prefix = ElleConfig::Instance().GetString("services.named_pipes.prefix", 
                                                        "\\\\.\\pipe\\ElleAnn_");
        return prefix + g_serviceNames[svc];
    }

    const char* GetServiceName(ELLE_SERVICE_ID svc) {
        if (svc >= 0 && svc < ELLE_SERVICE_COUNT) return g_serviceNames[svc];
        return "Unknown";
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * IPC MESSAGE
 *──────────────────────────────────────────────────────────────────────────────*/
ElleIPCMessage ElleIPCMessage::Create(ELLE_IPC_MSG_TYPE type, 
                                       ELLE_SERVICE_ID src, ELLE_SERVICE_ID dst,
                                       const void* data, uint32_t dataSize) {
    ElleIPCMessage msg;
    msg.header.magic = ELLE_IPC_MAGIC;
    msg.header.version = ELLE_VERSION_MAJOR;
    msg.header.msg_type = (uint32_t)type;
    msg.header.payload_size = dataSize;
    msg.header.timestamp_ms = ELLE_MS_NOW();
    msg.header.source_svc = (uint32_t)src;
    msg.header.dest_svc = (uint32_t)dst;
    msg.header.correlation_id = Elle_HighResTimestamp();
    msg.header.flags = 0;

    if (data && dataSize > 0) {
        msg.payload.resize(dataSize);
        memcpy(msg.payload.data(), data, dataSize);
    }

    /* Compute checksum over header (excluding checksum field) + payload */
    uint32_t checksumSize = sizeof(ELLE_IPC_HEADER) - sizeof(uint32_t) + dataSize;
    std::vector<uint8_t> checksumData(checksumSize);
    memcpy(checksumData.data(), &msg.header, sizeof(ELLE_IPC_HEADER) - sizeof(uint32_t));
    if (dataSize > 0) {
        memcpy(checksumData.data() + sizeof(ELLE_IPC_HEADER) - sizeof(uint32_t),
               data, dataSize);
    }
    msg.header.checksum = Elle_IPC_Checksum(checksumData.data(), checksumSize);

    return msg;
}

std::vector<uint8_t> ElleIPCMessage::Serialize() const {
    std::vector<uint8_t> buffer(sizeof(ELLE_IPC_HEADER) + payload.size());
    memcpy(buffer.data(), &header, sizeof(ELLE_IPC_HEADER));
    if (!payload.empty()) {
        memcpy(buffer.data() + sizeof(ELLE_IPC_HEADER), payload.data(), payload.size());
    }
    return buffer;
}

bool ElleIPCMessage::Deserialize(const uint8_t* data, uint32_t len, ElleIPCMessage& out) {
    if (len < sizeof(ELLE_IPC_HEADER)) return false;

    memcpy(&out.header, data, sizeof(ELLE_IPC_HEADER));

    if (out.header.magic != ELLE_IPC_MAGIC) return false;

    uint32_t payloadSize = out.header.payload_size;
    if (len < sizeof(ELLE_IPC_HEADER) + payloadSize) return false;

    if (payloadSize > 0) {
        out.payload.resize(payloadSize);
        memcpy(out.payload.data(), data + sizeof(ELLE_IPC_HEADER), payloadSize);
    }

    return true;
}

void ElleIPCMessage::SetStringPayload(const std::string& s) {
    payload.assign(s.begin(), s.end());
    payload.push_back(0);
    header.payload_size = (uint32_t)payload.size();
}

std::string ElleIPCMessage::GetStringPayload() const {
    if (payload.empty()) return "";
    return std::string(payload.begin(), payload.end() - (payload.back() == 0 ? 1 : 0));
}

/*──────────────────────────────────────────────────────────────────────────────
 * PIPE CONNECTION
 *──────────────────────────────────────────────────────────────────────────────*/
EllePipeConnection::EllePipeConnection() {
    ZeroMemory(&m_readOvl, sizeof(m_readOvl));
    ZeroMemory(&m_writeOvl, sizeof(m_writeOvl));
    m_readBuffer.resize(ELLE_PIPE_BUFFER_SIZE);
    m_writeBuffer.resize(ELLE_PIPE_BUFFER_SIZE);
}

EllePipeConnection::~EllePipeConnection() {
    if (m_hPipe != INVALID_HANDLE_VALUE) {
        DisconnectNamedPipe(m_hPipe);
        CloseHandle(m_hPipe);
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * IPC SERVER
 *──────────────────────────────────────────────────────────────────────────────*/
ElleIPCServer::ElleIPCServer() {}
ElleIPCServer::~ElleIPCServer() { Stop(); }

bool ElleIPCServer::Start(ELLE_SERVICE_ID myService, HANDLE hIOCP, uint32_t maxInstances) {
    m_serviceId = myService;
    m_hIOCP = hIOCP;
    m_maxInstances = maxInstances;
    m_pipeName = ElleIPC::GetPipeName(myService);
    m_running = true;

    /* Create initial pipe instances */
    for (uint32_t i = 0; i < maxInstances; i++) {
        auto conn = std::make_unique<EllePipeConnection>();
        if (CreatePipeInstance(conn.get())) {
            std::lock_guard<std::mutex> lock(m_connMutex);
            m_connections.push_back(std::move(conn));
        }
    }

    ELLE_INFO("IPC Server started on %s (%d instances)", m_pipeName.c_str(), maxInstances);
    return !m_connections.empty();
}

void ElleIPCServer::Stop() {
    m_running = false;
    std::lock_guard<std::mutex> lock(m_connMutex);
    m_connections.clear();
    ELLE_INFO("IPC Server stopped");
}

bool ElleIPCServer::CreatePipeInstance(EllePipeConnection* conn) {
    conn->m_hPipe = CreateNamedPipeA(
        m_pipeName.c_str(),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        m_maxInstances,
        ELLE_PIPE_BUFFER_SIZE,
        ELLE_PIPE_BUFFER_SIZE,
        0,
        nullptr
    );

    if (conn->m_hPipe == INVALID_HANDLE_VALUE) {
        ELLE_ERROR("CreateNamedPipe failed: %d", GetLastError());
        return false;
    }

    /* Associate with IOCP */
    if (!CreateIoCompletionPort(conn->m_hPipe, m_hIOCP, (ULONG_PTR)conn, 0)) {
        ELLE_ERROR("IOCP association failed: %d", GetLastError());
        CloseHandle(conn->m_hPipe);
        conn->m_hPipe = INVALID_HANDLE_VALUE;
        return false;
    }

    /* Begin async connect */
    conn->m_readOvl.operation = ELLE_IOCP_OP_CONNECT;
    conn->m_readOvl.service_id = m_serviceId;
    conn->m_readOvl.pipe_handle = conn->m_hPipe;

    BOOL connected = ConnectNamedPipe(conn->m_hPipe, &conn->m_readOvl.overlapped);
    if (!connected) {
        DWORD err = GetLastError();
        if (err == ERROR_IO_PENDING) {
            return true; /* Waiting for connection */
        }
        if (err == ERROR_PIPE_CONNECTED) {
            conn->m_connected = true;
            IssueRead(conn);
            return true;
        }
        ELLE_ERROR("ConnectNamedPipe failed: %d", err);
        return false;
    }

    conn->m_connected = true;
    IssueRead(conn);
    return true;
}

void ElleIPCServer::IssueRead(EllePipeConnection* conn) {
    if (!conn->m_connected || !m_running) return;

    ZeroMemory(&conn->m_readOvl.overlapped, sizeof(OVERLAPPED));
    conn->m_readOvl.operation = ELLE_IOCP_OP_READ;
    conn->m_readOvl.service_id = m_serviceId;
    conn->m_readOvl.pipe_handle = conn->m_hPipe;

    BOOL result = ReadFile(
        conn->m_hPipe,
        conn->m_readOvl.buffer,
        ELLE_PIPE_BUFFER_SIZE,
        nullptr,
        &conn->m_readOvl.overlapped
    );

    if (!result && GetLastError() != ERROR_IO_PENDING) {
        ELLE_WARN("ReadFile failed on pipe: %d", GetLastError());
        conn->m_connected = false;
    }
}

void ElleIPCServer::OnIOComplete(ELLE_IOCP_OVERLAPPED* ovl, DWORD bytesTransferred, DWORD error) {
    auto* conn = reinterpret_cast<EllePipeConnection*>(
        reinterpret_cast<uint8_t*>(ovl) - offsetof(EllePipeConnection, m_readOvl));

    if (error != 0) {
        if (error == ERROR_BROKEN_PIPE || error == ERROR_PIPE_NOT_CONNECTED) {
            conn->m_connected = false;
            ELLE_DEBUG("Pipe disconnected");
            /* Re-create pipe instance for new connections */
            DisconnectNamedPipe(conn->m_hPipe);
            ConnectNamedPipe(conn->m_hPipe, &conn->m_readOvl.overlapped);
        }
        return;
    }

    switch (ovl->operation) {
        case ELLE_IOCP_OP_CONNECT:
            conn->m_connected = true;
            ELLE_DEBUG("Client connected to pipe");
            IssueRead(conn);
            break;

        case ELLE_IOCP_OP_READ:
            ProcessReadComplete(conn, bytesTransferred);
            IssueRead(conn); /* Continue reading */
            break;

        case ELLE_IOCP_OP_WRITE:
            /* Write completed */
            break;
    }
}

void ElleIPCServer::ProcessReadComplete(EllePipeConnection* conn, DWORD bytes) {
    if (bytes == 0) return;

    ElleIPCMessage msg;
    if (ElleIPCMessage::Deserialize(conn->m_readOvl.buffer, bytes, msg)) {
        conn->m_remoteService = (ELLE_SERVICE_ID)msg.header.source_svc;
        if (m_handler) {
            m_handler(msg, (ELLE_SERVICE_ID)msg.header.source_svc);
        }
    } else {
        ELLE_WARN("Failed to deserialize IPC message (%d bytes)", bytes);
    }
}

bool ElleIPCServer::Send(ELLE_SERVICE_ID target, const ElleIPCMessage& msg) {
    std::lock_guard<std::mutex> lock(m_connMutex);
    for (auto& conn : m_connections) {
        if (conn->m_connected && conn->m_remoteService == target) {
            auto data = msg.Serialize();
            std::lock_guard<std::mutex> wlock(conn->m_writeMutex);

            ZeroMemory(&conn->m_writeOvl.overlapped, sizeof(OVERLAPPED));
            conn->m_writeOvl.operation = ELLE_IOCP_OP_WRITE;

            DWORD written;
            BOOL result = WriteFile(conn->m_hPipe, data.data(), (DWORD)data.size(),
                                     &written, &conn->m_writeOvl.overlapped);
            return result || GetLastError() == ERROR_IO_PENDING;
        }
    }
    return false;
}

void ElleIPCServer::Broadcast(const ElleIPCMessage& msg) {
    std::lock_guard<std::mutex> lock(m_connMutex);
    for (auto& conn : m_connections) {
        if (conn->m_connected) {
            auto data = msg.Serialize();
            std::lock_guard<std::mutex> wlock(conn->m_writeMutex);

            ELLE_IOCP_OVERLAPPED writeOvl;
            ZeroMemory(&writeOvl, sizeof(writeOvl));
            writeOvl.operation = ELLE_IOCP_OP_WRITE;

            DWORD written;
            WriteFile(conn->m_hPipe, data.data(), (DWORD)data.size(),
                      &written, &writeOvl.overlapped);
        }
    }
}

uint32_t ElleIPCServer::ConnectedClients() const {
    uint32_t count = 0;
    for (auto& conn : m_connections) {
        if (conn->m_connected) count++;
    }
    return count;
}

/*──────────────────────────────────────────────────────────────────────────────
 * IPC CLIENT
 *──────────────────────────────────────────────────────────────────────────────*/
ElleIPCClient::ElleIPCClient() {}
ElleIPCClient::~ElleIPCClient() { Disconnect(); }

bool ElleIPCClient::Connect(ELLE_SERVICE_ID myService, ELLE_SERVICE_ID targetService,
                             HANDLE hIOCP, uint32_t timeoutMs) {
    m_myService = myService;
    m_targetService = targetService;
    m_hIOCP = hIOCP;

    std::string pipeName = ElleIPC::GetPipeName(targetService);

    /* Wait for pipe to be available */
    if (!WaitNamedPipeA(pipeName.c_str(), timeoutMs)) {
        return false;
    }

    m_conn = std::make_unique<EllePipeConnection>();
    m_conn->m_hPipe = CreateFileA(
        pipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        nullptr
    );

    if (m_conn->m_hPipe == INVALID_HANDLE_VALUE) {
        m_conn.reset();
        return false;
    }

    /* Set message mode */
    DWORD mode = PIPE_READMODE_MESSAGE;
    SetNamedPipeHandleState(m_conn->m_hPipe, &mode, nullptr, nullptr);

    /* Associate with IOCP */
    CreateIoCompletionPort(m_conn->m_hPipe, m_hIOCP, (ULONG_PTR)m_conn.get(), 0);

    m_conn->m_connected = true;
    m_conn->m_remoteService = targetService;

    /* Start reading */
    m_conn->m_readOvl.operation = ELLE_IOCP_OP_READ;
    m_conn->m_readOvl.pipe_handle = m_conn->m_hPipe;

    ReadFile(m_conn->m_hPipe, m_conn->m_readOvl.buffer, ELLE_PIPE_BUFFER_SIZE,
             nullptr, &m_conn->m_readOvl.overlapped);

    return true;
}

void ElleIPCClient::Disconnect() {
    if (m_conn) {
        m_conn->m_connected = false;
        if (m_conn->m_hPipe != INVALID_HANDLE_VALUE) {
            CloseHandle(m_conn->m_hPipe);
            m_conn->m_hPipe = INVALID_HANDLE_VALUE;
        }
        m_conn.reset();
    }
}

bool ElleIPCClient::IsConnected() const {
    return m_conn && m_conn->m_connected;
}

bool ElleIPCClient::Send(const ElleIPCMessage& msg) {
    if (!IsConnected()) return false;
    auto data = msg.Serialize();
    std::lock_guard<std::mutex> lock(m_conn->m_writeMutex);

    ZeroMemory(&m_conn->m_writeOvl.overlapped, sizeof(OVERLAPPED));
    m_conn->m_writeOvl.operation = ELLE_IOCP_OP_WRITE;

    DWORD written;
    BOOL result = WriteFile(m_conn->m_hPipe, data.data(), (DWORD)data.size(),
                             &written, &m_conn->m_writeOvl.overlapped);
    return result || GetLastError() == ERROR_IO_PENDING;
}

/*──────────────────────────────────────────────────────────────────────────────
 * IPC HUB
 *──────────────────────────────────────────────────────────────────────────────*/
ElleIPCHub::ElleIPCHub() {}
ElleIPCHub::~ElleIPCHub() { Shutdown(); }

bool ElleIPCHub::Initialize(ELLE_SERVICE_ID myService, uint32_t workerThreads) {
    m_serviceId = myService;

    /* Create IOCP */
    m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, workerThreads);
    if (!m_hIOCP) {
        ELLE_ERROR("Failed to create IOCP: %d", GetLastError());
        return false;
    }

    /* Start pipe server */
    auto maxInstances = (uint32_t)ElleConfig::Instance().GetInt(
        "services.named_pipes.max_instances", 16);
    if (!m_server.Start(myService, m_hIOCP, maxInstances)) {
        CloseHandle(m_hIOCP);
        m_hIOCP = nullptr;
        return false;
    }

    m_server.SetMessageHandler([this](const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) {
        DispatchMessage(msg, sender);
    });

    /* Start IOCP worker threads */
    m_running = true;
    for (uint32_t i = 0; i < workerThreads; i++) {
        m_workers.emplace_back(&ElleIPCHub::WorkerThread, this);
    }

    m_initialized = true;
    return true;
}

void ElleIPCHub::Shutdown() {
    m_running = false;

    /* Post completion to wake worker threads */
    for (size_t i = 0; i < m_workers.size(); i++) {
        PostQueuedCompletionStatus(m_hIOCP, 0, 0, nullptr);
    }

    for (auto& t : m_workers) {
        if (t.joinable()) t.join();
    }
    m_workers.clear();

    m_server.Stop();

    {
        std::lock_guard<std::mutex> lock(m_clientMutex);
        m_clients.clear();
    }

    if (m_hIOCP) {
        CloseHandle(m_hIOCP);
        m_hIOCP = nullptr;
    }

    m_initialized = false;
}

bool ElleIPCHub::ConnectTo(ELLE_SERVICE_ID target, uint32_t timeoutMs) {
    std::lock_guard<std::mutex> lock(m_clientMutex);
    
    auto it = m_clients.find(target);
    if (it != m_clients.end() && it->second->IsConnected()) return true;

    auto client = std::make_unique<ElleIPCClient>();
    if (!client->Connect(m_serviceId, target, m_hIOCP, timeoutMs)) {
        return false;
    }

    client->SetMessageHandler([this](const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) {
        DispatchMessage(msg, sender);
    });

    m_clients[target] = std::move(client);
    return true;
}

bool ElleIPCHub::Send(ELLE_SERVICE_ID target, const ElleIPCMessage& msg) {
    m_sent++;

    /* Try client connection first */
    {
        std::lock_guard<std::mutex> lock(m_clientMutex);
        auto it = m_clients.find(target);
        if (it != m_clients.end() && it->second->IsConnected()) {
            return it->second->Send(msg);
        }
    }

    /* Fall back to server (if target connected as client to us) */
    return m_server.Send(target, msg);
}

void ElleIPCHub::Broadcast(const ElleIPCMessage& msg) {
    m_server.Broadcast(msg);
    std::lock_guard<std::mutex> lock(m_clientMutex);
    for (auto& [id, client] : m_clients) {
        if (client->IsConnected()) client->Send(msg);
    }
}

void ElleIPCHub::SetMessageHandler(IPCMessageHandler handler) {
    m_handler = handler;
}

bool ElleIPCHub::HasPendingMessage() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_queueMutex));
    return !m_incomingQueue.empty();
}

bool ElleIPCHub::PopMessage(ElleIPCMessage& out, uint32_t timeoutMs) {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    if (timeoutMs == 0) {
        if (m_incomingQueue.empty()) return false;
    } else {
        if (!m_queueCV.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                [this] { return !m_incomingQueue.empty(); })) {
            return false;
        }
    }

    out = m_incomingQueue.front();
    m_incomingQueue.pop();
    return true;
}

void ElleIPCHub::DispatchMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) {
    m_received++;

    if (m_handler) {
        m_handler(msg, sender);
    }

    /* Also push to queue for pull-based consumption */
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_incomingQueue.push(msg);
    m_queueCV.notify_one();
}

void ElleIPCHub::WorkerThread() {
    while (m_running) {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        LPOVERLAPPED ovl = nullptr;

        BOOL result = GetQueuedCompletionStatus(
            m_hIOCP, &bytesTransferred, &completionKey, &ovl, 1000);

        if (!m_running) break;

        if (!result) {
            DWORD err = GetLastError();
            if (err == WAIT_TIMEOUT) continue;
            if (ovl) {
                auto* elleOvl = reinterpret_cast<ELLE_IOCP_OVERLAPPED*>(ovl);
                m_server.OnIOComplete(elleOvl, 0, err);
            }
            continue;
        }

        if (!ovl) continue; /* Shutdown signal */

        auto* elleOvl = reinterpret_cast<ELLE_IOCP_OVERLAPPED*>(ovl);
        m_server.OnIOComplete(elleOvl, bytesTransferred, 0);
    }
}
