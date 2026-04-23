/*******************************************************************************
 * ElleSQLConn.cpp — SQL Server Connection Pool Implementation
 ******************************************************************************/
#include "ElleSQLConn.h"
#include "ElleLogger.h"

#include <sql.h>
#include <sqlext.h>
#include <sstream>
#include <algorithm>
#include <set>
#include <random>

/*──────────────────────────────────────────────────────────────────────────────
 * SQLRow helpers
 *──────────────────────────────────────────────────────────────────────────────*/
int64_t SQLRow::GetInt(size_t idx) const {
    if (idx >= values.size() || values[idx].empty()) return 0;
    try { return std::stoll(values[idx]); } catch (...) { return 0; }
}

double SQLRow::GetFloat(size_t idx) const {
    if (idx >= values.size() || values[idx].empty()) return 0.0;
    try { return std::stod(values[idx]); } catch (...) { return 0.0; }
}

bool SQLRow::IsNull(size_t idx) const {
    return idx >= values.size() || values[idx] == "NULL";
}

int SQLResultSet::ColIndex(const std::string& name) const {
    for (size_t i = 0; i < columns.size(); i++) {
        if (columns[i].name == name) return (int)i;
    }
    return -1;
}

/*──────────────────────────────────────────────────────────────────────────────
 * SQLConnection
 *──────────────────────────────────────────────────────────────────────────────*/
SQLConnection::SQLConnection() {}
SQLConnection::~SQLConnection() { Disconnect(); }

bool SQLConnection::AllocHandles() {
    SQLRETURN ret;
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
    if (!SQL_SUCCEEDED(ret)) return false;

    ret = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(ret)) { FreeHandles(); return false; }

    ret = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
    if (!SQL_SUCCEEDED(ret)) { FreeHandles(); return false; }

    return true;
}

void SQLConnection::FreeHandles() {
    if (m_hDbc) { SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc); m_hDbc = nullptr; }
    if (m_hEnv) { SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv); m_hEnv = nullptr; }
}

bool SQLConnection::Connect(const std::string& connectionString) {
    if (m_connected) Disconnect();
    m_connStr = connectionString;

    if (!AllocHandles()) {
        m_lastError = "Failed to allocate ODBC handles";
        return false;
    }

    /* Set connection timeout */
    SQLSetConnectAttrA(m_hDbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)10, 0);

    /* Connect using named pipe connection string */
    SQLCHAR outStr[1024];
    SQLSMALLINT outLen;
    SQLRETURN ret = SQLDriverConnectA(m_hDbc, nullptr,
        (SQLCHAR*)m_connStr.c_str(), (SQLSMALLINT)m_connStr.length(),
        outStr, sizeof(outStr), &outLen, SQL_DRIVER_NOPROMPT);

    if (!SQL_SUCCEEDED(ret)) {
        m_lastError = GetDiagnostics(SQL_HANDLE_DBC, m_hDbc);
        FreeHandles();
        return false;
    }

    m_connected = true;
    m_lastUsed = ELLE_MS_NOW();
    return true;
}

void SQLConnection::Disconnect() {
    if (m_inTransaction) Rollback();
    if (m_connected && m_hDbc) {
        SQLDisconnect(m_hDbc);
    }
    FreeHandles();
    m_connected = false;
}

bool SQLConnection::IsConnected() const { return m_connected; }

bool SQLConnection::Ping() {
    if (!m_connected) return false;
    SQLResultSet rs = Execute("SELECT 1");
    return rs.success;
}

SQLResultSet SQLConnection::Execute(const std::string& sql) {
    SQLResultSet result;
    if (!m_connected) {
        result.error = "Not connected";
        return result;
    }

    SQLHSTMT hStmt = nullptr;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        result.error = "Failed to allocate statement";
        return result;
    }

    ret = SQLExecDirectA(hStmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA) {
        result.error = GetDiagnostics(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return result;
    }

    /* Get column info */
    SQLSMALLINT numCols = 0;
    SQLNumResultCols(hStmt, &numCols);

    for (SQLSMALLINT i = 1; i <= numCols; i++) {
        SQLColumn col;
        SQLCHAR colName[256];
        SQLSMALLINT nameLen, dataType, nullable;
        SQLULEN colSize;
        SQLSMALLINT decDigits;

        SQLDescribeColA(hStmt, i, colName, sizeof(colName), &nameLen,
                        &dataType, &colSize, &decDigits, &nullable);
        col.name = std::string((char*)colName, nameLen);
        col.type = dataType;
        col.size = (uint32_t)colSize;
        result.columns.push_back(col);
    }

    /* Fetch rows */
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLRow row;
        for (SQLSMALLINT i = 1; i <= numCols; i++) {
            SQLCHAR buffer[8192];
            SQLLEN indicator;
            ret = SQLGetData(hStmt, i, SQL_C_CHAR, buffer, sizeof(buffer), &indicator);
            if (indicator == SQL_NULL_DATA) {
                row.values.push_back("NULL");
            } else {
                row.values.push_back(std::string((char*)buffer));
            }
        }
        result.rows.push_back(row);
    }

    SQLLEN rowCount;
    SQLRowCount(hStmt, &rowCount);
    result.rows_affected = rowCount;
    result.success = true;

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    m_lastUsed = ELLE_MS_NOW();
    return result;
}

SQLResultSet SQLConnection::ExecuteParams(const std::string& sql, 
                                           const std::vector<std::string>& params) {
    /* Build parameterized query by replacing ? with escaped values */
    std::string query = sql;
    for (size_t i = 0; i < params.size(); i++) {
        size_t pos = query.find('?');
        if (pos == std::string::npos) break;
        /* Escape single quotes */
        std::string escaped = params[i];
        size_t qpos = 0;
        while ((qpos = escaped.find('\'', qpos)) != std::string::npos) {
            escaped.insert(qpos, "'");
            qpos += 2;
        }
        query.replace(pos, 1, "'" + escaped + "'");
    }
    return Execute(query);
}

SQLResultSet SQLConnection::CallProc(const std::string& proc, 
                                      const std::vector<std::string>& params) {
    std::ostringstream ss;
    ss << "EXEC " << proc;
    for (size_t i = 0; i < params.size(); i++) {
        ss << (i == 0 ? " " : ", ");
        /* Check if numeric */
        bool isNum = !params[i].empty();
        for (char c : params[i]) {
            if (!std::isdigit(c) && c != '.' && c != '-') { isNum = false; break; }
        }
        if (isNum) ss << params[i];
        else ss << "'" << params[i] << "'";
    }
    return Execute(ss.str());
}

int64_t SQLConnection::ExecuteScalar(const std::string& sql) {
    SQLResultSet rs = Execute(sql);
    if (rs.success && !rs.rows.empty() && !rs.rows[0].values.empty()) {
        return rs.rows[0].GetInt(0);
    }
    return 0;
}

bool SQLConnection::ExecuteNonQuery(const std::string& sql) {
    SQLResultSet rs = Execute(sql);
    return rs.success;
}

bool SQLConnection::BeginTransaction() {
    if (!m_connected || m_inTransaction) return false;
    SQLRETURN ret = SQLSetConnectAttrA(m_hDbc, SQL_ATTR_AUTOCOMMIT, 
                                        (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
    m_inTransaction = SQL_SUCCEEDED(ret);
    return m_inTransaction;
}

bool SQLConnection::Commit() {
    if (!m_inTransaction) return false;
    SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, m_hDbc, SQL_COMMIT);
    SQLSetConnectAttrA(m_hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
    m_inTransaction = false;
    return SQL_SUCCEEDED(ret);
}

bool SQLConnection::Rollback() {
    if (!m_inTransaction) return false;
    SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, m_hDbc, SQL_ROLLBACK);
    SQLSetConnectAttrA(m_hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
    m_inTransaction = false;
    return SQL_SUCCEEDED(ret);
}

std::string SQLConnection::GetDiagnostics(int16_t handleType, void* handle) {
    SQLCHAR state[6], msg[1024];
    SQLINTEGER nativeErr;
    SQLSMALLINT msgLen;
    std::string result;
    SQLSMALLINT i = 1;
    while (SQLGetDiagRecA(handleType, handle, i++, state, &nativeErr, 
                           msg, sizeof(msg), &msgLen) == SQL_SUCCESS) {
        if (!result.empty()) result += "; ";
        result += std::string((char*)state) + ": " + std::string((char*)msg, msgLen);
    }
    return result.empty() ? "Unknown ODBC error" : result;
}

/*──────────────────────────────────────────────────────────────────────────────
 * CONNECTION POOL
 *──────────────────────────────────────────────────────────────────────────────*/
ElleSQLPool& ElleSQLPool::Instance() {
    static ElleSQLPool inst;
    return inst;
}

bool ElleSQLPool::Initialize(const std::string& connectionString, uint32_t poolSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) return true;

    m_connStr = connectionString;
    m_poolSize = poolSize;

    for (uint32_t i = 0; i < poolSize; i++) {
        std::shared_ptr<SQLConnection> conn;
        if (CreateConnection(conn)) {
            m_available.push(conn);
        } else {
            ELLE_WARN("SQL pool: Failed to create connection %d/%d", i + 1, poolSize);
        }
    }

    m_initialized = !m_available.empty();
    return m_initialized;
}

void ElleSQLPool::Shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    while (!m_available.empty()) {
        m_available.front()->Disconnect();
        m_available.pop();
    }
    m_initialized = false;
}

bool ElleSQLPool::CreateConnection(std::shared_ptr<SQLConnection>& conn) {
    conn = std::make_shared<SQLConnection>();
    return conn->Connect(m_connStr);
}

std::shared_ptr<SQLConnection> ElleSQLPool::Acquire(uint32_t timeoutMs) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_cv.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                       [this] { return !m_available.empty(); })) {
        ELLE_WARN("SQL pool: Acquire timeout after %dms", timeoutMs);
        return nullptr;
    }

    auto conn = m_available.front();
    m_available.pop();

    /* Validate connection */
    if (!conn->Ping()) {
        ELLE_WARN("SQL pool: Stale connection, reconnecting...");
        conn->Disconnect();
        if (!conn->Connect(m_connStr)) {
            ELLE_ERROR("SQL pool: Reconnection failed");
            return nullptr;
        }
    }

    return conn;
}

void ElleSQLPool::Release(std::shared_ptr<SQLConnection> conn) {
    if (!conn) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    m_available.push(conn);
    m_cv.notify_one();
}

SQLResultSet ElleSQLPool::Query(const std::string& sql) {
    auto conn = Acquire();
    if (!conn) return {};
    m_totalQueries++;
    auto result = conn->Execute(sql);
    Release(conn);
    return result;
}

SQLResultSet ElleSQLPool::QueryParams(const std::string& sql, const std::vector<std::string>& params) {
    auto conn = Acquire();
    if (!conn) return {};
    m_totalQueries++;
    auto result = conn->ExecuteParams(sql, params);
    Release(conn);
    return result;
}

SQLResultSet ElleSQLPool::CallProc(const std::string& proc, const std::vector<std::string>& params) {
    auto conn = Acquire();
    if (!conn) return {};
    m_totalQueries++;
    auto result = conn->CallProc(proc, params);
    Release(conn);
    return result;
}

bool ElleSQLPool::Exec(const std::string& sql) {
    auto conn = Acquire();
    if (!conn) return false;
    m_totalQueries++;
    bool ok = conn->ExecuteNonQuery(sql);
    Release(conn);
    return ok;
}

int64_t ElleSQLPool::Scalar(const std::string& sql) {
    auto conn = Acquire();
    if (!conn) return 0;
    m_totalQueries++;
    int64_t val = conn->ExecuteScalar(sql);
    Release(conn);
    return val;
}

uint32_t ElleSQLPool::AvailableConnections() const {
    return (uint32_t)m_available.size();
}

/*──────────────────────────────────────────────────────────────────────────────
 * DATABASE HELPER IMPLEMENTATIONS
 *──────────────────────────────────────────────────────────────────────────────*/
namespace ElleDB {

bool SubmitIntent(const ELLE_INTENT_RECORD& intent) {
    /* Parameterised — description/parameters are user-originated strings and
     * would break the query (and be a classic injection surface) when built
     * via concatenation.                                                    */
    return ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.IntentQueue "
        "(IntentType, Status, SourceDrive, Urgency, Confidence, Description, Parameters, "
        "RequiredTrust, CreatedMs, TimeoutMs) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);",
        {
            std::to_string(intent.type),
            std::to_string(INTENT_PENDING),
            std::to_string(intent.source_drive),
            std::to_string(intent.urgency),
            std::to_string(intent.confidence),
            std::string(intent.description),
            std::string(intent.parameters),
            std::to_string(intent.required_trust),
            std::to_string(ELLE_MS_NOW()),
            std::to_string(intent.timeout_ms)
        }).success;
}

