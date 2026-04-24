package com.elleann.android.data

import kotlinx.serialization.SerialName
import kotlinx.serialization.Serializable
import retrofit2.http.Body
import retrofit2.http.GET
import retrofit2.http.POST

/**
 * ElleApi — Retrofit interface mirroring the subset of routes the
 * companion app needs for the MVP scaffold:
 *
 *   - POST /api/auth/pair   — redeem a pairing code for a JWT
 *   - GET  /api/health      — liveness probe (unauthenticated)
 *   - GET  /api/emotions    — first authenticated call to verify the
 *                             Bearer plumbing works end-to-end
 *
 *   When the contract grows, the rest of Android/spec/XApi.kt etc.
 *   plug in here with no DI wiring changes.
 */
interface ElleApi {

    @POST("/api/auth/pair")
    suspend fun pair(@Body req: PairRequest): PairResponse

    @GET("/api/health")
    suspend fun health(): HealthResponse

    @GET("/api/emotions")
    suspend fun emotions(): EmotionsResponse
}

@Serializable
data class PairRequest(
    val code: String,
    @SerialName("device_name") val deviceName: String,
    @SerialName("device_id")   val deviceId: String,
)

@Serializable
data class PairResponse(
    val jwt: String,
    @SerialName("expires_ms")   val expiresMs: Long,
    @SerialName("paired_at_ms") val pairedAtMs: Long,
)

@Serializable
data class HealthResponse(
    val status: String,
    val name:   String,
    val version: String? = null,
)

@Serializable
data class EmotionsResponse(
    val valence:   Float? = null,
    val arousal:   Float? = null,
    val dominance: Float? = null,
    val mood:      String? = null,
)
