package com.elleann.android

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import com.elleann.android.data.AppContainerExtended
import com.elleann.android.navigation.ElleNavHost
import com.elleann.android.ui.theme.ElleAnnTheme

/**
 * MainActivity — NO_AUTH mode.
 * Login page and pairing state logic removed Feb 2026.
 */
class MainActivity : ComponentActivity() {

    private lateinit var containerExtended: AppContainerExtended

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

        val app = application as ElleApp

        containerExtended = AppContainerExtended(
            context       = this,
            baseContainer = app.container,
            onReauthRequired = {
                // No-op in no_auth mode
            },
        )

        // Always init WebSocket on start. Bails gracefully (no throw)
        // if no host has been paired yet — see ElleWebSocket.openConnection.
        containerExtended.initWebSocket()

        setContent {
            ElleAnnTheme {
                ElleNavHost(
                    container         = app.container,
                    containerExtended = containerExtended,
                    /* Personal AI — always treated as paired. The default
                     * host (158.62.137.73:8000) is hardcoded in
                     * AppContainerExtended; pair flow is not a gate. */
                    isPaired          = true,
                    onPaired          = { },
                    onUnpair          = { },
                )
            }
        }
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
    }

    override fun onStart() {
        super.onStart()
        containerExtended.reconnectWebSocketIfNeeded()
    }

    override fun onStop() {
        super.onStop()
    }

    override fun onDestroy() {
        super.onDestroy()
        containerExtended.disconnectWebSocket()
    }
}
