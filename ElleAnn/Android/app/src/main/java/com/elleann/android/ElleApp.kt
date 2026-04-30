package com.elleann.android

import android.app.Application
import android.content.Context
import androidx.security.crypto.EncryptedSharedPreferences
import androidx.security.crypto.MasterKey
import com.jakewharton.retrofit2.converter.kotlinx.serialization.asConverterFactory
import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import okhttp3.MediaType.Companion.toMediaType
import com.elleann.android.data.RedactingLoggingInterceptor
import okhttp3.OkHttpClient
import retrofit2.Retrofit
import java.util.concurrent.TimeUnit

// ─── Pairing payload — parsed from ellepair:// URI ────────────────────────────
data class PairingPayload(
    val host: String,
    val port: Int,
    val code: String,
)

// ─── Stored token + server coords ─────────────────────────────────────────────
@Serializable
data class StoredToken(
    val jwt: String,
    val host: String,
    val port: Int,
    val expiresMs: Long = Long.MAX_VALUE,
)

// ─── EmotionsResponse — canonical definition, one place only ──────────────────
@Serializable
data class EmotionsResponse(
    val valence: Float? = null,
    val arousal: Float? = null,
    val dominance: Float? = null,
    val mood: String? = null,
    val joy: Float? = null,
    val sadness: Float? = null,
    val anger: Float? = null,
    val fear: Float? = null,
    val disgust: Float? = null,
    val trust: Float? = null,
    val surprise: Float? = null,
    val anticipation: Float? = null,
)

// ─── PairResponse ─────────────────────────────────────────────────────────────
@Serializable
data class PairResponse(
    val jwt: String,
    @kotlinx.serialization.SerialName("expires_ms")    val expiresMs:   Long = Long.MAX_VALUE,
    @kotlinx.serialization.SerialName("paired_at_ms") val pairedAtMs:  Long = 0,
    /** Present only when game-account auth was used (sign-in path).
     *  Pair-code path leaves these null. */
    val nUserNo:   Int?    = null,
    val sUserName: String? = null,
)

// ─── TokenStore ───────────────────────────────────────────────────────────────
class TokenStore(context: Context) {
    companion object {
        private const val PREFS_NAME = "elle_secure_prefs"
        private const val KEY_TOKEN  = "elle_jwt"
    }

    private val json = Json { ignoreUnknownKeys = true; isLenient = true }

    private val prefs: android.content.SharedPreferences by lazy {
        val masterKey = MasterKey.Builder(context)
            .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
            .build()
        EncryptedSharedPreferences.create(
            context, PREFS_NAME, masterKey,
            EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
            EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM,
        )
    }

    fun save(token: StoredToken) =
        prefs.edit().putString(KEY_TOKEN, json.encodeToString(StoredToken.serializer(), token)).apply()

    fun load(): StoredToken? = runCatching {
        val raw = prefs.getString(KEY_TOKEN, null) ?: return null
        json.decodeFromString(StoredToken.serializer(), raw)
    }.getOrNull()

    fun clear() = prefs.edit().remove(KEY_TOKEN).apply()
}

// ─── Locked ElleApi interface (pair + health + emotions only) ─────────────────
interface ElleApi {
    @retrofit2.http.GET("/api/health")
    suspend fun health(): Map<String, String>

    @retrofit2.http.GET("/api/emotions")
    suspend fun emotions(): EmotionsResponse

    @retrofit2.http.POST("/api/auth/pair")
    suspend fun pair(@retrofit2.http.Body body: com.elleann.android.data.models.PairRequest): PairResponse
}

// ─── AppContainer — locked base container ─────────────────────────────────────
class AppContainer(context: Context) {
    val tokenStore = TokenStore(context)

    private val json = Json {
        ignoreUnknownKeys = true; isLenient = true
        coerceInputValues = true; encodeDefaults = true
    }

    val baseOkHttpClient: OkHttpClient = OkHttpClient.Builder()
        .addInterceptor(RedactingLoggingInterceptor())
        .connectTimeout(10, TimeUnit.SECONDS)
        .readTimeout(20, TimeUnit.SECONDS)
        .build()

