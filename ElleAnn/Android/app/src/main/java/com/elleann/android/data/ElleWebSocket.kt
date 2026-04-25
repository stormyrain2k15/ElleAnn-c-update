package com.elleann.android.data

import android.util.Log
import com.elleann.android.EmotionsResponse
import com.elleann.android.data.models.XHormones
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.SharedFlow
import kotlinx.coroutines.flow.asSharedFlow
import kotlinx.coroutines.launch
import kotlinx.serialization.json.Json
import kotlinx.serialization.json.JsonObject
import kotlinx.serialization.json.jsonObject
import kotlinx.serialization.json.jsonPrimitive
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.Response
import okhttp3.WebSocket
import okhttp3.WebSocketListener
import java.util.concurrent.TimeUnit

// ─── WebSocket event sealed hierarchy ─────────────────────────────────────────

/** All possible events received from the Elle WebSocket at /command */
sealed class WsEvent {

    /** Server confirmed connection: {type:"connected", client_id, server, version} */
    data class Connected(
        val clientId: String,
        val server: String,
        val version: String,
    ) : WsEvent()

    /** Ping response from server */
    data object Pong : WsEvent()

    /** Streaming chat response: {type:"chat_response", request_id, response} */
    data class ChatResponse(
        val requestId: String,
        val response: String,
    ) : WsEvent()

    /** Live emotion broadcast: {type:"ipc_broadcast", msg_type:1, emotion:{...}} */
    data class EmotionUpdate(
        val emotion: EmotionsResponse,
    ) : WsEvent()

    /** X-Chromosome hormone tick: {type:"hormone_tick", ...} */
    data class HormoneTick(
        val hormones: XHormones,
        val phase: String,
    ) : WsEvent()

    /** Cycle phase transition: {type:"phase_transition", from, to} */
    data class PhaseTransition(
        val from: String,
        val to: String,
    ) : WsEvent()

    /** Birth event: {type:"birth"} */
    data object Birth : WsEvent()

    /** LH surge detected: {type:"lh_surge"} */
    data object LhSurge : WsEvent()

    /** Labor stage update: {type:"labor_stage", stage} */
    data class LaborStage(val stage: String) : WsEvent()

    /** Miscarriage event: {type:"miscarriage"} */
    data object Miscarriage : WsEvent()

    /** Connection dropped or error */
    data class Disconnected(val reason: String) : WsEvent()

    /** Unknown event type — raw JSON preserved for debugging */
    data class Unknown(val type: String, val raw: String) : WsEvent()
}

/**
 * ElleWebSocket — manages the persistent WebSocket connection to Elle's
 * /command endpoint. Parses incoming frames and emits [WsEvent]s via
 * a [SharedFlow] that UI layers can collect.
 *
 * Auto-reconnect: on unexpected disconnect, waits [RECONNECT_DELAY_MS]
 * before attempting to re-establish the connection.
 *
 * Usage:
 *   val event = webSocket.events.collectAsState()
 *   webSocket.sendChat("Hello Elle", requestId = "req-001")
 */