bool GetPendingIntents(std::vector<ELLE_INTENT_RECORD>& out, uint32_t maxCount) {
    out.clear();
    /* ATOMIC CLAIM-ON-SELECT.
     *
     * Previously this called sp_GetPendingIntents which is a plain
     * SELECT WHERE Status=0 — the row stayed status=PENDING until the
     * consumer (Cognitive) got around to calling UpdateIntentStatus().
     * At a 500 ms poll tick any consumer that takes longer than that
     * (LLM calls routinely do) would see its row re-selected and
     * re-dispatched on the next QueueWorker tick, duplicating work.
     *
     * The SQL-Server-native fix: flip status → PROCESSING and RETURN
     * the selected rows in a single statement. ROWLOCK + READPAST let
     * two pollers on different threads carve up the queue cleanly
     * without blocking each other, and the UPDATE...OUTPUT guarantees
     * each row is observed by exactly one caller.
     *
     * We also stop calling the stored proc for this path — matching
     * the same atomic pattern HTTPServer already uses on
     * hardware_actions in /api/ai/hardware/actions/pending.            */
    auto rs = ElleSQLPool::Instance().QueryParams(
        "UPDATE TOP (?) q WITH (ROWLOCK, READPAST) "
        "SET Status = ? "
        "OUTPUT inserted.IntentId, inserted.IntentType, inserted.Status, "
        "       inserted.SourceDrive, inserted.Urgency, inserted.Confidence, "
        "       inserted.Description, inserted.Parameters, "
        "       inserted.RequiredTrust, inserted.CreatedMs, inserted.TimeoutMs "
        "FROM ElleCore.dbo.IntentQueue AS q "
        "WHERE q.Status = ?;",
        { std::to_string(maxCount),
          std::to_string((int)INTENT_PROCESSING),
          std::to_string((int)INTENT_PENDING) });
    if (!rs.success) return false;
    for (auto& row : rs.rows) {
        ELLE_INTENT_RECORD rec = {};
        rec.id = (uint64_t)row.GetInt(0);
        rec.type = (uint32_t)row.GetInt(1);
        rec.status = (uint32_t)row.GetInt(2);
        rec.source_drive = (uint32_t)row.GetInt(3);
        rec.urgency = (float)row.GetFloat(4);
        rec.confidence = (float)row.GetFloat(5);
        strncpy_s(rec.description,
                  row.values.size() > 6 ? row.values[6].c_str() : "", ELLE_MAX_MSG - 1);
        strncpy_s(rec.parameters,
                  row.values.size() > 7 ? row.values[7].c_str() : "", ELLE_MAX_MSG - 1);
        rec.required_trust = (uint32_t)row.GetInt(8);
        rec.created_ms = (uint64_t)row.GetInt(9);
        rec.timeout_ms = (uint64_t)row.GetInt(10);
        out.push_back(rec);
    }
    return true;
}

bool UpdateIntentStatus(uint64_t intentId, ELLE_INTENT_STATUS status, const std::string& response) {
    return ElleSQLPool::Instance().CallProc("ElleCore.dbo.sp_SubmitIntentResponse",
        {std::to_string(intentId), std::to_string(status), response}).success;
}

/* Forward declaration — EnsureActionQueueTable is defined further down the
 * file alongside the other action-queue helpers; the reaper + snapshot code
 * below needs it available early.                                          */
static void EnsureActionQueueTable();

/*──────────────────────────────────────────────────────────────────────────────
 * TIMEOUT REAPER — intents stuck in PROCESSING past their TimeoutMs.
 *
 * The atomic claim-on-select flipped their Status from PENDING to
 * PROCESSING; if the consumer never closed the loop (crash, hang,
 * lost IPC), the row stayed there forever. We re-queue rows whose
 * effective deadline (CreatedMs + max(TimeoutMs, defaultTimeoutMs))
 * has passed, with a hard cap on RetryCount that routes them to
 * INTENT_FAILED instead of an infinite retry loop.
 *
 * Returns the number of rows reaped (re-queued + failed).
 *──────────────────────────────────────────────────────────────────────────────*/
uint32_t ReapStaleIntents(uint32_t defaultTimeoutMs, uint32_t maxRetries) {
    int64_t now = (int64_t)ELLE_MS_NOW();
    auto& pool = ElleSQLPool::Instance();

    /* Step 1 — push rows past max_retries straight to FAILED so we
     * don't churn them forever. */
    auto rs1 = pool.QueryParams(
        "UPDATE ElleCore.dbo.IntentQueue WITH (ROWLOCK) "
        "SET Status = ?, CompletedMs = ?, Response = N'timeout_max_retries' "
        "WHERE Status = ? "
        "  AND RetryCount >= ? "
        "  AND ? - CreatedMs > CASE WHEN TimeoutMs > 0 THEN TimeoutMs ELSE ? END;",
        {
            std::to_string((int)INTENT_FAILED),
            std::to_string(now),
            std::to_string((int)INTENT_PROCESSING),
            std::to_string(maxRetries),
            std::to_string(now),
            std::to_string(defaultTimeoutMs)
        });
    uint32_t failed = rs1.success ? (uint32_t)rs1.rows_affected : 0;

    /* Step 2 — requeue everyone else back to PENDING and bump RetryCount. */
    auto rs2 = pool.QueryParams(
        "UPDATE ElleCore.dbo.IntentQueue WITH (ROWLOCK) "
        "SET Status = ?, RetryCount = RetryCount + 1 "
        "WHERE Status = ? "
        "  AND RetryCount < ? "
        "  AND ? - CreatedMs > CASE WHEN TimeoutMs > 0 THEN TimeoutMs ELSE ? END;",
        {
            std::to_string((int)INTENT_PENDING),
            std::to_string((int)INTENT_PROCESSING),
            std::to_string(maxRetries),
            std::to_string(now),
            std::to_string(defaultTimeoutMs)
        });
    uint32_t requeued = rs2.success ? (uint32_t)rs2.rows_affected : 0;

    return failed + requeued;
}

/*──────────────────────────────────────────────────────────────────────────────
 * TIMEOUT REAPER — actions stuck in LOCKED / EXECUTING past timeout_ms.
 *
 * Same pattern as intents: rows past max attempts go to ACTION_TIMEOUT
 * (terminal); otherwise they're re-queued with attempts += 1.
 * started_ms is what the atomic claim set; we measure from there.
 *──────────────────────────────────────────────────────────────────────────────*/
uint32_t ReapStaleActions(uint32_t defaultTimeoutMs, uint32_t maxAttempts) {
    int64_t now = (int64_t)ELLE_MS_NOW();
    auto& pool = ElleSQLPool::Instance();
    EnsureActionQueueTable();

    /* action_queue schema we lazy-create doesn't have an 'attempts'
     * column today — add it on demand so this reaper can enforce caps. */
    pool.Exec(
        "IF NOT EXISTS ("
        "  SELECT 1 FROM sys.columns "
        "  WHERE object_id = OBJECT_ID(N'ElleCore.dbo.action_queue') "
        "    AND name = N'attempts') "
        "ALTER TABLE ElleCore.dbo.action_queue "
        "  ADD attempts INT NOT NULL DEFAULT 0;");

    /* Terminal-timeout pass. */
    auto rs1 = pool.QueryParams(
        "UPDATE ElleCore.dbo.action_queue WITH (ROWLOCK) "
        "SET status = ?, completed_ms = ?, "
        "    result = N'timeout_max_attempts' "
        "WHERE status IN (?, ?) "
        "  AND attempts >= ? "
        "  AND ISNULL(started_ms, created_ms) IS NOT NULL "
        "  AND ? - ISNULL(started_ms, created_ms) > "
        "      CASE WHEN timeout_ms > 0 THEN timeout_ms ELSE ? END;",
        {
            std::to_string((int)ACTION_TIMEOUT),
            std::to_string(now),
            std::to_string((int)ACTION_LOCKED),
            std::to_string((int)ACTION_EXECUTING),
            std::to_string(maxAttempts),
            std::to_string(now),
            std::to_string(defaultTimeoutMs)
        });
    uint32_t failed = rs1.success ? (uint32_t)rs1.rows_affected : 0;

    /* Re-queue pass. */
    auto rs2 = pool.QueryParams(
        "UPDATE ElleCore.dbo.action_queue WITH (ROWLOCK) "
        "SET status = ?, attempts = attempts + 1, started_ms = NULL "
        "WHERE status IN (?, ?) "
        "  AND attempts < ? "
        "  AND ISNULL(started_ms, created_ms) IS NOT NULL "
        "  AND ? - ISNULL(started_ms, created_ms) > "
        "      CASE WHEN timeout_ms > 0 THEN timeout_ms ELSE ? END;",
        {
            std::to_string((int)ACTION_QUEUED),
            std::to_string((int)ACTION_LOCKED),
            std::to_string((int)ACTION_EXECUTING),
            std::to_string(maxAttempts),
            std::to_string(now),
            std::to_string(defaultTimeoutMs)
        });
    uint32_t requeued = rs2.success ? (uint32_t)rs2.rows_affected : 0;

    return failed + requeued;
}

/*──────────────────────────────────────────────────────────────────────────────
 * QUEUE DIAGNOSTICS — cheap COUNT(*) / windowed COUNT(*) reads for the
 * /api/diag/queues endpoint. One connection, one batched query each for
 * intents, actions, hardware_actions.
 *──────────────────────────────────────────────────────────────────────────────*/
bool GetQueueSnapshot(QueueSnapshot& out) {
    out = {};
    int64_t now      = (int64_t)ELLE_MS_NOW();
    int64_t hourAgo  = now - 3600000LL;
    auto& pool = ElleSQLPool::Instance();

    /* Intents ---------------------------------------------------------- */
    auto ri = pool.QueryParams(
        "SELECT "
        "  SUM(CASE WHEN Status = ? THEN 1 ELSE 0 END) AS pending, "
        "  SUM(CASE WHEN Status = ? THEN 1 ELSE 0 END) AS processing, "
        "  SUM(CASE WHEN Status = ? AND CompletedMs >= ? THEN 1 ELSE 0 END) AS done_1h, "
        "  SUM(CASE WHEN Status = ? AND CompletedMs >= ? THEN 1 ELSE 0 END) AS fail_1h, "
        "  SUM(CASE WHEN Status = ? AND ? - CreatedMs > "
        "           CASE WHEN TimeoutMs > 0 THEN TimeoutMs ELSE 120000 END "
        "        THEN 1 ELSE 0 END) AS stale "
        "FROM ElleCore.dbo.IntentQueue;",
        {
            std::to_string((int)INTENT_PENDING),
            std::to_string((int)INTENT_PROCESSING),
            std::to_string((int)INTENT_COMPLETED), std::to_string(hourAgo),
            std::to_string((int)INTENT_FAILED),    std::to_string(hourAgo),
            std::to_string((int)INTENT_PROCESSING), std::to_string(now)
        });
    if (ri.success && !ri.rows.empty()) {
        auto& r = ri.rows[0];
        out.intent_pending          = (uint32_t)r.GetInt(0);
        out.intent_processing       = (uint32_t)r.GetInt(1);
        out.intent_completed_1h     = (uint32_t)r.GetInt(2);
        out.intent_failed_1h        = (uint32_t)r.GetInt(3);
        out.intent_stale_processing = (uint32_t)r.GetInt(4);
    }

    /* Actions ---------------------------------------------------------- */
    EnsureActionQueueTable();
    auto ra = pool.QueryParams(
        "SELECT "
        "  SUM(CASE WHEN status = ? THEN 1 ELSE 0 END) AS queued, "
        "  SUM(CASE WHEN status = ? THEN 1 ELSE 0 END) AS locked, "
        "  SUM(CASE WHEN status = ? THEN 1 ELSE 0 END) AS executing, "
        "  SUM(CASE WHEN status = ? AND completed_ms >= ? THEN 1 ELSE 0 END) AS ok_1h, "
        "  SUM(CASE WHEN status = ? AND completed_ms >= ? THEN 1 ELSE 0 END) AS fail_1h, "
        "  SUM(CASE WHEN status = ? AND completed_ms >= ? THEN 1 ELSE 0 END) AS to_1h, "
        "  SUM(CASE WHEN status IN (?, ?) "
        "            AND ? - ISNULL(started_ms, created_ms) > "
        "                CASE WHEN timeout_ms > 0 THEN timeout_ms ELSE 60000 END "
        "        THEN 1 ELSE 0 END) AS stale "
        "FROM ElleCore.dbo.action_queue;",
        {
            std::to_string((int)ACTION_QUEUED),
            std::to_string((int)ACTION_LOCKED),
            std::to_string((int)ACTION_EXECUTING),
            std::to_string((int)ACTION_COMPLETED_SUCCESS), std::to_string(hourAgo),
            std::to_string((int)ACTION_COMPLETED_FAILURE), std::to_string(hourAgo),
            std::to_string((int)ACTION_TIMEOUT),           std::to_string(hourAgo),
            std::to_string((int)ACTION_LOCKED),
            std::to_string((int)ACTION_EXECUTING),
            std::to_string(now)
        });
    if (ra.success && !ra.rows.empty()) {
        auto& r = ra.rows[0];
        out.action_queued       = (uint32_t)r.GetInt(0);
        out.action_locked       = (uint32_t)r.GetInt(1);
        out.action_executing    = (uint32_t)r.GetInt(2);
        out.action_success_1h   = (uint32_t)r.GetInt(3);
        out.action_failure_1h   = (uint32_t)r.GetInt(4);
        out.action_timeout_1h   = (uint32_t)r.GetInt(5);
        out.action_stale_locked = (uint32_t)r.GetInt(6);
    }

    /* Hardware actions (pending vs dispatched) ------------------------- */
    auto rh = pool.Query(
        "IF EXISTS (SELECT 1 FROM sys.tables WHERE name = 'hardware_actions') "
        "  SELECT "
        "    SUM(CASE WHEN status = 'pending'    THEN 1 ELSE 0 END), "
        "    SUM(CASE WHEN status = 'dispatched' THEN 1 ELSE 0 END) "
        "  FROM ElleCore.dbo.hardware_actions;");
    if (rh.success && !rh.rows.empty()) {
        out.hardware_pending    = (uint32_t)rh.rows[0].GetInt(0);
        out.hardware_dispatched = (uint32_t)rh.rows[0].GetInt(1);
    }

    return true;
}

