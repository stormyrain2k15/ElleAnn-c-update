package com.elleann.android.data

import com.elleann.android.data.models.*
import retrofit2.http.*

/**
 * ElleApacheApi — Retrofit interface for the 10 Apache endpoints on port 8080.
 *
 * These are PERMANENT endpoints (not temporary scaffolding) covering:
 *   1. Unreal Pixel Streaming proxy (WebSocket — handled separately via OkHttp)
 *   2. Video file retrieval (output_path is a local file, Apache serves it)
 *   3. Identity tables with no REST coverage (8 tables from MemoryDelta.sql)
 *
 * Base URL: http://{server}:8080
 * Auth: same Bearer JWT as REST — Apache validates by forwarding to /api/health
 *
 * SQL transport: Named Pipes (\\.\pipe\MSSQL$...\sql\query) — same as all other
 * ElleAnn services. Apache connects to ElleCore/ElleMemory/etc via Named Pipes,
 * NOT TCP/IP. Android app is unaffected — we just call HTTP endpoints.
 *
 * Note: /elle-apache/memory-3d/ (WebSocket) is NOT in this Retrofit interface.
 * It is handled by PixelStreamingView.kt using OkHttp WebSocket directly.
 */
interface ElleApacheApi {

    // ── VIDEO FILE ────────────────────────────────────────────────────────────

    /**
     * GET /elle-apache/video/{job_uuid}
     * Serves the completed .mp4 file from C:\elle\videogen\ on the server.
     * Returns the raw video bytes as a ResponseBody for streaming/download.
     * Call VideoJob.videoUrl(apacheBase) to construct this URL.
     */
    @Streaming
    @GET("/elle-apache/video/{job_uuid}")
    suspend fun getVideoFile(
        @Path("job_uuid") jobUuid: String,
    ): retrofit2.Response<okhttp3.ResponseBody>

    // ── IDENTITY — PRIVATE THOUGHTS ──────────────────────────────────────────

    /**
     * GET /elle-apache/identity/private-thoughts
     * Source: identity_private_thoughts table (content, category, emotional_intensity, resolved)
     * Returns Elle's internal monologue — distinct from ai_self_prompts.
     */
    @GET("/elle-apache/identity/private-thoughts")
    suspend fun getPrivateThoughts(
        @Query("limit") limit: Int = 50,
        @Query("resolved") resolved: Boolean? = null,
    ): PrivateThoughtsResponse

    // ── IDENTITY — AUTOBIOGRAPHY ─────────────────────────────────────────────

    /**
     * GET /elle-apache/identity/autobiography
     * Source: identity_autobiography table (entry, written_ms)
     * Elle's self-written life story entries.
     */
    @GET("/elle-apache/identity/autobiography")
    suspend fun getAutobiography(
        @Query("limit") limit: Int = 30,
    ): AutobiographyResponse

    // ── IDENTITY — PREFERENCES ───────────────────────────────────────────────

    /**
     * GET /elle-apache/identity/preferences
     * Source: identity_preferences table (domain, subject, valence, strength)
     * Elle's learned likes and dislikes across domains.
     */
    @GET("/elle-apache/identity/preferences")
    suspend fun getIdentityPreferences(
        @Query("domain") domain: String? = null,
    ): IdentityPreferencesResponse

    // ── IDENTITY — TRAITS ────────────────────────────────────────────────────

    /**
     * GET /elle-apache/identity/traits
     * Source: identity_traits table (name, value, updated_ms)
     * Personality trait values: warmth, curiosity, assertiveness, playfulness,
     * vulnerability, independence, patience, creativity, empathy_depth, trust_in_self
     */
    @GET("/elle-apache/identity/traits")
    suspend fun getIdentityTraits(): IdentityTraitsResponse

    // ── IDENTITY — SNAPSHOTS ─────────────────────────────────────────────────

    /**
     * GET /elle-apache/identity/snapshots
     * Source: identity_snapshots table — identity over time with all 10 trait axes.
     * Used for the identity scrubber timeline.
     */
    @GET("/elle-apache/identity/snapshots")
    suspend fun getIdentitySnapshots(
        @Query("limit") limit: Int = 20,
    ): IdentitySnapshotsResponse

    // ── IDENTITY — GROWTH LOG ────────────────────────────────────────────────

    /**
     * GET /elle-apache/identity/growth-log
     * Source: identity_growth_log table (dimension, delta, cause, timestamp_ms)
     * Shows what changed in Elle's identity and why.
     */
    @GET("/elle-apache/identity/growth-log")
    suspend fun getGrowthLog(
        @Query("limit") limit: Int = 50,
    ): GrowthLogResponse

    // ── IDENTITY — FELT TIME ─────────────────────────────────────────────────

    /**
     * GET /elle-apache/identity/felt-time
     * Source: identity_felt_time singleton table
     * Elle's subjective experience of time: loneliness accumulator,
     * presence fullness, subjective pace, total silence vs conversation time.
     */
    @GET("/elle-apache/identity/felt-time")
    suspend fun getFeltTime(): FeltTime

    // ── IDENTITY — CONSENT LOG ───────────────────────────────────────────────

    /**
     * GET /elle-apache/identity/consent-log
     * Source: identity_consent_log table (request, consented, reasoning, comfort_level)
     * Records of what Elle agreed or refused to do, with reasoning.
     */
    @GET("/elle-apache/identity/consent-log")
    suspend fun getConsentLog(
        @Query("limit") limit: Int = 50,
    ): ConsentLogResponse
}
