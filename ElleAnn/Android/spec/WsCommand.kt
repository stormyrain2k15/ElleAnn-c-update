/*══════════════════════════════════════════════════════════════════════════════
 * WsCommand.kt — WebSocket command channel contract (server → app push)
 *
 *   Path:         /command (configurable via http.ws_path)
 *   Frame type:   Text (UTF-8 JSON). No binary frames.
 *   Frame limit:  1 MiB (configurable via http.max_ws_frame_bytes)
 *   Protocol:     RFC 6455, bcrypt-SHA1 handshake (already implemented
 *                 in HTTPServer.cpp).
 *
 *   Authentication
 *   ──────────────
 *   The WebSocket upgrade request MUST carry the same
 *   `Authorization: Bearer <jwt>` header as REST calls. The server
 *   validates at handshake time and closes the connection with
 *   code 1008 ("policy violation") if the token is invalid/expired.
 *
 *   Envelope shape (every frame the server pushes)
 *   ──────────────────────────────────────────────
 *     {
 *       "type":    "phase_transition" | "lh_surge" | "labor_stage" |
 *                  "birth" | "miscarriage" | "hormone_tick" | "hello",
 *       "ts_ms":   <epoch ms>,
 *       "opcode":  <int, IPC_X_* for debugging; see IpcOpcodes.kt>,
 *       "payload": { ... type-specific ... }
 *     }
 *
 *   The `type` field is the canonical switch point. `opcode` is for
 *   debugging/tracing — clients MUST route on `type`, not `opcode`.
 *══════════════════════════════════════════════════════════════════════════════*/
package com.elleann.android.data.spec

/** Possible push-event types. The server sends these without the app
 *  asking; the app's job is to subscribe once at app start and route each
 *  frame to the right UI state holder.                                     */
sealed class WsEvent {

    /** Sent immediately after successful handshake. Useful for proving
     *  the token still works without making a throwaway REST call.        */
    data class Hello(val ts_ms: Long, val server_version: String) : WsEvent()

    /** The cycle transitioned from one phase to another (e.g. folliuclar
     *  → ovulatory). App should refresh /api/x/state and /api/x/modulation. */
    data class PhaseTransition(
        val ts_ms:     Long,
        val from:      String,      // CyclePhase or PregnancyPhase wire value
        val to:        String,
        val cycle_day: Int,
    ) : WsEvent()

    /** LH surge detected — ovulation within ~12-24h. High-signal event the
     *  app should surface as a notification (fertility/TTC users care).     */
    data class LHSurge(
        val ts_ms:    Long,
        val lh_value: Double,
        val cycle_day: Int,
    ) : WsEvent()

    /** Labor stage advanced (early → active → transition → delivery).      */
    data class LaborStage(
        val ts_ms:          Long,
        val stage:          String,   // PregnancyPhase wire value
        val contractions_per_10min: Int = 0,
    ) : WsEvent()

    /** A child was born. `pregnancy_id` maps back to the event row in
     *  /api/x/pregnancy/events.                                             */
    data class Birth(
        val ts_ms:         Long,
        val pregnancy_id:  Long,
        val multiplicity:  Int,
    ) : WsEvent()

    /** Miscarriage event (sensitive — UX must handle with care).           */
    data class Miscarriage(
        val ts_ms:         Long,
        val pregnancy_id:  Long,
        val gestational_day: Int,
    ) : WsEvent()

    /** Low-signal hourly-ish tick of the 9-hormone vector. Throttled
     *  server-side (~once per 60s). App can ignore for UI and use
     *  /api/x/history for charts, or apply in-memory to the live state.    */
    data class HormoneTick(
        val ts_ms:     Long,
        val hormones:  HormoneVector,
        val cycle_phase: String,
    ) : WsEvent()
}

/** Reconnect policy.
 *
 *  - Connect on app foreground; disconnect on background (don't keep a
 *    socket open when the app isn't active — Android will reap us anyway).
 *  - On unexpected close: exponential back-off 1s → 2s → 4s → 8s → 30s cap.
 *  - On 4011 (app-defined "pairing revoked"): clear token, go to pairing.
 *  - On any close: the next poll of /api/x/state is authoritative; never
 *    assume a missed event invalidates cached state silently.              */
object WsReconnectPolicy {
    const val INITIAL_BACKOFF_MS: Long = 1_000
    const val MAX_BACKOFF_MS:     Long = 30_000
    const val BACKOFF_FACTOR:     Double = 2.0
    const val CLOSE_CODE_PAIRING_REVOKED: Int = 4011
}