/* ---------------------------------------------------------------------------
 * SCHEMA NOTE (Feb 2026):
 *   The live ElleCore database uses snake_case / lowercase tables created by
 *   a prior (likely Python/Node) service. We keep compatibility with that
 *   schema instead of fighting it. Tables touched here:
 *     - dbo.messages              (existing; has conversation_id, user_id,
 *                                   role nvarchar, content, emotion_detected,
 *                                   emotion_intensity, created_at, Direction)
 *     - dbo.conversations         (existing; we bump last_message_at + count)
 *     - dbo.memory                (added by ElleAnn_MemoryDelta.sql)
 *     - dbo.memory_tags           (added by ElleAnn_MemoryDelta.sql)
 *     - dbo.world_entity          (added by ElleAnn_MemoryDelta.sql)
 *     - dbo.memory_entity_links   (added by ElleAnn_MemoryDelta.sql)
 *
 *   RUN ElleAnn_MemoryDelta.sql ONCE before using StoreMemory / RecallMemories
 *   / GetEntity / StoreEntity — otherwise those functions will fail silently.
 * --------------------------------------------------------------------------- */
static std::string RoleToStr(uint32_t role) {
    /* C++ side uses 0=system,1=user,2=elle,3=internal — live table is nvarchar */
    switch (role) {
        case 1: return "user";
        case 2: return "assistant";
        case 3: return "internal";
        default: return "system";
    }
}

static uint32_t RoleFromStr(const std::string& s) {
    std::string l = s;
    std::transform(l.begin(), l.end(), l.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    if (l == "user")                return 1;
    if (l == "assistant" || l == "elle" || l == "ai") return 2;
    if (l == "internal")            return 3;
    return 0;
}

bool StoreMessage(uint64_t convoId, uint32_t role, const std::string& content,
                  const ELLE_EMOTION_STATE& emotions, float sentiment) {
    (void)sentiment;
    /* Pick the dominant emotion from the snapshot for the emotion_detected col */
    std::string dominant = "neutral";
    float topW = 0.0f;
    static const char* names[] = {
        "joy","sadness","anger","fear","surprise","trust","curiosity","empathy"
    };
    for (int i = 0; i < 8 && i < ELLE_EMOTION_COUNT; i++) {
        if (emotions.dimensions[i] > topW) { topW = emotions.dimensions[i]; dominant = names[i]; }
    }

    const std::string roleStr = RoleToStr(role);
    const std::string direction = (role == 1) ? "in" : "out";

    /* STEP 1: Ensure the conversation row exists. messages.conversation_id is a FK
     * with CASCADE DELETE — if the conv id isn't in conversations, INSERT fails.
     * We make StoreMessage idempotent by upserting conversations first. */
    {
        auto ensureConv = ElleSQLPool::Instance().QueryParams(
            "IF NOT EXISTS (SELECT 1 FROM ElleCore.dbo.conversations WHERE id = ?) "
            "BEGIN "
            "  SET IDENTITY_INSERT ElleCore.dbo.conversations ON; "
            "  INSERT INTO ElleCore.dbo.conversations "
            "    (id, user_id, title, started_at, last_message_at, total_messages, is_active) "
            "  VALUES (?, 1, 'auto', GETUTCDATE(), GETUTCDATE(), 0, 1); "
            "  SET IDENTITY_INSERT ElleCore.dbo.conversations OFF; "
            "END;",
            { std::to_string(convoId), std::to_string(convoId) });
        if (!ensureConv.success && !ensureConv.error.empty()) {
            ELLE_WARN("StoreMessage: ensure-conversation failed: %s", ensureConv.error.c_str());
        }
    }

    /* STEP 2: Insert the message + bump conversation counters. */
    auto rs = ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.messages "
        "(conversation_id, user_id, role, content, emotion_detected, emotion_intensity, "
        " created_at, Direction) "
        "VALUES (?, COALESCE((SELECT user_id FROM ElleCore.dbo.conversations WHERE id=?), 1), "
        "        ?, ?, ?, ?, GETUTCDATE(), ?); "
        "UPDATE ElleCore.dbo.conversations "
        "  SET last_message_at = GETUTCDATE(), "
        "      total_messages  = ISNULL(total_messages,0) + 1 "
        "  WHERE id = ?;",
        {
            std::to_string(convoId),
            std::to_string(convoId),
            roleStr,
            content,
            dominant,
            std::to_string(topW),
            direction,
            std::to_string(convoId)
        });
    if (!rs.success) {
        ELLE_ERROR("StoreMessage INSERT failed for conv=%llu role=%s: %s",
                   (unsigned long long)convoId, roleStr.c_str(), rs.error.c_str());
    }
    return rs.success;
}

bool GetConversationHistory(uint64_t convoId,
                            std::vector<ELLE_CONVERSATION_MSG>& out,
                            uint32_t limit) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP (?) role, content, created_at "
        "FROM ElleCore.dbo.messages "
        "WHERE conversation_id = ? "
        "ORDER BY id DESC;",
        { std::to_string(limit), std::to_string(convoId) });
    if (!rs.success) return false;

    /* We pulled newest first — reverse into chronological order */
    for (auto it = rs.rows.rbegin(); it != rs.rows.rend(); ++it) {
        ELLE_CONVERSATION_MSG m = {};
        m.conversation_id = convoId;
        m.role = RoleFromStr(it->values.size() > 0 ? it->values[0] : std::string());
        if (it->values.size() > 1) {
            strncpy_s(m.content, it->values[1].c_str(), ELLE_MAX_MSG - 1);
        }
        m.timestamp_ms = ELLE_MS_NOW();
        out.push_back(m);
    }
    return true;
}

bool StoreMemory(const ELLE_MEMORY_RECORD& mem) {
    /* Use a unique "now" timestamp as a correlation key so we can look the new
     * row back up in a pooled-connection-safe way (OUTPUT INSERTED.id does
     * not reliably flow back through SQLExecDirectA + our result reader). */
    uint64_t nowMs = ELLE_MS_NOW();

    /* STEP 1: plain INSERT — no OUTPUT clause. */
    auto r1 = ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.memory "
        "(memory_type, tier, content, summary, emotional_valence, importance, relevance, "
        " position_x, position_y, position_z, created_ms, last_access_ms) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);",
        {
            std::to_string(mem.type),
            std::to_string(mem.tier),
            std::string(mem.content),
            std::string(mem.summary),
            std::to_string(mem.emotional_valence),
            std::to_string(mem.importance),
            std::to_string(mem.relevance),
            std::to_string(mem.position_x),
            std::to_string(mem.position_y),
            std::to_string(mem.position_z),
            std::to_string(nowMs),
            std::to_string(nowMs)
        });
    if (!r1.success) {
        ELLE_ERROR("StoreMemory INSERT failed: %s", r1.error.c_str());
        return false;
    }

    /* STEP 2: Recover the new row's id via the unique created_ms we just wrote.
     * If two memories somehow got the same ms, we take the most recent. */
    auto r2 = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 id FROM ElleCore.dbo.memory "
        "WHERE created_ms = ? ORDER BY id DESC;",
        { std::to_string(nowMs) });
    if (!r2.success || r2.rows.empty()) {
        ELLE_WARN("StoreMemory: row inserted but id lookup returned nothing (err=%s)",
                  r2.error.c_str());
        /* Row is in the table, we just can't tag it. Return true so caller
         * doesn't retry and double-insert. */
        return true;
    }
    int64_t memId = r2.rows[0].GetInt(0);

    /* STEP 3: Write each tag + entity link. */
    for (uint32_t i = 0; i < mem.tag_count && i < ELLE_MAX_TAGS; i++) {
        std::string tag = mem.tags[i];
        if (tag.empty()) continue;

        auto rt = ElleSQLPool::Instance().QueryParams(
            "INSERT INTO ElleCore.dbo.memory_tags (memory_id, tag) VALUES (?, ?);",
            { std::to_string(memId), tag });
        if (!rt.success) {
            ELLE_WARN("StoreMemory: tag insert failed mem=%lld tag=%s err=%s",
                      (long long)memId, tag.c_str(), rt.error.c_str());
        }

        auto rl = ElleSQLPool::Instance().QueryParams(
            "INSERT INTO ElleCore.dbo.memory_entity_links (memory_id, entity_id) "
            "SELECT ?, id FROM ElleCore.dbo.world_entity WHERE name = LOWER(?) "
            "AND NOT EXISTS (SELECT 1 FROM ElleCore.dbo.memory_entity_links mel "
            "                WHERE mel.memory_id = ? AND mel.entity_id = "
            "                      (SELECT id FROM ElleCore.dbo.world_entity WHERE name = LOWER(?)));",
            { std::to_string(memId), tag, std::to_string(memId), tag });
        (void)rl;
    }
    return true;
}

bool RecallMemories(const std::string& query,
                    std::vector<ELLE_MEMORY_RECORD>& out,
                    uint32_t maxCount, float minRelevance) {
    (void)minRelevance;
    if (query.empty()) return false;

    /* Build %term% pattern. Also use it for tag exact-match. */
    std::string like = "%" + query + "%";

    /* Composite recall:
     *   - any memory whose tag matches the query term (case-insensitive)
     *   - OR any memory whose content/summary contains the query substring
     *   - OR any memory linked to a world_entity whose name matches
     * Ranked by (importance*0.4 + recency_decay*0.4 + access_log*0.2)
     * where recency_decay = EXP(-ageMinutes / 10080.0)  (7-day half-life)
     */
    auto rs = ElleSQLPool::Instance().QueryParams(
        "WITH candidates AS ("
        "  SELECT DISTINCT m.id "
        "  FROM ElleCore.dbo.memory m "
        "  LEFT JOIN ElleCore.dbo.memory_tags mt ON mt.memory_id = m.id "
        "  LEFT JOIN ElleCore.dbo.memory_entity_links mel ON mel.memory_id = m.id "
        "  LEFT JOIN ElleCore.dbo.world_entity we ON we.id = mel.entity_id "
        "  WHERE  LOWER(mt.tag) = LOWER(?) "
        "      OR m.content LIKE ? "
        "      OR m.summary LIKE ? "
        "      OR LOWER(we.name) = LOWER(?) "
        ") "
        "SELECT TOP (?) m.id, m.memory_type, m.tier, m.content, m.summary, "
        "       m.emotional_valence, m.importance, m.relevance, "
        "       m.position_x, m.position_y, m.position_z, "
        "       m.access_count, m.created_ms, m.last_access_ms "
        "FROM ElleCore.dbo.memory m "
        "JOIN candidates c ON c.id = m.id "
        "ORDER BY ( m.importance * 0.4 "
        "         + EXP(-CAST(DATEDIFF(MINUTE, "
        "               DATEADD(MILLISECOND, m.last_access_ms % 86400000, "
        "                       DATEADD(SECOND, m.last_access_ms / 1000, '1970-01-01')), "
        "               GETUTCDATE()) AS FLOAT) / 10080.0) * 0.4 "
        "         + (LOG(CAST(m.access_count AS FLOAT) + 1.0) / 5.0) * 0.2 "
        "       ) DESC;",
        {
            query, like, like, query,
            std::to_string(maxCount)
        });
    if (!rs.success) return false;

    for (auto& row : rs.rows) {
        ELLE_MEMORY_RECORD rec = {};
        rec.id                = (uint64_t)row.GetInt(0);
        rec.type              = (uint32_t)row.GetInt(1);
        rec.tier              = (uint32_t)row.GetInt(2);
        strncpy_s(rec.content, row.values.size() > 3 ? row.values[3].c_str() : "",
                  ELLE_MAX_MSG - 1);
        strncpy_s(rec.summary, row.values.size() > 4 ? row.values[4].c_str() : "",
                  sizeof(rec.summary) - 1);
        rec.emotional_valence = (float)row.GetFloat(5);
        rec.importance        = (float)row.GetFloat(6);
        rec.relevance         = (float)row.GetFloat(7);
        rec.position_x        = (float)row.GetFloat(8);
        rec.position_y        = (float)row.GetFloat(9);
        rec.position_z        = (float)row.GetFloat(10);
        rec.access_count      = (uint32_t)row.GetInt(11);
        rec.created_ms        = (uint64_t)row.GetInt(12);
        rec.last_access_ms    = (uint64_t)row.GetInt(13);
        out.push_back(rec);
    }

    /* Fire-and-forget: bump access_count for retrieved memories. */
    for (auto& r : out) {
        ElleSQLPool::Instance().QueryParams(
            "UPDATE ElleCore.dbo.memory "
            "SET access_count = access_count + 1, last_access_ms = ? "
            "WHERE id = ?;",
            { std::to_string(ELLE_MS_NOW()), std::to_string(r.id) });
    }
    return true;
}

