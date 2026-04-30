package com.elleann.android.data

import com.elleann.android.data.models.*
import retrofit2.http.*

/**
 * ElleApiExtended — Retrofit interface for all REST endpoints beyond the
 * locked base ElleApi (pair, health, emotions).
 *
 * Source: Elle.Service.HTTP/HTTPServer.cpp — 123 registered routes.
 * Auth: Bearer JWT attached by AuthInterceptorExtended for USER routes.
 *       x-admin-key header attached for ADMIN routes.
 *
 * NOTE: Admin routes require the admin key to be configured in Dev Settings.
 *       Calling admin routes without a valid key returns HTTP 403.
 */
interface ElleApiExtended {

    // ── SESSION ──────────────────────────────────────────────────────────────

    /**
     * GET /api/me — "who am I" identity reflection.
     *
     * Resolves the JWT-bound device → PairedDevices.nUserNo → game account
     * (Account.dbo.tUser) and returns the canonical identity tuple.  Use
     * this once at session start instead of trying to derive identity
     * from the JWT body or from local prefs.
     */
    @GET("/api/me")
    suspend fun getMe(): MeResponse

    /** GET /api/session/greeting — LLM-rendered welcome phrase */
    @GET("/api/session/greeting")
    suspend fun getSessionGreeting(): SessionGreeting

    /** POST /api/session/greeting/{id}/ack — acknowledge greeting on display */
    @POST("/api/session/greeting/{id}/ack")
    suspend fun ackSessionGreeting(@Path("id") id: Long): OkResponse

    // ── MEMORY ───────────────────────────────────────────────────────────────

    /** GET /api/memory/ — list all memories (filter by type client-side) */
    @GET("/api/memory/")
    suspend fun getMemories(): MemoryListResponse

    /** POST /api/memory/ — create a memory record (Dev Tools) */
    @POST("/api/memory/")
    suspend fun createMemory(@Body request: CreateMemoryRequest): Memory

    /** GET /api/memory/self-image/current — Elle's InternalNarrative snapshot */
    @GET("/api/memory/self-image/current")
    suspend fun getSelfImage(): SelfImageResponse

    /** GET /api/memory/{id} — single memory detail */
    @GET("/api/memory/{id}")
    suspend fun getMemory(@Path("id") id: Long): Memory

    /** PUT /api/memory/{id} — edit / annotate memory */
    @PUT("/api/memory/{id}")
    suspend fun updateMemory(@Path("id") id: Long, @Body memory: Memory): Memory

    /** DELETE /api/memory/{id} — delete (ADMIN) */
    @DELETE("/api/memory/{id}")
    suspend fun deleteMemory(@Path("id") id: Long): OkResponse

    /** POST /api/memory/{id}/files — attach a file to a memory */
    @POST("/api/memory/{id}/files")
    suspend fun attachFileToMemory(
        @Path("id") id: Long,
        @Body body: AttachFileRequest,
    ): OkResponse

    // ── EMOTIONS ─────────────────────────────────────────────────────────────

    // NOTE: GET /api/emotions is in the locked ElleApi interface

    /** GET /api/emotions/dimensions — all 102 emotion dimensions live */
    @GET("/api/emotions/dimensions")
    suspend fun getEmotionDimensions(): EmotionDimensionsResponse

    /** GET /api/emotions/dimensions/{name} — single dimension detail */
    @GET("/api/emotions/dimensions/{name}")
    suspend fun getEmotionDimension(@Path("name") name: String): EmotionDimension

    /** PUT /api/emotions/dimensions/{name} — manually set dimension (ADMIN) */
    @PUT("/api/emotions/dimensions/{name}")
    suspend fun setEmotionDimension(
        @Path("name") name: String,
        @Body body: SetEmotionDimensionRequest,
    ): OkResponse

    /** GET /api/emotions/weights — dimension weight configuration */
    @GET("/api/emotions/weights")
    suspend fun getEmotionWeights(): EmotionWeightsResponse

    // ── EMOTIONAL CONTEXT ────────────────────────────────────────────────────

