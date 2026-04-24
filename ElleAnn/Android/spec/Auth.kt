/*══════════════════════════════════════════════════════════════════════════════
 * Auth.kt — Pairing handshake + JWT storage contract
 *
 *   The Android app has NO username/password UI. Pairing is a one-time
 *   QR-or-manual handshake that exchanges a short-lived pairing code for
 *   a long-lived JWT. Subsequent calls attach `Authorization: Bearer <jwt>`.
 *
 *   This file documents the contract. Implementation:
 *     - Token storage → EncryptedSharedPreferences backed by Android Keystore
 *     - Token refresh → on 401, trigger pairing flow (NO refresh tokens)
 *     - HTTP client → OkHttp Interceptor attaches header, handles 401 retry
 *══════════════════════════════════════════════════════════════════════════════*/
package com.elleann.android.data.spec

/** Pairing protocol.
 *
 *   Step 1. User opens desktop ElleAnn → Settings → "Pair device". The
 *           HTTP server generates a 6-digit numeric code and a matching
 *           QR that encodes: `ellepair://<host>:<port>/<code>`.
 *
 *   Step 2. Android app scans the QR (or user types host + code manually).
 *
 *   Step 3. App POSTs the code to `/api/auth/pair` with a device identifier:
 *
 *             POST /api/auth/pair
 *             { "code": "429156", "device_name": "Pixel 9", "device_id": "<uuid>" }
 *
 *           Server validates the code, issues a JWT, and returns:
 *
 *             200 OK
 *             { "jwt": "<opaque>", "expires_ms": 1737580800000,
 *               "paired_at_ms": 1737494400000 }
 *
 *           Code is single-use and expires server-side after 5 minutes.
 *
 *   Step 4. App stores the JWT in Android Keystore / EncryptedSharedPreferences.
 *           All subsequent requests attach `Authorization: Bearer <jwt>`.
 *
 *   Step 5. On 401 from any endpoint: clear stored JWT, navigate back to
 *           pairing screen, show "Re-pair with ElleAnn".
 *
 *   NOTE: The `/api/auth/pair` endpoint does NOT YET EXIST in
 *         Services/Elle.Service.HTTP/HTTPServer.cpp — it needs to be added
 *         before any Android implementation begins. This spec defines the
 *         target contract. Implementation ticket should:
 *           (a) Add m_pairingCodes registry (code → { expiry_ms, single_use })
 *           (b) Add POST /api/auth/pair route
 *           (c) Mint JWT via existing helpers (http.jwt_secret already exists)
 *           (d) Persist paired devices to a new x_paired_devices table for
 *               audit + revocation.
 */
object Auth {

    /** Pairing request body.                                                 */
    data class PairRequest(
        val code:         String,       // 6-digit numeric
        val device_name:  String,       // human label, e.g. "Pixel 9"
        val device_id:    String,       // client UUID, persisted across reinstalls
    )

    /** Pairing response body on success (200).                               */
    data class PairResponse(
        val jwt:          String,       // opaque; do not parse
        val expires_ms:   Long,         // absolute epoch ms
        val paired_at_ms: Long,
    )

    /** Stored token snapshot. Keep this tight — anything we persist ends up
     *  in Keystore-wrapped shared prefs, which is not designed for large
     *  blobs.                                                                */
    data class StoredToken(
        val jwt:        String,
        val expires_ms: Long,
    )

    /** Interface the app implements around Android Keystore. The spec does
     *  NOT prescribe the implementation — just the shape. See
     *  androidx.security:security-crypto for the recommended EncryptedSharedPreferences.
     */
    interface TokenStore {
        fun save(token: StoredToken)
        fun load(): StoredToken?
        fun clear()
    }

    /** OkHttp Interceptor contract (pseudocode).
     *
     *   class AuthInterceptor(private val store: TokenStore) : Interceptor {
     *       override fun intercept(chain: Chain): Response {
     *           val tok = store.load()
     *           val req = if (tok != null && tok.expires_ms > System.currentTimeMillis())
     *               chain.request().newBuilder()
     *                   .addHeader("Authorization", "Bearer ${tok.jwt}")
     *                   .build()
     *           else chain.request()
     *           val resp = chain.proceed(req)
     *           if (resp.code == 401) {
     *               store.clear()
     *               // Signal to navigation layer: re-pair required.
     *           }
     *           return resp
     *       }
     *   }
     */
}