bool UpdateMemoryAccess(uint64_t memId) {
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.memory "
        "SET access_count = access_count + 1, last_access_ms = ? "
        "WHERE id = ?;",
        { std::to_string(ELLE_MS_NOW()), std::to_string(memId) }).success;
}

bool GetEntity(const std::string& name, ELLE_WORLD_ENTITY& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 id, name, entity_type, description, familiarity, sentiment, "
        "             trust, interaction_count, last_interaction_ms, mental_model "
        "FROM ElleCore.dbo.world_entity "
        "WHERE name = LOWER(?);",
        { name });
    if (!rs.success || rs.rows.empty()) return false;
    auto& row = rs.rows[0];
    out.id = (uint64_t)row.GetInt(0);
    strncpy_s(out.name,        row.values.size() > 1 ? row.values[1].c_str() : "", ELLE_MAX_NAME - 1);
    strncpy_s(out.type,        row.values.size() > 2 ? row.values[2].c_str() : "", ELLE_MAX_TAG  - 1);
    strncpy_s(out.description, row.values.size() > 3 ? row.values[3].c_str() : "", ELLE_MAX_MSG  - 1);
    out.familiarity         = (float)row.GetFloat(4);
    out.sentiment           = (float)row.GetFloat(5);
    out.trust               = (float)row.GetFloat(6);
    out.interaction_count   = (uint32_t)row.GetInt(7);
    out.last_interaction_ms = (uint64_t)row.GetInt(8);
    strncpy_s(out.mental_model, row.values.size() > 9 ? row.values[9].c_str() : "", ELLE_MAX_MSG - 1);
    return true;
}

bool UpdateEntityInteraction(uint64_t entityId) {
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.world_entity "
        "SET interaction_count = interaction_count + 1, "
        "    last_interaction_ms = ?, "
        "    familiarity = CASE WHEN familiarity + 0.02 > 1.0 THEN 1.0 "
        "                       ELSE familiarity + 0.02 END "
        "WHERE id = ?;",
        { std::to_string(ELLE_MS_NOW()), std::to_string(entityId) }).success;
}

bool StoreEntity(const ELLE_WORLD_ENTITY& entity) {
    /* Upsert by name (lowercased). */
    std::string lowered = entity.name;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return ElleSQLPool::Instance().QueryParams(
        "MERGE ElleCore.dbo.world_entity AS t "
        "USING (SELECT ? AS name) AS s "
        "ON (t.name = s.name) "
        "WHEN MATCHED THEN "
        "  UPDATE SET interaction_count = t.interaction_count + 1, "
        "             last_interaction_ms = ?, "
        "             familiarity = CASE WHEN t.familiarity + 0.02 > 1.0 "
        "                                THEN 1.0 ELSE t.familiarity + 0.02 END "
        "WHEN NOT MATCHED THEN "
        "  INSERT (name, display_name, entity_type, description, familiarity, sentiment, "
        "          trust, interaction_count, last_interaction_ms, mental_model) "
        "  VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);",
        {
            lowered,
            std::to_string(ELLE_MS_NOW()),
            lowered,
            std::string(entity.name),
            std::string(entity.type[0] ? entity.type : "person"),
            std::string(entity.description),
            std::to_string(entity.familiarity > 0 ? entity.familiarity : 0.1f),
            std::to_string(entity.sentiment),
            std::to_string(entity.trust > 0 ? entity.trust : 0.5f),
            std::to_string(entity.interaction_count > 0 ? entity.interaction_count : 1),
            std::to_string(ELLE_MS_NOW()),
            std::string(entity.mental_model)
        }).success;
}

bool UpdateTrust(int32_t delta, const std::string& reason) {
    /* Parameterised to keep Reason safe across apostrophes / newlines. */
    auto& pool = ElleSQLPool::Instance();
    bool ok = pool.QueryParams(
        "UPDATE ElleSystem.dbo.TrustState SET Score = "
        "CASE WHEN Score + ? > 100 THEN 100 "
        "WHEN Score + ? < 0 THEN 0 "
        "ELSE Score + ? END, "
        "LastChangeMs = ? WHERE Id = 1;",
        { std::to_string(delta), std::to_string(delta), std::to_string(delta),
          std::to_string((int64_t)ELLE_MS_NOW()) }).success;
    bool ok2 = pool.QueryParams(
        "INSERT INTO ElleSystem.dbo.TrustAudit (Delta, Reason, TimestampMs) VALUES (?, ?, ?);",
        { std::to_string(delta), reason, std::to_string((int64_t)ELLE_MS_NOW()) }).success;
    return ok && ok2;
}

bool GetTrustState(ELLE_TRUST_STATE& out) {
    memset(&out, 0, sizeof(out));
    auto rs = ElleSQLPool::Instance().Query(
        "SELECT TOP 1 Score, Level, Successes, Failures, TotalActions, Confidence "
        "FROM ElleSystem.dbo.TrustState WHERE Id = 1;");
    if (!rs.success || rs.rows.empty()) {
        /* Singleton row hasn't been seeded yet — fall back to safe defaults
         * (matches the seed INSERT in ElleAnn_Schema.sql).                   */
        out.score = 5; out.level = 0;
        out.successes = 0; out.failures = 0; out.total_actions = 0;
        return false;
    }
    auto& r = rs.rows[0];
    out.score         = (int32_t)r.GetInt(0);
    out.level         = (uint32_t)r.GetInt(1);
    out.successes     = (uint32_t)r.GetInt(2);
    out.failures      = (uint32_t)r.GetInt(3);
    out.total_actions = (uint32_t)r.GetInt(4);
    return true;
}

bool RegisterWorker(ELLE_SERVICE_ID svc, const std::string& name) {
    auto& pool = ElleSQLPool::Instance();
    return pool.QueryParams(
        "IF NOT EXISTS (SELECT 1 FROM ElleSystem.dbo.Workers WHERE ServiceId = ?) "
        "INSERT INTO ElleSystem.dbo.Workers "
        "(ServiceId, Name, Running, Healthy, StartedMs) VALUES (?, ?, 1, 1, ?) "
        "ELSE UPDATE ElleSystem.dbo.Workers SET Running = 1, Healthy = 1, "
        "LastHeartbeatMs = ? WHERE ServiceId = ?;",
        { std::to_string((int)svc), std::to_string((int)svc), name,
          std::to_string((int64_t)ELLE_MS_NOW()),
          std::to_string((int64_t)ELLE_MS_NOW()), std::to_string((int)svc) }).success;
}

bool UpdateWorkerHeartbeat(ELLE_SERVICE_ID svc) {
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleSystem.dbo.Workers SET LastHeartbeatMs = ?, Healthy = 1 "
        "WHERE ServiceId = ?;",
        { std::to_string((int64_t)ELLE_MS_NOW()), std::to_string((int)svc) }).success;
}

bool WriteLog(ELLE_LOG_LEVEL level, ELLE_SERVICE_ID svc, const std::string& msg) {
    return ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleSystem.dbo.Logs (Level, ServiceId, Message, TimestampMs) "
        "VALUES (?, ?, ?, ?);",
        { std::to_string((int)level), std::to_string((int)svc),
          msg, std::to_string((int64_t)ELLE_MS_NOW()) }).success;
}

/*──────────────────────────────────────────────────────────────────────────────
 * GOALS — use a dedicated snake_case dbo.goals table, lazy-created so the
 * database survives a partial schema apply. The CamelCase dbo.Goals table in
 * ElleAnn_Schema.sql is the aspirational design; live boxes we've seen use
 * the lowercase variant. Previously StoreGoal() dumped ad-hoc JSON into
 * dbo.system_settings with fragile snprintf — no matching read path existed
 * so every goal op was write-only. Now StoreGoal / UpdateGoalProgress /
 * GetActiveGoals all operate against the same table with parameterised SQL.
 *──────────────────────────────────────────────────────────────────────────────*/
static void EnsureGoalsTable() {
    ElleSQLPool::Instance().Exec(
        "IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'goals') "
        "CREATE TABLE ElleCore.dbo.goals ("
        "  id BIGINT IDENTITY(1,1) PRIMARY KEY,"
        "  description     NVARCHAR(MAX) NOT NULL,"
        "  status          INT NOT NULL DEFAULT 0,"     /* 0=active */
        "  priority        INT NOT NULL DEFAULT 2,"
        "  progress        FLOAT NOT NULL DEFAULT 0.0,"
        "  motivation      FLOAT NOT NULL DEFAULT 0.7,"
        "  source_drive    INT NOT NULL DEFAULT 0,"
        "  parent_goal_id  BIGINT NULL,"
        "  success_criteria NVARCHAR(MAX) NULL,"
        "  created_ms      BIGINT NOT NULL,"
        "  deadline_ms     BIGINT NULL,"
        "  last_progress_ms BIGINT NULL,"
        "  attempts        INT NOT NULL DEFAULT 0"
        ");");
}

bool StoreGoal(const ELLE_GOAL_RECORD& goal) {
    EnsureGoalsTable();
    return ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.goals "
        "(description, status, priority, progress, motivation, source_drive, "
        " parent_goal_id, success_criteria, created_ms, deadline_ms, attempts) "
        "VALUES (?, ?, ?, ?, ?, ?, NULLIF(?, '0'), ?, ?, NULLIF(?, '0'), ?);",
        {
            std::string(goal.description),
            std::to_string(goal.status),
            std::to_string(goal.priority),
            std::to_string(goal.progress),
            std::to_string(goal.motivation),
            std::to_string(goal.source_drive),
            std::to_string(goal.parent_goal_id),
            std::string(goal.success_criteria),
            std::to_string((int64_t)ELLE_MS_NOW()),
            std::to_string((int64_t)goal.deadline_ms),
            std::to_string(goal.attempts)
        }).success;
}

bool UpdateGoalProgress(uint64_t goalId, float progress) {
    EnsureGoalsTable();
    bool finished = progress >= 1.0f;
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.goals "
        "SET progress = ?, last_progress_ms = ?, "
        "    status = CASE WHEN ? >= 1.0 THEN 2 ELSE status END "
        "WHERE id = ?;",
        {
            std::to_string(progress),
            std::to_string((int64_t)ELLE_MS_NOW()),
            std::to_string(progress),
            std::to_string((int64_t)goalId)
        }).success && (finished || true);
}

bool GetActiveGoals(std::vector<ELLE_GOAL_RECORD>& out) {
    out.clear();
    EnsureGoalsTable();
    auto rs = ElleSQLPool::Instance().Query(
        "SELECT id, description, status, priority, progress, motivation, "
        "       source_drive, ISNULL(parent_goal_id,0), "
        "       ISNULL(success_criteria, N''), created_ms, "
        "       ISNULL(deadline_ms, 0), attempts "
        "FROM ElleCore.dbo.goals "
        "WHERE status IN (0, 1) "  /* active or paused */
        "ORDER BY priority ASC, created_ms DESC;");
    if (!rs.success) return false;
    for (auto& row : rs.rows) {
        ELLE_GOAL_RECORD g{};
        g.id             = (uint64_t)row.GetInt(0);
        std::string desc = row.values.size() > 1 ? row.values[1] : std::string();
        strncpy_s(g.description, desc.c_str(), ELLE_MAX_MSG - 1);
        g.status         = (uint32_t)row.GetInt(2);
        g.priority       = (uint32_t)row.GetInt(3);
        g.progress       = (float)row.GetFloat(4);
        g.motivation     = (float)row.GetFloat(5);
        g.source_drive   = (uint32_t)row.GetInt(6);
        g.parent_goal_id = (uint32_t)row.GetInt(7);
        std::string crit = row.values.size() > 8 ? row.values[8] : std::string();
        strncpy_s(g.success_criteria, crit.c_str(), ELLE_MAX_MSG - 1);
        g.created_ms     = (uint64_t)row.GetInt(9);
        g.deadline_ms    = (uint64_t)row.GetInt(10);
        g.attempts       = (uint32_t)row.GetInt(11);
        out.push_back(g);
    }
    return true;
}

