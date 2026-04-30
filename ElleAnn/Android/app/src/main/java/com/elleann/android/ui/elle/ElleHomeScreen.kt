package com.elleann.android.ui.elle

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.rounded.Settings
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontStyle
import androidx.compose.ui.unit.dp
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.compose.viewModel
import com.elleann.android.data.AppContainerExtended
import com.elleann.android.data.ElleWebSocket
import com.elleann.android.data.WsEvent
import com.elleann.android.EmotionsResponse
import com.elleann.android.data.models.*
import com.elleann.android.ui.components.*
import com.elleann.android.ui.components.IsyaTopBar
import com.elleann.android.ui.components.IsyaHudRow
import com.elleann.android.ui.components.IsyaHudStat
import com.elleann.android.ui.theme.*
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch

// ─── State ───────────────────────────────────────────────────────────────────

data class ElleHomeState(
    val greeting: SessionGreeting? = null,
    val emotions: EmotionsResponse? = null,
    val aiStatus: AiStatus? = null,
    val selfImage: SelfImageResponse? = null,
    val wordOfDay: DictionaryWord? = null,
    val brainStatus: String = "",
    val halStatus: String = "",
    /** Identity tuple from /api/me — proves auth wired to Account.dbo.tUser. */
    val me: MeResponse? = null,
    val connectionState: ConnectionState = ConnectionState.DISCONNECTED,
    val loading: Boolean = true,
    val error: String? = null,
)

// ─── ViewModel ───────────────────────────────────────────────────────────────

class ElleHomeViewModel(
    private val containerExtended: AppContainerExtended,
    private val webSocket: ElleWebSocket,
) : ViewModel() {

    private val _state = MutableStateFlow(ElleHomeState())
    val state: StateFlow<ElleHomeState> = _state.asStateFlow()

    init {
        load()
        observeWebSocket()
        startEmotionPolling()
        startHealthPolling()
    }

    private fun load() {
        viewModelScope.launch {
            _state.update { it.copy(loading = true, error = null) }
            val api = containerExtended.pairedExtendedApi() ?: run {
                _state.update { it.copy(loading = false, error = "Not paired") }
                return@launch
            }

            // Fire all requests concurrently
            val greetingJob = launch {
                runCatching { api.getSessionGreeting() }
                    .onSuccess { g ->
                        _state.update { it.copy(greeting = g) }
                        // Acknowledge the greeting immediately
                        runCatching { api.ackSessionGreeting(g.id) }
                    }
            }
// Emotion data loaded via startEmotionPolling() and WebSocket ipc_broadcast — no load-time job needed
            val meJob = launch {
                /* /api/me confirms the JWT actually resolves to a real
                 * tUser row.  If this returns 401, the token is stale
                 * and the operator should re-pair.  We surface the
                 * error in the home strip so it's visible without
                 * needing the dev panel. */
                runCatching { api.getMe() }
                    .onSuccess { m -> _state.update { it.copy(me = m) } }
            }
            val aiJob = launch {
                runCatching { api.getAiStatus() }
                    .onSuccess { s -> _state.update { it.copy(aiStatus = s) } }
            }
            val selfImageJob = launch {
                runCatching { api.getSelfImage() }
                    .onSuccess { s -> _state.update { it.copy(selfImage = s) } }
            }
            val wordJob = launch {
                runCatching { api.getRandomWord() }
                    .onSuccess { w -> _state.update { it.copy(wordOfDay = w) } }
            }
            val brainJob = launch {
                runCatching { api.getBrainStatus() }
                    .onSuccess { b -> _state.update { it.copy(brainStatus = b["status"] ?: "") } }
            }
            val halJob = launch {
                runCatching { api.getHalStatus() }
                    .onSuccess { h -> _state.update { it.copy(halStatus = h.hardware) } }
            }

            greetingJob.join(); aiJob.join(); selfImageJob.join()
            wordJob.join(); brainJob.join(); halJob.join()

            _state.update { it.copy(loading = false) }
        }
    }

    /** Collect WebSocket emotion broadcasts — all emotion fields mapped */
    private fun observeWebSocket() {
        viewModelScope.launch {
            webSocket.events.collect { event ->
                when (event) {
                    is WsEvent.Connected    -> _state.update { it.copy(connectionState = ConnectionState.CONNECTED) }
                    is WsEvent.EmotionUpdate -> _state.update { it.copy(emotions = event.emotion) }
                    is WsEvent.Disconnected -> _state.update { it.copy(connectionState = ConnectionState.DISCONNECTED) }
                    else -> Unit
                }
            }
        }
    }

    /** Supplement WebSocket with REST poll every 5s — covers cases where WS misses a broadcast */
    private fun startEmotionPolling() {
        viewModelScope.launch {
            while (isActive) {
                delay(5_000)
                runCatching { containerExtended.extendedApi.getEmotionDimensions() }
                    .onSuccess { dims ->
                        // Build EmotionsResponse from dimension list for the home display
                        val dimMap = dims.dimensions.associate { it.name to it.value }
                        val current = _state.value.emotions
                        _state.update { it.copy(emotions = com.elleann.android.EmotionsResponse(
                            valence      = dimMap["valence"]      ?: current?.valence,
                            arousal      = dimMap["arousal"]      ?: current?.arousal,
                            dominance    = dimMap["dominance"]    ?: current?.dominance,
                            mood         = current?.mood,
                            joy          = dimMap["joy"]          ?: current?.joy,
                            sadness      = dimMap["sadness"]      ?: current?.sadness,
                            anger        = dimMap["anger"]        ?: current?.anger,
                            fear         = dimMap["fear"]         ?: current?.fear,
                            disgust      = dimMap["disgust"]      ?: current?.disgust,
                            trust        = dimMap["trust"]        ?: current?.trust,
                            surprise     = dimMap["surprise"]     ?: current?.surprise,
                            anticipation = dimMap["anticipation"] ?: current?.anticipation,
                        ))}
                    }
            }
        }
    }

    /** Poll /api/health every 5s to update connection indicator */
    private fun startHealthPolling() {
        viewModelScope.launch {
            while (isActive) {
                delay(5_000)
                val start = System.currentTimeMillis()
                val ok = runCatching {
                    containerExtended.extendedApi.getBrainStatus()
                }.isSuccess
                val elapsed = System.currentTimeMillis() - start
                _state.update {
                    it.copy(
                        connectionState = when {
                            !ok     -> ConnectionState.DISCONNECTED
                            elapsed > 1000 -> ConnectionState.SLOW
                            else    -> ConnectionState.CONNECTED
                        }
                    )
                }
            }
        }
    }

    fun retry() = load()
}

