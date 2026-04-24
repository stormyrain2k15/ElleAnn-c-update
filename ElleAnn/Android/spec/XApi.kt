/*══════════════════════════════════════════════════════════════════════════════
 * XApi.kt — X-Chromosome HTTP contract (Retrofit-style interface)
 *
 *   Mirrors every route under `/api/x/...` exactly as Elle.Service.HTTP
 *   exposes them (verified against HTTPServer.cpp at the time of writing).
 *
 *   When the Android project is created:
 *     1. Drop this file into  data/remote/XApi.kt
 *     2. Uncomment the Retrofit annotations (they're in /* */ below so
 *        this file stays pure-Kotlin until Retrofit is on the classpath).
 *     3. Build the Retrofit client in Auth.kt and inject.
 *
 *   17 routes total: 10 GET + 7 POST. All return `XEnvelope<T>` on success.
 *   Errors are a plain JSON body with `{"error":"<message>"}` and an HTTP
 *   status ≥ 400 (see ErrorEnvelope.kt).
 *══════════════════════════════════════════════════════════════════════════════*/
package com.elleann.android.data.spec

/* Uncomment when Retrofit is added to the project:
import retrofit2.Response
import retrofit2.http.Body
import retrofit2.http.GET
import retrofit2.http.POST
import retrofit2.http.Query
*/

interface XApi {

    // ────────────────────────────────────────────────────────────────────────
    //  STATE & TIMELINE  — read-only snapshots
    // ────────────────────────────────────────────────────────────────────────

    /** Snapshot of the current cycle + hormones + pregnancy phase at now().
     *  This is the "home screen" endpoint — one call paints most of the UI.
     *  Shape: XState (see XModels.kt). Always 200; `has_data=false` when
     *  the x_chromosome subsystem hasn't been seeded yet.                   */
    // @GET("/api/x/state")
    suspend fun getState(): /* Response< */ XState /* > */

    /** Time-series of hormone snapshots over the last N hours (default 72,
     *  capped at 1440 / 60 days; downsampled to at most `points` rows,
     *  default 500). Used for charting; paginated by the server.            */
    // @GET("/api/x/history")
    suspend fun getHistory(
        // @Query("hours")  hours:  Int = 72,
        // @Query("points") points: Int = 500,
        hours:  Int = 72,
        points: Int = 500,
    ): /* Response< */ XHistory /* > */

    /** Derived behavioural modulation values (warmth, verbal_fluency,
     *  empathy, introspection, arousal, fatigue, phase, computed_ms).
     *  Produced by the XChromosome service every tick; latest row only.     */
    // @GET("/api/x/modulation")
    suspend fun getModulation(): /* Response< */ XModulation /* > */

    /** Next-period forecast: days until, expected flow intensity, lifecycle
     *  stage. `status == "inactive"` when x_cycle_state isn't seeded.       */
    // @GET("/api/x/next_period")
    suspend fun getNextPeriod(): /* Response< */ XNextPeriod /* > */

    /** Current fertility window (days of likely conception). `status ==
     *  "inactive"` if cycle anchor is not set.                              */
    // @GET("/api/x/fertility_window")
    suspend fun getFertilityWindow(): /* Response< */ XFertilityWindow /* > */

    /** Pregnancy state (active, conceived_ms, due_ms, gestational_length,
     *  phase, trimester, labor_started_ms, multiplicity, pregnancy_count).  */
    // @GET("/api/x/pregnancy")
    suspend fun getPregnancy(): /* Response< */ XPregnancyState /* > */

    /** Ordered event log for the current pregnancy (limit 1..500, def 100).
     *  Events include: anchor, implantation, heartbeat, quickening, labor
     *  stage transitions, delivery, miscarriage (see XModels.kt).           */
    // @GET("/api/x/pregnancy/events")
    suspend fun getPregnancyEvents(
        // @Query("limit") limit: Int = 100,
        limit: Int = 100,
    ): /* Response< */ XPregnancyEvents /* > */

