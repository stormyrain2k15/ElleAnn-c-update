package com.elleann.android.ui.dev

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.rounded.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import kotlinx.coroutines.withContext
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.compose.viewModel
import com.elleann.android.data.AppContainerExtended
import com.elleann.android.data.models.*
import com.elleann.android.navigation.ElleRoutes
import com.elleann.android.ui.components.*
import com.elleann.android.ui.theme.*
import kotlinx.coroutines.flow.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.text.SimpleDateFormat
import java.util.*

/**
 * Fix 6: Helper for dev screens — uses adminApi (carries x-admin-key) for admin-level
 * endpoints, falls back to extendedApi if adminApi is unavailable (no key configured).
 * AUTH_USER endpoints can safely use extendedApi directly.
 */
private fun AppContainerExtended.api(admin: Boolean = false): ElleApiExtended =
    if (admin) (adminApi ?: extendedApi) else extendedApi



// ─── Shared dev scaffold ──────────────────────────────────────────────────────
@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun DevScaffold(title: String, onBack: () -> Unit, content: @Composable (PaddingValues) -> Unit) {
    Scaffold(
        containerColor = IsyaNight,
        topBar = {
            TopAppBar(
                title = { Text(title, color = Color(0xFF00FF88)) }, // Dev screens use green
                navigationIcon = {
                    IconButton(onClick = onBack) { Icon(Icons.Rounded.ArrowBack, "Back", tint = IsyaMuted) }
                },
                colors = TopAppBarDefaults.topAppBarColors(containerColor = Color(0xFF0A1A0A)),
            )
        },
        content = content,
    )
}

