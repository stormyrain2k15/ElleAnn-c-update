/*******************************************************************************
 * ElleQueueIPC.cpp — IOCP Named Pipe IPC Implementation
 ******************************************************************************/
#include "ElleQueueIPC.h"
#include "ElleLogger.h"
#include "ElleConfig.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

static std::string ElleHexPreview(const uint8_t* p, size_t len, size_t maxBytes = 32) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    size_t n = (len < maxBytes) ? len : maxBytes;
    for (size_t i = 0; i < n; i++) {
        ss << std::setw(2) << (unsigned)p[i];
        if (i + 1 < n) ss << ' ';
    }
    if (len > maxBytes) ss << " ...";
    return ss.str();
}

static uint32_t ElleIPCComputeChecksum(const ELLE_IPC_HEADER& header, const uint8_t* payload, uint32_t payloadSize) {
    const uint32_t total = (uint32_t)sizeof(ELLE_IPC_HEADER) + payloadSize;
    std::vector<uint8_t> tmp(total);
    ELLE_IPC_HEADER zeroed = header;
    zeroed.checksum = 0;
    memcpy(tmp.data(), &zeroed, sizeof(ELLE_IPC_HEADER));
    if (payloadSize > 0 && payload) {
        memcpy(tmp.data() + sizeof(ELLE_IPC_HEADER), payload, payloadSize);
    }
    return Elle_IPC_Checksum(tmp.data(), total);
}

uint32_t ElleIPCMessage::ComputeChecksum(const ELLE_IPC_HEADER& header,
                                        const uint8_t* payload,
                                        uint32_t payloadSize) {
    return ElleIPCComputeChecksum(header, payload, payloadSize);
}

/*──────────────────────────────────────────────────────────────────────────────
 * PIPE NAME HELPERS
 *──────────────────────────────────────────────────────────────────────────────*/
static const char* g_serviceNames[] = {
    "QueueWorker", "HTTPServer", "Emotional", "Memory", "Cognitive",
    "Action", "Identity", "Heartbeat", "SelfPrompt", "Dream",
    "GoalEngine", "WorldModel", "LuaBehavioral",
    /* Phase 2+ services — ORDER MUST match ELLE_SERVICE_ID enum exactly. */
    "Bonding", "Continuity", "InnerLife", "Solitude",
    "Family", "XChromosome", "Consent", "Fiesta"
};
static_assert(sizeof(g_serviceNames) / sizeof(g_serviceNames[0]) == (size_t)ELLE_SERVICE_COUNT,
              "g_serviceNames size must equal ELLE_SERVICE_COUNT — "
              "every ELLE_SERVICE_ID enum value needs a matching string here.");

namespace ElleIPC {
    std::string GetPipeName(ELLE_SERVICE_ID svc) {
        auto prefix = ElleConfig::Instance().GetString("services.named_pipes.prefix", 
                                                        "\\\\.\\pipe\\ElleAnn_");
        if ((int)svc < 0 || (int)svc >= (int)ELLE_SERVICE_COUNT) {
            /* Out-of-range — never read past g_serviceNames[]. */
            return prefix + std::string("Unknown_") + std::to_string((int)svc);
        }
        return prefix + g_serviceNames[svc];
    }

