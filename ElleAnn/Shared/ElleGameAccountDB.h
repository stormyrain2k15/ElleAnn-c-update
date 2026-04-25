/*******************************************************************************
 * ElleGameAccountDB.h — Read-mostly bridge into the game's MSSQL `Account` DB.
 *
 *   Purpose
 *   -------
 *   Lets Elle authenticate users against the same `tUser` row the
 *   ShineEngine game checks. This is the unification path the user
 *   asked for: one credential set ⇒ one identity (`nUserNo`) shared
 *   across phone-pairing, web sessions, and Elle's in-game presence.
 *
 *   Schema reference
 *   ----------------
 *   See `Services/Elle.Service.Fiesta/_re_artifacts/SQL_SCHEMA.md` —
 *   recovered from `Account.bak` (Mischief 6.11.2015).
 *
 *   Trust model
 *   -----------
 *   The game stores plaintext passwords in `tUser.sUserPW`. We read
 *   them only inside the auth procedure and never log them. The ODBC
 *   binding MUST stay on 127.0.0.1 (or a tunnelled link) — see
 *   `fiesta.json::game_db.odbc_dsn` for the operator-controlled DSN.
 *
 *   Read-mostly
 *   -----------
 *   Elle does NOT mutate the game's account table. All write paths
 *   (revocation, last-seen tracking, JWT bookkeeping) go to the local
 *   `ElleCore.dbo.UserPairing` and `ElleCore.dbo.UserContinuity`
 *   tables — see ElleAnn_UserDeltaV2.sql.
 ******************************************************************************/
#pragma once
#ifndef ELLE_GAME_ACCOUNT_DB_H
#define ELLE_GAME_ACCOUNT_DB_H

#include "ElleSQLConn.h"

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

/*──────────────────────────────────────────────────────────────────────────────
 * SECONDARY POOL — separate from the primary ElleSQLPool because the
 *   game DB lives on its own DSN (`Account` vs. `ElleCore`). Same
 *   wire protocol (MSSQL/TDS via ODBC), separate credential set.
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleGameAccountPool {
public:
    static ElleGameAccountPool& Instance();

    /* Initialize from a full ODBC connection string. The game's
     * server-side DSNs in `JHR_ServerInfo.txt` use:
     *   "Driver={SQL Server Native Client 11.0};Server=127.0.0.1;
     *    Database=Account;UID=sa;PWD=;"
     * Elle should use a DEDICATED low-privilege account if possible
     * (a SQL login granted only EXEC on usp_User_loginWeb / read on
     * tUser is enough). Fail-fast on missing DSN.                    */
    bool Initialize(const std::string& connectionString,
                    uint32_t poolSize = 4);
    void Shutdown();

    bool IsAvailable() const { return m_initialized; }

    /* Direct query path — use sparingly. Prefer the AuthenticateUser()
     * helper below so we don't sprinkle ad-hoc SQL across services.   */
    SQLResultSet Query(const std::string& sql);
    SQLResultSet QueryParams(const std::string& sql,
                             const std::vector<std::string>& params);

private:
    ElleGameAccountPool() = default;
    ~ElleGameAccountPool() = default;
    ElleGameAccountPool(const ElleGameAccountPool&) = delete;
    ElleGameAccountPool& operator=(const ElleGameAccountPool&) = delete;

    std::string  m_connStr;
    uint32_t     m_poolSize = 0;
    bool         m_initialized = false;

    std::queue<std::shared_ptr<SQLConnection>> m_available;
    std::mutex                                  m_mutex;
    std::condition_variable                     m_cv;
};

/*──────────────────────────────────────────────────────────────────────────────
 * AUTH HELPERS — narrow, deliberately small surface.
 *──────────────────────────────────────────────────────────────────────────────*/
namespace ElleGameAuth {

/* Result of a tUser lookup. Populated only when AuthenticateUser
 * returns true. */
struct UserIdentity {
    int64_t     nUserNo = 0;     /* PK; the stable identity Elle keys off  */
    std::string sUserID;          /* login name (e.g. "Klurr")              */
    std::string sUserName;        /* display name                           */
    bool        bIsBlock  = false;
    bool        bIsDelete = false;
    int32_t     nAuthID   = 0;
    char        QX        = 'A';  /* permission level                       */
};

/* Verify (sUserID, sUserPW) against the game's Account DB.
 *
 * Returns true on success and populates `out` with the row's row's
 * identity columns. Returns false in every other case — including:
 *   - DSN not configured                  (ElleGameAccountPool not init'd)
 *   - row not found                        (no such user)
 *   - password mismatch                    (sUserPW != provided)
 *   - row is soft-deleted (bIsDelete = 1)
 *   - row is blocked      (bIsBlock = 1)
 *
 * Implementation calls `usp_User_loginWeb` if available (matches the
 * production game flow) and falls back to a direct SELECT against
 * tUser when the proc isn't present. NEVER logs the password.       */
bool AuthenticateUser(const std::string& sUserID,
                      const std::string& sUserPW,
                      UserIdentity& out);

/* Read-only identity lookup by primary key. Used after the JWT-gate
 * verifies a token and we want to enrich the request context with
 * the identity behind the `sub`. Returns false if the user no longer
 * exists or has been blocked/deleted since last login.              */
bool GetUserById(int64_t nUserNo, UserIdentity& out);

}  /* namespace ElleGameAuth */

#endif  /* ELLE_GAME_ACCOUNT_DB_H */