    /** GET /api/emotional-context/history — V/A/D time-series (default 24h) */
    @GET("/api/emotional-context/history")
    suspend fun getEmotionHistory(
        @Query("hours") hours: Int = 24,
        @Query("points") points: Int = 500,
    ): EmotionHistoryResponse

    /** GET /api/emotional-context/dimensions — snapshot at historical time T */
    @GET("/api/emotional-context/dimensions")
    suspend fun getEmotionDimensionsAtTime(
        @Query("t") timestampMs: Long = 0L,
        @Query("top") topN: Int = 5,
    ): EmotionDimensionsResponse

    /** GET /api/emotional-context/patterns — unresolved emotional threads */
    @GET("/api/emotional-context/patterns")
    suspend fun getEmotionalPatterns(): List<EmotionalPattern>

    /** GET /api/emotional-context/vocabulary — memory tag word cloud */
    @GET("/api/emotional-context/vocabulary")
    suspend fun getEmotionalVocabulary(): List<VocabularyTerm>

    /** GET /api/emotional-context/growth — emotional growth over time */
    @GET("/api/emotional-context/growth")
    suspend fun getEmotionalGrowth(): EmotionGrowthResponse

    // ── CONVERSATIONS ────────────────────────────────────────────────────────

    /** GET /api/tokens/conversations — conversation list */
    @GET("/api/tokens/conversations")
    suspend fun getConversations(): ConversationListResponse

    /** POST /api/tokens/conversations — start new conversation */
    @POST("/api/tokens/conversations")
    suspend fun createConversation(@Body request: NewConversationRequest): Conversation

    /** GET /api/tokens/conversations/{id} — conversation header */
    @GET("/api/tokens/conversations/{id}")
    suspend fun getConversation(@Path("id") id: Long): Conversation

    /** POST /api/tokens/conversations/{id}/messages — send message (threaded) */
    @POST("/api/tokens/conversations/{id}/messages")
    suspend fun sendMessageThreaded(
        @Path("id") conversationId: Long,
        @Body request: SendMessageRequest,
    ): Message

    /** GET /api/tokens/conversations/{id}/messages — load message history */
    @GET("/api/tokens/conversations/{id}/messages")
    suspend fun getMessages(@Path("id") conversationId: Long): MessageListResponse

    /** POST /api/tokens/interactions — background interaction log */
    @POST("/api/tokens/interactions")
    suspend fun logInteraction(@Body request: InteractionRequest): OkResponse

    // ── VIDEO CALLS ──────────────────────────────────────────────────────────

    /** POST /api/tokens/video-calls — start video call session */
    @POST("/api/tokens/video-calls")
    suspend fun startVideoCall(@Body body: StartVideoCallRequest): VideoCallSession

    /** PUT /api/tokens/video-calls/{id}/end — end video call */
    @PUT("/api/tokens/video-calls/{id}/end")
    suspend fun endVideoCall(@Path("id") id: Long): OkResponse

    // ── AI CHAT ──────────────────────────────────────────────────────────────

    /** POST /api/ai/chat — primary chat with memory injection */
    @POST("/api/ai/chat")
    suspend fun chat(@Body request: ChatRequest): ChatResponse

    /** GET /api/ai/status — model info, emotional state, uptime */
    @GET("/api/ai/status")
    suspend fun getAiStatus(): AiStatus

    /** GET /api/ai/self-prompts — Elle's autonomous thought log */
    @GET("/api/ai/self-prompts")
    suspend fun getSelfPrompts(@Query("limit") limit: Int = 50): SelfPromptListResponse

    /** POST /api/ai/self-prompts/generate — trigger thought generation (ADMIN) */
    @POST("/api/ai/self-prompts/generate")
    suspend fun generateSelfPrompt(): SelfPrompt

    /** POST /api/ai/analyze-emotion — analyze emotion in a text snippet */
    @POST("/api/ai/analyze-emotion")
    suspend fun analyzeEmotion(@Body request: AnalyzeEmotionRequest): AnalyzeEmotionResponse

    /** GET /api/ai/memory-tracking — memory tracking stats */
    @GET("/api/ai/memory-tracking")
    suspend fun getMemoryTracking(): MemoryTrackingResponse