    const char* GetServiceName(ELLE_SERVICE_ID svc) {
        if ((int)svc >= 0 && (int)svc < (int)ELLE_SERVICE_COUNT) return g_serviceNames[svc];
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
    /* Create() and Deserialize() must hash the same byte sequence.
     * Deserialize() zeroes checksum before hashing; do the same here. */
    msg.header.checksum = 0;

    if (data && dataSize > 0) {
        msg.payload.resize(dataSize);
        memcpy(msg.payload.data(), data, dataSize);
    }

    /* Compute checksum over the full header (with checksum field zeroed)
     * plus payload. Do NOT assume the checksum field is the last uint32_t
     * in ELLE_IPC_HEADER; that assumption breaks if the header layout ever
     * changes and causes cross-service checksum mismatches.
     */
    msg.header.checksum = ElleIPCMessage::ComputeChecksum(msg.header,
                                                          (const uint8_t*)data,
                                                          dataSize);

    return msg;
}

std::vector<uint8_t> ElleIPCMessage::Serialize() const {
    std::vector<uint8_t> buffer(sizeof(ELLE_IPC_HEADER) + payload.size());

    /* Compute checksum at serialization time so any header mutations
     * (flags, correlation_id, etc.) done after Create() are reflected
     * in the wire bytes. This prevents fragile call-order bugs where a
     * caller sets header fields after payload setters.
     */
    ELLE_IPC_HEADER outHeader = header;
    outHeader.payload_size = (uint32_t)payload.size();
    outHeader.checksum = ElleIPCMessage::ComputeChecksum(outHeader,
                                                         payload.empty() ? nullptr : payload.data(),
                                                         (uint32_t)payload.size());

    memcpy(buffer.data(), &outHeader, sizeof(ELLE_IPC_HEADER));
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

    /* Framing guard: cap payload_size BEFORE we allocate. A corrupted or
     * hostile sender can put 0xFFFFFFFF here and trick us into a 4 GiB
     * allocation (OOM) or a payload.resize() on a size we never actually
     * have on the wire. Cap lookup goes through ElleConfig so ops can
     * raise the ceiling at runtime without rebuilding.                   */
    uint32_t maxPayload = (uint32_t)ElleConfig::Instance().GetInt(
        "services.named_pipes.max_payload_bytes", (int64_t)ELLE_IPC_MAX_PAYLOAD);
    if (maxPayload == 0) maxPayload = ELLE_IPC_MAX_PAYLOAD;
    if (payloadSize > maxPayload) {
        ELLE_WARN("IPC frame rejected: payload_size=%u > cap=%u (svc=%u→%u)",
                  payloadSize, maxPayload,
                  (unsigned)out.header.source_svc,
                  (unsigned)out.header.dest_svc);
        return false;
    }
    if (len < sizeof(ELLE_IPC_HEADER) + payloadSize) return false;

    if (payloadSize > 0) {
        out.payload.resize(payloadSize);
        memcpy(out.payload.data(), data + sizeof(ELLE_IPC_HEADER), payloadSize);
    }

    /* Verify checksum — Create() fills header.checksum over header-minus-
     * checksum + payload. Previously we accepted any frame; corrupted or
     * tampered wire bytes would silently feed garbage into every handler. */
    uint32_t claimed = out.header.checksum;
    uint32_t actual = ElleIPCMessage::ComputeChecksum(out.header,
                                                      out.payload.empty() ? nullptr : out.payload.data(),
                                                      payloadSize);
    if (claimed != actual) {
        /* Feb-2026 pivot: tolerate checksum mismatch in dev builds to prevent
         * a single out-of-sync service from spamming the entire mesh with
         * warnings. The frame is still dropped, but we log at DEBUG level.
         * Production should keep this as WARN.
         */
#ifdef _DEBUG
        ELLE_WARN("IPC checksum mismatch: claimed=%08x actual=%08x src=%u dst=%u type=%u ver=%u payload=%u head=%s",
                  claimed, actual,
                  (unsigned)out.header.source_svc,
                  (unsigned)out.header.dest_svc,
                  (unsigned)out.header.msg_type,
                  (unsigned)out.header.version,
                  (unsigned)payloadSize,
                  ElleHexPreview((const uint8_t*)&out.header, sizeof(ELLE_IPC_HEADER), 24).c_str());
#else
        ELLE_WARN("IPC checksum mismatch: claimed=%08x actual=%08x — frame dropped",
                  claimed, actual);
#endif
        return false;
    }

    return true;
}

void ElleIPCMessage::SetStringPayload(const std::string& s) {
    payload.assign(s.begin(), s.end());
    payload.push_back(0);
    header.payload_size = (uint32_t)payload.size();

    /* Any payload mutation invalidates the existing checksum. Recompute it
     * so callers that create a message then set payload still produce a
     * verifiable frame.
     */
    header.checksum = ComputeChecksum(header,
                                      payload.empty() ? nullptr : payload.data(),
                                      header.payload_size);
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
    /* Reassembly buffer must start EMPTY. We only reserve capacity.
     * Using resize() here pre-fills the buffer with zeros and causes
     * ProcessReadComplete() to append after 64KiB of garbage, leading
     * to huge accumulated sizes, checksum mismatches, and deserialize
     * failures across the mesh.
     */
    m_readBuffer.reserve(ELLE_PIPE_BUFFER_SIZE);
    /* m_writeBuffer removed — PendingWrite owns wire bytes per-send. */
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
    /* The connection was identified by the dispatcher via the IOCP
     * completion key and pointer classification, not offsetof magic. The
     * caller sets ovl's carrier pipe_handle; we trust the dispatcher. */
    EllePipeConnection* conn = reinterpret_cast<EllePipeConnection*>(
        reinterpret_cast<uint8_t*>(ovl) - offsetof(EllePipeConnection, m_readOvl));

    /* Write completions come in on m_writeOvl — reject them here; the
     * dispatcher already routes writes separately to ReleasePendingWrite. */
    if (ovl->operation == ELLE_IOCP_OP_WRITE) return;

    if (error != 0) {
        if (error == ERROR_BROKEN_PIPE || error == ERROR_PIPE_NOT_CONNECTED) {
            conn->m_connected = false;
            conn->m_remoteService = (ELLE_SERVICE_ID)0;
            conn->m_readBuffer.clear();
            ELLE_DEBUG("Pipe disconnected — recycling instance for new client");

            /* Clean recycle: Disconnect, fully reset the OVERLAPPED struct,
             * then re-arm ConnectNamedPipe so the next peer can bind.      */
            DisconnectNamedPipe(conn->m_hPipe);
            ZeroMemory(&conn->m_readOvl.overlapped, sizeof(OVERLAPPED));
            conn->m_readOvl.operation = ELLE_IOCP_OP_CONNECT;

            BOOL ok = ConnectNamedPipe(conn->m_hPipe, &conn->m_readOvl.overlapped);
            if (!ok) {
                DWORD e = GetLastError();
                if (e == ERROR_PIPE_CONNECTED) {
                    conn->m_connected = true;
                    IssueRead(conn);
                } else if (e != ERROR_IO_PENDING) {
                    ELLE_WARN("Pipe recycle ConnectNamedPipe failed: %d", e);
                }
            }
        } else if (error == ERROR_MORE_DATA) {
            /* Message bigger than ELLE_PIPE_BUFFER_SIZE — read the remainder
             * in a follow-up synchronous read so the whole message is
             * assembled before deserialization. Rare, but matters for
             * large SetStringPayload (e.g. long system prompts).           */
            ProcessReadComplete(conn, bytesTransferred, /*partial*/ true);
            IssueRead(conn);
        } else {
            ELLE_WARN("Server IOCP error %u on pipe", error);
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
            ProcessReadComplete(conn, bytesTransferred, /*partial*/ false);
            IssueRead(conn);
            break;

        default:
            break;
    }
}

void ElleIPCServer::ProcessReadComplete(EllePipeConnection* conn, DWORD bytes) {
    ProcessReadComplete(conn, bytes, false);
}

void ElleIPCServer::ProcessReadComplete(EllePipeConnection* conn, DWORD bytes, bool partial) {
    if (bytes == 0 && !partial) return;

    /* Accumulate into m_readBuffer to support messages that span multiple
     * ReadFile completions (PIPE_READMODE_MESSAGE returns ERROR_MORE_DATA
     * when a single message is larger than the buffer). */
    size_t off = conn->m_readBuffer.size();
    conn->m_readBuffer.resize(off + bytes);
    memcpy(conn->m_readBuffer.data() + off, conn->m_readOvl.buffer, bytes);

    if (partial) {
        /* Guard: if the accumulated message grows beyond the configured cap,
         * drop it and resync. This prevents endless deserialization failures
         * and checksum spam if a sender goes out-of-protocol or the stream
         * becomes misaligned.
         */
        uint32_t maxPayload = (uint32_t)ElleConfig::Instance().GetInt(
            "services.named_pipes.max_payload_bytes", (int64_t)ELLE_IPC_MAX_PAYLOAD);
        if (maxPayload == 0) maxPayload = ELLE_IPC_MAX_PAYLOAD;
        const size_t cap = sizeof(ELLE_IPC_HEADER) + (size_t)maxPayload;
        if (conn->m_readBuffer.size() > cap) {
            ELLE_ERROR("IPC reassembly overflow: accumulated=%zu cap=%zu (dropping)",
                       conn->m_readBuffer.size(), cap);
            conn->m_readBuffer.clear();
        }
        return;  /* wait for the rest */
    }

    auto tryResync = [&](const uint8_t* buf, size_t len) -> size_t {
        if (len < sizeof(ELLE_IPC_HEADER)) return len;
        const uint32_t magic = ELLE_IPC_MAGIC;
        /* Find the next plausible header boundary by scanning for magic. */
        for (size_t i = 1; i + sizeof(ELLE_IPC_HEADER) <= len; i++) {
            uint32_t m = 0;
            memcpy(&m, buf + i, sizeof(uint32_t));
            if (m == magic) return i;
        }
        return len;
    };

    ElleIPCMessage msg;
    if (ElleIPCMessage::Deserialize(
            conn->m_readBuffer.data(), (uint32_t)conn->m_readBuffer.size(), msg)) {
        conn->m_remoteService = (ELLE_SERVICE_ID)msg.header.source_svc;
        if (m_handler) {
            m_handler(msg, (ELLE_SERVICE_ID)msg.header.source_svc);
        }
        conn->m_readBuffer.clear();
        return;
    }

    /* Deserialization failed. Attempt to resynchronize by scanning for the next
     * header magic in the accumulated buffer and keeping the tail. This avoids
     * persistent WARN spam and allows recovery if a peer sends an out-of-protocol
     * frame or the stream becomes misaligned.
     */
    ELLE_WARN("IPC deserialize failed: accumulated=%zu head=%s",
              conn->m_readBuffer.size(),
              ElleHexPreview(conn->m_readBuffer.data(), conn->m_readBuffer.size(), 32).c_str());
    size_t drop = tryResync(conn->m_readBuffer.data(), conn->m_readBuffer.size());
    if (drop < conn->m_readBuffer.size()) {
        ELLE_WARN("IPC desync: dropping %zu bytes to resync (%zu bytes retained)",
                  drop, conn->m_readBuffer.size() - drop);
        std::vector<uint8_t> retained(conn->m_readBuffer.begin() + drop,
                                      conn->m_readBuffer.end());
        conn->m_readBuffer.swap(retained);
    } else {
        ELLE_WARN("Failed to deserialize IPC message (%zu bytes accumulated) - clearing buffer",
                  conn->m_readBuffer.size());
        conn->m_readBuffer.clear();
    }
}

bool ElleIPCServer::Send(ELLE_SERVICE_ID target, const ElleIPCMessage& msg) {
    std::lock_guard<std::mutex> lock(m_connMutex);
    for (auto& conn : m_connections) {
        if (conn->m_connected && conn->m_remoteService == target) {
            /* Ownership of the wire bytes AND the OVERLAPPED lives on the
             * connection until the IOCP completion fires. Previously both
             * were stack-locals — the stack frame died before WriteFile's
             * async completion, corrupting whatever happened to land in
             * that memory. Now the PendingWrite struct is heap-allocated,
             * keyed by its contained overlapped pointer, and released by
             * ReleasePendingWrite() on completion. */
            std::lock_guard<std::mutex> wlock(conn->m_writeMutex);
            auto pw = std::make_unique<EllePipeConnection::PendingWrite>();
            pw->data = msg.Serialize();
            pw->ovl.operation = ELLE_IOCP_OP_WRITE;
            pw->ovl.service_id = m_serviceId;
            pw->ovl.pipe_handle = conn->m_hPipe;

            DWORD written = 0;
            BOOL result = WriteFile(conn->m_hPipe, pw->data.data(),
                                    (DWORD)pw->data.size(), &written,
                                    &pw->ovl.overlapped);
            if (result || GetLastError() == ERROR_IO_PENDING) {
                conn->m_pendingWrites.push_back(std::move(pw));
                return true;
            }
            return false;
        }
    }
    return false;
}

void ElleIPCServer::Broadcast(const ElleIPCMessage& msg) {
    std::lock_guard<std::mutex> lock(m_connMutex);
    for (auto& conn : m_connections) {
        if (!conn->m_connected) continue;
        std::lock_guard<std::mutex> wlock(conn->m_writeMutex);
        auto pw = std::make_unique<EllePipeConnection::PendingWrite>();
        pw->data = msg.Serialize();
        pw->ovl.operation = ELLE_IOCP_OP_WRITE;
        pw->ovl.service_id = m_serviceId;
        pw->ovl.pipe_handle = conn->m_hPipe;

        DWORD written = 0;
        BOOL result = WriteFile(conn->m_hPipe, pw->data.data(),
                                (DWORD)pw->data.size(), &written,
                                &pw->ovl.overlapped);
        if (result || GetLastError() == ERROR_IO_PENDING) {
            conn->m_pendingWrites.push_back(std::move(pw));
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

    /* Tag this conn as client-owned so the IOCP dispatcher routes inbound
     * reads back to THIS client instead of defaulting to the server path. */
    m_conn->m_clientOwner = this;

    /* Set message mode on the client handle too. If this fails, the client may
     * see coalesced frames (byte-stream semantics) which breaks our message
     * framing assumptions and causes checksum/deserialize spam. */
    {
        DWORD mode = PIPE_READMODE_MESSAGE;
        if (!SetNamedPipeHandleState(m_conn->m_hPipe, &mode, nullptr, nullptr)) {
            ELLE_WARN("SetNamedPipeHandleState(PIPE_READMODE_MESSAGE) failed (target=%u): %u",
                      (unsigned)targetService, (unsigned)GetLastError());
        }
    }

    /* Associate with IOCP. Previously the return was unchecked; a failed
     * association silently breaks all async reads/writes on this client. */
    if (!CreateIoCompletionPort(m_conn->m_hPipe, m_hIOCP,
                                (ULONG_PTR)m_conn.get(), 0)) {
        ELLE_ERROR("Client IOCP association failed (target=%u): %d",
                   (unsigned)targetService, GetLastError());
        CloseHandle(m_conn->m_hPipe);
        m_conn->m_hPipe = INVALID_HANDLE_VALUE;
        m_conn.reset();
        return false;
    }

    m_conn->m_connected = true;
    m_conn->m_remoteService = targetService;

    /* Start reading */
    m_conn->m_readOvl.operation = ELLE_IOCP_OP_READ;
    m_conn->m_readOvl.pipe_handle = m_conn->m_hPipe;

    ReadFile(m_conn->m_hPipe, m_conn->m_readOvl.buffer, ELLE_PIPE_BUFFER_SIZE,
             nullptr, &m_conn->m_readOvl.overlapped);

    return true;
}

/*──────────────────────────────────────────────────────────────────────────────
 * Client-side IOCP completion handler.
 * Previously this was declared-only and the IOCP worker always routed to the
 * server. Result: every message the server pushed to a client was silently
 * dropped. This function deserializes the inbound frame, fires the registered
 * handler, and re-posts the overlapped read so the pipe keeps receiving.
 *──────────────────────────────────────────────────────────────────────────────*/
void ElleIPCClient::OnIOComplete(ELLE_IOCP_OVERLAPPED* ovl, DWORD bytesTransferred, DWORD error) {
    if (!m_conn) return;

    if (error != 0) {
        if (error == ERROR_BROKEN_PIPE || error == ERROR_PIPE_NOT_CONNECTED) {
            m_conn->m_connected = false;
            ELLE_DEBUG("Client pipe disconnected (target=%u)", (unsigned)m_targetService);
        } else if (error == ERROR_MORE_DATA) {
            /* Large message — accumulate the chunk and keep reading. */
            size_t off = m_conn->m_readBuffer.size();
            m_conn->m_readBuffer.resize(off + bytesTransferred);
            memcpy(m_conn->m_readBuffer.data() + off,
                   m_conn->m_readOvl.buffer, bytesTransferred);
            /* Re-post read to pick up the tail. */
            ZeroMemory(&m_conn->m_readOvl.overlapped, sizeof(OVERLAPPED));
            m_conn->m_readOvl.operation = ELLE_IOCP_OP_READ;
            ReadFile(m_conn->m_hPipe, m_conn->m_readOvl.buffer,
                     ELLE_PIPE_BUFFER_SIZE, nullptr,
                     &m_conn->m_readOvl.overlapped);
            return;
        } else {
            ELLE_WARN("Client IOCP error %u on target=%u", error, (unsigned)m_targetService);
        }
        return;
    }

    switch (ovl->operation) {
        case ELLE_IOCP_OP_READ: {
            if (bytesTransferred > 0) {
                /* Append this chunk to the reassembly buffer, then attempt
                 * to deserialize the whole message. Supports messages that
                 * span multiple IOCP completions via ERROR_MORE_DATA above. */
                size_t off = m_conn->m_readBuffer.size();
                m_conn->m_readBuffer.resize(off + bytesTransferred);
                memcpy(m_conn->m_readBuffer.data() + off,
                       m_conn->m_readOvl.buffer, bytesTransferred);

                ElleIPCMessage msg;
                if (ElleIPCMessage::Deserialize(
                        m_conn->m_readBuffer.data(),
                        (uint32_t)m_conn->m_readBuffer.size(), msg)) {
                    if (m_handler) {
                        m_handler(msg, (ELLE_SERVICE_ID)msg.header.source_svc);
                    }
                } else {
                    ELLE_WARN("Client failed to deserialize %zu bytes from target=%u",
                              m_conn->m_readBuffer.size(), (unsigned)m_targetService);
                }
                m_conn->m_readBuffer.clear();
            }
            /* Always re-post the read so further server→client messages land. */
            ZeroMemory(&m_conn->m_readOvl.overlapped, sizeof(OVERLAPPED));
            m_conn->m_readOvl.operation = ELLE_IOCP_OP_READ;
            m_conn->m_readOvl.pipe_handle = m_conn->m_hPipe;
            BOOL ok = ReadFile(m_conn->m_hPipe, m_conn->m_readOvl.buffer,
                               ELLE_PIPE_BUFFER_SIZE, nullptr,
                               &m_conn->m_readOvl.overlapped);
            if (!ok && GetLastError() != ERROR_IO_PENDING) {
                m_conn->m_connected = false;
                ELLE_WARN("Client re-post ReadFile failed: %d", GetLastError());
            }
            break;
        }
        case ELLE_IOCP_OP_WRITE:
            /* Write completed — PendingWrite cleanup happens in the Hub's
             * DispatchIOComplete classifier path; nothing to do here.      */
            break;
        default:
            break;
    }
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
    std::lock_guard<std::mutex> lock(m_conn->m_writeMutex);

    /* Same ownership pattern as server-side Send — the wire bytes and the
     * OVERLAPPED both live on the connection until the IOCP completion
     * releases them. Fixes use-after-free when WriteFile returns
     * ERROR_IO_PENDING and the old stack-local buffer got reclaimed.     */
    auto pw = std::make_unique<EllePipeConnection::PendingWrite>();
    pw->data = msg.Serialize();
    pw->ovl.operation = ELLE_IOCP_OP_WRITE;
    pw->ovl.service_id = m_myService;
    pw->ovl.pipe_handle = m_conn->m_hPipe;

    DWORD written = 0;
    BOOL result = WriteFile(m_conn->m_hPipe, pw->data.data(),
                            (DWORD)pw->data.size(), &written,
                            &pw->ovl.overlapped);
    if (result || GetLastError() == ERROR_IO_PENDING) {
        m_conn->m_pendingWrites.push_back(std::move(pw));
        return true;
    }
    return false;
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

    /* Cancel any outstanding I/O BEFORE waking workers or destroying conns.
     * This forces pending overlapped ops to complete with
     * ERROR_OPERATION_ABORTED so their IOCP entries drain safely.
     * Previously we cleared m_connections / m_clients while the IOCP thread
     * could still be about to dereference those same pointers — classic
     * use-after-free on shutdown.                                          */
    {
        std::lock_guard<std::mutex> lock(m_server.m_connMutex);
        for (auto& conn : m_server.m_connections) {
            if (conn && conn->m_hPipe != INVALID_HANDLE_VALUE) {
                CancelIoEx(conn->m_hPipe, nullptr);
            }
        }
    }
    {
        std::lock_guard<std::mutex> lock(m_clientMutex);
        for (auto& kv : m_clients) {
            auto* c = kv.second.get();
            if (c && c->m_conn && c->m_conn->m_hPipe != INVALID_HANDLE_VALUE) {
                CancelIoEx(c->m_conn->m_hPipe, nullptr);
            }
        }
    }

    /* Post completion to wake worker threads so they observe m_running = false */
    for (size_t i = 0; i < m_workers.size(); i++) {
        PostQueuedCompletionStatus(m_hIOCP, 0, 0, nullptr);
    }

    /* JOIN workers — guarantees no more completions will touch conns after
     * this point. Required before we destroy the conn vectors below. */
    for (auto& t : m_workers) {
        if (t.joinable()) t.join();
    }
    m_workers.clear();

    /* Now safe to tear down pipes — no threads observing them. */
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

bool ElleIPCHub::IsConnectedTo(ELLE_SERVICE_ID target) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_clientMutex));
    auto it = m_clients.find(target);
    return it != m_clients.end() && it->second->IsConnected();
}

bool ElleIPCHub::Send(ELLE_SERVICE_ID target, const ElleIPCMessage& msg) {
    m_sent++;
    /* Stamp the target so /api/diag/wires can show "we last spoke to
     * Cognitive at T". Note: this records ATTEMPTED contact, not
     * confirmed delivery. The dispatch path below stamps the SUCCESS
     * branches separately so a target that exists but is jammed will
     * still age out as "stale". */

    /* Try existing client connection first */
    {
        std::lock_guard<std::mutex> lock(m_clientMutex);
        auto it = m_clients.find(target);
        if (it != m_clients.end() && it->second->IsConnected()) {
            const bool ok = it->second->Send(msg);
            if (ok) StampLastSeen(target);
            return ok;
        }
    }

    /* Fall back to server (if target connected as client to us) */
    if (m_server.Send(target, msg)) { StampLastSeen(target); return true; }

    /* Last resort — lazy reconnect. We may have missed the startup window,
     * the peer may have restarted, or this target wasn't declared as a
     * dependency. Try to open a client pipe now with a short timeout and
     * send over it. This is what lets chat recover after Cognitive comes
     * up late or gets restarted. */
    if (ConnectTo(target, 1500)) {
        std::lock_guard<std::mutex> lock(m_clientMutex);
        auto it = m_clients.find(target);
        if (it != m_clients.end() && it->second->IsConnected()) {
            const bool ok = it->second->Send(msg);
            if (ok) StampLastSeen(target);
            return ok;
        }
    }
    return false;
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
    /* Stamp the sender — proves the wire to that service is alive in
     * the inbound direction. /api/diag/wires uses this to differentiate
     * "we sent but never heard back" from "two-way conversation". */
    StampLastSeen(sender);

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
                auto* conn    = reinterpret_cast<EllePipeConnection*>(completionKey);
                DispatchIOComplete(elleOvl, 0, err, conn);
            }
            continue;
        }