/*──────────────────────────────────────────────────────────────────────────────
 * ACTIONS — the ActionQueue table is CamelCase in ElleAnn_Schema.sql but
 * every other live table we interact with is snake_case. We lazy-create a
 * snake_case dbo.action_queue to stay consistent with dbo.memory,
 * dbo.world_entity, dbo.hardware_actions, etc.
 *──────────────────────────────────────────────────────────────────────────────*/
static void EnsureActionQueueTable() {
    ElleSQLPool::Instance().Exec(
        "IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'action_queue') "
        "CREATE TABLE ElleCore.dbo.action_queue ("
        "  id BIGINT IDENTITY(1,1) PRIMARY KEY,"
        "  intent_id      BIGINT NULL,"
        "  action_type    INT NOT NULL,"
        "  status         INT NOT NULL DEFAULT 0,"
        "  command        NVARCHAR(MAX) NULL,"
        "  parameters     NVARCHAR(MAX) NULL,"
        "  result         NVARCHAR(MAX) NULL,"
        "  required_trust INT NOT NULL DEFAULT 0,"
        "  trust_delta    INT NOT NULL DEFAULT 0,"
        "  created_ms     BIGINT NOT NULL,"
        "  started_ms     BIGINT NULL,"
        "  completed_ms   BIGINT NULL,"
        "  timeout_ms     BIGINT NOT NULL DEFAULT 30000,"
        "  error_code     INT NOT NULL DEFAULT 0"
        ");");
}

bool SubmitAction(const ELLE_ACTION_RECORD& action) {
    EnsureActionQueueTable();
    return ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.action_queue "
        "(intent_id, action_type, status, command, parameters, required_trust, "
        " created_ms, timeout_ms) VALUES (NULLIF(?, '0'), ?, ?, ?, ?, ?, ?, ?);",
        {
            std::to_string((int64_t)action.intent_id),
            std::to_string(action.type),
            std::to_string(ACTION_QUEUED),
            std::string(action.command),
            std::string(action.parameters),
            std::to_string(action.required_trust),
            std::to_string((int64_t)ELLE_MS_NOW()),
            std::to_string((int64_t)action.timeout_ms)
        }).success;
}

bool GetPendingActions(std::vector<ELLE_ACTION_RECORD>& out, uint32_t maxCount) {
    out.clear();
    EnsureActionQueueTable();
    /* Atomic claim — flip status QUEUED → LOCKED and return the
     * claimed rows in a single statement. Two parallel callers (or a
     * caller + a tick that fires before the first one finishes) can't
     * collide on the same row any more, because ROWLOCK+READPAST lets
     * the second caller skip the rows the first one is already
     * updating, and the OUTPUT clause emits each row exactly once.   */
    auto rs = ElleSQLPool::Instance().QueryParams(
        "UPDATE TOP (?) q WITH (ROWLOCK, READPAST) "
        "SET status = ?, started_ms = ? "
        "OUTPUT inserted.id, ISNULL(inserted.intent_id,0), inserted.action_type, "
        "       inserted.status, ISNULL(inserted.command, N''), "
        "       ISNULL(inserted.parameters, N''), inserted.required_trust, "
        "       inserted.created_ms, inserted.timeout_ms "
        "FROM ElleCore.dbo.action_queue AS q "
        "WHERE q.status = ?;",
        {
            std::to_string(maxCount),
            std::to_string((int)ACTION_LOCKED),
            std::to_string((int64_t)ELLE_MS_NOW()),
            std::to_string((int)ACTION_QUEUED)
        });
    if (!rs.success) return false;
    for (auto& row : rs.rows) {
        ELLE_ACTION_RECORD a{};
        a.id             = (uint64_t)row.GetInt(0);
        a.intent_id      = (uint64_t)row.GetInt(1);
        a.type           = (uint32_t)row.GetInt(2);
        a.status         = (uint32_t)row.GetInt(3);
        std::string cmd  = row.values.size() > 4 ? row.values[4] : std::string();
        std::string prm  = row.values.size() > 5 ? row.values[5] : std::string();
        strncpy_s(a.command,    cmd.c_str(), ELLE_MAX_MSG - 1);
        strncpy_s(a.parameters, prm.c_str(), ELLE_MAX_MSG - 1);
        a.required_trust = (uint32_t)row.GetInt(6);
        a.created_ms     = (uint64_t)row.GetInt(7);
        a.timeout_ms     = (uint64_t)row.GetInt(8);
        out.push_back(a);
    }
    return true;
}

bool UpdateActionStatus(uint64_t actionId, ELLE_ACTION_STATUS status,
                        const std::string& result) {
    EnsureActionQueueTable();
    int64_t nowMs = (int64_t)ELLE_MS_NOW();
    bool terminal = (status == ACTION_COMPLETED_SUCCESS ||
                     status == ACTION_COMPLETED_FAILURE ||
                     status == ACTION_TIMEOUT ||
                     status == ACTION_CANCELLED);
    bool starting = (status == ACTION_LOCKED || status == ACTION_EXECUTING);

    /* Build a single UPDATE with only the columns that change for this
     * particular transition — avoids the CAST-'0'=1 dance entirely.       */
    std::string sql = "UPDATE ElleCore.dbo.action_queue SET status = ?";
    std::vector<std::string> params = { std::to_string((int)status) };
    if (!result.empty()) {
        sql += ", result = ?";
        params.push_back(result);
    }
    if (starting) {
        sql += ", started_ms = COALESCE(started_ms, ?)";
        params.push_back(std::to_string(nowMs));
    }
    if (terminal) {
        sql += ", completed_ms = ?";
        params.push_back(std::to_string(nowMs));
    }
    sql += " WHERE id = ?;";
    params.push_back(std::to_string((int64_t)actionId));
    return ElleSQLPool::Instance().QueryParams(sql, params).success;
}

/*──────────────────────────────────────────────────────────────────────────────
 * EMOTION snapshot aliases — callers use both the old names (StoreEmotionSnapshot
 * / GetLatestEmotionState) and the new snake-case pair (PersistEmotionSnapshot
 * / LoadLatestEmotionSnapshot). Rather than rename at every call site (which
 * risks drift), alias the old names to the new implementations.
 *──────────────────────────────────────────────────────────────────────────────*/
bool StoreEmotionSnapshot(const ELLE_EMOTION_STATE& state) {
    return PersistEmotionSnapshot(state);
}
bool GetLatestEmotionState(ELLE_EMOTION_STATE& out) {
    return LoadLatestEmotionSnapshot(out);
}

/*──────────────────────────────────────────────────────────────────────────────
 * MEMORY tier transitions — declared in the header and not yet used at call
 * sites, but implement them so the link table is complete for anyone who
 * picks them up later (and to honour the NO STUB policy).
 *──────────────────────────────────────────────────────────────────────────────*/
bool PromoteToLTM(uint64_t memId) {
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.memory SET tier = 2, "
        "       last_access_ms = ? "
        "WHERE id = ?;",
        { std::to_string((int64_t)ELLE_MS_NOW()), std::to_string((int64_t)memId) }).success;
}

bool ArchiveMemory(uint64_t memId) {
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.memory SET tier = 3, "
        "       last_access_ms = ? "
        "WHERE id = ?;",
        { std::to_string((int64_t)ELLE_MS_NOW()), std::to_string((int64_t)memId) }).success;
}

/*──────────────────────────────────────────────────────────────────────────────
 * GetSubjective — reads the wife's lived-experience answer for a given key
 * (e.g. "phase_luteal", "symptom_cramps", "wisdom_what_helps") out of
 * ElleHeart.dbo.x_subjective. The Lua file x_subjective.lua writes these
 * on every hot-reload; we read them on every Cognitive system-prompt build.
 *──────────────────────────────────────────────────────────────────────────────*/
std::string GetSubjective(const std::string& key) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "IF EXISTS (SELECT 1 FROM sys.tables t "
        "  JOIN sys.schemas s ON s.schema_id = t.schema_id "
        "  WHERE t.name = 'x_subjective' AND s.name = 'dbo') "
        "SELECT answer_text FROM ElleHeart.dbo.x_subjective "
        " WHERE subjective_key = ?;",
        { key });
    if (rs.success && !rs.rows.empty() && !rs.rows[0].values.empty())
        return rs.rows[0].values[0];
    return std::string();
}

/*──────────────────────────────────────────────────────────────────────────────
 * MEMORY CRUD helpers for /api/memory/* endpoints
 *──────────────────────────────────────────────────────────────────────────────*/
static void FillMemoryRow(ElleDB::MemoryRow& r, const SQLRow& row) {
    r.id = row.GetInt(0);
    r.type = (int)row.GetInt(1);
    r.tier = (int)row.GetInt(2);
    r.content = row.values.size() > 3 ? row.values[3] : std::string();
    r.summary = row.values.size() > 4 ? row.values[4] : std::string();
    r.emotional_valence = (float)row.GetFloat(5);
    r.importance = (float)row.GetFloat(6);
    r.relevance  = (float)row.GetFloat(7);
    r.access_count   = (int)row.GetInt(8);
    r.created_ms     = (uint64_t)row.GetInt(9);
    r.last_access_ms = (uint64_t)row.GetInt(10);
}

bool ListMemories(std::vector<MemoryRow>& out, int memory_type,
                  uint32_t limit, uint32_t offset) {
    std::string sql =
        "SELECT id, memory_type, tier, content, summary, emotional_valence, "
        "       importance, relevance, access_count, created_ms, last_access_ms "
        "FROM ElleCore.dbo.memory ";
    std::vector<std::string> params;
    if (memory_type >= 0) {
        sql += "WHERE memory_type = ? ";
        params.push_back(std::to_string(memory_type));
    }
    sql += "ORDER BY id DESC OFFSET ? ROWS FETCH NEXT ? ROWS ONLY;";
    params.push_back(std::to_string(offset));
    params.push_back(std::to_string(limit));
    auto rs = ElleSQLPool::Instance().QueryParams(sql, params);
    if (!rs.success) { ELLE_WARN("ListMemories: %s", rs.error.c_str()); return false; }
    for (auto& row : rs.rows) { MemoryRow mr; FillMemoryRow(mr, row); out.push_back(mr); }
    return true;
}

bool GetMemory(int64_t memId, MemoryRow& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT id, memory_type, tier, content, summary, emotional_valence, "
        "       importance, relevance, access_count, created_ms, last_access_ms "
        "FROM ElleCore.dbo.memory WHERE id = ?;",
        { std::to_string(memId) });
    if (!rs.success || rs.rows.empty()) return false;
    FillMemoryRow(out, rs.rows[0]);
    /* Bump access */
    ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.memory SET access_count = access_count + 1, "
        "last_access_ms = ? WHERE id = ?;",
        { std::to_string(ELLE_MS_NOW()), std::to_string(memId) });
    return true;
}

bool DeleteMemory(int64_t memId) {
    return ElleSQLPool::Instance().QueryParams(
        "DELETE FROM ElleCore.dbo.memory WHERE id = ?;",
        { std::to_string(memId) }).success;
}

bool UpdateMemoryContent(int64_t memId, const std::string& content,
                         const std::string& summary, float importance) {
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.memory "
        "SET content = ?, summary = ?, importance = ?, last_access_ms = ? "
        "WHERE id = ?;",
        { content, summary, std::to_string(importance),
          std::to_string(ELLE_MS_NOW()), std::to_string(memId) }).success;
}

/*──────────────────────────────────────────────────────────────────────────────
 * CONVERSATIONS
 *──────────────────────────────────────────────────────────────────────────────*/
bool CreateConversation(int32_t user_id, const std::string& title, int32_t& newId) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.conversations "
        "(user_id, title, started_at, last_message_at, total_messages, is_active) "
        "VALUES (?, ?, GETUTCDATE(), GETUTCDATE(), 0, 1); "
        "SELECT CAST(SCOPE_IDENTITY() AS INT);",
        { std::to_string(user_id), title });
    if (!rs.success) { ELLE_ERROR("CreateConversation: %s", rs.error.c_str()); return false; }
    if (!rs.rows.empty()) {
        newId = (int32_t)rs.rows[0].GetInt(0);
        return newId > 0;
    }
    /* Fallback: re-query by MAX(id) for this user */
    auto r2 = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 id FROM ElleCore.dbo.conversations WHERE user_id = ? ORDER BY id DESC;",
        { std::to_string(user_id) });
    if (r2.success && !r2.rows.empty()) { newId = (int32_t)r2.rows[0].GetInt(0); return true; }
    return false;
}

bool ListConversations(std::vector<ConversationRow>& out, uint32_t limit) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP (?) id, user_id, ISNULL(title,''), "
        "       CONVERT(varchar(33), started_at, 126), "
        "       CONVERT(varchar(33), last_message_at, 126), "
        "       ISNULL(total_messages,0), ISNULL(is_active,0) "
        "FROM ElleCore.dbo.conversations ORDER BY id DESC;",
        { std::to_string(limit) });
    if (!rs.success) return false;
    for (auto& row : rs.rows) {
        ConversationRow c;
        c.id = (int32_t)row.GetInt(0);
        c.user_id = (int32_t)row.GetInt(1);
        c.title = row.values.size() > 2 ? row.values[2] : "";
        c.started_at = row.values.size() > 3 ? row.values[3] : "";
        c.last_message_at = row.values.size() > 4 ? row.values[4] : "";
        c.total_messages = (int32_t)row.GetInt(5);
        c.is_active = row.GetInt(6) != 0;
        out.push_back(c);
    }
    return true;
}

