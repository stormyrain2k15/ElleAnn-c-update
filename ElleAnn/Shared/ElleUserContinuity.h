/*******************************************************************************
 * ElleUserContinuity.h — relationship-memory + game-session-state sidecar.
 *
 *   The game's `tUser.nUserNo` is the stable identity across pairings,
 *   devices, and browser sessions. We piggy-back on it without writing
 *   to the game's DB — all our state lives in ElleCore.dbo.UserContinuity
 *   and ElleCore.dbo.GameSessionState. See ElleAnn_GameUnification.sql.
 *
 *   THE BIG IDEA
 *   ------------
 *   When the same person logs in from a new phone, Elle wants to pick
 *   up the relationship history *as if she remembered them*. The
 *   nUserNo is what makes that possible — Cognitive doesn't need to
 *   reason about device fingerprints or session tokens. It just asks
 *   "tell me what we know about user 47" and gets first_met_ms,
 *   conversation counts, last bond score, and a free-form note Elle
 *   has been keeping.
 ******************************************************************************/
#pragma once
#ifndef ELLE_USER_CONTINUITY_H
#define ELLE_USER_CONTINUITY_H

#include <cstdint>
#include <string>

namespace ElleDB {

struct UserContinuityRow {
    int64_t     nUserNo            = 0;
    std::string sUserID_cached;
    std::string sUserName_cached;
    uint64_t    first_met_ms       = 0;
    uint64_t    last_seen_ms       = 0;
    int32_t     total_conversations = 0;
    int32_t     total_messages     = 0;
    int32_t     total_pairings     = 0;
    double      last_bond_score    = 0.0;   /* 0.0 if never set */
    std::string last_bond_label;
    uint64_t    last_bond_updated_ms = 0;
    std::string private_note;
    uint64_t    created_ms         = 0;
    uint64_t    updated_ms         = 0;
};

/* On every successful pair / login: bump pairings count, refresh
 * last_seen_ms, lazily INSERT the row if this user has never been
 * seen before (which sets first_met_ms = now). Idempotent and cheap.  */
bool TouchUserContinuityOnPair(int64_t nUserNo,
                               const std::string& sUserID,
                               const std::string& sUserName);

/* Bonding writes here when it observes a sustained shift. */
bool UpdateUserBond(int64_t nUserNo,
                    double bondScore,
                    const std::string& bondLabel);

/* Cognitive can append a brief note (e.g. learned preference). The
 * column is capped at 4000 chars; we truncate from the FRONT (oldest)
 * if appending would exceed the cap, so the most recent observations
 * always survive.                                                      */
bool AppendUserNote(int64_t nUserNo, const std::string& note);

/* Read for the JWT-gate / Cognitive context-build. Returns false if
 * the user has never been seen (which is fine — caller treats it as
 * "first conversation").                                               */
bool GetUserContinuity(int64_t nUserNo, UserContinuityRow& out);


/*──────────────────────────────────────────────────────────────────────────────
 *  Game session state — populated by SVC_FIESTA.
 *──────────────────────────────────────────────────────────────────────────────*/
struct GameSessionStateRow {
    int64_t     nUserNo                 = 0;
    int32_t     char_index              = -1;
    std::string char_name;
    int32_t     zone_id                 = -1;
    std::string zone_name;
    double      last_x                  = 0.0;
    double      last_y                  = 0.0;
    double      last_z                  = 0.0;
    int32_t     last_hp                 = 0;
    int32_t     last_hp_max             = 0;
    uint64_t    last_session_ms         = 0;
    uint64_t    last_disconnect_ms      = 0;
    std::string last_disconnect_reason;
};

/* SVC_FIESTA calls this on character select so a service restart can
 * resume gracefully. Idempotent upsert keyed on nUserNo.               */
bool UpsertGameSession(const GameSessionStateRow& row);

/* SVC_FIESTA calls this on disconnect with a short reason string.    */
bool MarkGameSessionDisconnected(int64_t nUserNo,
                                 const std::string& reason);

/* Cognitive can ask "where were we?" at startup. Returns false if no
 * session has ever been recorded for this user.                        */
bool GetGameSession(int64_t nUserNo, GameSessionStateRow& out);

}  /* namespace ElleDB */

#endif  /* ELLE_USER_CONTINUITY_H */
