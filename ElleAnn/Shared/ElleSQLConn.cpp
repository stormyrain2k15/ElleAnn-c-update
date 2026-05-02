/*******************************************************************************
 * ElleSQLConn.cpp — SQL Server Connection Pool Implementation
 ******************************************************************************/
#include "ElleSQLConn.h"
#include "ElleLogger.h"

#include <sql.h>
#include <sqlext.h>
#include <sstream>
#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <set>
#include <random>

/*──────────────────────────────────────────────────────────────────────────────
 * SQLRow helpers
 *──────────────────────────────────────────────────────────────────────────────*/
int64_t SQLRow::GetIntOr(size_t idx, int64_t fallback) const {
    if (idx >= values.size() || values[idx].empty()) return fallback;
    /* Strict parse: strtoll + full-consume. Previous std::stoll/catch
     * silently returned 0 on "1.5", "abc", "  42  " — the first two
     * obscured bad schema assumptions, the third mis-parsed trimmable
     * whitespace as a failure. Now: trim ASCII whitespace, require the
     * remainder to be a pure integer. Anything else → WARN + fallback. */
    const std::string& raw = values[idx];
    size_t b = 0, e = raw.size();
    while (b < e && (raw[b] == ' ' || raw[b] == '\t')) ++b;
    while (e > b && (raw[e-1] == ' ' || raw[e-1] == '\t')) --e;
    if (b == e) return fallback;
    std::string s = raw.substr(b, e - b);
    errno = 0;
    char* endp = nullptr;
    long long v = std::strtoll(s.c_str(), &endp, 10);
    if (errno == ERANGE || !endp || endp == s.c_str() || *endp != '\0') {
        ELLE_WARN("SQLRow::GetIntOr(col=%zu) coerced non-integer '%s' -> %lld",
                  idx, raw.c_str(), (long long)fallback);
        return fallback;
    }
    return (int64_t)v;
}

double SQLRow::GetFloatOr(size_t idx, double fallback) const {
    if (idx >= values.size() || values[idx].empty()) return fallback;
    const std::string& raw = values[idx];
    size_t b = 0, e = raw.size();
    while (b < e && (raw[b] == ' ' || raw[b] == '\t')) ++b;
    while (e > b && (raw[e-1] == ' ' || raw[e-1] == '\t')) --e;
    if (b == e) return fallback;
    std::string s = raw.substr(b, e - b);
    errno = 0;
    char* endp = nullptr;
    double v = std::strtod(s.c_str(), &endp);
    if (errno == ERANGE || !endp || endp == s.c_str() || *endp != '\0') {
        ELLE_WARN("SQLRow::GetFloatOr(col=%zu) coerced non-numeric '%s' -> %g",
                  idx, raw.c_str(), fallback);
        return fallback;
    }
    return v;
}

bool SQLRow::TryGetInt(size_t idx, int64_t& outVal) const {
    if (idx >= values.size() || values[idx].empty() ||
        values[idx] == "NULL") return false;
    const std::string& raw = values[idx];
    size_t b = 0, e = raw.size();
    while (b < e && (raw[b] == ' ' || raw[b] == '\t')) ++b;
    while (e > b && (raw[e-1] == ' ' || raw[e-1] == '\t')) --e;
    if (b == e) return false;
    std::string s = raw.substr(b, e - b);
    errno = 0;
    char* endp = nullptr;
    long long v = std::strtoll(s.c_str(), &endp, 10);
    if (errno == ERANGE || !endp || endp == s.c_str() || *endp != '\0')
        return false;
    outVal = (int64_t)v;
    return true;
}

bool SQLRow::TryGetFloat(size_t idx, double& outVal) const {
    if (idx >= values.size() || values[idx].empty() ||
        values[idx] == "NULL") return false;
    const std::string& raw = values[idx];
    size_t b = 0, e = raw.size();
    while (b < e && (raw[b] == ' ' || raw[b] == '\t')) ++b;
    while (e > b && (raw[e-1] == ' ' || raw[e-1] == '\t')) --e;
    if (b == e) return false;
    std::string s = raw.substr(b, e - b);
    errno = 0;
    char* endp = nullptr;
    double v = std::strtod(s.c_str(), &endp);
    if (errno == ERANGE || !endp || endp == s.c_str() || *endp != '\0')
        return false;
    outVal = v;
    return true;
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

    /* Try to enable MARS (Multiple Active Result Sets) if the driver exposes
     * the SQL Server-specific attributes. Not all headers define these.
     * If missing, we rely on aggressive cursor draining/closing. */
#ifdef SQL_COPT_SS_MARS_ENABLED
    SQLSetConnectAttrA(m_hDbc, SQL_COPT_SS_MARS_ENABLED,
                       (SQLPOINTER)SQL_MARS_ENABLED_YES, 0);
#endif

    return true;
}

