/*══════════════════════════════════════════════════════════════════════════════
 * ElleWait.h — Pollable sleep helper for worker threads
 *
 *   Problem:
 *     A worker thread doing `Sleep(intervalMs)` between ticks cannot be
 *     stopped faster than `intervalMs` — a service asked to stop at the
 *     moment the thread enters a long Sleep has to wait the full sleep
 *     duration before it checks the shutdown flag. For intervals of
 *     several seconds (memory consolidation, emotional decay), this
 *     causes Windows Service Control Manager to log "service did not
 *     respond in a timely fashion" warnings and hard-kill the process.
 *
 *   Solution:
 *     `PollingSleep(total, flag, step)` sleeps for up to `total` ms in
 *     `step`-ms slices, checking `flag` between slices. If the flag
 *     becomes false during the sleep, the function returns immediately
 *     — stop latency bounded at one `step` (default 50ms) regardless of
 *     how long the interval is.
 *
 *   Why not a condition_variable?
 *     Each caller would need a paired mutex + cv stored on its owner
 *     object, plus every `Stop()` site would need to lock + notify.
 *     The atomic-flag + short-slice pattern is drop-in for existing
 *     `while (m_running) { ... Sleep(N); }` loops and has comparable
 *     latency at 50 ms step. If sub-50ms shutdown becomes a requirement
 *     for a specific caller, that caller can graduate to a cv locally;
 *     this helper is for the 80% case.
 *
 *   Thread-safety:
 *     Reads `flag` with std::memory_order_acquire implicitly (std::atomic
 *     default). Writer stopping the thread should use `store(false)` —
 *     default memory_order_seq_cst is fine.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once

#include <atomic>
#include <cstdint>
#include <windows.h>

namespace ElleWait {

/** Sleep up to `totalMs` milliseconds, checking `runningFlag` every
 *  `stepMs` ms. Returns early if the flag is cleared during the wait.  */
inline void PollingSleep(uint32_t totalMs,
                         const std::atomic<bool>& runningFlag,
                         uint32_t stepMs = 50) {
    if (stepMs == 0) stepMs = 50;
    uint32_t slept = 0;
    while (slept < totalMs) {
        if (!runningFlag.load()) return;
        uint32_t chunk = (totalMs - slept) < stepMs ? (totalMs - slept) : stepMs;
        Sleep(chunk);
        slept += chunk;
    }
}

/** Variant for a pointer flag (e.g. inside a watcher struct passed by
 *  reference-capture to a lambda). Semantics identical to the atomic
 *  overload.                                                           */
inline void PollingSleep(uint32_t totalMs,
                         const std::atomic<bool>* runningFlagPtr,
                         uint32_t stepMs = 50) {
    if (!runningFlagPtr) { Sleep(totalMs); return; }
    PollingSleep(totalMs, *runningFlagPtr, stepMs);
}

/** Inverted-polarity variant for "stop/cancel" flags (true == stop).
 *  Returns early if the flag becomes true during the wait. Kept as a
 *  separate name because overloading on semantics-of-the-same-type
 *  would be a readability trap.                                        */
inline void PollingSleepUntilSet(uint32_t totalMs,
                                  const std::atomic<bool>& stopFlag,
                                  uint32_t stepMs = 50) {
    if (stepMs == 0) stepMs = 50;
    uint32_t slept = 0;
    while (slept < totalMs) {
        if (stopFlag.load()) return;
        uint32_t chunk = (totalMs - slept) < stepMs ? (totalMs - slept) : stepMs;
        Sleep(chunk);
        slept += chunk;
    }
}

}  /* namespace ElleWait */
