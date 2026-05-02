package com.elleann.android.data

import com.elleann.android.TokenStore
import okhttp3.Interceptor
import okhttp3.Response

/**
 * AuthInterceptorExtended — NO_AUTH mode.
 *
 * This interceptor no longer attaches Authorization headers and no longer
 * reacts to 401 errors. It assumes the server is running in no_auth mode
 * (injected identity).
 */
class AuthInterceptorExtended(
    private val tokenStore: TokenStore,
    @Suppress("UNUSED_PARAMETER") adminKeyStore: AdminKeyStore? = null,
    private val onReauthRequired: () -> Unit,
) : Interceptor {

    override fun intercept(chain: Interceptor.Chain): Response {
        val requestBuilder = chain.request().newBuilder()
        
        // No-auth mode: we do NOT attach Bearer tokens.
        // The server provides identity via injected context.
        
        return chain.proceed(requestBuilder.build())
    }
}

/**
 * AdminKeyStore — shim for compatibility.
 */
@Suppress("UNUSED_PARAMETER")
class AdminKeyStore(prefs: android.content.SharedPreferences? = null) {
    fun getKey(): String = ""
    fun setKey(key: String) { }
    fun clearKey() { }
    fun hasKey(): Boolean = false
}