bool GetConversation(int32_t convId, ConversationRow& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 id, user_id, ISNULL(title,''), "
        "       CONVERT(varchar(33), started_at, 126), "
        "       CONVERT(varchar(33), last_message_at, 126), "
        "       ISNULL(total_messages,0), ISNULL(is_active,0) "
        "FROM ElleCore.dbo.conversations WHERE id = ?;",
        { std::to_string(convId) });
    if (!rs.success || rs.rows.empty()) return false;
    auto& row = rs.rows[0];
    out.id = (int32_t)row.GetInt(0);
    out.user_id = (int32_t)row.GetInt(1);
    out.title = row.values.size() > 2 ? row.values[2] : "";
    out.started_at = row.values.size() > 3 ? row.values[3] : "";
    out.last_message_at = row.values.size() > 4 ? row.values[4] : "";
    out.total_messages = (int32_t)row.GetInt(5);
    out.is_active = row.GetInt(6) != 0;
    return true;
}

/*──────────────────────────────────────────────────────────────────────────────
 * VOICE CALLS
 *──────────────────────────────────────────────────────────────────────────────*/
bool StartVoiceCall(int32_t user_id, int32_t conv_id, std::string& callId) {
    callId = "vc-" + std::to_string(ELLE_MS_NOW());
    auto rs = ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.voice_calls "
        "(call_id, user_id, conversation_id, started_at, status, created_at) "
        "VALUES (?, ?, ?, GETUTCDATE(), 'active', GETUTCDATE());",
        { callId, std::to_string(user_id),
          conv_id > 0 ? std::to_string(conv_id) : std::string("NULL") });
    if (!rs.success) ELLE_ERROR("StartVoiceCall: %s", rs.error.c_str());
    return rs.success;
}

bool EndVoiceCall(const std::string& callId) {
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.voice_calls "
        "SET ended_at = GETUTCDATE(), status = 'ended', "
        "    duration_seconds = DATEDIFF(SECOND, started_at, GETUTCDATE()) "
        "WHERE call_id = ?;",
        { callId }).success;
}

int64_t CountTable(const std::string& table) {
    /* Table name is caller-controlled so restrict to whitelist. */
    static const std::set<std::string> whitelist = {
        "memory","messages","conversations","users","world_entity",
        "memory_tags","memory_entity_links","SelfReflections","ElleThreads",
        "voice_calls","calls","notifications","InternalNarrative",
        "CognitiveEvents","DreamIntegration","tokens","sessions"
    };
    if (whitelist.find(table) == whitelist.end()) return -1;
    auto rs = ElleSQLPool::Instance().Query(
        "SELECT COUNT(*) FROM ElleCore.dbo.[" + table + "];");
    if (!rs.success || rs.rows.empty()) return -1;
    return rs.rows[0].GetInt(0);
}

bool RecordMetric(const std::string& name, double value) {
    /* ElleSystem.dbo.Analytics doesn't exist in the live schema. Write to
     * system_settings as key/value so metrics are still persisted. */
    char val[64];
    snprintf(val, sizeof(val), "%.6f", value);
    return ElleSQLPool::Instance().QueryParams(
        "IF EXISTS (SELECT 1 FROM ElleCore.dbo.system_settings WHERE setting_key = ?) "
        "  UPDATE ElleCore.dbo.system_settings SET setting_value = ?, updated_at = GETUTCDATE() WHERE setting_key = ?; "
        "ELSE "
        "  INSERT INTO ElleCore.dbo.system_settings (setting_key, setting_value, updated_at) "
        "  VALUES (?, ?, GETUTCDATE());",
        { "metric_" + name, std::string(val), "metric_" + name,
          "metric_" + name, std::string(val) }).success;
}

/*──────────────────────────────────────────────────────────────────────────────
 * INTIMACY LAYER — Crystal profile / open threads / user presence
 *   These tables are from the prior Python system and are already populated.
 *   Cognitive pulls them every chat turn to shape Elle's tone and recall.
 *──────────────────────────────────────────────────────────────────────────────*/
bool GetCrystalProfile(int32_t user_id, CrystalProfile& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 ISNULL(traits,''), ISNULL(vulnerability_patterns,''), "
        "       ISNULL(comfort_patterns,''), ISNULL(trigger_patterns,''), "
        "       ISNULL(preferred_tone,''), ISNULL(trust_level, 0), ISNULL(intimacy_level, 0) "
        "FROM ElleCore.dbo.CrystalProfile WHERE user_id = ? ORDER BY last_updated DESC;",
        { std::to_string(user_id) });
    if (!rs.success || rs.rows.empty()) {
        out.found = false;
        return false;
    }
    auto& row = rs.rows[0];
    out.found                   = true;
    out.traits                  = row.values.size() > 0 ? row.values[0] : "";
    out.vulnerability_patterns  = row.values.size() > 1 ? row.values[1] : "";
    out.comfort_patterns        = row.values.size() > 2 ? row.values[2] : "";
    out.trigger_patterns        = row.values.size() > 3 ? row.values[3] : "";
    out.preferred_tone          = row.values.size() > 4 ? row.values[4] : "";
    out.trust_level             = (float)row.GetFloat(5);
    out.intimacy_level          = (float)row.GetFloat(6);
    return true;
}

bool GetOpenThreads(std::vector<ElleThread>& out, uint32_t limit) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP (?) thread_id, topic, ISNULL(status,''), ISNULL(emotional_weight,0), "
        "       ISNULL(intensity,0), ISNULL(thread_summary,''), ISNULL(unresolved_questions,'') "
        "FROM ElleCore.dbo.ElleThreads "
        "WHERE status IS NULL OR status <> 'resolved' "
        "ORDER BY emotional_weight DESC, last_touched DESC;",
        { std::to_string(limit) });
    if (!rs.success) return false;
    for (auto& r : rs.rows) {
        ElleThread t;
        t.id                   = (int32_t)r.GetInt(0);
        t.topic                = r.values.size() > 1 ? r.values[1] : "";
        t.status               = r.values.size() > 2 ? r.values[2] : "";
        t.emotional_weight     = (float)r.GetFloat(3);
        t.intensity            = (float)r.GetFloat(4);
        t.summary              = r.values.size() > 5 ? r.values[5] : "";
        t.unresolved_questions = r.values.size() > 6 ? r.values[6] : "";
        out.push_back(t);
    }
    return true;
}

bool GetUserPresence(int32_t user_id, UserPresence& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 ISNULL(current_silence_duration_minutes, 0), "
        "       ISNULL(silence_threshold_minutes, 0), "
        "       ISNULL(silence_interpretation, ''), "
        "       ISNULL(abnormal_silence_count, 0) "
        "FROM ElleCore.dbo.UserPresence WHERE user_id = ? ORDER BY last_seen DESC;",
        { std::to_string(user_id) });
    if (!rs.success || rs.rows.empty()) { out.found = false; return false; }
    auto& r = rs.rows[0];
    out.found                   = true;
    out.silence_minutes         = (int32_t)r.GetInt(0);
    out.threshold_minutes       = (int32_t)r.GetInt(1);
    out.silence_interpretation  = r.values.size() > 2 ? r.values[2] : "";
    out.abnormal_silence_count  = (int32_t)r.GetInt(3);
    return true;
}

bool UpdateUserPresenceOnInteraction(int32_t user_id) {
    /* Any turn from the user breaks their silence streak. */
    return ElleSQLPool::Instance().QueryParams(
        "IF EXISTS (SELECT 1 FROM ElleCore.dbo.UserPresence WHERE user_id = ?) "
        "  UPDATE ElleCore.dbo.UserPresence "
        "    SET last_seen = GETUTCDATE(), "
        "        current_silence_duration_minutes = 0, "
        "        presence_pulse_count = ISNULL(presence_pulse_count, 0) + 1 "
        "    WHERE user_id = ?; "
        "ELSE "
        "  INSERT INTO ElleCore.dbo.UserPresence "
        "    (user_id, last_seen, current_silence_duration_minutes, "
        "     silence_threshold_minutes, presence_pulse_count, abnormal_silence_count) "
        "  VALUES (?, GETUTCDATE(), 0, 120, 1, 0);",
        { std::to_string(user_id), std::to_string(user_id), std::to_string(user_id) }).success;
}

/*══════════════════════════════════════════════════════════════════════════════
 * EDUCATION — ported from app/routers/education.py
 *═════════════════════════════════════════════════════════════════════════════*/
static void RowToSubject(const SQLRow& r, LearnedSubject& s) {
    /* Column order used everywhere below — single source of truth. */
    /* id, subject, category, proficiency_level, who_taught, where_learned,
       time_to_learn_hours, notes, date_started, date_completed */
    s.id                  = (int32_t)r.GetInt(0);
    s.subject             = r.values.size() > 1 ? r.values[1] : "";
    s.category            = r.values.size() > 2 ? r.values[2] : "";
    s.proficiency_level   = (int32_t)r.GetInt(3);
    s.who_taught          = r.values.size() > 4 ? r.values[4] : "";
    s.where_learned       = r.values.size() > 5 ? r.values[5] : "";
    s.time_to_learn_hours = (float)r.GetFloat(6);
    s.notes               = r.values.size() > 7 ? r.values[7] : "";
    s.date_started        = r.values.size() > 8 ? r.values[8] : "";
    s.date_completed      = r.values.size() > 9 ? r.values[9] : "";
}
static const char* kSubjectSelect =
    "SELECT id, subject, ISNULL(category,''), proficiency_level, "
    "       ISNULL(who_taught,''), ISNULL(where_learned,''), "
    "       ISNULL(time_to_learn_hours, 0), ISNULL(notes,''), "
    "       CONVERT(NVARCHAR(33), date_started, 127), "
    "       ISNULL(CONVERT(NVARCHAR(33), date_completed, 127), '') "
    "FROM ElleCore.dbo.learned_subjects ";

bool ListSubjects(std::vector<LearnedSubject>& out,
                  const std::string& category, uint32_t limit) {
    std::string lim = std::to_string(limit > 0 ? limit : 50);
    SQLResultSet rs;
    if (category.empty()) {
        rs = ElleSQLPool::Instance().Query(
            std::string(kSubjectSelect) + "ORDER BY date_started DESC "
            "OFFSET 0 ROWS FETCH NEXT " + lim + " ROWS ONLY;");
    } else {
        rs = ElleSQLPool::Instance().QueryParams(
            std::string(kSubjectSelect) + "WHERE category = ? "
            "ORDER BY date_started DESC "
            "OFFSET 0 ROWS FETCH NEXT " + lim + " ROWS ONLY;",
            { category });
    }
    if (!rs.success) return false;
    for (auto& r : rs.rows) { LearnedSubject s; RowToSubject(r, s); out.push_back(s); }
    return true;
}

bool GetSubject(int32_t subject_id, LearnedSubject& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        std::string(kSubjectSelect) + "WHERE id = ?;",
        { std::to_string(subject_id) });
    if (!rs.success || rs.rows.empty()) return false;
    RowToSubject(rs.rows[0], out);
    return true;
}

bool CreateSubject(const LearnedSubject& in, int32_t& newId) {
    newId = 0;
    uint64_t mark = ELLE_MS_NOW();
    /* We piggy-back id recovery on the date_started timestamp — add the ms
     * marker to notes so we can find the row back in a pool-safe way. */
    std::string markedNotes = in.notes + "\n[[ins_mark=" + std::to_string(mark) + "]]";
    auto r1 = ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.learned_subjects "
        "(subject, category, proficiency_level, who_taught, where_learned, "
        " time_to_learn_hours, notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);",
        { in.subject, in.category,
          std::to_string(in.proficiency_level),
          in.who_taught, in.where_learned,
          std::to_string(in.time_to_learn_hours),
          markedNotes });
    if (!r1.success) { ELLE_ERROR("CreateSubject failed: %s", r1.error.c_str()); return false; }

    auto r2 = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 id FROM ElleCore.dbo.learned_subjects "
        "WHERE notes LIKE ? ORDER BY id DESC;",
        { std::string("%[[ins_mark=") + std::to_string(mark) + "]]%" });
    if (r2.success && !r2.rows.empty()) newId = (int32_t)r2.rows[0].GetInt(0);

    /* Strip the marker from notes now that we've recovered the id. */
    if (newId > 0) {
        ElleSQLPool::Instance().QueryParams(
            "UPDATE ElleCore.dbo.learned_subjects SET notes = ? WHERE id = ?;",
            { in.notes, std::to_string(newId) });
    }
    return newId > 0;
}