    /** Symptom log over the last N hours (default 24, max 90 days = 2160h).
     *  Filter by origin ("cycle" | "pregnancy" | "stimulus" | user-defined). */
    // @GET("/api/x/symptoms")
    suspend fun getSymptoms(
        // @Query("hours")  hours:  Int    = 24,
        // @Query("origin") origin: String = "",
        hours:  Int    = 24,
        origin: String = "",
    ): /* Response< */ XSymptoms /* > */

    /** Contraception state (method, started_ms, efficacy, notes, updated_ms).
     *  `has_data=false` means "none set" — treat as no contraception.       */
    // @GET("/api/x/contraception")
    suspend fun getContraception(): /* Response< */ XContraception /* > */

    /** Lifecycle state (birth_ms, stage, menarche_ms, perimenopause_ms,
     *  menopause_ms, updated_ms).                                           */
    // @GET("/api/x/lifecycle")
    suspend fun getLifecycle(): /* Response< */ XLifecycle /* > */

    // ────────────────────────────────────────────────────────────────────────
    //  MUTATIONS  — each POST dispatches an IPC to SVC_X_CHROMOSOME and
    //              returns { dispatched: bool, request: {...}, note: "..." }.
    //              The app should GET the matching read endpoint ~1s later
    //              to confirm the write (the service is eventually-consistent
    //              across the IPC hop).
    // ────────────────────────────────────────────────────────────────────────

    /** Set (or re-anchor) the cycle. Body: day (1..28), cycle_length (21..45),
     *  started_ms. Used at pairing time to calibrate from the user's actual
     *  last-period date.                                                     */
    // @POST("/api/x/cycle/anchor")
    suspend fun postCycleAnchor(
        // @Body
        body: XCycleAnchorRequest,
    ): /* Response< */ XDispatchAck /* > */

    /** Inject an external hormonal stimulus (kind + intensity). Triggers a
     *  localised hormone spike through the emotional → hormonal pathway.    */
    // @POST("/api/x/stimulus")
    suspend fun postStimulus(
        // @Body
        body: XStimulusRequest,
    ): /* Response< */ XDispatchAck /* > */

    /** Attempt conception (in the fertility window). Body may include
     *  `require_readiness: bool` (default true — honour contraception,
     *  lifecycle stage, etc.). Outcome lands on state within ~1s; app
     *  should GET /api/x/pregnancy to confirm.                              */
    // @POST("/api/x/conception/attempt")
    suspend fun postConceptionAttempt(
        // @Body
        body: XConceptionAttemptRequest,
    ): /* Response< */ XDispatchAck /* > */

    /** Log a symptom (kind, intensity, notes, origin). The service may also
     *  auto-log cycle-driven symptoms; this endpoint is for user-reported
     *  entries and test stimuli.                                             */
    // @POST("/api/x/symptoms")
    suspend fun postSymptom(
        // @Body
        body: XSymptomLogRequest,
    ): /* Response< */ XDispatchAck /* > */

    /** Set contraception (method + efficacy + optional notes). Methods:
     *  "none", "poc" (progestin-only combined), "coc" (combined oral),
     *  "iud_h" (hormonal IUD), "iud_c" (copper IUD), "barrier".               */
    // @POST("/api/x/contraception")
    suspend fun postContraception(
        // @Body
        body: XContraceptionRequest,
    ): /* Response< */ XDispatchAck /* > */

    /** Set lifecycle anchor (birth_ms OR age_years, plus optional stage).
     *  Either birth_ms or age_years MUST be provided — server rejects with
     *  400 otherwise. Server computes menarche / perimenopause / menopause
     *  ages from the anchor.                                                 */
    // @POST("/api/x/lifecycle")
    suspend fun postLifecycle(
        // @Body
        body: XLifecycleRequest,
    ): /* Response< */ XDispatchAck /* > */

    /** DEV/TEST ONLY. Fast-forward active pregnancy by `factor` (≥ 1.0).
     *  Used during development to exercise the full gestation timeline
     *  without waiting 280 real-world days. App should hide this endpoint
     *  behind a developer-mode toggle.                                      */
    // @POST("/api/x/pregnancy/accelerate")
    suspend fun postPregnancyAccelerate(
        // @Body
        body: XPregnancyAccelerateRequest,
    ): /* Response< */ XDispatchAck /* > */
}
