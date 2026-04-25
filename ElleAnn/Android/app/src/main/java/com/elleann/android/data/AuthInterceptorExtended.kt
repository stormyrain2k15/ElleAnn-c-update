package com.elleann.android.data

import com.elleann.android.TokenStore
import okhttp3.Interceptor
import okhttp3.Response

/**
 * AuthInterceptorExtended — OkHttp interceptor that attaches both the
 * Bearer JWT (for AUTH_USER routes) and the optional x-admin-key header
 * (for AUTH_ADMIN routes) to every outgoing request.
 *
 * The x-admin-key is stored encrypted in EncryptedSharedPreferences via
 * AdminKeyStore and is optional — if not set, it is omitted. Admin routes
 * will return HTTP 403 if the key is missing or incorrect.
 *
 * JWT token refresh: on HTTP 401, the token is cleared and a
 * REAUTH_REQUIRED event is emitted via [onReauthRequired] so the host
 * Activity can navigate back to PairScreen.
 */
class AuthInterceptorExtended(
    private val tokenStore: TokenStore,
    private val adminKeyStore: AdminKeyStore,
    private val onReauthRequired: () -> Unit,
) : Interceptor {

    override fun intercept(chain: Interceptor.Chain): Response {
        val stored = tokenStore.load()
        val requestBuilder = chain.request().newBuilder()

        // Attach Bearer JWT if available and not expired
        if (stored != null && stored.expiresMs > System.currentTimeMillis()) {
            requestBuilder.addHeader("Authorization", "Bearer ${stored.jwt}")
        }

        // x-admin-key is NOT attached here.
        // It is only added by the dedicated adminOkHttpClient in AppContainerExtended.
        // This prevents the admin key from leaking onto every user-level request.

        val response = chain.proceed(requestBuilder.build())

        // JWT expired or invalid — clear and trigger re-pair flow
        if (response.code == 401) {
            tokenStore.clear()
            onReauthRequired()
        }

        return response
    }
}

/**
 * AdminKeyStore — encrypted storage for the x-admin-key shared secret.
 * The key is configured in Dev Settings → Admin Key and stored in the
 * same EncryptedSharedPreferences instance as the JWT.
 */
class AdminKeyStore(
    private val prefs: android.content.SharedPreferences,
) {
    companion object {
        private const val KEY_ADMIN = "elle_admin_key"
    }

    /** Retrieve the stored admin key, or empty string if not configured */
    fun getKey(): String = prefs.getString(KEY_ADMIN, "") ?: ""

    /** Save the admin key (encrypted at rest via EncryptedSharedPreferences) */
    fun setKey(key: String) {
        prefs.edit().putString(KEY_ADMIN, key).apply()
    }

    /** Remove the stored admin key */
    fun clearKey() {
        prefs.edit().remove(KEY_ADMIN).apply()
    }

    /** True if an admin key is currently configured */
    fun hasKey(): Boolean = getKey().isNotEmpty()
}
