/*══════════════════════════════════════════════════════════════════════════════
 * ErrorEnvelope.kt — What every non-2xx response body looks like
 *
 *   The server returns JSON on EVERY response, success or failure. Failures
 *   carry an `error` field (string) and may carry optional `details`.
 *
 *   Status code rules (per HTTPResponse::Err contract in HTTPServer.cpp):
 *     400 — Client sent malformed input (missing required field, bad JSON)
 *     401 — Authorization missing / invalid / expired → re-pair flow
 *     403 — Valid token but insufficient scope (admin routes the app
 *           should never hit; if it does, app has a bug)
 *     404 — Route or resource not found
 *     409 — Conflict (e.g. conception attempt while pregnancy already active)
 *     429 — Rate limited (if http.rate_limit_rpm > 0 server-side)
 *     500 — Server-side error (DB down, IPC hub not reachable). Retry once.
 *     503 — Subsystem temporarily unavailable (e.g. XChromosome restart).
 *           Retry with back-off.
 *══════════════════════════════════════════════════════════════════════════════*/
package com.elleann.android.data.spec

data class ErrorEnvelope(
    val error:   String,
    val details: String? = null,
)

/** Convenience bucket for mapping HTTP status → user-facing UX action.     */
enum class ErrorDisposition {
    /** Show a "please check and try again" inline validation error.        */
    USER_FIXABLE,
    /** Trigger the pairing flow. Clear stored JWT first.                   */
    REAUTH_REQUIRED,
    /** Show "ElleAnn isn't answering, try again in a moment", auto-retry. */
    TRANSIENT_BACKOFF,
    /** Hard bug — log to Crashlytics/analytics, don't spam the user.       */
    CLIENT_BUG;

    companion object {
        fun fromStatus(code: Int): ErrorDisposition = when (code) {
            400, 409    -> USER_FIXABLE
            401         -> REAUTH_REQUIRED
            403         -> CLIENT_BUG
            404         -> CLIENT_BUG
            429, 500, 503 -> TRANSIENT_BACKOFF
            else        -> if (code in 500..599) TRANSIENT_BACKOFF else CLIENT_BUG
        }
    }
}
