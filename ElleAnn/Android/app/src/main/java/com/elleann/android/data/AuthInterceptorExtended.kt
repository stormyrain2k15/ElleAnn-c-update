package com.elleann.android.data

import com.elleann.android.TokenStore
import okhttp3.Interceptor
import okhttp3.Response

/**
 * AuthInterceptorExtended — attaches the opaque session Bearer token to
 * every outgoing request and triggers a re-auth callback on 401.
 *
 * Feb 2026 pivot:
 *  - No JWT, no expiry check, no x-admin-key header.
 *  - A session token is an opaque 64-hex string minted by
 *    POST /api/auth/login and stored verbatim via TokenStore.
 *  - Admin privilege is server-side only: if the logged-in user's
 *    tUser.nAuthID meets the server's admin_auth_id_threshold, the
 *    SAME Bearer token authorises every admin route. No separate
 *    client-side secret to juggle.
 *  - On 401 (token deleted server-side, or never valid), the store
 *    is cleared and `onReauthRequired()` fires so the host Activity
 *    can bounce the user to the login screen.
 */
class AuthInterceptorExtended(
    private val tokenStore: TokenStore,
    // Kept as a constructor-signature shim so AppContainerExtended
    // wiring doesn't need to change. The value is ignored.
    @Suppress("UNUSED_PARAMETER") adminKeyStore: AdminKeyStore? = null,
    private val onReauthRequired: () -> Unit,
) : Interceptor {

    override fun intercept(chain: Interceptor.Chain): Response {
        val stored = tokenStore.load()
        val requestBuilder = chain.request().newBuilder()

        // Single header, always the session token when we have one.
        // No expiry math — the server is the only thing that knows
        // whether a token is still valid.
        if (stored != null && stored.jwt.isNotEmpty()) {
            requestBuilder.addHeader("Authorization", "Bearer ${stored.jwt}")
        }

        val response = chain.proceed(requestBuilder.build())

        if (response.code == 401) {
            tokenStore.clear()
            onReauthRequired()
        }

        return response
    }
}

/**
 * AdminKeyStore — retained as an empty shim for the Feb-2026 pivot.
 *
 * Prior to the pivot this stored an encrypted x-admin-key in
 * EncryptedSharedPreferences.  That shared secret has been retired —
 * admin routes are now gated by the logged-in user's tUser.nAuthID on
 * the server side.  The class stays present (empty) so call sites
 * that haven't been migrated yet still compile; all methods report
 * "no key" and writes are swallowed.  Safe to delete once the last
 * reference in the UI is gone.
 */
@Suppress("UNUSED_PARAMETER")
class AdminKeyStore(prefs: android.content.SharedPreferences? = null) {
    fun getKey(): String = ""
    fun setKey(key: String) { /* no-op — admin is tUser.nAuthID now */ }
    fun clearKey() { /* no-op */ }
    fun hasKey(): Boolean = false
}
