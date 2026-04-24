package com.elleann.android.data

import okhttp3.Interceptor
import okhttp3.Response

/**
 * AuthInterceptor — attaches `Authorization: Bearer <jwt>` to every
 * outgoing request EXCEPT the pairing endpoint (which is the one
 * endpoint that runs without a token).
 *
 *   On a 401 response, clears the stored token. The UI layer observes
 *   the empty store on next composition and navigates back to the
 *   pairing screen, per the contract in Android/spec/Auth.kt.
 */
class AuthInterceptor(
    private val tokenStore: TokenStore,
) : Interceptor {

    override fun intercept(chain: Interceptor.Chain): Response {
        val original = chain.request()

        val skip = original.url.encodedPath.endsWith("/api/auth/pair") ||
                   original.url.encodedPath.endsWith("/api/auth/pair-code") ||
                   original.url.encodedPath.endsWith("/api/health")

        val req = if (skip) {
            original
        } else {
            val tok = tokenStore.load()
            if (tok != null) {
                original.newBuilder()
                    .addHeader("Authorization", "Bearer ${tok.jwt}")
                    .build()
            } else {
                original
            }
        }

        val resp = chain.proceed(req)
        if (resp.code == 401 && !skip) {
            /* Server told us the token is dead — wipe it. The next
             * composition in MainActivity observes `tokenStore.load()
             * == null` and routes back to PairScreen.                */
            tokenStore.clear()
        }
        return resp
    }
}