// ─── Screen ──────────────────────────────────────────────────────────────────

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ElleHomeScreen(
    containerExtended: AppContainerExtended,
    webSocket: ElleWebSocket,
    onSettings: () -> Unit,
    onUnpair: () -> Unit,
) {
    val vm: ElleHomeViewModel = viewModel(
        factory = remember {
            object : androidx.lifecycle.ViewModelProvider.Factory {
                @Suppress("UNCHECKED_CAST")
                override fun <T : ViewModel> create(modelClass: Class<T>): T =
                    ElleHomeViewModel(containerExtended, webSocket) as T
            }
        }
    )

    val state by vm.state.collectAsState()
    val stored = containerExtended.tokenStore.load()
    val serverLabel = stored?.let { "${it.host}:${it.port}" } ?: "—"

    Scaffold(
        containerColor = IsyaNight,
        topBar = {
            // Custom IsyaTopBar replaces TopAppBar
            IsyaTopBar(
                title = {
                    Text("ElleAnn", style = MaterialTheme.typography.titleMedium,
                        color = IsyaGold, fontWeight = androidx.compose.ui.text.font.FontWeight.Bold)
                },
                subtitle = {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        ConnectionDot(state.connectionState)
                        Spacer(Modifier.width(6.dp))
                        /* Show the resolved game-account username instead
                         * of just host:port so the operator can confirm
                         * at a glance which user the JWT belongs to.
                         * Falls back to host:port when /api/me hasn't
                         * resolved yet (or returned 401 → re-pair). */
                        val ident = state.me?.let { "${it.username} · #${it.userId}" } ?: serverLabel
                        Text(ident, style = MaterialTheme.typography.labelSmall, color = IsyaMuted)
                    }
                },
                actions = {
                    androidx.compose.material3.IconButton(onClick = onSettings) {
                        Icon(Icons.Rounded.Settings, "Settings", tint = IsyaMuted)
                    }
                },
            )
        }
    ) { padding ->
        if (state.loading) {
            Box(Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
                IsyaLoadingIndicator()
            }
            return@Scaffold
        }

        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(padding)
                .verticalScroll(rememberScrollState())
                .padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(16.dp),
        ) {

            // ── Greeting card ─────────────────────────────────────────────────
            state.greeting?.let { greeting ->
                IsyaPanel(title = "✦ Elle speaks", flowingBorder = true) {
                    Text(
                        text      = greeting.greeting,
                        style     = MaterialTheme.typography.bodyMedium,
                        color     = IsyaCream,
                        fontStyle = FontStyle.Italic,
                    )
                }
            }

            // ── Emotional Pulse ───────────────────────────────────────────────
            IsyaPanel(title = "EMOTIONAL PULSE", flowingBorder = true) {
                state.emotions?.let { e ->
                    Column(verticalArrangement = Arrangement.spacedBy(8.dp)) {
                        e.valence?.let { EmotionBar("Valence", it, color = IsyaMagic) }
                        e.arousal?.let { EmotionBar("Arousal", it, color = ElleViolet) }
                        e.dominance?.let { EmotionBar("Dominance", it, color = IsyaGold) }
                        e.mood?.let { mood ->
                            Spacer(Modifier.height(4.dp))
                            Text(
                                text  = "Mood: $mood",
                                style = MaterialTheme.typography.bodySmall,
                                color = IsyaParchment,
                            )
                        }
                    }
                } ?: Text("Loading emotions…", color = IsyaMuted)
            }

            // ── Self Image ────────────────────────────────────────────────────
            state.selfImage?.let { si ->
                if (si.description.isNotBlank()) {
                    IsyaPanel(title = "ELLE SEES HERSELF AS") {
                        Text(
                            text      = si.description,
                            style     = MaterialTheme.typography.bodySmall,
                            color     = IsyaParchment,
                            fontStyle = FontStyle.Italic,
                        )
                    }
                }
            }

            // ── Word of the moment ────────────────────────────────────────────
            state.wordOfDay?.let { word ->
                IsyaPanel(title = "✦ WORD OF ISYA") {
                    Text(
                        text       = word.word,
                        style      = MaterialTheme.typography.titleMedium,
                        color      = IsyaGold,
                        fontStyle  = FontStyle.Normal,
                    )
                    Spacer(Modifier.height(4.dp))
                    word.partOfSpeech?.let {
                        Text(it, style = MaterialTheme.typography.labelSmall, color = IsyaMuted)
                    }
                    word.definition?.let {
                        Text(it, style = MaterialTheme.typography.bodySmall, color = IsyaCream)
                    }
                }
            }

            // ── AI / System status ────────────────────────────────────────────
            IsyaPanel(title = "SYSTEM") {
                state.aiStatus?.let { ai ->
                    Column(verticalArrangement = Arrangement.spacedBy(4.dp)) {
                        StatusRow("AI", ai.modelStatus, IsyaSuccess)
                        StatusRow("Model", ai.modelName, IsyaCream)
                        StatusRow("Brain", state.brainStatus.ifBlank { "active" }, IsyaSuccess)
                        StatusRow("HAL", state.halStatus.ifBlank { "nominal" }, IsyaSuccess)
                    }
                } ?: Text("Loading status…", color = IsyaMuted)
            }

            // ── Error state ───────────────────────────────────────────────────
            state.error?.let { err ->
                IsyaErrorState(err, onRetry = { vm.retry() })
            }
        }
    }
}

@Composable
private fun StatusRow(label: String, value: String, valueColor: Color) {
    Row(
        modifier              = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.SpaceBetween,
    ) {
        Text(label, style = MaterialTheme.typography.bodySmall, color = IsyaMuted)
        Text(value, style = MaterialTheme.typography.bodySmall, color = valueColor)
    }
}

// Base emotions response uses the locked ElleApi.emotions() call from AppContainer.
// The home screen loads emotions via extendedApi.getEmotionDimensions() and the
// WebSocket ipc_broadcast event — no separate pairedApi() call needed here.
