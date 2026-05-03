/*******************************************************************************
 * ElleSQLFallback.h — Offline SQL Queue (Fiesta-style SQLLogs)
 *
 * Contract
 * --------
 * When ElleSQLPool fails a write because ODBC is down (reconnect storms,
 * server restart, transient network outage), the payload is serialised to
 *      <exe_dir>/sqllogs/YYYY-MM-DD.txt
 * as one NDJSON line per query.  Drain runs whenever a subsequent healthy
 * connection returns from Acquire() — each line is replayed via
 * QueryParams().  Successfully drained lines are removed from the active
 * file; failures stay behind so the next drain picks them up.
 *
 * Format (one NDJSON per line):
 *   { "ts": 1739000000000, "kind": "Exec" | "QueryParams" | "CallProc",
 *     "sql":  "UPDATE ...",
 *     "proc": "usp_xxx",              // only for CallProc
 *     "params": [ "p0", "p1", ... ] } // empty [] for Exec
 *
 * Thread model: enqueue is lock-guarded per-day-file; drain runs on a
 * dedicated background thread started on first enqueue.  Zero background
 * threads when the pool has never failed a write.
 *
 * NO STUBS: every method has a real Windows implementation.  On Linux CI
 * (which can't compile ODBC anyway) the replay fast-path is a no-op —
 * Enqueue still writes the file so the format is observable offline.
 ******************************************************************************/
#pragma once
#ifndef ELLE_SQL_FALLBACK_H
#define ELLE_SQL_FALLBACK_H

#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

class ElleSQLFallback {
public:
    enum class Kind { Exec, QueryParams, CallProc };

    static ElleSQLFallback& Instance();

    /* Explicitly initialise — creates the sqllogs/ directory under the
     * caller's exe folder and starts the drain worker lazily. Idempotent.
     * If called with `enabled = false` (e.g. for unit tests) the module
     * becomes a no-op: Enqueue returns false, DrainNow is a no-op.      */
    void Initialize(bool enabled = true);

    /* Async-flavored shutdown — stops the drain worker and flushes any
     * open file handle. Safe to call before process exit.               */
    void Shutdown();

    bool IsEnabled() const { return m_enabled.load(std::memory_order_acquire); }

    /* Serialise one query to today's sqllogs file. Returns true on write
     * success, false on write failure (e.g. disk full) — the caller
     * then has no fallback and should surface the outage to the user. */
    bool Enqueue(Kind kind, const std::string& sqlOrProc,
                 const std::vector<std::string>& params);

    /* Signal the drain worker that SQL may be healthy now. Cheap, thread-
     * safe; hot path is a single atomic store + CV notify.               */
    void NudgeDrain();

    /* Drain loop step — reads each sqllogs/<file>.txt, replays each line
     * via ElleSQLPool, removes drained lines.  Returns number of rows
     * replayed.  Public so it can be triggered from diagnostics / tests. */
    uint32_t DrainNow();

    /* Introspection for /api/diag/sqlqueue or the Android health screen. */
    uint64_t PendingBytes() const;
    uint32_t FileCount() const;

private:
    ElleSQLFallback() = default;
    ~ElleSQLFallback();
    ElleSQLFallback(const ElleSQLFallback&) = delete;
    ElleSQLFallback& operator=(const ElleSQLFallback&) = delete;

    /* Directory that holds the rolling daily files. Cached at Initialize
     * and *never* changed thereafter (avoids TOCTOU races between worker
     * and producers).                                                   */
    std::string m_dir;

    /* The file-mutex is per-day; a single mutex is enough since the
     * hot path only ever writes to today's file.                       */
    std::mutex  m_fileMutex;

    std::atomic<bool> m_enabled{false};
    std::atomic<bool> m_running{false};

    /* Drain worker. Spawned on first Enqueue; runs until Shutdown.     */
    std::thread             m_worker;
    std::mutex              m_workerMutex;
    std::condition_variable m_workerCv;
    std::atomic<bool>       m_pendingWork{false};

    void WorkerLoop();
    bool ReplayLine(const std::string& jsonLine, std::string& outErr);
    std::string TodayYmd() const;
    std::string ExeDirectory() const;
};

#endif /* ELLE_SQL_FALLBACK_H */
