/*******************************************************************************
 * ElleSQLConn.h — SQL Server Connection Pool via Named Pipes
 * 
 * Manages a pool of SQL Server connections using named pipe transport.
 * Thread-safe, connection recycling, prepared statement caching.
 ******************************************************************************/
#pragma once
#ifndef ELLE_SQL_CONN_H
#define ELLE_SQL_CONN_H

#include "ElleTypes.h"
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>
#include <unordered_map>

/* Forward declare ODBC types to avoid header dependency */
typedef void* SQLHENV;
typedef void* SQLHDBC;
typedef void* SQLHSTMT;

/*──────────────────────────────────────────────────────────────────────────────
 * SQL RESULT SET
 *──────────────────────────────────────────────────────────────────────────────*/
struct SQLColumn {
    std::string name;
    int32_t     type;       /* SQL_C_CHAR, SQL_C_LONG, etc. */
    uint32_t    size;
};

struct SQLRow {
    std::vector<std::string> values;
    
    const std::string& operator[](size_t idx) const { return values[idx]; }
    int64_t     GetInt(size_t idx) const;
    double      GetFloat(size_t idx) const;
    bool        IsNull(size_t idx) const;
};

struct SQLResultSet {
    std::vector<SQLColumn>  columns;
    std::vector<SQLRow>     rows;
    int64_t                 rows_affected = 0;
    bool                    success = false;
    std::string             error;

    bool        Empty() const { return rows.empty(); }
    size_t      RowCount() const { return rows.size(); }
    size_t      ColCount() const { return columns.size(); }
    int         ColIndex(const std::string& name) const;
};

/*──────────────────────────────────────────────────────────────────────────────
 * SQL CONNECTION (single connection wrapper)
 *──────────────────────────────────────────────────────────────────────────────*/
class SQLConnection {
public:
    SQLConnection();
    ~SQLConnection();

    bool Connect(const std::string& connectionString);
    void Disconnect();
    bool IsConnected() const;
    bool Ping();

    SQLResultSet Execute(const std::string& sql);
    SQLResultSet ExecuteParams(const std::string& sql, const std::vector<std::string>& params);
    SQLResultSet CallProc(const std::string& proc, const std::vector<std::string>& params);

    int64_t ExecuteScalar(const std::string& sql);
    bool    ExecuteNonQuery(const std::string& sql);

    bool BeginTransaction();
    bool Commit();
    bool Rollback();

    const std::string& LastError() const { return m_lastError; }
    uint64_t LastUsedMs() const { return m_lastUsed; }

private:
    SQLHENV     m_hEnv = nullptr;
    SQLHDBC     m_hDbc = nullptr;
    bool        m_connected = false;
    bool        m_inTransaction = false;
    std::string m_connStr;
    std::string m_lastError;
    uint64_t    m_lastUsed = 0;

    bool AllocHandles();
    void FreeHandles();
    std::string GetDiagnostics(int16_t handleType, void* handle);
};

/*──────────────────────────────────────────────────────────────────────────────
 * SQL CONNECTION POOL
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleSQLPool {
public:
    static ElleSQLPool& Instance();

    bool Initialize(const std::string& connectionString, uint32_t poolSize = 8);
    void Shutdown();

    /* Acquire/Release pattern */
    std::shared_ptr<SQLConnection> Acquire(uint32_t timeoutMs = 5000);
    void Release(std::shared_ptr<SQLConnection> conn);

    /* Convenience: auto-acquire, execute, release */
    SQLResultSet Query(const std::string& sql);
    SQLResultSet QueryParams(const std::string& sql, const std::vector<std::string>& params);
    SQLResultSet CallProc(const std::string& proc, const std::vector<std::string>& params);
    bool         Exec(const std::string& sql);
    int64_t      Scalar(const std::string& sql);

    /* Pool stats */
    uint32_t AvailableConnections() const;
    uint32_t TotalConnections() const { return m_poolSize; }
    uint64_t TotalQueries() const { return m_totalQueries; }

