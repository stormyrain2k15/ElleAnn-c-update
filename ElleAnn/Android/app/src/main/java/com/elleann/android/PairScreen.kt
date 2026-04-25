package com.elleann.android

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.compose.viewModel
import com.elleann.android.ui.components.*
import com.elleann.android.ui.theme.*
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

// ─── ViewModel ───────────────────────────────────────────────────────────────
data class PairState(
    val host: String    = "",
    val port: String    = "8000",
    val code: String    = "",
    val loading: Boolean = false,
    val error: String?  = null,
    val success: Boolean = false,
)

class PairViewModel(
    private val container: AppContainer,
    prefill: PairingPayload? = null,
) : ViewModel() {

    private val _state = MutableStateFlow(
        PairState(
            host = prefill?.host ?: "",
            port = prefill?.port?.toString() ?: "8000",
            code = prefill?.code ?: "",
        )
    )
    val state: StateFlow<PairState> = _state.asStateFlow()

    fun onHost(v: String)  = _state.update { it.copy(host = v, error = null) }
    fun onPort(v: String)  = _state.update { it.copy(port = v, error = null) }
    fun onCode(v: String)  = _state.update { it.copy(code = v, error = null) }

    fun pair(onSuccess: () -> Unit) {
        val s = _state.value
        val port = s.port.toIntOrNull() ?: run {
            _state.update { it.copy(error = "Invalid port number") }; return
        }
        if (s.host.isBlank()) {
            _state.update { it.copy(error = "Server host is required") }; return
        }
        if (s.code.length != 6 || !s.code.all { it.isDigit() }) {
            _state.update { it.copy(error = "Pair code must be 6 digits") }; return
        }

        viewModelScope.launch {
            _state.update { it.copy(loading = true, error = null) }
            runCatching {
                val api = container.apiFor(s.host, port)
                val resp = api.pair(mapOf("code" to s.code, "device_name" to "ElleAnn Android"))
                container.tokenStore.save(
                    StoredToken(
                        jwt       = resp.jwt,
                        host      = s.host,
                        port      = port,
                        expiresMs = resp.expiresMs,
                    )
                )
            }.onSuccess {
                _state.update { it.copy(loading = false, success = true) }
                onSuccess()
            }.onFailure { e ->
                _state.update { it.copy(loading = false, error = e.message ?: "Pair failed") }
            }
        }
    }
}

// ─── Screen ───────────────────────────────────────────────────────────────────
@Composable
fun PairScreen(
    container: AppContainer,
    prefill: PairingPayload? = null,
    onPaired: () -> Unit,
) {
    val vm: PairViewModel = viewModel(
        factory = remember {
            object : androidx.lifecycle.ViewModelProvider.Factory {
                @Suppress("UNCHECKED_CAST")
                override fun <T : ViewModel> create(modelClass: Class<T>): T =
                    PairViewModel(container, prefill) as T
            }
        }
    )
    val state by vm.state.collectAsState()

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(IsyaNight),
        contentAlignment = Alignment.Center,
    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth(0.9f)
                .verticalScroll(rememberScrollState()),
            horizontalAlignment  = Alignment.CenterHorizontally,
            verticalArrangement  = Arrangement.spacedBy(24.dp),
        ) {
            Spacer(Modifier.height(48.dp))

            // Logo / title
            Text(
                text       = "ElleAnn",
                style      = MaterialTheme.typography.displaySmall,
                color      = IsyaGold,
                fontWeight = FontWeight.Bold,
                textAlign  = TextAlign.Center,
            )
            Text(
                text      = "ESI v3 Companion",
                style     = MaterialTheme.typography.titleSmall,
                color     = IsyaMuted,
                textAlign = TextAlign.Center,
            )

            Spacer(Modifier.height(8.dp))

            // Pair form
            IsyaPanel(title = "CONNECT TO SERVER", flowingBorder = true) {
                Column(verticalArrangement = Arrangement.spacedBy(16.dp)) {
                    IsyaInputField(
                        value         = state.host,
                        onValueChange = vm::onHost,
                        label         = "Server host or IP",
                        placeholder   = "192.168.1.x",
                    )
                    IsyaInputField(
                        value         = state.port,
                        onValueChange = vm::onPort,
                        label         = "Port",
                        placeholder   = "8000",
                        keyboardOptions = androidx.compose.foundation.text.KeyboardOptions(
                            keyboardType = androidx.compose.ui.text.input.KeyboardType.Number
                        ),
                    )
                    IsyaInputField(
                        value         = state.code,
                        onValueChange = { if (it.length <= 6 && it.all { c -> c.isDigit() }) vm.onCode(it) },
                        label         = "6-digit pair code",
                        placeholder   = "123456",
                        keyboardOptions = androidx.compose.foundation.text.KeyboardOptions(
                            keyboardType = androidx.compose.ui.text.input.KeyboardType.NumberPassword
                        ),
                    )

                    state.error?.let { err ->
                        Text(err, color = IsyaError, style = MaterialTheme.typography.bodySmall)
                    }

                    IsyaButton(
                        text     = if (state.loading) "Connecting…" else "CONNECT",
                        onClick  = { vm.pair(onPaired) },
                        loading  = state.loading,
                        variant  = IsyaButtonVariant.PRIMARY_GOLD,
                        modifier = Modifier.fillMaxWidth(),
                    )
                }
            }

            Text(
                text  = "Scan ellepair:// QR code from Elle's device manager\nor enter server details manually above.",
                style = MaterialTheme.typography.bodySmall,
                color = IsyaMuted,
                textAlign = TextAlign.Center,
            )

            Spacer(Modifier.height(48.dp))
        }
    }
}