void SQLConnection::FreeHandles() {
    if (m_hDbc) { SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc); m_hDbc = nullptr; }
    if (m_hEnv) { SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv); m_hEnv = nullptr; }
}

bool SQLConnection::Connect(const std::string& connectionString) {
    if (m_connected) Disconnect();
    m_connStr = connectionString;
    /* Workaround for ODBC Driver 18: enable MARS via connection string.
     * This does not rely on sqlncli-specific header constants.
     * If already specified, keep caller value. */
    if (m_connStr.find("MARS_Connection=") == std::string::npos) {
        if (!m_connStr.empty() && m_connStr.back() != ';') m_connStr.push_back(';');
        m_connStr += "MARS_Connection=Yes;";
    }

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
        ELLE_WARN("SQL connect failed: %s", m_lastError.c_str());
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

/*──────────────────────────────────────────────────────────────────────────────
 * CollectStatementResults — shared fetch path for Execute / ExecuteParams.
 *
 * Fixes audit items:
 *   • SQL_SUCCESS_WITH_INFO is treated as success (was previously dropped
 *     by SQL_SUCCEEDED-vs-SQL_SUCCESS checks in callers).
 *   • SQLGetData truncation is handled by re-reading the remainder into a
 *     growing std::string. Previously long NVARCHAR(MAX) columns were
 *     silently chopped at 8 KiB.
 *   • SQLFetch is checked for SUCCESS *and* SUCCESS_WITH_INFO.
 *
 * Caller already executed the statement (SQLExecDirect/SQLExecute) and
 * passes the return value as `execRet`.
 *────────────────────────────────────────────────────────────────────────────*/
SQLResultSet SQLConnection::CollectStatementResults(SQLHSTMT hStmt, SQLRETURN execRet) {
    SQLResultSet result;
    /* Always drain/close the statement before returning.
     * ODBC Driver 18 is strict: leaving a cursor open (even on statements
     * that return no resultset like INSERT) can cause 24000 Invalid cursor
     * state on the next use of the connection.
     */
    auto drainAndClose = [&]() {
        for (;;) {
            SQLRETURN mr = SQLMoreResults(hStmt);
            if (mr == SQL_NO_DATA) break;
            if (mr != SQL_SUCCESS && mr != SQL_SUCCESS_WITH_INFO) break;
        }
        /* SQLCloseCursor may itself fail when no cursor exists; ignore. */
        SQLCloseCursor(hStmt);
        SQLFreeStmt(hStmt, SQL_CLOSE);
    };

    /* If the driver says we're in an invalid cursor state already, force
     * statement cleanup and return a consistent failure. */
    if (execRet == SQL_ERROR) {
        SQLINTEGER native = 0;
        SQLCHAR state[6] = {0};
        SQLCHAR msg[256] = {0};
        SQLSMALLINT msgLen = 0;
        if (SQLGetDiagRecA(SQL_HANDLE_STMT, hStmt, 1, state, &native, msg, sizeof(msg), &msgLen) == SQL_SUCCESS) {
            if (std::string((char*)state) == "24000") {
                result.error = "24000: invalid cursor state";
                drainAndClose();
                return result;
            }
        }
    }

    if (execRet != SQL_SUCCESS && execRet != SQL_SUCCESS_WITH_INFO &&
        execRet != SQL_NO_DATA) {
        result.error = GetDiagnostics(SQL_HANDLE_STMT, hStmt);
        drainAndClose();
        return result;
    }

    SQLSMALLINT numCols = 0;
    SQLNumResultCols(hStmt, &numCols);
    for (SQLSMALLINT i = 1; i <= numCols; i++) {
        SQLColumn col;
        SQLCHAR colName[256];
        SQLSMALLINT nameLen = 0, dataType = 0, nullable = 0;
        SQLULEN colSize = 0;
        SQLSMALLINT decDigits = 0;
        SQLDescribeColA(hStmt, i, colName, sizeof(colName), &nameLen,
                        &dataType, &colSize, &decDigits, &nullable);
        col.name = std::string((char*)colName, nameLen);
        col.type = dataType;
        col.size = (uint32_t)colSize;
        result.columns.push_back(col);
    }

    for (;;) {
        SQLRETURN fr = SQLFetch(hStmt);
        if (fr == SQL_NO_DATA) break;
        if (fr != SQL_SUCCESS && fr != SQL_SUCCESS_WITH_INFO) {
            result.error = GetDiagnostics(SQL_HANDLE_STMT, hStmt);
            drainAndClose();
            return result;
        }
        SQLRow row;
        for (SQLSMALLINT i = 1; i <= numCols; i++) {
            std::string cell;
            SQLCHAR buf[8192];
            SQLLEN indicator = 0;
            SQLRETURN gr = SQLGetData(hStmt, i, SQL_C_CHAR, buf, sizeof(buf), &indicator);
            if (indicator == SQL_NULL_DATA) {
                row.values.push_back("NULL");
                continue;
            }
            if (gr == SQL_SUCCESS_WITH_INFO) {
                /* Column didn't fit in `buf`. The driver reports the
                 * remaining byte count via `indicator` OR indicates
                 * unknown (-4). Loop until SQL_SUCCESS.                */
                /* First chunk is already in buf; it's NUL-terminated
                 * in the SQL_C_CHAR case so subtract the terminator. */
                size_t firstLen = (indicator > 0 &&
                                   (SQLLEN)(sizeof(buf) - 1) < indicator)
                                    ? sizeof(buf) - 1
                                    : strnlen((char*)buf, sizeof(buf));
                cell.append((char*)buf, firstLen);
                while (gr == SQL_SUCCESS_WITH_INFO) {
                    gr = SQLGetData(hStmt, i, SQL_C_CHAR, buf, sizeof(buf), &indicator);
                    if (gr == SQL_NO_DATA) break;
                    if (gr != SQL_SUCCESS && gr != SQL_SUCCESS_WITH_INFO) {
                        ELLE_WARN("SQLGetData chunk failed: %s",
                                  GetDiagnostics(SQL_HANDLE_STMT, hStmt).c_str());
                        break;
                    }
                    size_t chunkLen = strnlen((char*)buf, sizeof(buf));
                    cell.append((char*)buf, chunkLen);
                }
            } else if (gr == SQL_SUCCESS) {
                cell = std::string((char*)buf);
            } else if (gr == SQL_NO_DATA) {
                /* empty cell */
            } else {
                cell = "";
            }
            row.values.push_back(std::move(cell));
        }
        result.rows.push_back(std::move(row));
    }

    SQLLEN rowCount = 0;
    SQLRowCount(hStmt, &rowCount);
    result.rows_affected = rowCount;
    result.success = true;
    m_lastUsed = ELLE_MS_NOW();
    drainAndClose();
    return result;
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
    result = CollectStatementResults(hStmt, ret);
    /* Defensive: CollectStatementResults closes cursor, but Driver 18 can still
     * leave statement state uncleared for non-row statements on some paths. */
    SQLCloseCursor(hStmt);
    SQLFreeStmt(hStmt, SQL_CLOSE);
    SQLFreeStmt(hStmt, SQL_RESET_PARAMS);
    SQLFreeStmt(hStmt, SQL_UNBIND);
    SQLFreeStmt(hStmt, SQL_CLOSE);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return result;
}

SQLResultSet SQLConnection::ExecuteParams(const std::string& sql, 
                                           const std::vector<std::string>& params) {
    /*──────────────────────────────────────────────────────────────────────
     * Real ODBC parameter binding via SQLBindParameter. The earlier
     * implementation interpolated escaped strings into the SQL text,
     * which is a string-injection prevention pattern, not parameterized
     * execution — it left dialect-specific escape edge cases (N-prefixed
     * literals, Unicode quotes, multi-byte truncation) exposed, and it
     * forced every numeric param through string comparison on the server.
     *
     * We bind every param as SQL_C_CHAR → SQL_VARCHAR at input length =
     * the string's byte size. SQL Server coerces to the column type
     * using its own rules, which is strictly safer than our hand-rolled
     * quoting.                                                         */
    if (!m_connected) {
        SQLResultSet rs; rs.success = false; rs.error = "not connected";
        return rs;
    }

    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hStmt);
    if (!SQL_SUCCEEDED(ret)) {
        SQLResultSet rs; rs.success = false;
        rs.error = "SQLAllocHandle(STMT) failed: " + GetDiagnostics(SQL_HANDLE_DBC, m_hDbc);
        return rs;
    }

    /* Prepare statement — gives the driver a chance to plan it ahead of
     * binding and detect syntax errors cleanly.                         */
    ret = SQLPrepareA(hStmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        SQLResultSet rs; rs.success = false;
        rs.error = "SQLPrepare failed: " + GetDiagnostics(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return rs;
    }

    /* Keep param string lifetimes valid for the duration of the call —
     * SQLBindParameter stores pointers, not copies. We don't mutate
     * `params` during the call so its .c_str() is stable.              */
    std::vector<SQLLEN> cbLens(params.size(), SQL_NTS);
    for (size_t i = 0; i < params.size(); i++) {
        /* Empty-but-present is bound as a zero-length VARCHAR — distinct
         * from NULL. Callers that want NULL should pass the literal
         * "NULL" text to the old code path or adopt a typed binder
         * when we have one. Non-empty values bind as their byte count.*/
        cbLens[i] = (SQLLEN)params[i].size();
        SQLRETURN br = SQLBindParameter(
            hStmt,
            (SQLUSMALLINT)(i + 1),
            SQL_PARAM_INPUT,
            SQL_C_CHAR,
            SQL_VARCHAR,
            (SQLULEN)params[i].size() + 1, /* column size hint */
            0,                              /* decimal digits */
            (SQLPOINTER)params[i].c_str(),  /* string stays live */
            (SQLLEN)params[i].size() + 1,   /* buffer length */
            &cbLens[i]);
        if (!SQL_SUCCEEDED(br)) {
            SQLResultSet rs; rs.success = false;
            rs.error = "SQLBindParameter(" + std::to_string(i + 1) + ") failed: "
                       + GetDiagnostics(SQL_HANDLE_STMT, hStmt);
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            return rs;
        }
    }

    ret = SQLExecute(hStmt);
    SQLResultSet rs = CollectStatementResults(hStmt, ret);
    /* Defensive: ensure cursor is closed regardless of driver quirks. */
    SQLCloseCursor(hStmt);
    SQLFreeStmt(hStmt, SQL_CLOSE);
    /* Hard reset statement state before freeing.
     * Some drivers are picky about statement cleanup even when we're
     * freeing the handle.
     */
    SQLFreeStmt(hStmt, SQL_RESET_PARAMS);
    SQLFreeStmt(hStmt, SQL_UNBIND);
    SQLFreeStmt(hStmt, SQL_CLOSE);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return rs;
}

SQLResultSet SQLConnection::CallProc(const std::string& proc, 
                                      const std::vector<std::string>& params) {
    /* Build a proper ODBC CALL escape with ? placeholders, then reuse
     * ExecuteParams for real binding. The previous body typed-guessed
     * numeric vs string and string-inlined everything, which was the
     * same class of bug ExecuteParams just got rid of.                 */
    std::ostringstream ss;
    ss << "{CALL " << proc;
    if (!params.empty()) {
        ss << "(";
        for (size_t i = 0; i < params.size(); i++) {
            if (i) ss << ",";
            ss << "?";
        }
        ss << ")";
    }
    ss << "}";
    return ExecuteParams(ss.str(), params);
}

int64_t SQLConnection::ExecuteScalar(const std::string& sql) {
    SQLResultSet rs = Execute(sql);
    if (rs.success && !rs.rows.empty() && !rs.rows[0].values.empty()) {
        return rs.rows[0].GetIntOr(0, 0);
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

bool ElleSQLPool::Reinitialize(const std::string& connectionString, uint32_t poolSize) {
    /* Two-step: tear down the old pool, then bring up the new one with
     * the (potentially) new credentials/host.  In-flight requests that
     * already hold a checked-out connection finish on the old handle —
     * we only close idle handles in m_available.  The next Acquire()
     * after this returns will hit the freshly-built pool. */
    Shutdown();
    return Initialize(connectionString, poolSize);
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

/*──────────────────────────────────────────────────────────────────────────────
 * NOTE: the previously-monolithic `namespace ElleDB { … }` that lived at the
 * bottom of this file has been split by domain into three sibling sources:
 *   ElleDB_Queues.cpp   — IntentQueue / ActionQueue / QueueSnapshot
 *   ElleDB_Domain.cpp   — Conversations / Memory / World / Trust / Workers /
 *                         Logs / Goals
 *   ElleDB_Content.cpp  — emotion snapshots, memory helpers, voice, metrics,
 *                         learning subjects / skills / video / dictionary /
 *                         drive state / emotion history
 *
 * All three live in the same `namespace ElleDB` and are built into
 * ElleCore.Shared.lib, so every caller's `ElleDB::Foo(...)` call site is
 * unchanged. This file is now purely the SQL connection / pool primitives.
 *──────────────────────────────────────────────────────────────────────────────*/