private:
    ElleSQLPool() = default;
    ~ElleSQLPool() = default;

    std::string     m_connStr;
    uint32_t        m_poolSize = 0;
    bool            m_initialized = false;

    std::queue<std::shared_ptr<SQLConnection>>  m_available;
    std::mutex                                   m_mutex;
    std::condition_variable                      m_cv;
    uint64_t                                     m_totalQueries = 0;

    bool CreateConnection(std::shared_ptr<SQLConnection>& conn);
};

/*──────────────────────────────────────────────────────────────────────────────
 * DATABASE HELPER — High-level operations for Elle-specific tables
 *──────────────────────────────────────────────────────────────────────────────*/
namespace ElleDB {

    /* Intent Queue */
    bool SubmitIntent(const ELLE_INTENT_RECORD& intent);
    bool GetPendingIntents(std::vector<ELLE_INTENT_RECORD>& out, uint32_t maxCount = 10);
    bool UpdateIntentStatus(uint64_t intentId, ELLE_INTENT_STATUS status, const std::string& response = "");

    /* Action Queue */
    bool SubmitAction(const ELLE_ACTION_RECORD& action);
    bool GetPendingActions(std::vector<ELLE_ACTION_RECORD>& out, uint32_t maxCount = 10);
    bool UpdateActionStatus(uint64_t actionId, ELLE_ACTION_STATUS status, const std::string& result = "");

    /* Messages */
    bool StoreMessage(uint64_t convoId, uint32_t role, const std::string& content, 
                      const ELLE_EMOTION_STATE& emotions, float sentiment);
    bool GetConversationHistory(uint64_t convoId, std::vector<ELLE_CONVERSATION_MSG>& out, uint32_t limit = 50);

    /* Memories */
    bool StoreMemory(const ELLE_MEMORY_RECORD& mem);
    bool RecallMemories(const std::string& query, std::vector<ELLE_MEMORY_RECORD>& out, 
                        uint32_t maxCount = 10, float minRelevance = 0.3f);
    bool UpdateMemoryAccess(uint64_t memId);
    bool PromoteToLTM(uint64_t memId);
    bool ArchiveMemory(uint64_t memId);

    /* Emotions */
    bool StoreEmotionSnapshot(const ELLE_EMOTION_STATE& state);
    bool GetLatestEmotionState(ELLE_EMOTION_STATE& out);

    /* Trust */
    bool UpdateTrust(int32_t delta, const std::string& reason);
    bool GetTrustState(ELLE_TRUST_STATE& out);

    /* Goals */
    bool StoreGoal(const ELLE_GOAL_RECORD& goal);
    bool UpdateGoalProgress(uint64_t goalId, float progress);
    bool GetActiveGoals(std::vector<ELLE_GOAL_RECORD>& out);

    /* World Model */
    bool StoreEntity(const ELLE_WORLD_ENTITY& entity);
    bool GetEntity(const std::string& name, ELLE_WORLD_ENTITY& out);
    bool UpdateEntityInteraction(uint64_t entityId);

    /* Workers/Services */
    bool RegisterWorker(ELLE_SERVICE_ID svc, const std::string& name);
    bool UpdateWorkerHeartbeat(ELLE_SERVICE_ID svc);
    bool GetWorkerStatuses(std::vector<ELLE_SERVICE_STATUS>& out);

    /* Logging */
    bool WriteLog(ELLE_LOG_LEVEL level, ELLE_SERVICE_ID svc, const std::string& msg);
    bool GetRecentLogs(std::vector<ELLE_LOG_ENTRY>& out, uint32_t count = 100, 
                       ELLE_LOG_LEVEL minLevel = LOG_INFO);

    /* Analytics */
    bool RecordMetric(const std::string& name, double value);
}

#endif /* ELLE_SQL_CONN_H */
