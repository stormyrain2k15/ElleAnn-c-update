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
     *  Resolution rule (Feb-2026 personal-AI build):
     *    1. Persisted host:port from TokenStore (if ever set via Settings)
     *    2. Else operator's home server: 158.62.137.73:8000
     *
     *  This is a personal AI with auth intentionally OFF (`no_auth=1` on
     *  the server). External security happens out-of-band (firewall,
     *  WireGuard, Cloudflare Tunnel, etc).  The app NEVER attaches a
     *  Bearer header — `AuthInterceptorExtended` is a passthrough.
     */
    fun getApi(): ElleApiExtended {
        val stored = tokenStore.load()
        val host = stored?.host?.takeIf { it.isNotBlank() } ?: DEFAULT_HOST
        val port = stored?.port?.takeIf { it > 0 } ?: DEFAULT_PORT

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

    /** Always paired — this is a personal AI; no pair-flow gate. */
    val isPaired: Boolean get() = true

    val restBaseUrl: String?
        get() {
            val stored = tokenStore.load()
            val host = stored?.host?.takeIf { it.isNotBlank() } ?: DEFAULT_HOST
            val port = stored?.port?.takeIf { it > 0 } ?: DEFAULT_PORT
            return "http://$host:$port"
        }

    fun initWebSocket() {
        val stored = tokenStore.load()
        val host = stored?.host?.takeIf { it.isNotBlank() } ?: DEFAULT_HOST
        val port = stored?.port?.takeIf { it > 0 } ?: DEFAULT_PORT

        _webSocket?.disconnect()
        _webSocket = ElleWebSocket(
            host   = host,
            port   = port,
            jwt    = stored?.jwt ?: "",
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
        /** Operator's home server — used as the fallback host:port when
         *  nothing has been persisted via Settings yet.  This is a
         *  personal AI; "from anywhere" public reach is the design. */
        const val DEFAULT_HOST = "158.62.137.73"
        const val DEFAULT_PORT = 8000
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