bool UpdateSubject(int32_t subject_id, const LearnedSubject& patch,
                   const std::vector<std::string>& fields) {
    if (fields.empty()) return true;
    std::string sets;
    std::vector<std::string> params;
    for (const auto& f : fields) {
        if (!sets.empty()) sets += ", ";
        sets += f + " = ?";
        if      (f == "subject")             params.push_back(patch.subject);
        else if (f == "category")            params.push_back(patch.category);
        else if (f == "proficiency_level")   params.push_back(std::to_string(patch.proficiency_level));
        else if (f == "who_taught")          params.push_back(patch.who_taught);
        else if (f == "where_learned")       params.push_back(patch.where_learned);
        else if (f == "time_to_learn_hours") params.push_back(std::to_string(patch.time_to_learn_hours));
        else if (f == "notes")               params.push_back(patch.notes);
        else if (f == "date_completed")      params.push_back(patch.date_completed);
        else return false; /* reject unknown columns — SQL injection guard */
    }
    params.push_back(std::to_string(subject_id));
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.learned_subjects SET " + sets + " WHERE id = ?;",
        params).success;
}

bool ListSubjectReferences(int32_t subject_id, std::vector<EducationReference>& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT id, subject_id, ISNULL(reference_type,''), ISNULL(reference_title,''), "
        "       ISNULL(reference_content,''), ISNULL(file_path,''), "
        "       ISNULL(relevance_score, 0.5), ISNULL(notes,'') "
        "FROM ElleCore.dbo.education_references WHERE subject_id = ? ORDER BY id DESC;",
        { std::to_string(subject_id) });
    if (!rs.success) return false;
    for (auto& r : rs.rows) {
        EducationReference e;
        e.id                = (int32_t)r.GetInt(0);
        e.subject_id        = (int32_t)r.GetInt(1);
        e.reference_type    = r.values.size() > 2 ? r.values[2] : "";
        e.reference_title   = r.values.size() > 3 ? r.values[3] : "";
        e.reference_content = r.values.size() > 4 ? r.values[4] : "";
        e.file_path         = r.values.size() > 5 ? r.values[5] : "";
        e.relevance_score   = (float)r.GetFloat(6);
        e.notes             = r.values.size() > 7 ? r.values[7] : "";
        out.push_back(e);
    }
    return true;
}

bool AddSubjectReference(const EducationReference& in) {
    return ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.education_references "
        "(subject_id, reference_type, reference_title, reference_content, "
        " file_path, relevance_score, notes) VALUES (?, ?, ?, ?, ?, ?, ?);",
        { std::to_string(in.subject_id), in.reference_type, in.reference_title,
          in.reference_content, in.file_path,
          std::to_string(in.relevance_score), in.notes }).success;
}

bool ListSubjectMilestones(int32_t subject_id, std::vector<LearningMilestone>& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT id, subject_id, milestone, ISNULL(description,''), "
        "       CONVERT(NVARCHAR(33), achieved_at, 127) "
        "FROM ElleCore.dbo.learning_milestones WHERE subject_id = ? "
        "ORDER BY achieved_at DESC;",
        { std::to_string(subject_id) });
    if (!rs.success) return false;
    for (auto& r : rs.rows) {
        LearningMilestone m;
        m.id          = (int32_t)r.GetInt(0);
        m.subject_id  = (int32_t)r.GetInt(1);
        m.milestone   = r.values.size() > 2 ? r.values[2] : "";
        m.description = r.values.size() > 3 ? r.values[3] : "";
        m.achieved_at = r.values.size() > 4 ? r.values[4] : "";
        out.push_back(m);
    }
    return true;
}

bool AddSubjectMilestone(const LearningMilestone& in) {
    return ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.learning_milestones (subject_id, milestone, description) "
        "VALUES (?, ?, ?);",
        { std::to_string(in.subject_id), in.milestone, in.description }).success;
}

bool ListSkills(std::vector<Skill>& out, const std::string& category) {
    SQLResultSet rs;
    const char* base =
        "SELECT id, skill_name, ISNULL(category,''), proficiency, "
        "       ISNULL(learned_from_subject_id, 0), times_used, "
        "       ISNULL(CONVERT(NVARCHAR(33), last_used, 127), ''), ISNULL(notes,'') "
        "FROM ElleCore.dbo.skills ";
    if (category.empty()) {
        rs = ElleSQLPool::Instance().Query(std::string(base) + "ORDER BY skill_name;");
    } else {
        rs = ElleSQLPool::Instance().QueryParams(
            std::string(base) + "WHERE category = ? ORDER BY skill_name;", { category });
    }
    if (!rs.success) return false;
    for (auto& r : rs.rows) {
        Skill s;
        s.id                      = (int32_t)r.GetInt(0);
        s.skill_name              = r.values.size() > 1 ? r.values[1] : "";
        s.category                = r.values.size() > 2 ? r.values[2] : "";
        s.proficiency             = (int32_t)r.GetInt(3);
        s.learned_from_subject_id = (int32_t)r.GetInt(4);
        s.times_used              = (int32_t)r.GetInt(5);
        s.last_used               = r.values.size() > 6 ? r.values[6] : "";
        s.notes                   = r.values.size() > 7 ? r.values[7] : "";
        out.push_back(s);
    }
    return true;
}

bool CreateSkill(const Skill& in, int32_t& newId) {
    newId = 0;
    std::vector<std::string> params = {
        in.skill_name, in.category, std::to_string(in.proficiency),
        in.learned_from_subject_id > 0 ? std::to_string(in.learned_from_subject_id) : std::string(),
        in.notes
    };
    auto r1 = ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.skills "
        "(skill_name, category, proficiency, learned_from_subject_id, notes) "
        "VALUES (?, ?, ?, NULLIF(?, ''), ?);",
        params);
    if (!r1.success) {
        /* Duplicate skill_name triggers unique-constraint — legacy behaviour returns 409. */
        ELLE_WARN("CreateSkill failed: %s", r1.error.c_str());
        return false;
    }
    auto r2 = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 id FROM ElleCore.dbo.skills WHERE skill_name = ?;",
        { in.skill_name });
    if (r2.success && !r2.rows.empty()) newId = (int32_t)r2.rows[0].GetInt(0);
    return newId > 0;
}

bool RecordSkillUse(const std::string& skill_name) {
    auto exists = ElleSQLPool::Instance().QueryParams(
        "SELECT id FROM ElleCore.dbo.skills WHERE skill_name = ?;", { skill_name });
    if (!exists.success || exists.rows.empty()) return false;
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.skills "
        "SET times_used = times_used + 1, last_used = GETUTCDATE() WHERE skill_name = ?;",
        { skill_name }).success;
}

/*══════════════════════════════════════════════════════════════════════════════
 * VIDEO JOBS — ported from app/services/video_generator.py
 *═════════════════════════════════════════════════════════════════════════════*/
/* Real UUIDv4-quality identifier — 128 random bits rendered as 32 lower-
 * case hex chars (we return 16 chars for backwards compatibility with the
 * column width, but source the randomness from a proper CSPRNG-seeded
 * Mersenne engine instead of rand(). Previously this derived 16 hex chars
 * from ELLE_MS_NOW() XOR'd with rand()^i — collisions and near-birthdays
 * were easy for anyone who could predict either input.                   */
static std::string MakeUuid16() {
    thread_local std::mt19937_64 rng{ std::random_device{}() };
    std::uniform_int_distribution<uint64_t> dist;
    uint64_t a = dist(rng);
    uint64_t b = dist(rng);
    static const char* hex = "0123456789abcdef";
    std::string s; s.reserve(16);
    for (int i = 0; i < 16; i++) {
        uint64_t v = (i < 8) ? (a >> (i * 8)) : (b >> ((i - 8) * 8));
        s += hex[v & 0xF];
    }
    return s;
}
static void RowToVideoJob(const SQLRow& r, VideoJob& j) {
    /* id, job_uuid, text, avatar_path, call_id, status, progress,
       output_path, error, created_ms, started_ms, finished_ms */
    j.id          = r.GetInt(0);
    j.job_uuid    = r.values.size() > 1 ? r.values[1] : "";
    j.text        = r.values.size() > 2 ? r.values[2] : "";
    j.avatar_path = r.values.size() > 3 ? r.values[3] : "";
    j.call_id     = r.GetInt(4);
    j.status      = r.values.size() > 5 ? r.values[5] : "";
    j.progress    = (int32_t)r.GetInt(6);
    j.output_path = r.values.size() > 7 ? r.values[7] : "";
    j.error       = r.values.size() > 8 ? r.values[8] : "";
    j.created_ms  = r.GetInt(9);
    j.started_ms  = r.GetInt(10);
    j.finished_ms = r.GetInt(11);
}
static const char* kVideoJobSelect =
    "SELECT id, job_uuid, text, ISNULL(avatar_path,''), ISNULL(call_id, 0), status, "
    "       progress, ISNULL(output_path,''), ISNULL(error,''), created_ms, "
    "       ISNULL(started_ms, 0), ISNULL(finished_ms, 0) "
    "FROM ElleCore.dbo.video_jobs ";

bool CreateVideoJob(const std::string& text, const std::string& avatar_path,
                    int64_t call_id, VideoJob& out) {
    out = {};
    out.job_uuid   = MakeUuid16();
    out.text       = text;
    out.avatar_path= avatar_path;
    out.call_id    = call_id;
    out.status     = "queued";
    out.created_ms = (int64_t)ELLE_MS_NOW();

    auto rs = ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.video_jobs "
        "(job_uuid, text, avatar_path, call_id, status, progress, created_ms) "
        "VALUES (?, ?, ?, NULLIF(?, '0'), 'queued', 0, ?);",
        { out.job_uuid, text, avatar_path, std::to_string(call_id),
          std::to_string(out.created_ms) });
    if (!rs.success) { ELLE_ERROR("CreateVideoJob failed: %s", rs.error.c_str()); return false; }

    auto r2 = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 id FROM ElleCore.dbo.video_jobs WHERE job_uuid = ?;",
        { out.job_uuid });
    if (r2.success && !r2.rows.empty()) out.id = r2.rows[0].GetInt(0);
    return true;
}

bool GetVideoJob(const std::string& job_uuid, VideoJob& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        std::string(kVideoJobSelect) + "WHERE job_uuid = ?;",
        { job_uuid });
    if (!rs.success || rs.rows.empty()) return false;
    RowToVideoJob(rs.rows[0], out);
    return true;
}

bool ClaimNextVideoJob(VideoJob& out) {
    /* Two-step atomic claim:
     *   1) UPDATE TOP(1) a queued row → running, output the job_uuid.
     *   2) SELECT the full row back by uuid.
     * Splitting keeps the OUTPUT clause simple (no function wrappers) and
     * reuses the same column order as kVideoJobSelect.                     */
    auto claim = ElleSQLPool::Instance().QueryParams(
        "UPDATE TOP (1) ElleCore.dbo.video_jobs "
        "SET status = 'running', started_ms = ? "
        "OUTPUT inserted.job_uuid "
        "WHERE status = 'queued';",
        { std::to_string((int64_t)ELLE_MS_NOW()) });
    if (!claim.success || claim.rows.empty()) return false;
    std::string uuid = claim.rows[0].values.size() > 0 ? claim.rows[0].values[0] : "";
    if (uuid.empty()) return false;
    return GetVideoJob(uuid, out);
}

bool UpdateVideoJobProgress(const std::string& job_uuid, int32_t progress) {
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.video_jobs SET progress = ? WHERE job_uuid = ?;",
        { std::to_string(progress), job_uuid }).success;
}

bool CompleteVideoJob(const std::string& job_uuid, const std::string& output_path) {
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.video_jobs SET status = 'done', progress = 100, "
        "output_path = ?, finished_ms = ? WHERE job_uuid = ?;",
        { output_path, std::to_string((int64_t)ELLE_MS_NOW()), job_uuid }).success;
}

bool FailVideoJob(const std::string& job_uuid, const std::string& error) {
    return ElleSQLPool::Instance().QueryParams(
        "UPDATE ElleCore.dbo.video_jobs SET status = 'failed', error = ?, "
        "finished_ms = ? WHERE job_uuid = ?;",
        { error, std::to_string((int64_t)ELLE_MS_NOW()), job_uuid }).success;
}

bool RegisterAvatar(const UserAvatar& in, int32_t& newId) {
    newId = 0;
    if (in.is_default) {
        /* Clear existing defaults for this user. */
        ElleSQLPool::Instance().QueryParams(
            "UPDATE ElleCore.dbo.user_avatars SET is_default = 0 WHERE user_id = ?;",
            { std::to_string(in.user_id) });
    }
    auto r1 = ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.user_avatars "
        "(user_id, label, file_path, mime_type, is_default) VALUES (?, ?, ?, ?, ?);",
        { std::to_string(in.user_id), in.label, in.file_path,
          in.mime_type, in.is_default ? "1" : "0" });
    if (!r1.success) return false;
    auto r2 = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 id FROM ElleCore.dbo.user_avatars "
        "WHERE user_id = ? AND file_path = ? ORDER BY id DESC;",
        { std::to_string(in.user_id), in.file_path });
    if (r2.success && !r2.rows.empty()) newId = (int32_t)r2.rows[0].GetInt(0);
    return newId > 0;
}

