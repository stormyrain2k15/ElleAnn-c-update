package com.elleann.android

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import com.elleann.android.data.AppContainerExtended
import com.elleann.android.navigation.ElleNavHost
import com.elleann.android.ui.theme.ElleAnnTheme
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow

class ElleAnnAppState(initialPaired: Boolean) {
    private val _isPaired   = MutableStateFlow(initialPaired)
    private val _prefill    = MutableStateFlow<PairingPayload?>(null)
    val isPaired: StateFlow<Boolean>          = _isPaired.asStateFlow()
    val pendingPrefill: StateFlow<PairingPayload?> = _prefill.asStateFlow()
    fun setPaired(v: Boolean)         { _isPaired.value = v }
    fun setPrefill(v: PairingPayload?) { _prefill.value = v }
}

/**
 * MainActivity — single-activity entry point.
 *
 * Fix 3: WebSocket initialized immediately on cold start if already paired.
 * Fix 4: onStop no longer disconnects WebSocket. Only onDestroy does.
 *        onStart calls reconnectWebSocketIfNeeded() instead.
 * Fix 6: isPaired→false triggers explicit navigate-to-pair via ElleNavHost LaunchedEffect.
 */
class MainActivity : ComponentActivity() {

    private lateinit var appState: ElleAnnAppState
    private lateinit var containerExtended: AppContainerExtended

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

        val app = application as ElleApp
        val alreadyPaired = app.container.tokenStore.load() != null
        appState = ElleAnnAppState(initialPaired = alreadyPaired)

        containerExtended = AppContainerExtended(
            context       = this,
            baseContainer = app.container,
            onReauthRequired = {
                app.container.tokenStore.clear()
                appState.setPaired(false)
            },
        )

        // Fix 3 — init WebSocket immediately if already paired (cold start)
        if (alreadyPaired) containerExtended.initWebSocket()

        intent?.data?.let { parseEllepairUri(it)?.let(appState::setPrefill) }

        setContent {
            val isPaired by appState.isPaired.collectAsState()
            val prefill  by appState.pendingPrefill.collectAsState()

            ElleAnnTheme {
                ElleNavHost(
                    container         = app.container,
                    containerExtended = containerExtended,
                    isPaired          = isPaired,
                    onPaired = {
                        containerExtended.initWebSocket()
                        appState.setPaired(true)
                        appState.setPrefill(null)
                    },
                    onUnpair = {
                        app.container.tokenStore.clear()
                        containerExtended.disconnectWebSocket()
                        appState.setPaired(false)
                    },
                    prefill = prefill,
                )
            }
        }
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        intent.data?.let { parseEllepairUri(it)?.let(appState::setPrefill) }
        // Does NOT touch isPaired — a paired session is never forcibly disrupted
    }

    // Fix 4 — resume reconnects WS without tearing it down
    override fun onStart() {
        super.onStart()
        if (appState.isPaired.value) containerExtended.reconnectWebSocketIfNeeded()
    }

    // Fix 4 — onStop no longer disconnects. Background = WS stays alive.
    override fun onStop() {
        super.onStop()
        // Intentionally empty — WebSocket persists through backgrounding.
        // Android will kill the process if memory pressure demands it;
        // the WS reconnect-with-backoff handles that case on next onStart.
    }

    // Full cleanup on process death only
    override fun onDestroy() {
        super.onDestroy()
        containerExtended.disconnectWebSocket()
    }

    companion object {
        fun parseEllepairUri(uri: Uri): PairingPayload? {
            if (uri.scheme != "ellepair") return null
            val host = uri.host?.takeIf { it.isNotBlank() } ?: return null
            val port = uri.port.takeIf { it != -1 } ?: return null
            val code = uri.pathSegments.firstOrNull()?.takeIf {
                it.length == 6 && it.all { c -> c.isDigit() }
            } ?: return null
            return PairingPayload(host, port, code)
        }
    }
}
