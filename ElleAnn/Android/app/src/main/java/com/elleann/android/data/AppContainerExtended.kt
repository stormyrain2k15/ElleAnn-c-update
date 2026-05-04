package com.elleann.android.data

import android.content.Context
import androidx.core.content.edit
import androidx.security.crypto.EncryptedSharedPreferences
import androidx.security.crypto.MasterKey
import com.elleann.android.AppContainer
import com.elleann.android.StoredToken
import com.elleann.android.TokenStore
import com.jakewharton.retrofit2.converter.kotlinx.serialization.asConverterFactory
import kotlinx.serialization.json.Json
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import retrofit2.Retrofit
import java.util.concurrent.TimeUnit

/**
 * AppContainerExtended — NO_AUTH mode.
 * Removed LAN IP lock and Login requirements Feb 2026.
 */
class AppContainerExtended(
    context: Context,
    val baseContainer: AppContainer,
    private val onReauthRequired: () -> Unit,
) {
    private val masterKey = MasterKey.Builder(context)
        .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
        .build()

    private val encryptedPrefs = EncryptedSharedPreferences.create(
        context, "elle_secure_prefs", masterKey,
        EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
        EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM,
    )

    val tokenStore: TokenStore get() = baseContainer.tokenStore
    val adminKeyStore = AdminKeyStore(encryptedPrefs)

    private val json = Json {
        ignoreUnknownKeys = true; isLenient = true
        coerceInputValues = true; encodeDefaults = true
    }

    private val loggingInterceptor = RedactingLoggingInterceptor()

    private val authInterceptor = AuthInterceptorExtended(
        tokenStore    = baseContainer.tokenStore,
        adminKeyStore = adminKeyStore,
        onReauthRequired = onReauthRequired,
    )

    private val okHttpClient = OkHttpClient.Builder()
        .addInterceptor(authInterceptor)
        .addInterceptor(loggingInterceptor)
        .connectTimeout(15, TimeUnit.SECONDS)
        .readTimeout(30, TimeUnit.SECONDS)
        .writeTimeout(30, TimeUnit.SECONDS)
        .build()

    val rawHttpClient: OkHttpClient = OkHttpClient.Builder()
        .addInterceptor(loggingInterceptor)
        .connectTimeout(15, TimeUnit.SECONDS)
        .readTimeout(120, TimeUnit.SECONDS)
        .writeTimeout(30, TimeUnit.SECONDS)
        .build()

    private var _webSocket: ElleWebSocket? = null
    val webSocketOrNull: ElleWebSocket? get() = _webSocket
    val isWebSocketInitialized: Boolean get() = _webSocket != null

    // ── REST API ─────────────────────────────────────────────────────────────
    
    /**
     * Get API instance.
     *
     *  Resolution rule (Feb-2026):
     *    1. Persisted host:port from TokenStore (set via PairScreen)
     *    2. Else 10.0.2.2:8000 — emulator-only convenience.  This NEVER
     *       resolves on a real device, but throwing here would crash
     *       the whole UI cold; we let OkHttp surface the connect failure
     *       so the screens can show "Pair Elle's host first".
     *
     *  The server's `no_auth=1` mode means we never need to attach a
     *  bearer token here — auth is intentionally off.  See
     *  `/app/memory/test_credentials.md`.
     */
    fun getApi(): ElleApiExtended {
        val stored = tokenStore.load()
        val host = stored?.host?.takeIf { it.isNotBlank() } ?: "10.0.2.2"
        val port = stored?.port?.takeIf { it > 0 } ?: 8000

        return Retrofit.Builder()
            .baseUrl("http://$host:$port/")
            .client(okHttpClient)
            .addConverterFactory(json.asConverterFactory("application/json".toMediaType()))
            .build()
            .create(ElleApiExtended::class.java)
    }

    // Keep for compatibility with existing code
    val extendedApi: ElleApiExtended get() = getApi()

    /**
     * Admin-keyed API. In no-auth mode, this just returns extendedApi.
     */
    val adminApi: ElleApiExtended? get() = extendedApi

    fun pairedExtendedApi(): ElleApiExtended? = getApi()

    /** True iff the operator has paired a host (PairScreen). UI uses this
     *  to gate connect attempts and show a "pair first" banner.          */
    val isPaired: Boolean
        get() {
            val stored = tokenStore.load() ?: return false
            return stored.host.isNotBlank() && stored.port > 0
        }

    val restBaseUrl: String?
        get() {
            val stored = tokenStore.load() ?: return null
            if (stored.host.isBlank() || stored.port <= 0) return null
            return "http://${stored.host}:${stored.port}"
        }

    fun initWebSocket() {
        val stored = tokenStore.load()
        val host = stored?.host?.takeIf { it.isNotBlank() } ?: return
        val port = stored?.port?.takeIf { it > 0 } ?: return

        _webSocket?.disconnect()
        _webSocket = ElleWebSocket(
            host   = host,
            port   = port,
            jwt    = stored.jwt,
            client = okHttpClient,
        )
        _webSocket?.connect()
    }

    fun reconnectWebSocketIfNeeded() {
        if (_webSocket == null) {
            initWebSocket()
        } else if (!_webSocket!!.isConnected()) {
            _webSocket!!.connect()
        }
    }

    fun disconnectWebSocket() {
        _webSocket?.disconnect()
        _webSocket = null
    }

    // ── Utils ────────────────────────────────────────────────────────────────
    
    fun setServerCoords(host: String, port: Int) {
        val current = tokenStore.load()
        tokenStore.save(StoredToken(
            jwt = current?.jwt ?: "",
            host = host,
            port = port
        ))
    }

    @Suppress("UNUSED_PARAMETER")
    fun getPairingQrSvg(code: String, host: String, port: Int): String? = null

    companion object {
        private const val KEY_DEV_PIN_HASH = "elle_dev_pin_hash"
    }

    fun setDevPin(pin: String) =
        encryptedPrefs.edit { putString(KEY_DEV_PIN_HASH, sha256(pin)) }

    fun verifyDevPin(pin: String): Boolean {
        val stored = encryptedPrefs.getString(KEY_DEV_PIN_HASH, null) ?: return false
        return stored == sha256(pin)
    }

    fun hasDevPin(): Boolean = encryptedPrefs.contains(KEY_DEV_PIN_HASH)

    private fun sha256(input: String): String {
        val digest = java.security.MessageDigest.getInstance("SHA-256")
        return digest.digest(input.toByteArray(Charsets.UTF_8))
            .joinToString("") { "%02x".format(it) }
    }
}