        if (!ovl) continue; /* Shutdown signal */

        auto* elleOvl = reinterpret_cast<ELLE_IOCP_OVERLAPPED*>(ovl);
        auto* conn    = reinterpret_cast<EllePipeConnection*>(completionKey);
        DispatchIOComplete(elleOvl, bytesTransferred, 0, conn);
    }
}

/* Route the completion to whichever side owns the connection.
 *
 * We use the completion key (set to the EllePipeConnection* in both
 * CreateIoCompletionPort calls) as the authoritative identifier — NOT
 * offsetof arithmetic on the overlapped pointer, because write
 * completions arrive on m_writeOvl and the offsetof(... m_readOvl) trick
 * used previously produced a wrong pointer on every write.
 *
 * Write completions: release the PendingWrite that owns the overlapped
 * pointer, freeing the wire-bytes buffer after the async WriteFile is
 * truly done. Read/connect completions: route by owner flag.           */
void ElleIPCHub::DispatchIOComplete(ELLE_IOCP_OVERLAPPED* ovl, DWORD bytes, DWORD err,
                                    EllePipeConnection* conn) {
    if (!conn) {
        /* Shouldn't happen — worker thread always passes completionKey. */
        return;
    }

    /* Classify completion by pointer location within the connection. */
    auto* ovlBase = reinterpret_cast<uint8_t*>(ovl);
    auto* connBase = reinterpret_cast<uint8_t*>(conn);
    bool isWrite = (ovlBase == connBase + EllePipeConnection::WriteOvlOffset())
                || (ovl->operation == ELLE_IOCP_OP_WRITE);

    /* Also check the PendingWrite list — write completions for buffered
     * async sends are NOT on m_writeOvl; each has its own overlapped. */
    if (!isWrite) {
        std::lock_guard<std::mutex> lock(conn->m_writeMutex);
        for (auto& pw : conn->m_pendingWrites) {
            if (&pw->ovl == ovl) { isWrite = true; break; }
        }
    }

    if (isWrite) {
        /* Free the owning PendingWrite (if any). */
        std::lock_guard<std::mutex> lock(conn->m_writeMutex);
        auto& pw = conn->m_pendingWrites;
        pw.erase(std::remove_if(pw.begin(), pw.end(),
                 [ovl](const std::unique_ptr<EllePipeConnection::PendingWrite>& p) {
                     return &p->ovl == ovl;
                 }), pw.end());
        return;
    }

    /* Read / connect — route by owner. */
    if (conn->m_clientOwner) {
        reinterpret_cast<ElleIPCClient*>(conn->m_clientOwner)->OnIOComplete(ovl, bytes, err);
    } else {
        m_server.OnIOComplete(ovl, bytes, err);
    }
}
