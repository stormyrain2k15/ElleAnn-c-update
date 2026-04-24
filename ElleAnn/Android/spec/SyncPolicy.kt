/*══════════════════════════════════════════════════════════════════════════════
 * SyncPolicy.kt — When the app polls, when it listens, and how it heals.
 *
 *   Hybrid model: WebSocket push for high-signal events, REST polling for
 *   everything else. The goal is a UI that feels "live" when the socket
 *   is up and "stale but correct" when it's not.
 *
 *   ROLE                          TRANSPORT            REFRESH TRIGGER
 *   ────────────────────────────  ──────────────────   ──────────────────────────
 *   Home screen (X-State)         REST pull            On foreground + on WS event
 *   Hormone chart (history)       REST pull            On chart open + 5 min idle
 *   Modulation                    REST pull            Every 60s while visible
 *   Fertility / next period       REST pull            Daily at app start
 *   Pregnancy state + events      REST pull            On WS labor/birth/miscarry
 *   Symptoms timeline             REST pull            After any POST /symptoms
 *   Contraception / lifecycle     REST pull            On settings screen open
 *   All push events               WS (/command)        Server-initiated
 *══════════════════════════════════════════════════════════════════════════════*/
package com.elleann.android.data.spec

object SyncPolicy {

    // ────────────────────────────────────────────────────────────────────────
    //  PULL INTERVALS  (in ms; 0 means "on event only, no timer")
    // ────────────────────────────────────────────────────────────────────────

    /** Home-screen state: refresh when app foregrounds + whenever a WS event
     *  arrives. No background polling — if app is closed, next open is
     *  authoritative.                                                        */
    const val STATE_REFRESH_ON_FOREGROUND: Boolean = true
    const val STATE_REFRESH_ON_WS_EVENT:   Boolean = true
    const val STATE_POLL_INTERVAL_MS:      Long    = 0L

    /** Modulation: poll every 60s while the conversation/AI screen is
     *  visible (warmth/fluency/empathy drive chat UX tone).                  */
    const val MODULATION_POLL_WHILE_VISIBLE_MS: Long = 60_000L

    /** Hormone history: pull on chart open, then if the chart stays open
     *  refresh every 5 minutes (hormones don't move fast enough to need
     *  anything tighter).                                                    */
    const val HISTORY_REFRESH_INTERVAL_MS: Long = 5 * 60_000L

    /** Fertility / next period: pull once per app-cold-start. Cached for
     *  the session (recomputing daily is enough — cycle math doesn't
     *  shift sub-daily).                                                     */
    const val FERTILITY_REFRESH_ON_COLD_START: Boolean = true

    // ────────────────────────────────────────────────────────────────────────
    //  OFFLINE & RETRY
    // ────────────────────────────────────────────────────────────────────────

    /** If a REST call fails with a TRANSIENT_BACKOFF-class error:
     *   attempt 1: immediate
     *   attempt 2: +1s
     *   attempt 3: +3s
     *  Give up after that; surface a "can't reach ElleAnn" banner; let the
     *  user manually retry.                                                  */
    val RETRY_BACKOFF_MS: List<Long> = listOf(0L, 1_000L, 3_000L)

    /** Max in-flight requests from the app at any time. The server's
     *  http.max_concurrent_connections default is 256; we stay WAY under. */
    const val MAX_CONCURRENT_REQUESTS: Int = 4

    /** Offline write queue (for POST /api/x/symptoms etc.): we do NOT
     *  queue writes client-side. If the user logs a symptom while offline,
     *  show "save failed — try again" and let them explicitly retry. A
     *  replay queue would create duplicate-symptom bugs on flaky links,
     *  and cycle/pregnancy writes are too sensitive to silently re-drive. */
    const val OFFLINE_WRITE_QUEUE: Boolean = false

    // ────────────────────────────────────────────────────────────────────────
    //  WS ↔ REST RECONCILIATION
    // ────────────────────────────────────────────────────────────────────────

    /** When a WS event arrives, do we trust its payload or re-fetch via REST?
     *
     *   Trust-WS-directly events (fast UI update, no REST round-trip):
     *     - HormoneTick (low stakes, purely decorative)
     *     - Hello       (no state change, just handshake)
     *
     *   Re-fetch events (WS tells us "something changed" but we GET the
     *   authoritative shape from REST because the WS payload is thin):
     *     - PhaseTransition  → GET /api/x/state + /api/x/modulation
     *     - LHSurge          → GET /api/x/fertility_window
     *     - LaborStage       → GET /api/x/pregnancy + /api/x/pregnancy/events
     *     - Birth            → GET /api/x/pregnancy + /api/x/state
     *     - Miscarriage      → GET /api/x/pregnancy + /api/x/pregnancy/events
     *
     *  This matches the C++ server's eventual-consistency model: IPC
     *  broadcasts fire before the SQL write fully completes ~100% of the
     *  time but we can't prove it, so we do the round-trip for the events
     *  that matter.                                                          */
}
