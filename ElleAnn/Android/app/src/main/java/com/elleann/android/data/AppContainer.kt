package com.elleann.android.data

import android.content.Context
import com.jakewharton.retrofit2.converter.kotlinx.serialization.asConverterFactory
import kotlinx.serialization.json.Json
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.logging.HttpLoggingInterceptor
import retrofit2.Retrofit
import java.util.concurrent.TimeUnit

/**
 * AppContainer — tiny hand-rolled DI container.
 *
 *   Holds the app-wide singletons:
 *     - [TokenStore] — persistent, Keystore-wrapped auth state
 *     - [OkHttpClient] — shared connection pool, pre-configured with
 *                       [AuthInterceptor] and timeouts
 *
 *   [apiFor] returns a Retrofit service bound to a specific host+port.
 *   The base URL changes when the user pairs against a different
 *   server, so we do NOT cache a single singleton — we cache by URL.
 */
class AppContainer(context: Context) {

    val tokenStore: TokenStore = TokenStore(context)

    private val json = Json {
        ignoreUnknownKeys = true
        isLenient = true
    }

    private val logging = HttpLoggingInterceptor().apply {
        level = HttpLoggingInterceptor.Level.BASIC
    }

    val client: OkHttpClient = OkHttpClient.Builder()
        .connectTimeout(10, TimeUnit.SECONDS)
        .readTimeout(30,  TimeUnit.SECONDS)
        .writeTimeout(30, TimeUnit.SECONDS)
        .addInterceptor(AuthInterceptor(tokenStore))
        .addInterceptor(logging)
        .build()

    private val apiCache = mutableMapOf<String, ElleApi>()

    fun apiFor(host: String, port: Int): ElleApi {
        val base = "http://$host:$port/"
        apiCache[base]?.let { return it }
        val retrofit = Retrofit.Builder()
            .baseUrl(base)
            .client(client)
            .addConverterFactory(json.asConverterFactory("application/json".toMediaType()))
            .build()
        val api = retrofit.create(ElleApi::class.java)
        apiCache[base] = api
        return api
    }

    /** Convenience — returns the API for the CURRENTLY-paired server,
     *  or null when no pairing exists yet.                              */
    fun pairedApi(): ElleApi? {
        val t = tokenStore.load() ?: return null
        return apiFor(t.host, t.port)
    }
}
