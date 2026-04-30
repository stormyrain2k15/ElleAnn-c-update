package com.elleann.android.ui.chat

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.rounded.CallEnd
import androidx.compose.material.icons.rounded.VideocamOff
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.compose.viewModel
import android.webkit.WebView
import android.webkit.WebViewClient
import com.elleann.android.data.AppContainerExtended
import com.elleann.android.data.models.VideoJob
import com.elleann.android.ui.components.IsyaLoadingIndicator
import com.elleann.android.ui.theme.*
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.*

// ─── State ───────────────────────────────────────────────────────────────────
data class VideoCallState(
    val callId: Long       = 0L,
    val jobId: String?     = null,
    val videoUrl: String?  = null,
    val status: String     = "Initializing…",
    val loading: Boolean   = true,
    val error: String?     = null,
)

// ─── ViewModel ───────────────────────────────────────────────────────────────
class VideoCallViewModel(
    private val callId: Long,
    private val container: AppContainerExtended,
    private val restBaseUrl: String,
) : ViewModel() {

    private val _state = MutableStateFlow(VideoCallState(callId = callId))
    val state: StateFlow<VideoCallState> = _state.asStateFlow()

    init { startCall() }

    private fun startCall() {
        viewModelScope.launch {
            // Queue video generation job for this call
            runCatching {
                container.extendedApi.generateVideo(
                    com.elleann.android.data.models.GenerateVideoRequest(
                        text   = "Hello. I'm here for our video call.",
                        callId = callId,
                    )
                )
            }.onSuccess { job ->
                _state.update { it.copy(jobId = job.jobId, status = "Generating video…") }
                pollVideoJob(job.jobId)
            }.onFailure { e ->
                _state.update { it.copy(error = e.message, loading = false) }
            }
        }
    }

    /** Poll video job status until done or failed */
    private fun pollVideoJob(jobId: String) {
        viewModelScope.launch {
            repeat(60) { // max 60 × 2s = 2 min timeout
                delay(2_000)
                runCatching { container.extendedApi.getVideoStatus(jobId) }
                    .onSuccess { job ->
                        when {
                            job.isDone -> {
                                val url = job.videoUrl(restBaseUrl)
                                _state.update { it.copy(videoUrl = url, status = "Connected", loading = false) }
                                return@launch
                            }
                            job.isFailed -> {
                                _state.update { it.copy(error = job.error ?: "Video generation failed", loading = false) }
                                return@launch
                            }
                            else -> _state.update { it.copy(status = "Generating… ${job.progress}%") }
                        }
                    }
            }
            // Timeout
            if (_state.value.videoUrl == null) {
                _state.update { it.copy(error = "Video generation timed out", loading = false) }
            }
        }
    }

    fun endCall(onEnded: () -> Unit) {
        viewModelScope.launch {
            runCatching { container.extendedApi.endVideoCall(callId) }
            onEnded()
        }
    }
}

// ─── Screen ──────────────────────────────────────────────────────────────────
@Composable
fun VideoCallScreen(
    callId: Long,
    containerExtended: AppContainerExtended,
    restBaseUrl: String,
    onBack: () -> Unit,
) {
    val vm: VideoCallViewModel = viewModel(
        key     = "video_$callId",
        factory = remember {
            object : androidx.lifecycle.ViewModelProvider.Factory {
                @Suppress("UNCHECKED_CAST")
                override fun <T : ViewModel> create(modelClass: Class<T>): T =
                    VideoCallViewModel(callId, containerExtended, restBaseUrl) as T
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
        when {
            state.loading -> Column(horizontalAlignment = Alignment.CenterHorizontally) {
                IsyaLoadingIndicator()
                Spacer(Modifier.height(16.dp))
                Text(state.status, color = IsyaMuted, style = MaterialTheme.typography.bodySmall)
            }

            state.error != null -> Column(horizontalAlignment = Alignment.CenterHorizontally) {
                Icon(Icons.Rounded.VideocamOff, null, tint = IsyaError, modifier = Modifier.size(48.dp))
                Spacer(Modifier.height(12.dp))
                Text(state.error!!, color = IsyaError)
                Spacer(Modifier.height(12.dp))
                Button(onClick = onBack) { Text("End call") }
            }

            state.videoUrl != null -> {
                // Video WebView — loads mp4 from /api/video/file/{job_id}
                Column(modifier = Modifier.fillMaxSize()) {
                    AndroidView(
                        modifier = Modifier.weight(1f).fillMaxWidth(),
                        factory  = { ctx ->
                            WebView(ctx).apply {
                                webViewClient = WebViewClient()
                                settings.apply {
                                    javaScriptEnabled = true
                                    mediaPlaybackRequiresUserGesture = false
                                }
                                loadUrl(state.videoUrl!!)
                            }
                        },
                        update   = { it.loadUrl(state.videoUrl!!) }
                    )
                    // Controls bar
                    Surface(color = IsyaHeader) {
                        Row(
                            modifier              = Modifier.fillMaxWidth().padding(16.dp),
                            horizontalArrangement = Arrangement.Center,
                        ) {
                            // End call
                            IconButton(
                                onClick  = { vm.endCall(onBack) },
                                modifier = Modifier
                                    .size(56.dp)
                                    .clip(CircleShape)
                                    .background(IsyaError),
                            ) {
                                Icon(Icons.Rounded.CallEnd, "End call", tint = IsyaCream)
                            }
                        }
                    }
                }
            }
        }
    }
}