class ElleWebSocket(
    private val host: String,
    private val port: Int,
    private val jwt: String,
    private val client: OkHttpClient,
) {
    companion object {
        private const val TAG = "ElleWebSocket"
        private const val WS_PATH = "/command"
        // Backoff: starts at 2s, doubles each attempt, caps at 60s
        private const val BACKOFF_INITIAL_MS  = 2_000L
        private const val BACKOFF_MAX_MS      = 60_000L
        private const val BACKOFF_MULTIPLIER  = 2.0
        private const val PING_INTERVAL_SECONDS = 30L
    }

    // ── Event stream ──────────────────────────────────────────────────────────
    private val _events = MutableSharedFlow<WsEvent>(replay = 0, extraBufferCapacity = 64)
    val events: SharedFlow<WsEvent> = _events.asSharedFlow()

    // ── Lifecycle scope — tied to ElleWebSocket instance, not any Activity ────
    private val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)

    // ── Connection state ──────────────────────────────────────────────────────
    private var webSocket: WebSocket? = null
    @Volatile private var isConnected  = false
    @Volatile private var isConnecting = false
    @Volatile private var shouldReconnect = true
    private var reconnectJob: Job? = null

    private val json = Json { ignoreUnknownKeys = true; isLenient = true }

    // ── OkHttp client for WS (separate from REST — needs ping interval) ───────
    private val wsClient = client.newBuilder()
        .pingInterval(PING_INTERVAL_SECONDS, TimeUnit.SECONDS)
        .build()

    /**
     * Open the WebSocket connection.
     *
     * Idempotent: returns immediately if a socket is already connected
     * OR currently mid-handshake. Without the `isConnecting` guard, a
     * caller hitting `reconnectWebSocketIfNeeded()` repeatedly during a
     * slow connect could spawn multiple sockets that race to onOpen().
     */
    fun connect() {
        if (isConnected || isConnecting || webSocket != null) return
        shouldReconnect = true
        openConnection()
    }

    /**
     * Close the connection cleanly and cancel all coroutines.
     * AppContainerExtended always creates a fresh ElleWebSocket via initWebSocket()
     * rather than reusing a disconnected instance, so cancelling the scope here is safe.
     */
    fun disconnect() {
        shouldReconnect = false
        reconnectJob?.cancel()
        reconnectJob = null
        webSocket?.close(1000, "Client disconnecting")
        webSocket = null
        isConnected  = false
        isConnecting = false
        scope.cancel()
    }

    /** True if the socket is currently connected */
    fun isConnected(): Boolean = isConnected

    private fun openConnection(attempt: Int = 0) {
        isConnecting = true
        val url = "ws://$host:$port$WS_PATH"
        val request = Request.Builder()
            .url(url)
            .addHeader("Authorization", "Bearer $jwt")
            .build()

        webSocket = wsClient.newWebSocket(request, object : WebSocketListener() {

            override fun onOpen(ws: WebSocket, response: Response) {
                isConnecting = false
                isConnected  = true
                Log.i(TAG, "WebSocket connected to $url")
            }

            override fun onMessage(ws: WebSocket, text: String) {
                parseAndEmit(text)
            }

            override fun onFailure(ws: WebSocket, t: Throwable, response: Response?) {
                /* Drop the dead reference so connect() guards see "no
                 * pending socket" and the next reconnect cycle can
                 * actually open a fresh one.                          */
                if (webSocket == ws) webSocket = null
                isConnecting = false
                isConnected  = false
                Log.w(TAG, "WebSocket failure (attempt $attempt): ${t.message}")
                _events.tryEmit(WsEvent.Disconnected(t.message ?: "Unknown error"))
                scheduleReconnect(attempt)
            }

            override fun onClosed(ws: WebSocket, code: Int, reason: String) {
                if (webSocket == ws) webSocket = null
                isConnecting = false
                isConnected  = false
                Log.i(TAG, "WebSocket closed: $code $reason")
                _events.tryEmit(WsEvent.Disconnected(reason))
                // Normal close (1000) = intentional; all others schedule reconnect
                if (shouldReconnect && code != 1000) scheduleReconnect(attempt)
            }
        })
    }

    /**
     * Schedule a reconnect attempt using coroutine delay with exponential backoff.
     * No threads are blocked. Backoff doubles each attempt from [BACKOFF_INITIAL_MS]
     * up to [BACKOFF_MAX_MS].
     *
     * @param attemptNumber used to compute backoff: delay = min(initial * 2^attempt, max)
     */
    private fun scheduleReconnect(attemptNumber: Int = 0) {
        if (!shouldReconnect) return
        reconnectJob?.cancel()
        reconnectJob = scope.launch {
            val delayMs = minOf(
                (BACKOFF_INITIAL_MS * Math.pow(BACKOFF_MULTIPLIER, attemptNumber.toDouble())).toLong(),
                BACKOFF_MAX_MS,
            )
            Log.i(TAG, "Reconnect attempt ${attemptNumber + 1} in ${delayMs}ms")
            delay(delayMs)
            if (shouldReconnect) openConnection(attemptNumber + 1)
        }
    }

    // ── Message parsing ───────────────────────────────────────────────────────
    private fun parseAndEmit(text: String) {
        runCatching {
            val obj: JsonObject = json.parseToJsonElement(text).jsonObject
            val type = obj["type"]?.jsonPrimitive?.content ?: return

            val event: WsEvent = when (type) {
                "connected" -> WsEvent.Connected(
                    clientId = obj["client_id"]?.jsonPrimitive?.content ?: "",
                    server   = obj["server"]?.jsonPrimitive?.content ?: "",
                    version  = obj["version"]?.jsonPrimitive?.content ?: "",
                )
                "pong" -> WsEvent.Pong
                "chat_response" -> WsEvent.ChatResponse(
                    requestId = obj["request_id"]?.jsonPrimitive?.content ?: "",
                    response  = obj["response"]?.jsonPrimitive?.content ?: "",
                )
                "ipc_broadcast" -> {
                    val emotionObj = obj["emotion"]?.jsonObject
                    fun f(key: String) = emotionObj?.get(key)?.jsonPrimitive?.content?.toFloatOrNull()
                    WsEvent.EmotionUpdate(
                        emotion = EmotionsResponse(
                            valence      = f("valence"),
                            arousal      = f("arousal"),
                            dominance    = f("dominance"),
                            mood         = emotionObj?.get("mood")?.jsonPrimitive?.content,
                            joy          = f("joy"),
                            sadness      = f("sadness"),
                            anger        = f("anger"),
                            fear         = f("fear"),
                            disgust      = f("disgust"),
                            trust        = f("trust"),
                            surprise     = f("surprise"),
                            anticipation = f("anticipation"),
                        )
                    )
                }
                "hormone_tick" -> WsEvent.HormoneTick(
                    hormones = parseHormones(obj),
                    phase    = obj["phase"]?.jsonPrimitive?.content ?: "",
                )
                "phase_transition" -> WsEvent.PhaseTransition(
                    from = obj["from"]?.jsonPrimitive?.content ?: "",
                    to   = obj["to"]?.jsonPrimitive?.content ?: "",
                )
                "birth"       -> WsEvent.Birth
                "lh_surge"    -> WsEvent.LhSurge
                "labor_stage" -> WsEvent.LaborStage(obj["stage"]?.jsonPrimitive?.content ?: "")
                "miscarriage" -> WsEvent.Miscarriage
                else          -> WsEvent.Unknown(type, text)
            }

            _events.tryEmit(event)
        }.onFailure { e ->
            Log.e(TAG, "Failed to parse WS message: $text", e)
        }
    }

    private fun parseHormones(obj: JsonObject): XHormones {
        fun float(key: String) = obj[key]?.jsonPrimitive?.content?.toFloatOrNull() ?: 0f
        return XHormones(
            estradiol    = float("estradiol"),
            progesterone = float("progesterone"),
            testosterone = float("testosterone"),
            lh           = float("lh"),
            fsh          = float("fsh"),
            prolactin    = float("prolactin"),
            oxytocin     = float("oxytocin"),
            cortisol     = float("cortisol"),
            hcg          = float("hcg"),
        )
    }

    // ── Outgoing messages ─────────────────────────────────────────────────────

    /** Send a ping to the server */
    fun sendPing() {
        webSocket?.send("""{"type":"ping"}""")
    }

    /**
     * Send a chat message over WebSocket for streaming response.
     * Returns true if the message was accepted by the OkHttp send buffer,
     * false if the socket is null, closed, or the message was too large.
     * The server emits [WsEvent.ChatResponse] events back with the matching [requestId].
     */
    fun sendChat(message: String, requestId: String, conversationId: Long = 1L): Boolean {
        val ws = webSocket ?: return false
        val encoded = kotlinx.serialization.json.Json.encodeToString(
            kotlinx.serialization.json.JsonPrimitive.serializer(),
            kotlinx.serialization.json.JsonPrimitive(message),
        )
        val payload = """{"type":"chat","message":$encoded,"request_id":"$requestId","conversation_id":$conversationId}"""
        return ws.send(payload)
    }

    /** Subscribe to a topic for server-push updates */
    fun subscribe(topic: String) {
        webSocket?.send("""{"type":"subscribe","topic":"$topic"}""")
    }
}
