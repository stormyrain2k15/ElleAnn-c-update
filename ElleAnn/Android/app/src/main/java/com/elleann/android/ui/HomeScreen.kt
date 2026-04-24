package com.elleann.android.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.elleann.android.data.AppContainer
import com.elleann.android.data.EmotionsResponse
import kotlinx.coroutines.launch

/**
 * HomeScreen — minimal post-pair surface.
 *
 *   Verifies that the stored JWT is actually honoured by the
 *   authenticated `/api/emotions` endpoint. Acts as the build-sanity
 *   check for the scaffold: if this screen shows a valence/arousal
 *   card after pairing, the end-to-end path (pair → JWT → Bearer
 *   plumbing → Dispatch auth gate → route) is green.
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun HomeScreen(
    container: AppContainer,
    onUnpair:  () -> Unit,
) {
    val scope = rememberCoroutineScope()
    val stored = remember { container.tokenStore.load() }

    var loading by remember { mutableStateOf(true) }
    var state by remember { mutableStateOf<EmotionsResponse?>(null) }
    var error by remember { mutableStateOf<String?>(null) }

    LaunchedEffect(Unit) {
        val api = container.pairedApi()
        if (api == null) {
            error = "No paired server"
            loading = false
            return@LaunchedEffect
        }
        scope.launch {
            val r = runCatching { api.emotions() }
            loading = false
            r.onSuccess { state = it }
             .onFailure { error = it.message ?: "Call failed" }
        }
    }

    Scaffold(
        topBar = { TopAppBar(title = { Text("Elle-Ann") }) }
    ) { inner: PaddingValues ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(inner)
                .padding(24.dp),
            verticalArrangement   = Arrangement.spacedBy(16.dp),
            horizontalAlignment   = Alignment.Start
        ) {
            Text(
                "Paired with ${stored?.host}:${stored?.port}",
                style = MaterialTheme.typography.titleMedium
            )
            Text(
                "Device id: ${stored?.deviceId?.take(8) ?: "?"}…",
                style = MaterialTheme.typography.bodySmall,
                color = MaterialTheme.colorScheme.onSurfaceVariant
            )

            Card(
                modifier = Modifier.fillMaxWidth(),
                colors = CardDefaults.cardColors()
            ) {
                Column(Modifier.padding(16.dp)) {
                    when {
                        loading  -> Text("Querying /api/emotions…")
                        error != null -> Text(
                            "error: $error",
                            color = MaterialTheme.colorScheme.error
                        )
                        else -> {
                            val s = state
                            if (s == null) Text("No state")
                            else {
                                Text("Mood:      ${s.mood ?: "—"}")
                                Text("Valence:   ${s.valence ?: "—"}")
                                Text("Arousal:   ${s.arousal ?: "—"}")
                                Text("Dominance: ${s.dominance ?: "—"}")
                            }
                        }
                    }
                }
            }

            OutlinedButton(
                onClick = onUnpair,
                modifier = Modifier.fillMaxWidth()
            ) {
                Text("Unpair")
            }
        }
    }
}
