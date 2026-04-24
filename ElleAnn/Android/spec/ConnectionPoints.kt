/*══════════════════════════════════════════════════════════════════════════════
 * ConnectionPoints.kt — The full map of what connects where.
 *
 *   This is the master index for the Android companion's integration
 *   with the ElleAnn Windows service stack. Every external contact the
 *   app makes routes through ONE of the connection points documented
 *   here. No direct IPC, no direct SQL, no file-share mounts.
 *
 *   The app is a thin client over Elle.Service.HTTP. That service is
 *   itself a façade over the 19 Windows services + SQL; the app does
 *   not need to know about that topology.
 *══════════════════════════════════════════════════════════════════════════════*/
package com.elleann.android.data.spec

/** Where and how the app reaches the ElleAnn server.                         */
object ConnectionPoints {

    // ────────────────────────────────────────────────────────────────────────
    //  A.  TRANSPORT
    // ────────────────────────────────────────────────────────────────────────

    /** Default TCP port the HTTP service binds. Configurable server-side via
     *  `elle_master_config.json` → `http.port`. If the user changes it, the
     *  app MUST be told at pairing time (QR/manual entry). We do not do
     *  network discovery.                                                    */
    const val DEFAULT_PORT: Int = 8000

    /** Default bind. The service ships bound to loopback only (`127.0.0.1`).
     *  Remote-device access requires the user to flip this to `0.0.0.0` (or
     *  a specific LAN IP) in the master config AND open the firewall rule.
     *  The app UI should explain this before sending the first request to a
     *  non-loopback host.                                                    */
    const val DEFAULT_BIND_ADDRESS: String = "127.0.0.1"

    /** Preferred protocol.  HTTP/1.1 only. The service speaks raw Winsock
     *  + RFC 6455 — no TLS termination, no HTTP/2. For LAN use this is
     *  acceptable; for any over-the-internet use the user is expected to
     *  front it with a reverse proxy (nginx / Cloudflare Tunnel) that
     *  handles TLS. The app should therefore accept `http://` and
     *  `https://` prefixes identically — just forward the scheme the user
     *  entered at pairing.                                                   */
    enum class Scheme { HTTP, HTTPS }

    /** WebSocket upgrade path (RFC 6455 text frames, ≤ 1 MiB).  Used for
     *  server-pushed events (phase transitions, LH surge, birth, labor
     *  stage, miscarriage). Configurable server-side via
     *  `elle_master_config.json` → `http.ws_path`, default `/command`.       */
    const val WS_PATH_DEFAULT: String = "/command"

    // ────────────────────────────────────────────────────────────────────────
    //  B.  AUTHENTICATION (see Auth.kt for the contract)
    // ────────────────────────────────────────────────────────────────────────

    /** Every request to an `AUTH_USER` endpoint carries
     *  `Authorization: Bearer <jwt>`. See Auth.kt for the pairing
     *  handshake. The JWT is opaque to the app — do NOT attempt to decode
     *  it for its claims; the server treats it as a shared secret.          */
    const val AUTH_HEADER: String = "Authorization"
    const val AUTH_SCHEME: String = "Bearer"

    /** JWTs carry a server-side expiry (default 24h, configurable via
     *  `http.jwt_expiry_hours`). The server returns 401 on expired tokens;
     *  the app should then re-run the pairing handshake (Auth.kt). We
     *  deliberately do NOT do refresh tokens — the pairing handshake is
     *  cheap enough that sliding-window refresh adds complexity without
     *  value.                                                                */

    // ────────────────────────────────────────────────────────────────────────
    //  C.  FUNCTIONAL SURFACES THE APP CONSUMES
    // ────────────────────────────────────────────────────────────────────────

    /** All 17 X-Chromosome routes live under this prefix. Every route is
     *  documented in XApi.kt with exact request/response shapes.             */
    const val X_API_PREFIX: String = "/api/x"

    /** Optional cross-cutting endpoints the app may use opportunistically.   */
    object CrossCutting {
        /** Liveness check. Returns 200 with `{"ok":true,"uptime_ms":N}`.    */
        const val HEALTH: String = "/api/health"

        /** Non-blocking version of health — guaranteed 200 unless the
         *  process itself is down. No body contract (may be empty string).  */
        const val HEALTHZ: String = "/healthz"

        /** Session greeting — returns an LLM-rendered welcome phrase the
         *  app can surface on cold open. Safe to show; idempotent.          */
        const val SESSION_GREETING: String = "/api/session/greeting"
    }

    // ────────────────────────────────────────────────────────────────────────
    //  D.  DATA SHAPE RULES
    // ────────────────────────────────────────────────────────────────────────

    /** Timestamps: `uint64` milliseconds since Unix epoch. ALL of them.
     *  Never ISO strings, never seconds. Field names end in `_ms` so they
     *  are self-documenting.                                                */

    /** Hormones: IEEE-754 `float` on the wire, 0.0..∞ (not bounded; e.g.
     *  hCG can exceed 100 000 mIU/mL in pregnancy). Nine-hormone vector:
     *  estradiol, progesterone, testosterone, lh, fsh, prolactin, oxytocin,
     *  cortisol, hcg. Same order as the SQL row, same order in every JSON. */

    /** Phases: strings from a fixed enum (see XModels.kt). Never parse
     *  these as free-form text — if a phase arrives that isn't in the
     *  enum, surface as "unknown" rather than crashing.                    */

    // ────────────────────────────────────────────────────────────────────────
    //  E.  THINGS THE APP EXPLICITLY DOES NOT DO
    // ────────────────────────────────────────────────────────────────────────

    /** NEVER talks directly to:
     *    • SQL Server (ElleHeart / ElleCore database)
     *    • Any named pipe / IOCP channel
     *    • Any service .exe other than Elle.Service.HTTP
     *    • The Lua host
     *    • The ASM DLLs
     *  All of that is server-side internal topology.                       */

    /** NEVER performs background work when backgrounded:
     *    • No WorkManager periodic tasks syncing while app is closed
     *    • No FCM push from the server (we don't have Firebase keys on
     *      the server side and we're not adding that dependency to a
     *      desktop service)
     *  If the user wants an event while the app is closed, they see it
     *  the next time they foreground — that's an acceptable UX for this
     *  companion, per the "local-first, paired-device" design.            */
}
