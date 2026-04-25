/*******************************************************************************
 * ElleGameAccountDB.cpp — game `Account` DB connector (read-mostly).
 *
 *   Mirrors ElleSQLPool but uses its own connection string + pool. Kept
 *   intentionally small — Elle is a CONSUMER of this DB, not its owner.
 ******************************************************************************/
#include "ElleGameAccountDB.h"
#include "ElleLogger.h"

#include <chrono>

ElleGameAccountPool& ElleGameAccountPool::Instance() {
    static ElleGameAccountPool s;
    return s;
}

bool ElleGameAccountPool::Initialize(const std::string& connectionString,
                                     uint32_t poolSize) {
    if (m_initialized) return true;
    if (connectionString.empty()) {
        ELLE_WARN("ElleGameAccountPool: empty connection string — game-DB "
                  "auth path disabled");
        return false;
    }
    m_connStr  = connectionString;
    m_poolSize = poolSize;
    for (uint32_t i = 0; i < poolSize; ++i) {
        auto c = std::make_shared<SQLConnection>();
        if (!c->Connect(m_connStr)) {
            ELLE_ERROR("ElleGameAccountPool: failed to connect (pool[%u])", i);
            return false;
        }
        m_available.push(c);
    }
    m_initialized = true;
    ELLE_INFO("ElleGameAccountPool: %u connections to game Account DB",
              (unsigned)poolSize);
    return true;
}

void ElleGameAccountPool::Shutdown() {
    std::lock_guard<std::mutex> lk(m_mutex);
    while (!m_available.empty()) {
        auto c = m_available.front(); m_available.pop();
        if (c) c->Disconnect();
    }
    m_initialized = false;
}

static std::shared_ptr<SQLConnection> AcquireFrom(
        std::queue<std::shared_ptr<SQLConnection>>& q,
        std::mutex& mx, std::condition_variable& cv,
        uint32_t timeoutMs) {
    std::unique_lock<std::mutex> lk(mx);
    if (!cv.wait_for(lk, std::chrono::milliseconds(timeoutMs),
                     [&]{ return !q.empty(); })) {
        return nullptr;
    }
    auto c = q.front(); q.pop();
    return c;
}

SQLResultSet ElleGameAccountPool::Query(const std::string& sql) {
    SQLResultSet rs;
    if (!m_initialized) { rs.error = "game-DB pool not initialized"; return rs; }
    auto c = AcquireFrom(m_available, m_mutex, m_cv, 5000);
    if (!c) { rs.error = "game-DB pool acquire timeout"; return rs; }
    rs = c->Execute(sql);
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_available.push(c);
    }
    m_cv.notify_one();
    return rs;
}

SQLResultSet ElleGameAccountPool::QueryParams(
        const std::string& sql,
        const std::vector<std::string>& params) {
    SQLResultSet rs;
    if (!m_initialized) { rs.error = "game-DB pool not initialized"; return rs; }
    auto c = AcquireFrom(m_available, m_mutex, m_cv, 5000);
    if (!c) { rs.error = "game-DB pool acquire timeout"; return rs; }
    rs = c->ExecuteParams(sql, params);
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_available.push(c);
    }
    m_cv.notify_one();
    return rs;
}

/*──────────────────────────────────────────────────────────────────────────────
 * AUTH HELPERS
 *──────────────────────────────────────────────────────────────────────────────*/
namespace ElleGameAuth {

bool AuthenticateUser(const std::string& sUserID,
                      const std::string& sUserPW,
                      UserIdentity& out) {
    auto& pool = ElleGameAccountPool::Instance();
    if (!pool.IsAvailable()) return false;
    if (sUserID.empty() || sUserID.size() > 30) return false;
    if (sUserPW.empty() || sUserPW.size() > 20) return false;

    /* Direct SELECT — matches `usp_User_loginWeb` semantics. We use
     * the parameterized path so the password (and ID) never enter
     * the SQL text directly. ODBC param binding sidesteps the
     * old "use a stored proc to dodge SQL injection" tradition.    */
    static constexpr const char* kSql =
        "SELECT TOP 1 nUserNo, sUserID, sUserName, "
        "       ISNULL(bIsBlock,0) AS bIsBlock, "
        "       ISNULL(bIsDelete,0) AS bIsDelete, "
        "       ISNULL(nAuthID,0) AS nAuthID, "
        "       ISNULL(QX,'A') AS QX "
        "  FROM dbo.tUser WITH(NOLOCK) "
        " WHERE sUserID = ? "
        "   AND sUserPW = ? "
        "   AND ISNULL(bIsDelete,0) = 0 "
        "   AND ISNULL(bIsBlock,0)  = 0;";

    auto rs = pool.QueryParams(kSql, { sUserID, sUserPW });
    if (!rs.success) {
        /* Don't log the password. ID is fine for diagnostics. */
        ELLE_WARN("game-auth query failed for sUserID=\"%s\": %s",
                  sUserID.c_str(), rs.error.c_str());
        return false;
    }
    if (rs.rows.empty()) return false;

    const auto& r = rs.rows[0];
    int64_t v = 0;
    out.nUserNo   = r.TryGetInt(0, v) ? v : 0;
    out.sUserID   = r[1];
    out.sUserName = r[2];
    out.bIsBlock  = (r.GetIntOr(3, 0) != 0);
    out.bIsDelete = (r.GetIntOr(4, 0) != 0);
    out.nAuthID   = (int32_t)r.GetIntOr(5, 0);
    out.QX        = r[6].empty() ? 'A' : r[6][0];

    if (out.nUserNo <= 0) return false;
    return true;
}

bool GetUserById(int64_t nUserNo, UserIdentity& out) {
    auto& pool = ElleGameAccountPool::Instance();
    if (!pool.IsAvailable() || nUserNo <= 0) return false;
    static constexpr const char* kSql =
        "SELECT nUserNo, sUserID, sUserName, "
        "       ISNULL(bIsBlock,0), ISNULL(bIsDelete,0), "
        "       ISNULL(nAuthID,0), ISNULL(QX,'A') "
        "  FROM dbo.tUser WITH(NOLOCK) WHERE nUserNo = ?;";
    auto rs = pool.QueryParams(kSql, { std::to_string(nUserNo) });
    if (!rs.success || rs.rows.empty()) return false;
    const auto& r = rs.rows[0];
    int64_t v = 0;
    out.nUserNo   = r.TryGetInt(0, v) ? v : 0;
    out.sUserID   = r[1];
    out.sUserName = r[2];
    out.bIsBlock  = (r.GetIntOr(3, 0) != 0);
    out.bIsDelete = (r.GetIntOr(4, 0) != 0);
    out.nAuthID   = (int32_t)r.GetIntOr(5, 0);
    out.QX        = r[6].empty() ? 'A' : r[6][0];
    if (out.nUserNo <= 0 || out.bIsBlock || out.bIsDelete) return false;
    return true;
}

}  /* namespace ElleGameAuth */