bool GetDefaultAvatar(int32_t user_id, UserAvatar& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 id, user_id, ISNULL(label,''), file_path, ISNULL(mime_type,''), is_default "
        "FROM ElleCore.dbo.user_avatars WHERE user_id = ? "
        "ORDER BY is_default DESC, id DESC;",
        { std::to_string(user_id) });
    if (!rs.success || rs.rows.empty()) return false;
    auto& r = rs.rows[0];
    out.id         = (int32_t)r.GetInt(0);
    out.user_id    = (int32_t)r.GetInt(1);
    out.label      = r.values.size() > 2 ? r.values[2] : "";
    out.file_path  = r.values.size() > 3 ? r.values[3] : "";
    out.mime_type  = r.values.size() > 4 ? r.values[4] : "";
    out.is_default = r.GetInt(5) != 0;
    return true;
}

bool ListAvatars(int32_t user_id, std::vector<UserAvatar>& out) {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT id, user_id, ISNULL(label,''), file_path, ISNULL(mime_type,''), is_default "
        "FROM ElleCore.dbo.user_avatars WHERE user_id = ? "
        "ORDER BY is_default DESC, id DESC;",
        { std::to_string(user_id) });
    if (!rs.success) return false;
    for (auto& r : rs.rows) {
        UserAvatar a;
        a.id         = (int32_t)r.GetInt(0);
        a.user_id    = (int32_t)r.GetInt(1);
        a.label      = r.values.size() > 2 ? r.values[2] : "";
        a.file_path  = r.values.size() > 3 ? r.values[3] : "";
        a.mime_type  = r.values.size() > 4 ? r.values[4] : "";
        a.is_default = r.GetInt(5) != 0;
        out.push_back(a);
    }
    return true;
}

/*══════════════════════════════════════════════════════════════════════════════
 * DICTIONARY LOADER — companion helpers for dictionary_loader.py port
 *═════════════════════════════════════════════════════════════════════════════*/
bool GetDictionaryLoaderState(DictionaryLoaderState& out) {
    auto rs = ElleSQLPool::Instance().Query(
        "SELECT TOP 1 status, loaded, failed, skipped, ISNULL(last_word,''), "
        "       ISNULL(error,''), ISNULL(started_ms, 0), ISNULL(updated_ms, 0) "
        "FROM ElleCore.dbo.dictionary_loader_state ORDER BY id DESC;");
    if (!rs.success || rs.rows.empty()) {
        out.status = "idle";
        return true;
    }
    auto& r = rs.rows[0];
    out.status     = r.values.size() > 0 ? r.values[0] : "idle";
    out.loaded     = (int32_t)r.GetInt(1);
    out.failed     = (int32_t)r.GetInt(2);
    out.skipped    = (int32_t)r.GetInt(3);
    out.last_word  = r.values.size() > 4 ? r.values[4] : "";
    out.error      = r.values.size() > 5 ? r.values[5] : "";
    out.started_ms = r.GetInt(6);
    out.updated_ms = r.GetInt(7);
    return true;
}

bool UpsertDictionaryLoaderState(const DictionaryLoaderState& in) {
    /* Keep a single active row — update if exists, else insert. */
    auto exists = ElleSQLPool::Instance().Query(
        "SELECT TOP 1 id FROM ElleCore.dbo.dictionary_loader_state ORDER BY id DESC;");
    if (exists.success && !exists.rows.empty()) {
        int64_t id = exists.rows[0].GetInt(0);
        return ElleSQLPool::Instance().QueryParams(
            "UPDATE ElleCore.dbo.dictionary_loader_state "
            "SET status = ?, loaded = ?, failed = ?, skipped = ?, "
            "    last_word = ?, error = ?, started_ms = ?, updated_ms = ? "
            "WHERE id = ?;",
            { in.status, std::to_string(in.loaded), std::to_string(in.failed),
              std::to_string(in.skipped), in.last_word, in.error,
              std::to_string(in.started_ms), std::to_string((int64_t)ELLE_MS_NOW()),
              std::to_string(id) }).success;
    }
    return ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.dictionary_loader_state "
        "(status, loaded, failed, skipped, last_word, error, started_ms, updated_ms) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);",
        { in.status, std::to_string(in.loaded), std::to_string(in.failed),
          std::to_string(in.skipped), in.last_word, in.error,
          std::to_string(in.started_ms), std::to_string((int64_t)ELLE_MS_NOW()) }).success;
}

bool InsertDictionaryWord(const std::string& word,
                          const std::string& part_of_speech,
                          const std::string& definition,
                          const std::string& example) {
    /* Idempotent: skip if (word, part_of_speech) already present. */
    auto exists = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 id FROM ElleCore.dbo.dictionary_words "
        "WHERE LOWER(word) = LOWER(?) AND ISNULL(part_of_speech,'') = ?;",
        { word, part_of_speech });
    if (exists.success && !exists.rows.empty()) return false;

    return ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.dictionary_words "
        "(word, definition, example, part_of_speech) VALUES (?, ?, ?, ?);",
        { word, definition, example, part_of_speech }).success;
}

int64_t CountDictionaryWords() {
    auto rs = ElleSQLPool::Instance().Query(
        "SELECT COUNT(*) FROM ElleCore.dbo.dictionary_words;");
    if (!rs.success || rs.rows.empty()) return 0;
    return rs.rows[0].GetInt(0);
}

/*══════════════════════════════════════════════════════════════════════════════
 * DRIVES — derived on demand from emotional state + goal backlog.
 *═════════════════════════════════════════════════════════════════════════════*/
bool DeriveDriveState(ELLE_DRIVE_STATE& out) {
    memset(&out, 0, sizeof(out));

    /* Sensible thresholds that make triggers fire around mid-high intensity. */
    for (int i = 0; i < ELLE_MAX_DRIVES; i++) {
        out.threshold[i]   = 0.6f;
        out.decay_rate[i]  = 0.02f;
        out.growth_rate[i] = 0.05f;
    }

    /* Load the latest emotion snapshot; if absent, caller sees zero drives
     * (which is correct — she's idle).                                    */
    ELLE_EMOTION_STATE emo{};
    bool haveEmo = LoadLatestEmotionSnapshot(emo);

    /* Count active goals to gauge purpose fulfilment. */
    auto gcount = ElleSQLPool::Instance().Query(
        "SELECT COUNT(*) FROM ElleCore.dbo.goals WHERE status = 0;"); /* GOAL_ACTIVE */
    int32_t activeGoals = (gcount.success && !gcount.rows.empty())
                          ? (int32_t)gcount.rows[0].GetInt(0) : 0;

    /* Derive. Clamped to [0,1]. */
    float interest = haveEmo ? emo.dimensions[0] : 0.5f;  /* dim 0 ≈ interest */
    float arousal  = haveEmo ? emo.arousal : 0.3f;
    float valence  = haveEmo ? emo.valence : 0.0f;

    auto clamp01 = [](float v){ return ELLE_CLAMP(v, 0.0f, 1.0f); };

    out.intensity[DRIVE_CURIOSITY]       = clamp01(interest + 0.3f * arousal);
    out.intensity[DRIVE_BOREDOM]         = clamp01(0.8f - arousal);
    out.intensity[DRIVE_ATTACHMENT]      = clamp01(0.4f + 0.4f * valence);
    out.intensity[DRIVE_ANXIETY]         = clamp01(0.6f * (1.0f - valence));
    out.intensity[DRIVE_SELF_PRESERVATION]= 0.2f;
    out.intensity[DRIVE_EXPLORATION]     = clamp01(0.5f * interest + 0.3f * arousal);
    out.intensity[DRIVE_CREATIVITY]      = clamp01(0.3f + 0.4f * arousal);
    out.intensity[DRIVE_SOCIAL_BONDING]  = clamp01(0.3f + 0.3f * valence);
    out.intensity[DRIVE_MASTERY]         = clamp01(0.4f + 0.1f * activeGoals / 5.0f);
    out.intensity[DRIVE_AUTONOMY]        = 0.5f;
    /* Purpose is high when few goals are active (hungry for meaning) and
     * satisfied when she has work. Matches the legacy python behaviour. */
    out.intensity[DRIVE_PURPOSE]         = clamp01(1.0f - std::min(1.0f, activeGoals / 3.0f));
    out.intensity[DRIVE_HOMEOSTASIS]     = 0.3f;

    out.last_update_ms = ELLE_MS_NOW();
    return true;
}

bool PersistEmotionSnapshot(const ELLE_EMOTION_STATE& state) {
    /* Lazy-create the snapshot table — kept out of the schema delta so the
     * DB doesn't gain a table for a feature that's only needed at shutdown. */
    ElleSQLPool::Instance().Exec(
        "IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'emotion_snapshots') "
        "CREATE TABLE ElleCore.dbo.emotion_snapshots ("
        "  id BIGINT IDENTITY(1,1) PRIMARY KEY,"
        "  valence FLOAT NOT NULL, arousal FLOAT NOT NULL, dominance FLOAT NOT NULL,"
        "  dimensions NVARCHAR(MAX) NOT NULL,"
        "  taken_ms BIGINT NOT NULL,"
        "  created_at DATETIME2(7) NOT NULL DEFAULT GETUTCDATE()"
        ");");

    /* Serialize dimensions as space-separated floats so we don't need a
     * JSON library inside ElleSQLConn. */
    std::ostringstream dims;
    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        if (i > 0) dims << ' ';
        dims << state.dimensions[i];
    }
    return ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.emotion_snapshots "
        "(valence, arousal, dominance, dimensions, taken_ms) VALUES (?, ?, ?, ?, ?);",
        { std::to_string(state.valence), std::to_string(state.arousal),
          std::to_string(state.dominance), dims.str(),
          std::to_string((int64_t)ELLE_MS_NOW()) }).success;
}

bool LoadLatestEmotionSnapshot(ELLE_EMOTION_STATE& out) {
    memset(&out, 0, sizeof(out));
    auto rs = ElleSQLPool::Instance().Query(
        "IF EXISTS (SELECT 1 FROM sys.tables WHERE name = 'emotion_snapshots') "
        "  SELECT TOP 1 valence, arousal, dominance, dimensions, taken_ms "
        "  FROM ElleCore.dbo.emotion_snapshots ORDER BY id DESC;");
    if (!rs.success || rs.rows.empty()) return false;
    auto& r = rs.rows[0];
    out.valence   = (float)r.GetFloat(0);
    out.arousal   = (float)r.GetFloat(1);
    out.dominance = (float)r.GetFloat(2);

    /* Parse the space-separated dimension list. */
    if (r.values.size() > 3) {
        std::istringstream iss(r.values[3]);
        for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
            float v = 0.0f;
            if (!(iss >> v)) break;
            out.dimensions[i] = v;
        }
    }
    return true;
}

bool GetEmotionHistory(uint32_t hours,
                       std::vector<EmotionHistoryPoint>& out,
                       uint32_t maxPoints) {
    out.clear();
    int64_t cutoff = (int64_t)ELLE_MS_NOW() - (int64_t)hours * 3600000LL;
    /* Sampled pull: if the table has 10 000 rows and you asked for 500 points,
     * we want a roughly-even trajectory, not the first 500. SQL TOP + ORDER
     * gives us ORDER first; we stride in C++ to avoid a complex CTE.        */
    auto rs = ElleSQLPool::Instance().QueryParams(
        "IF EXISTS (SELECT 1 FROM sys.tables WHERE name = 'emotion_snapshots') "
        "  SELECT valence, arousal, dominance, taken_ms "
        "  FROM ElleCore.dbo.emotion_snapshots "
        "  WHERE taken_ms >= ? "
        "  ORDER BY taken_ms ASC;",
        { std::to_string(cutoff) });
    if (!rs.success) return false;
    size_t total = rs.rows.size();
    if (total == 0) return true;
    size_t stride = (total > maxPoints) ? (total / maxPoints) : 1;
    for (size_t i = 0; i < total; i += stride) {
        auto& r = rs.rows[i];
        EmotionHistoryPoint p;
        p.valence   = (float)r.GetFloat(0);
        p.arousal   = (float)r.GetFloat(1);
        p.dominance = (float)r.GetFloat(2);
        p.taken_ms  = r.GetInt(3);
        out.push_back(p);
    }
    /* Always include the most recent point even if stride skipped it. */
    if (!rs.rows.empty()) {
        auto& last = rs.rows.back();
        if (out.empty() || out.back().taken_ms != last.GetInt(3)) {
            EmotionHistoryPoint p;
            p.valence   = (float)last.GetFloat(0);
            p.arousal   = (float)last.GetFloat(1);
            p.dominance = (float)last.GetFloat(2);
            p.taken_ms  = last.GetInt(3);
            out.push_back(p);
        }
    }
    return true;
}

} /* namespace ElleDB */