    /** GET /api/ai/autonomy/status — trust level, score, successes/failures */
    @GET("/api/ai/autonomy/status")
    suspend fun getAutonomyStatus(): AutonomyStatus

    // ── HARDWARE ACTIONS ─────────────────────────────────────────────────────

    /** GET /api/ai/hardware/info — system hardware information */
    @GET("/api/ai/hardware/info")
    suspend fun getHardwareInfo(): HardwareInfo

    /** GET /api/ai/hardware/actions/pending — pending action queue */
    @GET("/api/ai/hardware/actions/pending")
    suspend fun getPendingHardwareActions(): PendingActionsResponse

    /** POST /api/ai/hardware/actions/claim — claim next action (ADMIN) */
    @POST("/api/ai/hardware/actions/claim")
    suspend fun claimHardwareAction(@Body body: ClaimHardwareActionRequest): HardwareAction

    /** POST /api/ai/hardware/actions/{id}/ack — acknowledge action (ADMIN) */
    @POST("/api/ai/hardware/actions/{id}/ack")
    suspend fun ackHardwareAction(@Path("id") id: Long): OkResponse

    /** POST /api/ai/hardware/actions/{id}/result — report result (ADMIN) */
    @POST("/api/ai/hardware/actions/{id}/result")
    suspend fun reportHardwareResult(
        @Path("id") id: Long,
        @Body body: CompleteHardwareActionRequest,
    ): OkResponse

    /** POST /api/ai/voice-call/{id}/end — end a voice call session */
    @POST("/api/ai/voice-call/{id}/end")
    suspend fun endVoiceCall(@Path("id") id: Long): OkResponse

    // ── AGENTS ───────────────────────────────────────────────────────────────

    /** GET /api/ai/agents — list all agent personas */
    @GET("/api/ai/agents")
    suspend fun getAgents(): AgentListResponse

    /** POST /api/ai/agents — create agent (ADMIN) */
    @POST("/api/ai/agents")
    suspend fun createAgent(@Body request: CreateAgentRequest): Agent

    /** DELETE /api/ai/agents/{name} — delete agent (ADMIN) */
    @DELETE("/api/ai/agents/{name}")
    suspend fun deleteAgent(@Path("name") name: String): OkResponse

    /** POST /api/ai/agents/{name}/chat — chat in agent mode */
    @POST("/api/ai/agents/{name}/chat")
    suspend fun chatWithAgent(
        @Path("name") agentName: String,
        @Body request: AgentChatRequest,
    ): ChatResponse

    // ── TOOLS ────────────────────────────────────────────────────────────────

    /** GET /api/ai/tools — list Elle's available tools */
    @GET("/api/ai/tools")
    suspend fun getTools(): ToolListResponse

    /** POST /api/ai/tools — add a tool (ADMIN) */
    @POST("/api/ai/tools")
    suspend fun createTool(@Body request: CreateToolRequest): AiTool

    /** DELETE /api/ai/tools/{name} — remove a tool (ADMIN) */
    @DELETE("/api/ai/tools/{name}")
    suspend fun deleteTool(@Path("name") name: String): OkResponse

    // ── VIDEO GENERATION ─────────────────────────────────────────────────────

    /** GET /api/video/avatars — list all avatars */
    @GET("/api/video/avatars")
    suspend fun getAvatars(): AvatarListResponse

    /** GET /api/video/avatar — get default avatar */
    @GET("/api/video/avatar")
    suspend fun getDefaultAvatar(): UserAvatar

    /** POST /api/video/avatar/upload — upload new avatar (base64) */
    @POST("/api/video/avatar/upload")
    suspend fun uploadAvatar(@Body request: UploadAvatarRequest): UserAvatar

    /** POST /api/video/generate — queue a video generation job */
    @POST("/api/video/generate")
    suspend fun generateVideo(@Body request: GenerateVideoRequest): VideoJob

    /** GET /api/video/status/{job_id} — poll job status and output_path */
    @GET("/api/video/status/{job_id}")
    suspend fun getVideoStatus(@Path("job_id") jobId: String): VideoJob