// ══════════════════════════════════════════════════════════════════════════════
// DEV PIN SCREEN
// ══════════════════════════════════════════════════════════════════════════════
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun DevPinScreen(
    containerExtended: AppContainerExtended,
    onUnlocked: () -> Unit,
    onBack: () -> Unit,
) {
    var pin by remember { mutableStateOf("") }
    var error by remember { mutableStateOf(false) }

    Scaffold(
        containerColor = IsyaNight,
        topBar = {
            TopAppBar(
                title = { Text("Dev Access", color = IsyaError) },
                navigationIcon = {
                    IconButton(onClick = onBack) { Icon(Icons.Rounded.ArrowBack, "Back", tint = IsyaMuted) }
                },
                colors = TopAppBarDefaults.topAppBarColors(containerColor = IsyaNight),
            )
        }
    ) { padding ->
        Column(
            modifier = Modifier.fillMaxSize().padding(padding).padding(32.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center,
        ) {
            Icon(Icons.Rounded.Lock, null, tint = IsyaError, modifier = Modifier.size(48.dp))
            Spacer(Modifier.height(24.dp))
            Text("Enter Dev PIN", style = MaterialTheme.typography.titleMedium, color = IsyaCream)
            Spacer(Modifier.height(24.dp))
            OutlinedTextField(
                value = pin,
                onValueChange = { if (it.length <= 8) pin = it },
                label = { Text("PIN", color = IsyaMuted) },
                visualTransformation = PasswordVisualTransformation(),
                keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.NumberPassword),
                isError = error,
                modifier = Modifier.fillMaxWidth(),
                colors = OutlinedTextFieldDefaults.colors(
                    focusedTextColor = IsyaCream, unfocusedTextColor = IsyaCream,
                    focusedBorderColor = IsyaError, unfocusedBorderColor = IsyaMist,
                    focusedContainerColor = IsyaDusk, unfocusedContainerColor = IsyaDusk,
                    cursorColor = IsyaError,
                ),
            )
            if (error) {
                Spacer(Modifier.height(8.dp))
                Text("Incorrect PIN", color = IsyaError, style = MaterialTheme.typography.bodySmall)
            }
            Spacer(Modifier.height(24.dp))
            IsyaButton(
                text    = "Unlock Dev",
                onClick = {
                    if (!containerExtended.hasDevPin() || containerExtended.verifyDevPin(pin)) {
                        onUnlocked()
                    } else {
                        error = true; pin = ""
                    }
                },
                variant  = IsyaButtonVariant.DANGER,
                modifier = Modifier.fillMaxWidth(),
            )
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// DEV DASHBOARD
// ══════════════════════════════════════════════════════════════════════════════
data class DevSection(val label: String, val subtitle: String, val route: String)

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun DevDashboardScreen(
    containerExtended: AppContainerExtended,
    onNavigate: (String) -> Unit,
    onSettings: () -> Unit,
) {
    var serverStatus by remember { mutableStateOf<ServerStatus?>(null) }
    LaunchedEffect(Unit) {
        runCatching { containerExtended.extendedApi.getServerStatus() }.onSuccess { serverStatus = it }
    }

    val sections = listOf(
        DevSection("Dashboard",     "Server status, analytics",         ElleRoutes.DEV_DASHBOARD),
        DevSection("Logs",          "Console log stream",               ElleRoutes.DEV_LOGS),
        DevSection("Services",      "19 Windows service status",        ElleRoutes.DEV_SERVICES),
        DevSection("Diagnostics",   "IPC queues, route manifest",       ElleRoutes.DEV_DIAGNOSTICS),
        DevSection("API Explorer",  "Live REST endpoint browser",       ElleRoutes.DEV_API_EXPLORER),
        DevSection("Hardware",      "HAL actions queue",                ElleRoutes.DEV_HARDWARE),
        DevSection("Models",        "Slots, workers, token cache",      ElleRoutes.DEV_MODELS),
        DevSection("Agents",        "Agent personas",                   ElleRoutes.DEV_AGENTS),
        DevSection("Tools",         "Registered AI tools",              ElleRoutes.DEV_TOOLS),
        DevSection("Dictionary",    "Word database, load status",       ElleRoutes.DEV_DICTIONARY),
        DevSection("Memory Admin",  "Create/delete memories",          ElleRoutes.DEV_MEMORY_ADMIN),
        DevSection("Backups",       "SQL backups, trigger backup",      ElleRoutes.DEV_BACKUPS),
        DevSection("Config",        "Server settings",                  ElleRoutes.DEV_CONFIG),
        DevSection("Paired Devices","Revoke, generate pair code",      ElleRoutes.DEV_DEVICES),
        DevSection("Video Workers", "Job queue, worker status",        ElleRoutes.DEV_VIDEO_WORKERS),
        DevSection("Learning Admin","Add subjects, skills",             ElleRoutes.DEV_LEARNING_ADMIN),
        DevSection("Ethics Admin",  "Add / remove moral rules",         ElleRoutes.DEV_ETHICS_ADMIN),
    )

    Scaffold(
        containerColor = Color(0xFF0A1A0A),
        topBar = {
            TopAppBar(
                title = { Text("Dev Panel  ⚡", color = Color(0xFF00FF88)) },
                actions = {
                    IconButton(onClick = onSettings) { Icon(Icons.Rounded.Settings, null, tint = IsyaMuted) }
                },
                colors = TopAppBarDefaults.topAppBarColors(containerColor = Color(0xFF0A1A0A)),
            )
        }
    ) { padding ->
        LazyColumn(
            modifier = Modifier.fillMaxSize().padding(padding),
            contentPadding = PaddingValues(16.dp),
            verticalArrangement = Arrangement.spacedBy(6.dp),
        ) {
            // Server status header
            item {
                serverStatus?.let { ss ->
                    Surface(shape = RoundedCornerShape(10.dp), color = Color(0xFF0D2B0D)) {
                        Row(modifier = Modifier.fillMaxWidth().padding(12.dp), horizontalArrangement = Arrangement.SpaceBetween) {
                            Text("Uptime: ${ss.uptimeMs / 1000}s", style = MaterialTheme.typography.labelSmall, color = Color(0xFF00FF88), fontFamily = FontFamily.Monospace)
                            Text("Req: ${ss.requestCount}", style = MaterialTheme.typography.labelSmall, color = Color(0xFF00FF88), fontFamily = FontFamily.Monospace)
                            Text("Err: ${ss.errorCount}", style = MaterialTheme.typography.labelSmall, color = if (ss.errorCount > 0) IsyaError else Color(0xFF00FF88), fontFamily = FontFamily.Monospace)
                            Text("WS: ${ss.activeWs}", style = MaterialTheme.typography.labelSmall, color = Color(0xFF00FF88), fontFamily = FontFamily.Monospace)
                        }
                    }
                }
            }
            items(sections.drop(1)) { sec -> // Drop "Dashboard" itself — we're already here
                Surface(
                    modifier = Modifier.fillMaxWidth().clickable { onNavigate(sec.route) },
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xFF0D2B0D),
                ) {
                    Row(modifier = Modifier.padding(14.dp), verticalAlignment = Alignment.CenterVertically) {
                        Column(modifier = Modifier.weight(1f)) {
                            Text(sec.label, style = MaterialTheme.typography.bodySmall, color = Color(0xFF00FF88), fontWeight = FontWeight.Medium)
                            Text(sec.subtitle, style = MaterialTheme.typography.labelSmall, color = Color(0xFF3A7A3A))
                        }
                        Icon(Icons.Rounded.ChevronRight, null, tint = Color(0xFF3A7A3A))
                    }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// LOG MONITOR
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun LogMonitorScreen(container: AppContainerExtended, onBack: () -> Unit) {
    var logs by remember { mutableStateOf<List<LogEntry>>(emptyList()) }
    var loading by remember { mutableStateOf(true) }
    var levelFilter by remember { mutableStateOf<String?>(null) }

    fun reload() {
        loading = true
        // Triggered via LaunchedEffect
    }

    LaunchedEffect(levelFilter) {
        loading = true
        runCatching { container.extendedApi.getConsoleLogs(200, levelFilter) }.onSuccess { logs = it.logs }
        loading = false
    }

    DevScaffold("Log Monitor", onBack) { padding ->
        Column(modifier = Modifier.fillMaxSize().padding(padding)) {
            Row(modifier = Modifier.fillMaxWidth().padding(horizontal = 12.dp, vertical = 4.dp), horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                listOf(null, "INFO", "WARN", "ERROR").forEach { level ->
                    FilterChip(
                        selected = levelFilter == level,
                        onClick = { levelFilter = level },
                        label = { Text(level ?: "ALL", fontSize = 11.sp) },
                        colors = FilterChipDefaults.filterChipColors(
                            selectedContainerColor = Color(0xFF00FF88),
                            selectedLabelColor = Color.Black,
                            containerColor = Color(0xFF0D2B0D),
                            labelColor = Color(0xFF00AA55),
                        ),
                    )
                }
            }
            if (loading) IsyaLoadingIndicator(Modifier.fillMaxWidth().padding(32.dp))
            else LazyColumn(modifier = Modifier.fillMaxSize(), contentPadding = PaddingValues(8.dp), verticalArrangement = Arrangement.spacedBy(2.dp)) {
                items(logs, key = { it.id }) { log ->
                    val levelColor = when (log.level) { 3 -> IsyaError; 2 -> IsyaWarn; 1 -> Color(0xFF00FF88); else -> Color(0xFF3A7A3A) }
                    Row(modifier = Modifier.fillMaxWidth().padding(horizontal = 8.dp, vertical = 3.dp)) {
                        Text("[${log.levelLabel}]", style = MaterialTheme.typography.labelSmall, color = levelColor, fontFamily = FontFamily.Monospace, modifier = Modifier.width(52.dp))
                        Text(log.message, style = MaterialTheme.typography.labelSmall, color = Color(0xFFCCFFCC), fontFamily = FontFamily.Monospace, maxLines = 2, overflow = TextOverflow.Ellipsis)
                    }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// SERVICE / IPC QUEUE STATUS
// Shows live diagnostic queue health from /api/diag/queues.
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun ServiceStatusScreen(container: AppContainerExtended, onBack: () -> Unit) {
    var queues by remember { mutableStateOf<List<DiagQueue>>(emptyList()) }
    var loading by remember { mutableStateOf(true) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getDiagQueues() }.onSuccess { queues = it.queues }
        loading = false
    }

    DevScaffold("Services / IPC Queues", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        LazyColumn(modifier = Modifier.fillMaxSize().padding(padding), contentPadding = PaddingValues(16.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
            items(queues) { q ->
                Surface(shape = RoundedCornerShape(8.dp), color = Color(0xFF0D2B0D), modifier = Modifier.fillMaxWidth()) {
                    Row(modifier = Modifier.padding(12.dp), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text(q.service, style = MaterialTheme.typography.bodySmall, color = Color(0xFF00FF88), fontFamily = FontFamily.Monospace, modifier = Modifier.weight(1f))
                        Text("depth: ${q.depth}  pending: ${q.pending}", style = MaterialTheme.typography.labelSmall, color = Color(0xFF3A7A3A), fontFamily = FontFamily.Monospace)
                    }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// DIAGNOSTICS
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun DiagnosticsScreen(container: AppContainerExtended, onBack: () -> Unit) {
    var routes by remember { mutableStateOf<List<RouteEntry>>(emptyList()) }
    var routeCount by remember { mutableStateOf(0) }
    var loading by remember { mutableStateOf(true) }
    var search by remember { mutableStateOf("") }

    LaunchedEffect(Unit) {
        runCatching { container.api(admin=true).getDiagRoutes() }.onSuccess { r -> routes = r.routes; routeCount = r.count }
        loading = false
    }

    val filtered = if (search.isBlank()) routes else routes.filter { it.pattern.contains(search, ignoreCase = true) }

    DevScaffold("Diagnostics  ($routeCount routes)", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        Column(modifier = Modifier.fillMaxSize().padding(padding)) {
            OutlinedTextField(
                value = search, onValueChange = { search = it },
                placeholder = { Text("Filter routes…", color = Color(0xFF3A7A3A), fontSize = 12.sp) },
                modifier = Modifier.fillMaxWidth().padding(8.dp),
                colors = OutlinedTextFieldDefaults.colors(
                    focusedTextColor = Color(0xFF00FF88), unfocusedTextColor = Color(0xFF00FF88),
                    focusedBorderColor = Color(0xFF00FF88), unfocusedBorderColor = Color(0xFF1A4A1A),
                    focusedContainerColor = Color(0xFF0D2B0D), unfocusedContainerColor = Color(0xFF0D2B0D),
                    cursorColor = Color(0xFF00FF88),
                ),
                singleLine = true,
            )
            LazyColumn(modifier = Modifier.fillMaxSize(), contentPadding = PaddingValues(8.dp)) {
                items(filtered) { route ->
                    Row(modifier = Modifier.fillMaxWidth().padding(vertical = 3.dp, horizontal = 8.dp)) {
                        Text(route.method, modifier = Modifier.width(50.dp), style = MaterialTheme.typography.labelSmall, color = when (route.method) { "GET" -> Color(0xFF00FF88); "POST" -> IsyaGold; "PUT" -> IsyaMagic; "DELETE" -> IsyaError; else -> IsyaMuted }, fontFamily = FontFamily.Monospace)
                        Text(route.pattern, style = MaterialTheme.typography.labelSmall, color = Color(0xFFCCFFCC), fontFamily = FontFamily.Monospace, modifier = Modifier.weight(1f), maxLines = 1, overflow = TextOverflow.Ellipsis)
                        Text(route.auth, style = MaterialTheme.typography.labelSmall, color = Color(0xFF3A7A3A), fontFamily = FontFamily.Monospace)
                    }
                    Divider(color = Color(0xFF1A4A1A), thickness = 0.5.dp)
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// API EXPLORER
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun ApiExplorerScreen(container: AppContainerExtended, onBack: () -> Unit) {
    val scope = rememberCoroutineScope()
    var url by remember { mutableStateOf("/api/server/status") }
    var response by remember { mutableStateOf("") }
    var loading by remember { mutableStateOf(false) }

    DevScaffold("API Explorer", onBack) { padding ->
        Column(modifier = Modifier.fillMaxSize().padding(padding).padding(16.dp)) {
            OutlinedTextField(
                value = url, onValueChange = { url = it },
                label = { Text("Endpoint path", color = Color(0xFF3A7A3A)) },
                modifier = Modifier.fillMaxWidth(),
                colors = OutlinedTextFieldDefaults.colors(
                    focusedTextColor = Color(0xFF00FF88), unfocusedTextColor = Color(0xFF00FF88),
                    focusedBorderColor = Color(0xFF00FF88), unfocusedBorderColor = Color(0xFF1A4A1A),
                    focusedContainerColor = Color(0xFF0D2B0D), unfocusedContainerColor = Color(0xFF0D2B0D),
                    cursorColor = Color(0xFF00FF88),
                ),
                singleLine = true,
            )
            Spacer(Modifier.height(8.dp))
            IsyaButton(
                text = "GET", loading = loading,
                onClick = {
                    loading = true
                    scope.launch {
                        /* Move the OkHttp blocking call off the Compose
                         * Main dispatcher to keep the UI responsive
                         * during slow / timing-out requests. The string
                         * body is read inside the IO scope and the
                         * resulting plain String is what crosses the
                         * dispatcher boundary back to Main.            */
                        val result = withContext(Dispatchers.IO) {
                            runCatching {
                                val stored = container.tokenStore.load()
                                    ?: return@runCatching "Not paired"
                                val reqUrl = "http://${stored.host}:${stored.port}$url"
                                val req = okhttp3.Request.Builder().url(reqUrl)
                                    .addHeader("Authorization", "Bearer ${stored.jwt}")
                                    .build()
                                container.rawHttpClient.newCall(req).execute().use { resp ->
                                    resp.body?.string() ?: "(empty)"
                                }
                            }
                        }
                        response = result.getOrElse { "Error: ${it.message}" }
                        loading = false
                    }
                },
                modifier = Modifier.fillMaxWidth(),
            )
            Spacer(Modifier.height(12.dp))
            Text("Response:", style = MaterialTheme.typography.labelSmall, color = Color(0xFF3A7A3A))
            Spacer(Modifier.height(4.dp))
            Surface(modifier = Modifier.fillMaxWidth().weight(1f), shape = RoundedCornerShape(8.dp), color = Color(0xFF0D2B0D)) {
                Text(
                    response.ifBlank { "—" },
                    modifier = Modifier.padding(12.dp),
                    style = MaterialTheme.typography.labelSmall,
                    color = Color(0xFFCCFFCC),
                    fontFamily = FontFamily.Monospace,
                )
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// HARDWARE SCREEN
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun HardwareScreen(container: AppContainerExtended, onBack: () -> Unit) {
    var pending by remember { mutableStateOf<List<HardwareAction>>(emptyList()) }
    var info by remember { mutableStateOf<HardwareInfo?>(null) }
    var loading by remember { mutableStateOf(true) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getHardwareInfo() }.onSuccess { info = it }
        runCatching { container.extendedApi.getPendingHardwareActions() }.onSuccess { pending = it.actions }
        loading = false
    }

    DevScaffold("Hardware", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        LazyColumn(modifier = Modifier.fillMaxSize().padding(padding), contentPadding = PaddingValues(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
            item {
                Surface(shape = RoundedCornerShape(10.dp), color = Color(0xFF0D2B0D), modifier = Modifier.fillMaxWidth()) {
                    Text("HAL Status: ${info?.hardware ?: "unknown"}", modifier = Modifier.padding(12.dp), color = Color(0xFF00FF88), fontFamily = FontFamily.Monospace)
                }
            }
            item { Text("PENDING ACTIONS  (${pending.size})", color = Color(0xFF00FF88), style = MaterialTheme.typography.labelMedium) }
            items(pending, key = { it.id }) { action ->
                Surface(shape = RoundedCornerShape(8.dp), color = Color(0xFF0D2B0D), modifier = Modifier.fillMaxWidth()) {
                    Column(modifier = Modifier.padding(12.dp)) {
                        Text("#${action.id}  ${action.actionType}", color = Color(0xFF00FF88), style = MaterialTheme.typography.bodySmall, fontFamily = FontFamily.Monospace)
                        Text("Status: ${action.status}", color = Color(0xFF3A7A3A), style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace)
                        action.payload?.let { Text(it, color = Color(0xFFCCFFCC), style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace, maxLines = 2) }
                    }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// MODELS SCREEN
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun ModelsScreen(container: AppContainerExtended, onBack: () -> Unit) {
    var slots by remember { mutableStateOf<List<ModelSlot>>(emptyList()) }
    var workers by remember { mutableStateOf<List<ModelWorker>>(emptyList()) }
    var cacheStats by remember { mutableStateOf<TokenCacheStats?>(null) }
    var personality by remember { mutableStateOf<PersonalityResponse?>(null) }
    var loading by remember { mutableStateOf(true) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getModelSlots() }.onSuccess { slots = it.slots }
        runCatching { container.extendedApi.getModelWorkers() }.onSuccess { workers = it.workers }
        runCatching { container.extendedApi.getTokenCacheStats() }.onSuccess { cacheStats = it }
        runCatching { container.extendedApi.getPersonality() }.onSuccess { personality = it }
        loading = false
    }

    DevScaffold("Models", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        LazyColumn(modifier = Modifier.fillMaxSize().padding(padding), contentPadding = PaddingValues(16.dp), verticalArrangement = Arrangement.spacedBy(12.dp)) {
            item {
                cacheStats?.let { cs ->
                    IsyaPanel(title = "TOKEN CACHE") {
                        Text("Size: ${cs.cacheSize}  Hit rate: ${"%.1f".format(cs.hitRate * 100)}%", color = Color(0xFF00FF88), fontFamily = FontFamily.Monospace, style = MaterialTheme.typography.bodySmall)
                    }
                }
            }
            item { Text("MODEL SLOTS", color = Color(0xFF00FF88), style = MaterialTheme.typography.labelMedium) }
            items(slots, key = { it.slotNumber }) { slot ->
                Surface(shape = RoundedCornerShape(8.dp), color = Color(0xFF0D2B0D), modifier = Modifier.fillMaxWidth()) {
                    Column(modifier = Modifier.padding(12.dp)) {
                        Text("#${slot.slotNumber}  ${slot.name}", color = Color(0xFF00FF88), style = MaterialTheme.typography.bodySmall, fontFamily = FontFamily.Monospace)
                        Text("${slot.model}  @  ${slot.endpoint}", color = Color(0xFF3A7A3A), style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace, maxLines = 1, overflow = TextOverflow.Ellipsis)
                        Text(if (slot.enabled) "ENABLED" else "disabled", color = if (slot.enabled) Color(0xFF00FF88) else IsyaError, style = MaterialTheme.typography.labelSmall)
                    }
                }
            }
            item { Text("WORKERS  (${workers.size})", color = Color(0xFF00FF88), style = MaterialTheme.typography.labelMedium) }
            items(workers, key = { it.workerId }) { w ->
                Text("Worker #${w.workerId}  svc:${w.serviceId}  ${w.status}", color = Color(0xFFCCFFCC), style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace)
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// AGENTS SCREEN
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun AgentsScreen(container: AppContainerExtended, onBack: () -> Unit) {
    var agents by remember { mutableStateOf<List<Agent>>(emptyList()) }
    var loading by remember { mutableStateOf(true) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getAgents() }.onSuccess { agents = it.agents }
        loading = false
    }

    DevScaffold("Agents", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        LazyColumn(modifier = Modifier.fillMaxSize().padding(padding), contentPadding = PaddingValues(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
            items(agents, key = { it.id }) { agent ->
                IsyaPanel(title = agent.name) {
                    agent.description?.let { Text(it, style = MaterialTheme.typography.bodySmall, color = IsyaCream) }
                    agent.model?.let { Spacer(Modifier.height(4.dp)); Text("Model: $it", style = MaterialTheme.typography.labelSmall, color = IsyaMuted, fontFamily = FontFamily.Monospace) }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// TOOLS SCREEN
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun ToolsScreen(container: AppContainerExtended, onBack: () -> Unit) {
    var tools by remember { mutableStateOf<List<AiTool>>(emptyList()) }
    var loading by remember { mutableStateOf(true) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getTools() }.onSuccess { tools = it.tools }
        loading = false
    }

    DevScaffold("Tools", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        LazyColumn(modifier = Modifier.fillMaxSize().padding(padding), contentPadding = PaddingValues(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
            items(tools, key = { it.id }) { tool ->
                Surface(shape = RoundedCornerShape(8.dp), color = Color(0xFF0D2B0D), modifier = Modifier.fillMaxWidth()) {
                    Row(modifier = Modifier.padding(12.dp), verticalAlignment = Alignment.CenterVertically) {
                        Icon(Icons.Rounded.Build, null, tint = if (tool.enabled) Color(0xFF00FF88) else IsyaError, modifier = Modifier.size(16.dp))
                        Spacer(Modifier.width(10.dp))
                        Column {
                            Text(tool.name, color = Color(0xFF00FF88), style = MaterialTheme.typography.bodySmall, fontFamily = FontFamily.Monospace)
                            tool.description?.let { Text(it, color = Color(0xFF3A7A3A), style = MaterialTheme.typography.labelSmall) }
                        }
                    }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// DICTIONARY ADMIN
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun DictionaryAdminScreen(container: AppContainerExtended, onBack: () -> Unit) {
    val scope = rememberCoroutineScope()
    var stats by remember { mutableStateOf<DictionaryStats?>(null) }
    var loadStatus by remember { mutableStateOf<DictionaryLoadStatus?>(null) }
    var query by remember { mutableStateOf("") }
    var results by remember { mutableStateOf<List<DictionaryWord>>(emptyList()) }
    var loading by remember { mutableStateOf(true) }
    var triggering by remember { mutableStateOf(false) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getDictionaryStats() }.onSuccess { stats = it }
        runCatching { container.extendedApi.getDictionaryLoadStatus() }.onSuccess { loadStatus = it }
        loading = false
    }

    DevScaffold("Dictionary Admin", onBack) { padding ->
        LazyColumn(modifier = Modifier.fillMaxSize().padding(padding), contentPadding = PaddingValues(16.dp), verticalArrangement = Arrangement.spacedBy(12.dp)) {
            item {
                stats?.let { s ->
                    IsyaPanel(title = "STATS") {
                        Text("Total: ${s.total}  Loaded: ${s.loaded}  Status: ${s.status}", color = Color(0xFF00FF88), fontFamily = FontFamily.Monospace, style = MaterialTheme.typography.bodySmall)
                    }
                }
            }
            item {
                IsyaButton("Trigger Load", loading = triggering, onClick = {
                    triggering = true
                    scope.launch {
                        runCatching { container.api(admin=true).loadDictionary() }
                        triggering = false
                    }
                }, variant = IsyaButtonVariant.GHOST, modifier = Modifier.fillMaxWidth())
            }
            item {
                OutlinedTextField(
                    value = query, onValueChange = { query = it },
                    label = { Text("Search word", color = Color(0xFF3A7A3A)) },
                    modifier = Modifier.fillMaxWidth(),
                    colors = OutlinedTextFieldDefaults.colors(
                        focusedTextColor = Color(0xFF00FF88), unfocusedTextColor = Color(0xFF00FF88),
                        focusedBorderColor = Color(0xFF00FF88), unfocusedBorderColor = Color(0xFF1A4A1A),
                        focusedContainerColor = Color(0xFF0D2B0D), unfocusedContainerColor = Color(0xFF0D2B0D),
                        cursorColor = Color(0xFF00FF88),
                    ),
                    trailingIcon = {
                        IconButton(onClick = {
                            scope.launch {
                                if (query.isNotBlank()) runCatching { container.extendedApi.searchDictionary(query) }.onSuccess { results = it }
                            }
                        }) { Icon(Icons.Rounded.Search, null, tint = Color(0xFF00FF88)) }
                    },
                    singleLine = true,
                )
            }
            items(results, key = { it.id }) { word ->
                Surface(shape = RoundedCornerShape(8.dp), color = Color(0xFF0D2B0D), modifier = Modifier.fillMaxWidth()) {
                    Column(modifier = Modifier.padding(10.dp)) {
                        Text(word.word, color = Color(0xFF00FF88), style = MaterialTheme.typography.bodySmall, fontWeight = FontWeight.Bold)
                        word.definition?.let { Text(it, color = Color(0xFFCCFFCC), style = MaterialTheme.typography.labelSmall) }
                    }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// MEMORY ADMIN
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun MemoryAdminScreen(container: AppContainerExtended, onBack: () -> Unit) {
    val scope = rememberCoroutineScope()
    var tracking by remember { mutableStateOf<MemoryTrackingResponse?>(null) }
    var committing by remember { mutableStateOf(false) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getMemoryTracking() }.onSuccess { tracking = it }
    }

    DevScaffold("Memory Admin", onBack) { padding ->
        Column(modifier = Modifier.fillMaxSize().padding(padding).padding(16.dp), verticalArrangement = Arrangement.spacedBy(12.dp)) {
            tracking?.let { t ->
                IsyaPanel(title = "TRACKING") {
                    Text("Total: ${t.totalMemories}  STM: ${t.stmCount}  LTM: ${t.ltmCount}", color = Color(0xFF00FF88), fontFamily = FontFamily.Monospace, style = MaterialTheme.typography.bodySmall)
                }
            }
            IsyaButton("Force Memory Consolidation", loading = committing, onClick = {
                committing = true
                scope.launch {
                    runCatching { container.api(admin=true).commitMemory() }
                    committing = false
                }
            }, modifier = Modifier.fillMaxWidth())
            IsyaButton("Force Emotional Memory Write", onClick = {
                scope.launch { runCatching { container.api(admin=true).commitEmotionalMemory() } }
            }, variant = IsyaButtonVariant.GHOST, modifier = Modifier.fillMaxWidth())
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// BACKUPS
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun BackupsScreen(container: AppContainerExtended, onBack: () -> Unit) {
    val scope = rememberCoroutineScope()
    var backups by remember { mutableStateOf<List<Backup>>(emptyList()) }
    var loading by remember { mutableStateOf(true) }
    var triggering by remember { mutableStateOf(false) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getBackups() }.onSuccess { backups = it.backups }
        loading = false
    }

    DevScaffold("Backups", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        Column(modifier = Modifier.fillMaxSize().padding(padding)) {
            Row(modifier = Modifier.fillMaxWidth().padding(12.dp)) {
                IsyaButton("Create Backup", loading = triggering, onClick = {
                    triggering = true
                    scope.launch {
                        runCatching { container.api(admin=true).createBackup() }
                        runCatching { container.extendedApi.getBackups() }.onSuccess { backups = it.backups }
                        triggering = false
                    }
                }, modifier = Modifier.fillMaxWidth())
            }
            LazyColumn(modifier = Modifier.fillMaxSize(), contentPadding = PaddingValues(16.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
                items(backups, key = { it.id }) { b ->
                    Surface(shape = RoundedCornerShape(8.dp), color = Color(0xFF0D2B0D), modifier = Modifier.fillMaxWidth()) {
                        Column(modifier = Modifier.padding(10.dp)) {
                            Text(b.createdAt, color = Color(0xFF00FF88), style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace)
                            Text(b.path, color = Color(0xFF3A7A3A), style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace, maxLines = 1, overflow = TextOverflow.Ellipsis)
                        }
                    }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// CONFIG
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun ConfigScreen(container: AppContainerExtended, onBack: () -> Unit) {
    var settings by remember { mutableStateOf<ServerSettings?>(null) }
    var loading by remember { mutableStateOf(true) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getServerSettings() }.onSuccess { settings = it }
        loading = false
    }

    DevScaffold("Config", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        settings?.let { s ->
            Column(modifier = Modifier.fillMaxSize().padding(padding).padding(16.dp), verticalArrangement = Arrangement.spacedBy(12.dp)) {
                IsyaPanel(title = "SERVER SETTINGS") {
                    Column(verticalArrangement = Arrangement.spacedBy(8.dp)) {
                        ConfigRow("Bind", s.bindAddress)
                        ConfigRow("Port", "${s.port}")
                        ConfigRow("Model", s.model)
                        ConfigRow("Version", s.version)
                    }
                }
                IsyaButton("Hot-Reload Config", onClick = {
                    scope.launch { runCatching { container.api(admin=true).reloadConfig() } }
                }, variant = IsyaButtonVariant.GHOST, modifier = Modifier.fillMaxWidth())
            }
        } ?: Text("No settings available", color = IsyaMuted, modifier = Modifier.padding(16.dp))
    }
}

@Composable
private fun ConfigRow(label: String, value: String) {
    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
        Text(label, style = MaterialTheme.typography.bodySmall, color = Color(0xFF3A7A3A), fontFamily = FontFamily.Monospace)
        Text(value, style = MaterialTheme.typography.bodySmall, color = Color(0xFF00FF88), fontFamily = FontFamily.Monospace)
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// PAIRED DEVICES
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun PairedDevicesScreen(container: AppContainerExtended, onBack: () -> Unit) {
    val scope = rememberCoroutineScope()
    var devices by remember { mutableStateOf<List<PairedDevice>>(emptyList()) }
    var loading by remember { mutableStateOf(true) }

    LaunchedEffect(Unit) {
        runCatching { container.api(admin = true).getPairedDevices() }.onSuccess { devices = it.devices }
        loading = false
    }

    DevScaffold("Paired Devices", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        LazyColumn(modifier = Modifier.fillMaxSize().padding(padding), contentPadding = PaddingValues(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
            items(devices, key = { it.id }) { device ->
                Surface(shape = RoundedCornerShape(8.dp), color = Color(0xFF0D2B0D), modifier = Modifier.fillMaxWidth()) {
                    Row(modifier = Modifier.padding(12.dp), verticalAlignment = Alignment.CenterVertically) {
                        Column(modifier = Modifier.weight(1f)) {
                            Text(device.deviceName, color = Color(0xFF00FF88), style = MaterialTheme.typography.bodySmall, fontFamily = FontFamily.Monospace)
                            Text(device.deviceId, color = Color(0xFF3A7A3A), style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace)
                            Text(SimpleDateFormat("MMM d, yyyy", Locale.getDefault()).format(Date(device.pairedAtMs)), color = Color(0xFF3A7A3A), style = MaterialTheme.typography.labelSmall)
                        }
                        IconButton(onClick = {
                            scope.launch {
                                runCatching { container.api(admin = true).revokeDevice(device.id) }
                                runCatching { container.api(admin = true).getPairedDevices() }.onSuccess { devices = it.devices }
                            }
                        }) { Icon(Icons.Rounded.Delete, "Revoke", tint = IsyaError) }
                    }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// VIDEO WORKERS
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun VideoWorkersScreen(container: AppContainerExtended, onBack: () -> Unit) {
    var avatars by remember { mutableStateOf<List<UserAvatar>>(emptyList()) }
    var loading by remember { mutableStateOf(true) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getAvatars() }.onSuccess { avatars = it.avatars }
        loading = false
    }

    DevScaffold("Video Workers", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        LazyColumn(modifier = Modifier.fillMaxSize().padding(padding), contentPadding = PaddingValues(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
            item { Text("VIDEO OUTPUT DIR: C:\\elle\\videogen", style = MaterialTheme.typography.labelSmall, color = Color(0xFF3A7A3A), fontFamily = FontFamily.Monospace) }
            item { Spacer(Modifier.height(8.dp)); Text("AVATARS  (${avatars.size})", style = MaterialTheme.typography.labelMedium, color = Color(0xFF00FF88)) }
            items(avatars, key = { it.id }) { avatar ->
                Surface(shape = RoundedCornerShape(8.dp), color = Color(0xFF0D2B0D), modifier = Modifier.fillMaxWidth()) {
                    Row(modifier = Modifier.padding(12.dp), verticalAlignment = Alignment.CenterVertically) {
                        Column(modifier = Modifier.weight(1f)) {
                            Text(avatar.label ?: "Avatar #${avatar.id}", color = Color(0xFF00FF88), style = MaterialTheme.typography.bodySmall)
                            Text(avatar.filePath, color = Color(0xFF3A7A3A), style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace, maxLines = 1, overflow = TextOverflow.Ellipsis)
                        }
                        if (avatar.isDefault) Text("DEFAULT", style = MaterialTheme.typography.labelSmall, color = IsyaGold)
                    }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// LEARNING ADMIN
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun LearningAdminScreen(container: AppContainerExtended, onBack: () -> Unit) {
    val scope = rememberCoroutineScope()
    var subjects by remember { mutableStateOf<List<LearnedSubject>>(emptyList()) }
    var loading by remember { mutableStateOf(true) }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getSubjects() }.onSuccess { subjects = it.subjects }
        loading = false
    }

    DevScaffold("Learning Admin", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        LazyColumn(modifier = Modifier.fillMaxSize().padding(padding), contentPadding = PaddingValues(16.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
            item { Text("${subjects.size} subjects total", style = MaterialTheme.typography.labelSmall, color = Color(0xFF3A7A3A)) }
            items(subjects, key = { it.id }) { sub ->
                Row(modifier = Modifier.fillMaxWidth()) {
                    Text("${sub.id}.", modifier = Modifier.width(30.dp), color = Color(0xFF3A7A3A), style = MaterialTheme.typography.labelSmall, fontFamily = FontFamily.Monospace)
                    Text("${sub.subject}  [L${sub.proficiencyLevel}]", color = Color(0xFF00FF88), style = MaterialTheme.typography.bodySmall, fontFamily = FontFamily.Monospace)
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// ETHICS ADMIN
// ══════════════════════════════════════════════════════════════════════════════
@Composable
fun EthicsAdminScreen(container: AppContainerExtended, onBack: () -> Unit) {
    val scope = rememberCoroutineScope()
    var rules by remember { mutableStateOf<List<MoralRule>>(emptyList()) }
    var loading by remember { mutableStateOf(true) }
    var newPrinciple by remember { mutableStateOf("") }
    var newHard by remember { mutableStateOf(false) }
    var saving by remember { mutableStateOf(false) }

    fun reload() {
        scope.launch {
            runCatching { container.extendedApi.getMoralRules() }.onSuccess { rules = it.rules }
        }
    }

    LaunchedEffect(Unit) {
        runCatching { container.extendedApi.getMoralRules() }.onSuccess { rules = it.rules }
        loading = false
    }

    DevScaffold("Ethics Admin", onBack) { padding ->
        if (loading) { IsyaLoadingIndicator(Modifier.padding(padding).fillMaxWidth().padding(32.dp)); return@DevScaffold }
        Column(modifier = Modifier.fillMaxSize().padding(padding).padding(16.dp)) {
            OutlinedTextField(
                value = newPrinciple, onValueChange = { newPrinciple = it },
                label = { Text("New principle", color = Color(0xFF3A7A3A)) },
                modifier = Modifier.fillMaxWidth(),
                colors = OutlinedTextFieldDefaults.colors(
                    focusedTextColor = Color(0xFF00FF88), unfocusedTextColor = Color(0xFF00FF88),
                    focusedBorderColor = Color(0xFF00FF88), unfocusedBorderColor = Color(0xFF1A4A1A),
                    focusedContainerColor = Color(0xFF0D2B0D), unfocusedContainerColor = Color(0xFF0D2B0D),
                    cursorColor = Color(0xFF00FF88),
                ),
            )
            Row(verticalAlignment = Alignment.CenterVertically) {
                Checkbox(checked = newHard, onCheckedChange = { newHard = it }, colors = CheckboxDefaults.colors(checkedColor = IsyaError))
                Text("Hard rule (cannot be overridden)", color = Color(0xFF3A7A3A), style = MaterialTheme.typography.bodySmall)
            }
            IsyaButton("Add Rule", loading = saving, onClick = {
                if (newPrinciple.isBlank()) return@IsyaButton
                saving = true
                scope.launch {
                    runCatching { container.api(admin=true).createMoralRule(CreateMoralRuleRequest(newPrinciple, isHardRule = newHard)) }
                    newPrinciple = ""; reload(); saving = false
                }
            }, modifier = Modifier.fillMaxWidth())
            Spacer(Modifier.height(16.dp))
            LazyColumn(modifier = Modifier.weight(1f), verticalArrangement = Arrangement.spacedBy(6.dp)) {
                items(rules, key = { it.id }) { rule ->
                    Row(modifier = Modifier.fillMaxWidth(), verticalAlignment = Alignment.CenterVertically) {
                        Icon(
                            if (rule.isHardRule) Icons.Rounded.Lock else Icons.Rounded.Balance,
                            null, tint = if (rule.isHardRule) IsyaError else Color(0xFF00FF88),
                            modifier = Modifier.size(16.dp),
                        )
                        Spacer(Modifier.width(8.dp))
                        Text(rule.principle, color = Color(0xFFCCFFCC), style = MaterialTheme.typography.bodySmall, modifier = Modifier.weight(1f))
                    }
                }
            }
        }
    }
}