    fun apiFor(host: String, port: Int): ElleApi {
        PrivateLanValidator.require(host)
        val client = baseOkHttpClient.newBuilder()
            .addInterceptor { chain ->
                val stored = tokenStore.load()
                val req = chain.request().newBuilder()
                    .let { b -> if (stored != null) b.addHeader("Authorization", "Bearer ${stored.jwt}") else b }
                    .build()
                chain.proceed(req)
            }.build()

        return Retrofit.Builder()
            .baseUrl("http://$host:$port/")
            .client(client)
            .addConverterFactory(json.asConverterFactory("application/json".toMediaType()))
            .build()
            .create(ElleApi::class.java)
    }

    val api: ElleApi?
        get() = tokenStore.load()?.let { apiFor(it.host, it.port) }
}

// ─── Private LAN validator ───────────────────────────────────────────
/**
 * Rejects any host that is not a private RFC 1918 / loopback address.
 * Since the Elle server is always on a local network, connecting to a public
 * IP is either a misconfiguration or a SSRF attempt and must be rejected
 * at the app layer regardless of what the network security config allows.
 */
object PrivateLanValidator {
    private val PRIVATE_RANGES = listOf(
        Regex("""^10\.\d+\.\d+\.\d+$"""),          // RFC 1918 class A
        Regex("""^172\.(1[6-9]|2\d|3[01])\.\d+\.\d+$"""), // RFC 1918 class B
        Regex("""^192\.168\.\d+\.\d+$"""),            // RFC 1918 class C
        Regex("""^127\.\d+\.\d+\.\d+$"""),           // Loopback IPv4
        Regex("""^::1$"""),                                 // Loopback IPv6
        Regex("""^localhost$"""),                           // Loopback hostname
        Regex("""^.+\.local$"""),                         // mDNS — Josh's server may use .local
    )

    /** @throws IllegalArgumentException if [host] is not a private/loopback address */
    fun require(host: String) {
        val clean = host.trim().lowercase()
        require(PRIVATE_RANGES.any { it.matches(clean) }) {
            "ElleAnn only connects to private LAN addresses. Rejected host: $host"
        }
    }

    fun isValid(host: String): Boolean = runCatching { require(host); true }.getOrDefault(false)
}

// ─── ElleApp ──────────────────────────────────────────────────────────────────
class ElleApp : Application() {
    lateinit var container: AppContainer
        private set
    lateinit var chatCacheManager: com.elleann.android.data.ChatCacheManager
        private set

    override fun onCreate() {
        super.onCreate()
        container = AppContainer(applicationContext)

        /* Crash-safe chat cache — required by product spec. The local
         * .txt cache is a startup-latency optimisation, but it MUST
         * stay current across both clean exits and hard crashes,
         * otherwise the next launch shows stale messages and the user
         * sees the chat "rewind".
         *
         * Three escalating pathways to flush the cache:
         *   (a) Per-message:  `ChatViewModel.flushCache()` rewrites the
         *       file on every state mutation. (Cheapest, normal case.)
         *   (b) Lifecycle:    [ProcessLifecycleOwner] ON_STOP fires when
         *       the user backgrounds the app — flush every tracked
         *       conversation in case (a) missed a write.
         *   (c) Crash:        [Thread.setDefaultUncaughtExceptionHandler]
         *       runs SYNCHRONOUSLY before the previous handler escalates.
         *       Writes every tracked conversation to disk via the
         *       atomic temp-file rename pattern in ChatCacheManager.
         */
        chatCacheManager = com.elleann.android.data.ChatCacheManager(applicationContext)
        com.elleann.android.data.ChatCacheManager.installAsGlobal(chatCacheManager)

        val previousHandler = Thread.getDefaultUncaughtExceptionHandler()
        Thread.setDefaultUncaughtExceptionHandler { thread, throwable ->
            runCatching { com.elleann.android.data.ChatCacheManager.crashFlush() }
            previousHandler?.uncaughtException(thread, throwable)
                ?: Thread.getDefaultUncaughtExceptionHandler()?.uncaughtException(thread, throwable)
        }

        androidx.lifecycle.ProcessLifecycleOwner.get().lifecycle.addObserver(
            object : androidx.lifecycle.DefaultLifecycleObserver {
                override fun onStop(owner: androidx.lifecycle.LifecycleOwner) {
                    /* Background flush — non-blocking is fine here, the
                     * user already left the app; anything they typed has
                     * already been pushed via per-message writes.       */
                    runCatching { chatCacheManager.flushAllBlocking() }
                }
            }
        )
    }
}
