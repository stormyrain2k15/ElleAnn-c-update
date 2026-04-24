package com.elleann.android

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import com.elleann.android.data.AppContainer
import com.elleann.android.ui.HomeScreen
import com.elleann.android.ui.PairScreen
import com.elleann.android.ui.theme.ElleTheme

class MainActivity : ComponentActivity() {

    private lateinit var container: AppContainer
    private var initialDeepLink: PairingPayload? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        container = (application as ElleApp).container

        /* If launched via an ellepair:// deep link, capture the payload
         * so PairScreen can prefill without the user retyping. */
        initialDeepLink = parseDeepLink(intent)

        setContent {
            ElleTheme {
                var paired by remember { mutableStateOf(container.tokenStore.load() != null) }
                val prefill = remember { initialDeepLink }
                if (paired) {
                    HomeScreen(
                        container = container,
                        onUnpair  = {
                            container.tokenStore.clear()
                            paired = false
                        }
                    )
                } else {
                    PairScreen(
                        container = container,
                        prefill   = prefill,
                        onPaired  = { paired = true }
                    )
                }
            }
        }
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        parseDeepLink(intent)?.let { initialDeepLink = it }
    }
}

/** Payload parsed out of a `ellepair://host:port/code` URI. */
data class PairingPayload(val host: String, val port: Int, val code: String)

private fun parseDeepLink(intent: Intent?): PairingPayload? {
    val data: Uri = intent?.data ?: return null
    if (data.scheme != "ellepair") return null
    val host = data.host ?: return null
    val port = if (data.port != -1) data.port else return null
    /* path is "/<6-digit code>" */
    val code = data.pathSegments.firstOrNull() ?: return null
    if (code.length != 6 || !code.all { it.isDigit() }) return null
    return PairingPayload(host, port, code)
}