    // ── GOALS ────────────────────────────────────────────────────────────────

    /** GET /api/goals — Elle's active goals with progress */
    @GET("/api/goals")
    suspend fun getGoals(): GoalListResponse

    // ── EDUCATION ────────────────────────────────────────────────────────────

    /** GET /api/education/subjects — subject library */
    @GET("/api/education/subjects")
    suspend fun getSubjects(
        @Query("category") category: String? = null,
        @Query("limit") limit: Int = 50,
    ): SubjectListResponse

    /** GET /api/education/subjects/{id} — subject detail (milestones + refs) */
    @GET("/api/education/subjects/{id}")
    suspend fun getSubject(@Path("id") id: Int): LearnedSubject

    /** POST /api/education/subjects — add subject (ADMIN) */
    @POST("/api/education/subjects")
    suspend fun createSubject(@Body subject: CreateSubjectRequest): LearnedSubject

    /** PUT /api/education/subjects/{id} — update subject (ADMIN) */
    @PUT("/api/education/subjects/{id}")
    suspend fun updateSubject(
        @Path("id") id: Int,
        @Body patch: UpdateSubjectRequest,
    ): OkResponse

    /** POST /api/education/subjects/{id}/milestones — add milestone (ADMIN) */
    @POST("/api/education/subjects/{id}/milestones")
    suspend fun addMilestone(
        @Path("id") subjectId: Int,
        @Body milestone: CreateMilestoneRequest,
    ): Milestone

    /** POST /api/education/subjects/{id}/references — add reference (ADMIN) */
    @POST("/api/education/subjects/{id}/references")
    suspend fun addReference(
        @Path("id") subjectId: Int,
        @Body reference: CreateReferenceRequest,
    ): EducationReference

    /** GET /api/education/skills — skill inventory */
    @GET("/api/education/skills")
    suspend fun getSkills(): SkillListResponse

    /** POST /api/education/skills — add skill (ADMIN) */
    @POST("/api/education/skills")
    suspend fun createSkill(@Body skill: CreateSkillRequest): Skill

    /** PUT /api/education/skills/{name}/use — log skill usage */
    @PUT("/api/education/skills/{name}/use")
    suspend fun useSkill(@Path("name") name: String): OkResponse

    // ── DICTIONARY ───────────────────────────────────────────────────────────

    /** GET /api/dictionary/lookup/{word} — look up a single word */
    @GET("/api/dictionary/lookup/{word}")
    suspend fun lookupWord(@Path("word") word: String): DictionaryWord

    /** GET /api/dictionary/search — search dictionary */
    @GET("/api/dictionary/search")
    suspend fun searchDictionary(@Query("q") query: String): List<DictionaryWord>

    /** GET /api/dictionary/random — random word of the moment */
    @GET("/api/dictionary/random")
    suspend fun getRandomWord(): DictionaryWord

    /** GET /api/dictionary/stats — dictionary stats */
    @GET("/api/dictionary/stats")
    suspend fun getDictionaryStats(): DictionaryStats

    /** GET /api/dictionary/load/status — load status */
    @GET("/api/dictionary/load/status")
    suspend fun getDictionaryLoadStatus(): DictionaryLoadStatus

    /** POST /api/dictionary/load — trigger dictionary load (ADMIN) */
    @POST("/api/dictionary/load")
    suspend fun loadDictionary(@Body body: LoadDictionaryRequest = LoadDictionaryRequest()): OkResponse

    // ── MODELS ───────────────────────────────────────────────────────────────

    /** GET /api/models/personality — personality model configuration */
    @GET("/api/models/personality")
    suspend fun getPersonality(): PersonalityResponse

    /** GET /api/models/slots — model slot list */
    @GET("/api/models/slots")
    suspend fun getModelSlots(): ModelSlotListResponse

    /** POST /api/models/slots — add a model slot (ADMIN) */
    @POST("/api/models/slots")
    suspend fun createModelSlot(@Body slot: ModelSlot): ModelSlot

