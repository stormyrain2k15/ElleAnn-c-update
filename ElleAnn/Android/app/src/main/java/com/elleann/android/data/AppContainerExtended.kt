package com.elleann.android.data

import android.content.Context
import androidx.security.crypto.EncryptedSharedPreferences
import androidx.security.crypto.MasterKey
import com.elleann.android.AppContainer
import com.elleann.android.PrivateLanValidator
import com.elleann.android.TokenStore
import com.jakewharton.retrofit2.converter.kotlinx.serialization.asConverterFactory
import kotlinx.serialization.json.Json
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import retrofit2.Retrofit
import java.util.concurrent.TimeUnit

/**
 * AppContainerExtended — extends the locked AppContainer with:
 *   - ElleApiExtended (all additional REST endpoints)
 *   - ElleApacheApi (10 Apache endpoints on port 8080, Named Pipes SQL)
 *   - AdminKeyStore (encrypted x-admin-key)
 *   - ElleWebSocket (real-time events)
 *   - Re-auth callback for JWT expiry
 */
class AppContainerExtended(
    context: Context,
    val baseContainer: AppContainer,
    private val onReauthRequired: () -> Unit,
) {
    // ── Encrypted SharedPreferences ───────────────────────────────────────────
    private val masterKey = MasterKey.Builder(context)
        .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
        .build()

    private val encryptedPrefs = EncryptedSharedPreferences.create(
        context, "elle_secure_prefs", masterKey,
        EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
        EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM,
    )

    // ── Accessors for locked-layer types ──────────────────────────────────────
    val tokenStore: TokenStore get() = baseContainer.tokenStore
    val adminKeyStore = AdminKeyStore(encryptedPrefs)

    // ── JSON ──────────────────────────────────────────────────────────────────
    private val json = Json {
        ignoreUnknownKeys = true; isLenient = true
        coerceInputValues = true; encodeDefaults = true
    }

    // ── Redacting logging interceptor ─────────────────────────────────
    // Never log Authorization or x-admin-key header values.
    // Level.HEADERS in debug only — never BODY (logs JWT, admin key, chat content).
    private val loggingInterceptor = RedactingLoggingInterceptor()

    // ── Auth interceptor ──────────────────────────────────────────────────────
    private val authInterceptor = AuthInterceptorExtended(
        tokenStore    = baseContainer.tokenStore,
        adminKeyStore = adminKeyStore,
        onReauthRequired = onReauthRequired,
    )

    // ── Base OkHttp client (REST) ─────────────────────────────────────────────
    private val okHttpClient = OkHttpClient.Builder()
        .addInterceptor(authInterceptor)
        .addInterceptor(loggingInterceptor)
        .connectTimeout(15, TimeUnit.SECONDS)
        .readTimeout(30, TimeUnit.SECONDS)
        .writeTimeout(30, TimeUnit.SECONDS)
        .build()

    // ── Raw OkHttp client — longer read timeout for identity/Apache endpoints ─
    val rawHttpClient: OkHttpClient = OkHttpClient.Builder()
        .addInterceptor(loggingInterceptor)
        .connectTimeout(15, TimeUnit.SECONDS)
        .readTimeout(120, TimeUnit.SECONDS)
        .writeTimeout(30, TimeUnit.SECONDS)
        .build()

    // ── WebSocket : nullable guard ────────────────────────────────────
    private var _webSocket: ElleWebSocket? = null
    val webSocket: ElleWebSocket get() = _webSocket
        ?: error("WebSocket not initialized. Call initWebSocket() after pairing.")
    val webSocketOrNull: ElleWebSocket? get() = _webSocket
    val isWebSocketInitialized: Boolean get() = _webSocket != null

    // ── REST API (extended) ───────────────────────────────────────────────────
    private var _extendedApi: ElleApiExtended? = null

    fun extendedApiFor(host: String, port: Int): ElleApiExtended {
        PrivateLanValidator.require(host)
        return Retrofit.Builder()
            .baseUrl("http://$host:$port/")
            .client(okHttpClient)
            .addConverterFactory(json.asConverterFactory("application/json".toMediaType()))
            .build()
            .create(ElleApiExtended::class.java)
    }

    fun pairedExtendedApi(): ElleApiExtended? {
        val stored = tokenStore.load() ?: return null
        return extendedApiFor(stored.host, stored.port).also { _extendedApi = it }
    }

    val extendedApi: ElleApiExtended
        get() = _extendedApi ?: (pairedExtendedApi()
            ?: error("No paired server."))

    // ── Apache API (Named Pipes SQL on server side) ───────────────────────────
    private val apachePort = 8080
    private var _apacheApi: ElleApacheApi? = null

    fun apacheApiFor(host: String): ElleApacheApi {
        PrivateLanValidator.require(host)
        return Retrofit.Builder()
            .baseUrl("http://$host:$apachePort/")
            .client(okHttpClient)
            .addConverterFactory(json.asConverterFactory("application/json".toMediaType()))
            .build()
            .create(ElleApacheApi::class.java)
    }

    fun pairedApacheApi(): ElleApacheApi? {
        val stored = tokenStore.load() ?: return null
        return apacheApiFor(stored.host).also { _apacheApi = it }
    }

    val apacheApi: ElleApacheApi
        get() = _apacheApi ?: (pairedApacheApi()
            ?: error("No paired server."))

    val apacheBaseUrl: String?
        get() = tokenStore.load()?.let { "http://${it.host}:$apachePort" }


    // ── Separate admin OkHttp client — x-admin-key only on admin calls ──
    // The main okHttpClient no longer attaches x-admin-key (see AuthInterceptorExtended).
    // Use adminApi for any route requiring x-admin-key (ADMIN auth level).
    private val adminOkHttpClient = OkHttpClient.Builder()
        .addInterceptor(authInterceptor)
        .addInterceptor { chain ->
            val key = adminKeyStore.getKey()
            val req = if (key.isNotEmpty())
                chain.request().newBuilder().addHeader("x-admin-key", key).build()
            else chain.request()
            chain.proceed(req)
        }
        .addInterceptor(loggingInterceptor)
        .connectTimeout(15, TimeUnit.SECONDS)
        .readTimeout(30, TimeUnit.SECONDS)
        .writeTimeout(30, TimeUnit.SECONDS)
        .build()

    /** Admin-scoped API — carries x-admin-key. Use for ADMIN auth-level endpoints only. */
    fun adminApiFor(host: String, port: Int): ElleApiExtended {
        PrivateLanValidator.require(host)
        return Retrofit.Builder()
            .baseUrl("http://$host:$port/")
            .client(adminOkHttpClient)
            .addConverterFactory(json.asConverterFactory("application/json".toMediaType()))
            .build()
            .create(ElleApiExtended::class.java)
    }

    val adminApi: ElleApiExtended?
        get() = tokenStore.load()?.let { adminApiFor(it.host, it.port) }

    // ── Initialize WebSocket (call on cold start if already paired) ────
    fun initWebSocket() {
        val stored = tokenStore.load() ?: return
        _webSocket?.disconnect()
        _webSocket = ElleWebSocket(
            host   = stored.host,
            port   = stored.port,
            jwt    = stored.jwt,
            client = okHttpClient,
        )
        _webSocket?.connect()
    }

    // ── Reconnect without full teardown — safe to call on resume ───────
    fun reconnectWebSocketIfNeeded() {
        val ws = _webSocket
        if (ws == null) {
            initWebSocket()
        } else if (!ws.isConnected()) {
            ws.connect()
        }
        // Already connected — do nothing
    }

    /** Full disconnect — call only from onDestroy, not onStop */
    fun disconnectWebSocket() {
        _webSocket?.disconnect()
        _webSocket = null
    }

    // ── QR SVG retrieval ─────────────────────────────────────────────────────
    suspend fun getPairingQrSvg(code: String, host: String, port: Int): String? {
        val stored = tokenStore.load() ?: return null
        val url = "http://${stored.host}:${stored.port}/api/auth/qr?code=$code&host=$host&port=$port"
        return runCatching {
            val req = okhttp3.Request.Builder().url(url)
                .addHeader("Authorization", "Bearer ${stored.jwt}")
                .addHeader("x-admin-key", adminKeyStore.getKey())
                .build()
            rawHttpClient.newCall(req).execute().use { r ->
                if (r.isSuccessful) r.body?.string() else null
            }
        }.getOrNull()
    }

    // ── Dev PIN — SHA-256 ─────────────────────────────────────────────────────
    companion object {
        private const val KEY_DEV_PIN_HASH = "elle_dev_pin_hash"
    }

    fun setDevPin(pin: String) =
        encryptedPrefs.edit().putString(KEY_DEV_PIN_HASH, sha256(pin)).apply()

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

