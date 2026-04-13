/*******************************************************************************
 * ElleSQLConn.cpp — SQL Server Connection Pool Implementation
 ******************************************************************************/
#include "ElleSQLConn.h"
#include "ElleLogger.h"

#include <sql.h>
#include <sqlext.h>
#include <sstream>
#include <algorithm>

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

bool StoreMessage(uint64_t convoId, uint32_t role, const std::string& content,
                  const ELLE_EMOTION_STATE& emotions, float sentiment) {
    /* Serialize emotion snapshot as comma-delimited string */
    std::ostringstream emoStr;
    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        if (i > 0) emoStr << ",";
        emoStr << emotions.dimensions[i];
    }
    return ElleSQLPool::Instance().Exec(
        "INSERT INTO ElleCore.dbo.Messages "
        "(ConversationId, Role, Content, EmotionSnapshot, Sentiment, TimestampMs) VALUES (" +
        std::to_string(convoId) + ", " + std::to_string(role) + ", '" + content + "', '" +
        emoStr.str() + "', " + std::to_string(sentiment) + ", " + 
        std::to_string(ELLE_MS_NOW()) + ")"
    );
}

bool StoreMemory(const ELLE_MEMORY_RECORD& mem) {
    std::ostringstream tags;
    for (uint32_t i = 0; i < mem.tag_count; i++) {
        if (i > 0) tags << ",";
        tags << mem.tags[i];
    }
    return ElleSQLPool::Instance().Exec(
        "INSERT INTO ElleMemory.dbo.Memories "
        "(MemoryType, Tier, Content, Summary, EmotionalValence, Importance, "
        "Relevance, PositionX, PositionY, PositionZ, Tags, CreatedMs) VALUES (" +
        std::to_string(mem.type) + ", " + std::to_string(mem.tier) + ", '" +
        mem.content + "', '" + mem.summary + "', " + std::to_string(mem.emotional_valence) +
        ", " + std::to_string(mem.importance) + ", " + std::to_string(mem.relevance) +
        ", " + std::to_string(mem.position_x) + ", " + std::to_string(mem.position_y) +
        ", " + std::to_string(mem.position_z) + ", '" + tags.str() + "', " +
        std::to_string(ELLE_MS_NOW()) + ")"
    );
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
    return ElleSQLPool::Instance().Exec(
        "INSERT INTO ElleCore.dbo.Goals "
        "(Description, Status, Priority, Progress, Motivation, ParentGoalId, "
        "SourceDrive, CreatedMs, DeadlineMs, SuccessCriteria) VALUES ('" +
        std::string(goal.description) + "', " + std::to_string(goal.status) + ", " +
        std::to_string(goal.priority) + ", " + std::to_string(goal.progress) + ", " +
        std::to_string(goal.motivation) + ", " + std::to_string(goal.parent_goal_id) + ", " +
        std::to_string(goal.source_drive) + ", " + std::to_string(ELLE_MS_NOW()) + ", " +
        std::to_string(goal.deadline_ms) + ", '" + std::string(goal.success_criteria) + "')"
    );
}

bool StoreEntity(const ELLE_WORLD_ENTITY& entity) {
    return ElleSQLPool::Instance().Exec(
        "INSERT INTO ElleKnowledge.dbo.WorldEntities "
        "(Name, EntityType, Description, Familiarity, Sentiment, Trust, "
        "PositionX, PositionY, PositionZ, MentalModel) VALUES ('" +
        std::string(entity.name) + "', '" + std::string(entity.type) + "', '" +
        std::string(entity.description) + "', " + std::to_string(entity.familiarity) + ", " +
        std::to_string(entity.sentiment) + ", " + std::to_string(entity.trust) + ", " +
        std::to_string(entity.position_x) + ", " + std::to_string(entity.position_y) + ", " +
        std::to_string(entity.position_z) + ", '" + std::string(entity.mental_model) + "')"
    );
}

bool RecordMetric(const std::string& name, double value) {
    return ElleSQLPool::Instance().Exec(
        "INSERT INTO ElleSystem.dbo.Analytics (MetricName, MetricValue, TimestampMs) VALUES ('" +
        name + "', " + std::to_string(value) + ", " + std::to_string(ELLE_MS_NOW()) + ")"
    );
}

} /* namespace ElleDB */