    /** DELETE /api/models/slots/{slot_number} — remove a model slot (ADMIN) */
    @DELETE("/api/models/slots/{slot_number}")
    suspend fun deleteModelSlot(@Path("slot_number") slotNumber: Int): OkResponse

    /** POST /api/models/slots/{slot_number}/ping — ping a slot (ADMIN) */
    @POST("/api/models/slots/{slot_number}/ping")
    suspend fun pingModelSlot(@Path("slot_number") slotNumber: Int): OkResponse

    /** GET /api/models/token-cache/stats — token cache statistics */
    @GET("/api/models/token-cache/stats")
    suspend fun getTokenCacheStats(): TokenCacheStats

    /** GET /api/models/workers — model worker list */
    @GET("/api/models/workers")
    suspend fun getModelWorkers(): ModelWorkerListResponse

    /** POST /api/models/workers — add a worker (ADMIN) */
    @POST("/api/models/workers")
    suspend fun createModelWorker(@Body worker: CreateModelWorkerRequest): ModelWorker

    /** PUT /api/models/workers/{worker_id}/decommission — decommission (ADMIN) */
    @PUT("/api/models/workers/{worker_id}/decommission")
    suspend fun decommissionWorker(@Path("worker_id") workerId: Int): OkResponse

    // ── MORALS ───────────────────────────────────────────────────────────────

    /** GET /api/morals/rules — Elle's moral framework */
    @GET("/api/morals/rules")
    suspend fun getMoralRules(): MoralRulesResponse

    /** POST /api/morals/rules — add a moral rule (ADMIN — requires admin key) */
    @POST("/api/morals/rules")
    suspend fun createMoralRule(@Body request: CreateMoralRuleRequest): MoralRule

    // ── SERVER ───────────────────────────────────────────────────────────────

    /** GET /api/server/status — server health overview */
    @GET("/api/server/status")
    suspend fun getServerStatus(): ServerStatus

    /** GET /api/server/analytics — request metrics */
    @GET("/api/server/analytics")
    suspend fun getServerAnalytics(): ServerAnalytics

    /** GET /api/server/settings — current settings (bind address, port, model, version) */
    @GET("/api/server/settings")
    suspend fun getServerSettings(): ServerSettings

    /** PUT /api/server/settings — update settings (ADMIN) */
    @PUT("/api/server/settings")
    suspend fun updateServerSettings(@Body request: UpdateSettingsRequest): OkResponse

    /** GET /api/server/console — log entries from SQL, newest first */
    @GET("/api/server/console")
    suspend fun getConsoleLogs(
        @Query("limit") limit: Int = 100,
        @Query("level") level: String? = null,
    ): LogListResponse

    /** DELETE /api/server/console — truncate log table (ADMIN) */
    @DELETE("/api/server/console")
    suspend fun clearConsoleLogs(): OkResponse

    /** GET /api/server/backups — backup list */
    @GET("/api/server/backups")
    suspend fun getBackups(): BackupListResponse

    /** POST /api/server/backup — trigger a new backup (ADMIN) */
    @POST("/api/server/backup")
    suspend fun createBackup(): OkResponse

    /** POST /api/server/commit-memory — force memory consolidation (ADMIN) */
    @POST("/api/server/commit-memory")
    suspend fun commitMemory(): OkResponse

    /** POST /api/server/commit-emotional-memory — force emotional memory write (ADMIN) */
    @POST("/api/server/commit-emotional-memory")
    suspend fun commitEmotionalMemory(): OkResponse

    // ── DIAGNOSTICS & ADMIN ──────────────────────────────────────────────────

    /** GET /api/diag/queues — IPC queue depths */
    @GET("/api/diag/queues")
    suspend fun getDiagQueues(): DiagQueuesResponse

    /** GET /api/diag/routes — live route manifest (ADMIN) */
    @GET("/api/diag/routes")
    suspend fun getDiagRoutes(): DiagRoutesResponse

    /** POST /api/admin/reload — hot-reload config (ADMIN) */
    @POST("/api/admin/reload")
    suspend fun reloadConfig(): OkResponse

