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
    return ElleSQLPool::Instance().Exec(
        "INSERT INTO ElleCore.dbo.IntentQueue "
        "(IntentType, Status, SourceDrive, Urgency, Confidence, Description, Parameters, "
        "RequiredTrust, CreatedMs, TimeoutMs) VALUES (" +
        std::to_string(intent.type) + ", " + std::to_string(INTENT_PENDING) + ", " +
        std::to_string(intent.source_drive) + ", " + std::to_string(intent.urgency) + ", " +
        std::to_string(intent.confidence) + ", '" + intent.description + "', '" +
        intent.parameters + "', " + std::to_string(intent.required_trust) + ", " +
        std::to_string(ELLE_MS_NOW()) + ", " + std::to_string(intent.timeout_ms) + ")"
    );
}

bool GetPendingIntents(std::vector<ELLE_INTENT_RECORD>& out, uint32_t maxCount) {
    auto rs = ElleSQLPool::Instance().CallProc("ElleCore.dbo.sp_GetPendingIntents", 
                                                {std::to_string(maxCount)});
    if (!rs.success) return false;
    for (auto& row : rs.rows) {
        ELLE_INTENT_RECORD rec = {};
        rec.id = (uint64_t)row.GetInt(0);
        rec.type = (uint32_t)row.GetInt(1);
        rec.status = (uint32_t)row.GetInt(2);
        rec.source_drive = (uint32_t)row.GetInt(3);
        rec.urgency = (float)row.GetFloat(4);
        rec.confidence = (float)row.GetFloat(5);
        strncpy_s(rec.description, row.values[6].c_str(), ELLE_MAX_MSG - 1);
        strncpy_s(rec.parameters, row.values[7].c_str(), ELLE_MAX_MSG - 1);
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
    return ElleSQLPool::Instance().Exec(
        "UPDATE ElleSystem.dbo.TrustState SET Score = "
        "CASE WHEN Score + " + std::to_string(delta) + " > 100 THEN 100 "
        "WHEN Score + " + std::to_string(delta) + " < 0 THEN 0 "
        "ELSE Score + " + std::to_string(delta) + " END, "
        "LastChangeMs = " + std::to_string(ELLE_MS_NOW()) + "; "
        "INSERT INTO ElleSystem.dbo.TrustAudit (Delta, Reason, TimestampMs) VALUES (" +
        std::to_string(delta) + ", '" + reason + "', " + std::to_string(ELLE_MS_NOW()) + ")"
    );
}

bool RegisterWorker(ELLE_SERVICE_ID svc, const std::string& name) {
    return ElleSQLPool::Instance().Exec(
        "IF NOT EXISTS (SELECT 1 FROM ElleSystem.dbo.Workers WHERE ServiceId = " +
        std::to_string(svc) + ") INSERT INTO ElleSystem.dbo.Workers "
        "(ServiceId, Name, Running, Healthy, StartedMs) VALUES (" +
        std::to_string(svc) + ", '" + name + "', 1, 1, " + std::to_string(ELLE_MS_NOW()) +
        ") ELSE UPDATE ElleSystem.dbo.Workers SET Running = 1, Healthy = 1, "
        "LastHeartbeatMs = " + std::to_string(ELLE_MS_NOW()) +
        " WHERE ServiceId = " + std::to_string(svc)
    );
}

bool UpdateWorkerHeartbeat(ELLE_SERVICE_ID svc) {
    return ElleSQLPool::Instance().Exec(
        "UPDATE ElleSystem.dbo.Workers SET LastHeartbeatMs = " +
        std::to_string(ELLE_MS_NOW()) + ", Healthy = 1 WHERE ServiceId = " +
        std::to_string(svc)
    );
}

bool WriteLog(ELLE_LOG_LEVEL level, ELLE_SERVICE_ID svc, const std::string& msg) {
    return ElleSQLPool::Instance().Exec(
        "INSERT INTO ElleSystem.dbo.Logs (Level, ServiceId, Message, TimestampMs) VALUES (" +
        std::to_string(level) + ", " + std::to_string(svc) + ", '" + msg + "', " +
        std::to_string(ELLE_MS_NOW()) + ")"
    );
}

bool StoreGoal(const ELLE_GOAL_RECORD& goal) {
    (void)goal;
    /* dbo.Goals table is not part of the live ElleCore schema. Persist to
     * dbo.system_settings as a JSON-line until a real Goals table lands. */
    char buf[2048];
    snprintf(buf, sizeof(buf),
        "{\"description\":\"%s\",\"priority\":%u,\"progress\":%.3f,\"source_drive\":%u}",
        goal.description, (unsigned)goal.priority, goal.progress, (unsigned)goal.source_drive);
    std::string key = "goal_" + std::to_string(ELLE_MS_NOW());
    return ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleCore.dbo.system_settings (setting_key, setting_value, updated_at) "
        "VALUES (?, ?, GETUTCDATE());",
        { key, std::string(buf) }).success;
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

} /* namespace ElleDB */
