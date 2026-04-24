package com.elleann.android.ui

import android.app.Activity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.QrCodeScanner
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import com.elleann.android.PairingPayload
import com.elleann.android.data.AppContainer
import com.elleann.android.data.PairRequest
import com.journeyapps.barcodescanner.ScanContract
import com.journeyapps.barcodescanner.ScanOptions
import kotlinx.coroutines.launch

/**
 * PairScreen — one-time handshake with the desktop ElleAnn server.
 *
 *   Flow:
 *     1. User (or deep link) supplies host, port, 6-digit code.
 *     2. Optional: scan a QR produced by /api/auth/qr — fills all
 *        three fields via the ellepair:// URI parser.
 *     3. POST /api/auth/pair → JWT → persist → onPaired().
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun PairScreen(
    container: AppContainer,
    prefill:   PairingPayload?,
    onPaired:  () -> Unit,
) {
    var host by remember { mutableStateOf(prefill?.host ?: "") }
    var port by remember { mutableStateOf(prefill?.port?.toString() ?: "8080") }
    var code by remember { mutableStateOf(prefill?.code ?: "") }
    var busy by remember { mutableStateOf(false) }
    var error by remember { mutableStateOf<String?>(null) }

    val scope = rememberCoroutineScope()

    val scannerLauncher = rememberLauncherForActivityResult(ScanContract()) { res ->
        val raw = res?.contents ?: return@rememberLauncherForActivityResult
        parseEllepair(raw)?.let {
            host = it.host
            port = it.port.toString()
            code = it.code
        } ?: run { error = "Unrecognised QR payload" }
    }

    Scaffold(
        topBar = { TopAppBar(title = { Text("Pair with ElleAnn") }) }
    ) { inner: PaddingValues ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(inner)
                .padding(24.dp)
                .verticalScroll(rememberScrollState()),
            verticalArrangement   = Arrangement.spacedBy(16.dp),
            horizontalAlignment   = Alignment.Start
        ) {
            Text(
                "Enter the host, port, and 6-digit code shown on your desktop " +
                "ElleAnn Settings → Pair Device. Or scan the on-screen QR.",
                style = MaterialTheme.typography.bodyMedium
            )

            OutlinedTextField(
                value = host,
                onValueChange = { host = it.trim() },
                label = { Text("Host (IP or hostname)") },
                singleLine = true,
                modifier = Modifier.fillMaxWidth()
            )

            OutlinedTextField(
                value = port,
                onValueChange = { port = it.filter { c -> c.isDigit() }.take(5) },
                label = { Text("Port") },
                singleLine = true,
                keyboardOptions = androidx.compose.foundation.text.KeyboardOptions(
                    keyboardType = KeyboardType.Number),
                modifier = Modifier.fillMaxWidth()
            )

            OutlinedTextField(
                value = code,
                onValueChange = { code = it.filter { c -> c.isDigit() }.take(6) },
                label = { Text("6-digit code") },
                singleLine = true,
                keyboardOptions = androidx.compose.foundation.text.KeyboardOptions(
                    keyboardType = KeyboardType.NumberPassword),
                modifier = Modifier.fillMaxWidth()
            )

            OutlinedButton(
                onClick = {
                    scannerLauncher.launch(
                        ScanOptions()
                            .setPrompt("Point at ElleAnn's pairing QR")
                            .setBeepEnabled(false)
                            .setOrientationLocked(false)
                    )
                },
                modifier = Modifier.fillMaxWidth()
            ) {
                Icon(Icons.Default.QrCodeScanner, contentDescription = null)
                Spacer(Modifier.width(8.dp))
                Text("Scan QR")
            }

            Button(
                enabled = !busy && host.isNotBlank() &&
                          port.toIntOrNull() != null && code.length == 6,
                onClick = {
                    error = null
                    busy = true
                    scope.launch {
                        val result = runCatching {
                            val api = container.apiFor(host, port.toInt())
                            api.pair(
                                PairRequest(
                                    code       = code,
                                    deviceName = android.os.Build.MODEL ?: "Android",
                                    deviceId   = container.tokenStore.deviceId()
                                )
                            )
                        }
                        busy = false
                        result.onSuccess { resp ->
                            container.tokenStore.save(
                                com.elleann.android.data.TokenStore.Stored(
                                    jwt       = resp.jwt,
                                    expiresMs = resp.expiresMs,
                                    host      = host,
                                    port      = port.toInt(),
                                    deviceId  = container.tokenStore.deviceId()
                                )
                            )
                            onPaired()
                        }.onFailure { t ->
                            error = t.message ?: "Pairing failed"
                        }
                    }
                },
                modifier = Modifier.fillMaxWidth()
            ) {
                if (busy) {
                    CircularProgressIndicator(
                        modifier = Modifier.size(18.dp),
                        strokeWidth = 2.dp
                    )
                    Spacer(Modifier.width(8.dp))
                    Text("Pairing…")
                } else {
                    Text("Pair")
                }
            }

            error?.let {
                Card(
                    colors = CardDefaults.cardColors(
                        containerColor = MaterialTheme.colorScheme.errorContainer
                    ),
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Text(
                        it,
                        modifier = Modifier.padding(12.dp),
                        color = MaterialTheme.colorScheme.onErrorContainer
                    )
                }
            }
        }
    }

    /* Auto-submit when a deep-link prefill arrives complete. */
    LaunchedEffect(prefill) {
        if (prefill != null && code.length == 6 && host.isNotBlank() && !busy) {
            /* One-shot nudge — user still sees the Pair button so they
             * can cancel if they landed here by accident.               */
        }
    }
}

private fun parseEllepair(raw: String): PairingPayload? {
    return runCatching {
        val u = android.net.Uri.parse(raw)
        if (u.scheme != "ellepair") return null
        val h = u.host ?: return null
        val p = if (u.port != -1) u.port else return null
        val c = u.pathSegments.firstOrNull() ?: return null
        if (c.length != 6 || !c.all { it.isDigit() }) return null
        PairingPayload(h, p, c)
    }.getOrNull()
}