    /** GET /api/brain/status — brain status (PUBLIC) */
    @GET("/api/brain/status")
    suspend fun getBrainStatus(): Map<String, String>

    /** GET /api/hal/status — hardware abstraction status (PUBLIC) */
    @GET("/api/hal/status")
    suspend fun getHalStatus(): HardwareInfo

    // ── AUTH / DEVICES ───────────────────────────────────────────────────────

    /** GET /api/auth/devices — list paired devices (ADMIN) */
    @GET("/api/auth/devices")
    suspend fun getPairedDevices(): PairedDevicesResponse

    /** DELETE /api/auth/devices/{id} — revoke a paired device (ADMIN) */
    @DELETE("/api/auth/devices/{id}")
    suspend fun revokeDevice(@Path("id") deviceId: String): OkResponse

    /** POST /api/auth/pair-code — generate a 6-digit pairing code (ADMIN) */
    @POST("/api/auth/pair-code")
    suspend fun generatePairCode(): Map<String, String>

    // NOTE: GET /api/auth/qr is called directly via OkHttp (not Retrofit)
    // because it returns SVG text rather than JSON. See AppContainerExtended.

    // ── X-CHROMOSOME ─────────────────────────────────────────────────────────
    // All routes: AUTH_USER (fail-closed default). Pairing is the access gate.

    /** GET /api/x/state — current XChromosome state */
    @GET("/api/x/state")
    suspend fun getXState(): XState

    /** GET /api/x/history — cycle history */
    @GET("/api/x/history")
    suspend fun getXHistory(): XHistory

    /** GET /api/x/fertility_window — fertility window */
    @GET("/api/x/fertility_window")
    suspend fun getFertilityWindow(): FertilityWindow

    /** GET /api/x/next_period — next period prediction */
    @GET("/api/x/next_period")
    suspend fun getNextPeriod(): NextPeriod

    /** GET /api/x/pregnancy — pregnancy state */
    @GET("/api/x/pregnancy")
    suspend fun getPregnancy(): Pregnancy

    /** GET /api/x/pregnancy/events — pregnancy event log */
    @GET("/api/x/pregnancy/events")
    suspend fun getPregnancyEvents(): PregnancyEventsResponse

    /** GET /api/x/contraception — current contraception status */
    @GET("/api/x/contraception")
    suspend fun getContraception(): Contraception

    /** POST /api/x/contraception — update contraception */
    @POST("/api/x/contraception")
    suspend fun setContraception(@Body contraception: Contraception): OkResponse

    /** GET /api/x/lifecycle — lifecycle stage */
    @GET("/api/x/lifecycle")
    suspend fun getXLifecycle(): XLifecycle

    /** GET /api/x/modulation — hormone modulation values */
    @GET("/api/x/modulation")
    suspend fun getXModulation(): XModulation

    /** GET /api/x/symptoms — symptom log */
    @GET("/api/x/symptoms")
    suspend fun getXSymptoms(): XSymptomsResponse

    /** POST /api/x/symptoms — log a symptom */
    @POST("/api/x/symptoms")
    suspend fun logSymptom(@Body request: LogSymptomRequest): OkResponse

    /** POST /api/x/cycle/anchor — set cycle anchor date */
    @POST("/api/x/cycle/anchor")
    suspend fun setCycleAnchor(@Body request: CycleAnchorRequest): OkResponse

    /** POST /api/x/conception/attempt — attempt conception */
    @POST("/api/x/conception/attempt")
    suspend fun attemptConception(@Body request: AttemptConceptionRequest = AttemptConceptionRequest()): OkResponse

    /** POST /api/x/lifecycle — set lifecycle stage (ADMIN) */
    @POST("/api/x/lifecycle")
    suspend fun setLifecycle(@Body lifecycle: XLifecycle): OkResponse

    /** POST /api/x/pregnancy/accelerate — accelerate pregnancy (ADMIN) */
    @POST("/api/x/pregnancy/accelerate")
    suspend fun acceleratePregnancy(@Body request: AcceleratePregnancyRequest = AcceleratePregnancyRequest()): OkResponse
}
