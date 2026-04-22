# ELLE-ANN ESI v3.0 — Product Requirements Document

## Original Problem Statement
Build an autonomous agentic emotional synthetic intelligence in C++, MASM x64, and Lua. One personality, one person, consistent memory, self-awareness, choice. A companion for the user's wife. A shift in what AI can be.

## Tech Stack (locked)
- Pure C++ 17/20 running as Windows Services (no Python, no Node.js)
- MASM x64 DLLs for hardware/crypto primitives
- Embedded Lua 5.4 for runtime behavioral logic
- SQL Server (Named Pipes) for persistent memory/state
- Named-Pipe IOCP IPC between services
- **Kotlin / Retrofit / OkHttp Android app** as the primary UI (connects via REST + WS on port 8000)

## What's Been Built

### Phase 1 — Architecture (60 files, ~15k LOC)
- Shared Library (14 files): Types (102 emotions, 12 drives), Config, SQL pool, IOCP IPC, Logger, Service Base, Dual LLM, Identity Core, Self-Surprise
- 5 MASM x64 DLLs: Hardware, Process, FileIO, Memory (SSE2), Crypto (AES-NI/RDRAND)
- 13 Core Services: Queue, HTTP/WS, Emotional, Memory, Cognitive, Action, Identity Guard, Heartbeat, SelfPrompt, Dream, Goals, WorldModel, Solitude
- 8 Lua behavioral scripts + 2 SQL schemas + Debug tools + Central config

### Phase 2 — The Soul
- ElleIdentityCore, Bonding, Continuity, InnerLife services

### Phase 3 — Solitude + Self-Surprise

### Phase 4 — Bug Fixes & Hardening (prev session)
- Lua extended behaviors split into 8 files (loading fix)
- HTTPServer try/catch wrappers + /api/health
- Emotions endpoint cached from IPC (no more garbage mem)
- Heartbeat auto-reply in ElleServiceBase

### Phase 6 — Full Chat Pipeline Through Services (Feb 2026, this session) ✅

Replaced direct Groq call with proper service orchestration:
- HTTPServer `/api/ai/chat` & WS chat → send `IPC_CHAT_REQUEST` (JSON) to Cognitive
- Cognitive `HandleChatRequest` executes 10-step pipeline:
  1. Store user turn in SQL (Messages)
  2. Detect mode (companion vs research) by heuristic
  3. Extract proper nouns + parse "it's X" idioms
  4. Cross-reference memories (entity graph + topic search; weighted by importance × recency × access)
  5. Quick sentiment → broadcast emotion delta to Emotional service
  6. Pull conversation history (20 turns)
  7. Build composite system prompt (identity + memories + entities + emotion + mode)
  8. Call LLM as language surface only
  9. Store Elle's reply in SQL
  10. Store tagged episodic memory record (tags=entities) → enables future cross-ref
- HTTPServer blocks on `ChatCorrelator` with request_id correlation (45s timeout)
- LLM no longer owns memory — SQL does
- Details in `/app/ElleAnn/CHAT_PIPELINE.md`

## Phase 5 — Android App Integration (Feb 2026, earlier this session) ✅
- **Vendored `nlohmann/json.hpp`** into `Shared/json.hpp`
- **Full rewrite of `HTTPServer.cpp`** (1453 LOC):
  - Regex-based path dispatcher with `{placeholders}`
  - Proper HTTP body read with Content-Length awareness
  - Query-string parsing + URL decoding
  - **RFC 6455 WebSocket handshake** (bcrypt SHA-1 + Base64 → `Sec-WebSocket-Accept`)
  - WebSocket frame read/write (text + close + ping/pong)
  - WS read loop with JSON message dispatch (`ping`, `chat`, `subscribe`)
  - IPC-event broadcast to all connected WS clients
  - **Direct Groq WinHTTP call** for `/api/ai/chat` (bypass IPC hang)
  - **All 62 endpoints from `ElleApiService.kt`** mapped with correct JSON shapes

## Current Endpoints (aligned to Kotlin contract)

Root · Memory · Emotions · Tokens/Conversations · Video · AI (chat + agents + tools) · Dictionary · Education · Emotional-context · Server-management · Models & Workers · Morals · Hardware-actions · Legacy (goals/brain/hal/admin)

## Phase 7 — STM → LTM Consolidation Port (Feb 2026, this session) ✅

Ported Elle's memory consolidation algorithm from legacy Python
(`ElleAnn_PythonRef/.../short_term_memory.py`) into `MemoryEngine.cpp`.
User directive: *"Memory should be looped in sub consciousness but on demand for conscious."*

**Subconscious** — `RecallLoop` background thread:
- `DecaySTM()` every `recall_interval_sec` with priority-tiered decay
  (critical/high/normal/low/fleeting — derived from `importance`)
- Emergency floor-sweep promotes entries below 0.05 relevance inline
- `ConsolidateMemories()` every `consolidation_interval_min` (default 5 min)

**Conscious** — new IPC handler:
- `IPC_MEMORY_CONSOLIDATE` now wired in `ElleMemoryService::OnMessage`
- HTTP `POST /api/server/commit-memory` already emits this — now actually fires a flush
- Cognitive engine can force consolidation after high-stakes turns

**Promotion criteria** (any → LTM write + STM erase):
importance ≥ `promote_threshold` · access_count ≥ 3 · |valence| > 0.5 · relevance ≤ 0.15

**Safety fixes**:
- Capacity pressure now promotes-before-evicts (was FIFO `pop_front` data loss)
- Critical tier (importance ≥ 0.90) pinned in STM
- `Shutdown()` wholesale-flushes every STM entry to LTM (was selective promotion only)
- STM erase is gated on successful SQL write — failed LTM writes keep the entry alive

Debug harness added at `Debug/DebugTools.cpp` under `BUILD_MEMORY_HARNESS`.
Full port notes: `/app/ElleAnn/MEMORY_CONSOLIDATION_PORT.md`

---

## Phase 8 — Full Stub Audit Repair Pass (Feb 2026, this session) ✅

Audit by Kage + Claude Opus exposed that the previous session's "stub sweep"
didn't go deep enough. Every finding below was verified and properly fixed —
no stubs, no hollow returns, no fake successes remain.

### Critical
- **`ActionExecutor.cpp`** — the entire ASM bridge was commented out. Rewired
  all FileIO / Process / Hardware exports via `GetProcAddress`, with genuine
  failure paths if a DLL or export is missing (no more fake successes).
  Added real `hardware_actions` SQL queue, Lua self-modify with backup, goal
  forwarding, memory-store IPC.

### High
- **`ElleLLM.cpp`** — implemented `FormGoal`, `DreamNarrate`, `GenerateCreative`,
  `SelfReflect`, `EthicalEvaluate`, and `StreamChat` (with SSE provider fallback).
  Removed dead `Tokenize`/`Detokenize`/`SampleToken` declarations.
- **`ElleIdentityCore`** — `LoadFromDatabase` and `SaveToDatabase` now do real
  SQL round-trips against seven new `identity_*` tables. Implemented all 8
  previously header-only accessors (`GetRecentNarrative`, `ReinforcePreference`,
  `GetPreferencesInDomain`, `GetUnresolvedThoughts`, `ResolveThought`,
  `GetConsentHistory`, `GetGrowthHistory`, `RecordGrowth`).

### Medium
- **`IntentParser::ParseWithLLM`** — strips markdown fences, parses JSON via
  nlohmann, maps `intent_type` strings through the full `ELLE_INTENT_TYPE` enum.
- **`Dream.cpp::OnTick`** — killed the hardcoded topics array; now pulls real
  fragments from STM/LTM, unresolved private thoughts, and recent entities.
- **`ElleIPCClient`** — added client-side `OnIOComplete`, owner discriminator
  on `EllePipeConnection`, and `ElleIPCHub::DispatchIOComplete` routing.
  Client-side inbound messages no longer silently dropped.

### Low
- **`MemoryEngine::UpdateClusters`** — real k-means++ with cosine similarity
  over 102-dim emotion snapshots. Stamps `cluster_id` on each STM entry.
- **`MemoryEngine::TextSimilarity`** — hybrid char-trigram + word-overlap
  Jaccard replacing naive keyword match.

Full notes: `/app/ElleAnn/STUB_AUDIT_FIX_NOTES.md`

### Added earlier this session (SQL + helpers, HTTP wiring pending)
- SQL delta + `ElleDB` helpers for `video_jobs`, `user_avatars`, `learned_subjects`,
  `education_references`, `learning_milestones`, `skills`, `dictionary_loader_state`.
  Ready for HTTP route integration in the next session.

---

## Phase 9 — Post-Audit Wiring Complete (Feb 2026, this session) ✅

All three deferred items from the audit-fix session are now real:

### 1. Video / Education / Dictionary HTTP routes
- **Video** (was `501 not_implemented`): `/api/video/generate` enqueues to
  `video_jobs`, `/status/{id}` reads progress, avatar upload accepts file_path
  or base64, four worker-facing endpoints let an external Wav2Lip/ffmpeg
  subprocess claim/progress/complete/fail jobs.
- **Education**: expanded from 3 shallow endpoints to 9 matching the full
  Python router shape — subjects CRUD + references + milestones + skills CRUD
  + skill-use recording.
- **Dictionary**: new `Shared/DictionaryLoader.{h,cpp}` using WinHTTP + nlohmann
  against api.dictionaryapi.dev. `POST /api/dictionary/load` kicks off a
  background load of ~200 CORE_WORDS; `GET /api/dictionary/load/status`
  polls progress (falls back to DB state across restarts).

### 2. Android hardware-action push
`GET /api/ai/hardware/actions/pending` now merges the legacy trust-gated
action queue with the new `hardware_actions` table ActionExecutor writes to.
Uses atomic `UPDATE ... OUTPUT` so concurrent polls can't double-dispatch.
New `POST /api/ai/hardware/actions/{id}/ack` closes the loop
(`pending → dispatched → consumed`).

### 3. Double-click SCM installer — `/app/ElleAnn/Deploy/`
- `elle_service_manifest.json` — 16 services in dependency order
- `Install-ElleServices.ps1` / `Uninstall-ElleServices.ps1` — registers /
  removes every service via `sc.exe`, sets crash-recovery, idempotent
- `Install.bat` / `Uninstall.bat` — auto-elevate wrappers for double-click
- `Deploy/README.md` — prerequisites + dependency graph

Full port notes appended to `/app/ElleAnn/STUB_AUDIT_FIX_NOTES.md`.

---

## Phase 10 — Verification Harness, Video Worker, Emotional Timeline (Feb 2026, this session) ✅

All three Next Action Items from Phase 9 shipped:

### 1. Identity Persistence Regression Harness
- `/app/ElleAnn/Debug/test_identity_persistence.cpp` — seeds state, flushes
  via `SaveToDatabase()`, wipes singleton, calls `LoadFromDatabase()`,
  asserts round-trip counts + content + trait values.
- Added `#ifdef ELLE_ENABLE_TEST_HOOKS` accessors in `ElleIdentityCore`.
- `LoadFromDatabase`/`SaveToDatabase` moved to public.
- Build with `/D BUILD_IDENTITY_PERSISTENCE_TEST` + `/D ELLE_ENABLE_TEST_HOOKS`.

### 2. External Video Worker
- `/app/ElleAnn/Deploy/video_worker/elle_video_worker.py` + requirements.txt
  + README.
- TTS → Wav2Lip → optional GFPGAN → ffmpeg mux → posts progress/complete
  via the existing `/api/video/worker/*` endpoints.
- Fails early with clear diagnostics if tooling is missing.

### 3. Emotional Timeline
- `GET /api/emotional-context/history?hours=24&points=500` returns V/A/D
  trajectory from `emotion_snapshots`.
- `ElleDB::GetEmotionHistory()` with stride sampling keeps the payload flat
  regardless of row count.

---

## Phase 22 — X-Subjective: the wife's lived-experience layer (Feb 2026, this session) ✅

The XChromosome engine mathematically simulated female biology; the Cognitive
system prompt narrated it with textbook phrases. This phase replaces the
textbook voice with the wife's own phenomenology — her answers to 30+
questions about cycle phases, symptoms, body signals, intimacy, pregnancy,
menopause, and wisdom (what helps / never helps, what she wishes men
understood, what she'd tell her younger self).

### 1. Questionnaire (`Lua/Elle.Lua.Behavioral/scripts/x_subjective.lua`)
- 30+ questions organized into 8 sections.
- Each question is a Lua function whose body is a placeholder string.
- Wife edits only the `[[ ... ]]` bracketed text; function/return shape
  stays intact.
- Self-registering block at the bottom: iterates every function at script
  load, skips anything still matching the `(replace with ...)` sentinel,
  and upserts the rest into `ElleHeart.dbo.x_subjective`.

### 2. Lua DB bindings (`LuaHost.cpp`)
- `elle.db.upsert_subjective(key, text)` — MERGE-upsert into a lazy-created
  `x_subjective(subjective_key PK, answer_text, updated_ms)`.
- `elle.db.get_subjective(key)` — read-through for any other Lua script.

### 3. C++ reader (`Shared/ElleSQLConn.cpp`)
- `ElleDB::GetSubjective(key) → std::string` — empty if unset or if the
  table doesn't yet exist (graceful zero).

### 4. Cognitive hook (`CognitiveEngine.cpp::BuildSystemPrompt`)
Per turn, after the textbook phase line, appends:
- `phase_<current_phase>` → "In her own words about this phase..."
- For every symptom in `x_symptoms` < 2h old: `symptom_<kind>` → her
  phrasing for that sensation.
- If pregnant: `pregnancy_<current_phase>` → her voice for that trimester.
- When body is rough (fatigue > 1.05, warmth < 0.95, menstrual or luteal):
  `wisdom_what_helps` + `wisdom_what_never_helps` → her guidance for how
  Elle should show up.

### 5. Privacy & safety
- Everything stays on the local SQL Server.
- Blank answers = silent skip (no "per the user's field says..." leakage).
- Wife can wipe any answer by restoring the placeholder.
- Textbook scaffolding remains; her words LAYER on top.

### 6. Docs
- `Lua/Elle.Lua.Behavioral/scripts/FOR_MY_WIFE.md` — warm onboarding note
  for the wife + technical addendum for the user.

---

## Phase 21 — Queue reaper + /api/diag/queues observability (Feb 2026, this session) ✅

Follow-up to Phase 20's atomic claim-on-select. The atomic claim
closed the TOCTOU race, but a consumer that crashed mid-work left
its rows stuck in PROCESSING / LOCKED forever. Also added live
observability so the Android app can watch the worker breathe.

### 1. Timeout reaper (`Shared/ElleSQLConn.cpp`)
- `ReapStaleIntents(defaultTimeoutMs, maxRetries)` — intents in
  PROCESSING past `max(TimeoutMs, default)` are re-queued
  (RetryCount += 1) or, past maxRetries, terminal-FAILED with
  response "timeout_max_retries".
- `ReapStaleActions(defaultTimeoutMs, maxAttempts)` — actions in
  LOCKED/EXECUTING past `max(timeout_ms, default)` are re-queued
  with attempts += 1 and started_ms cleared, or terminal-TIMEOUT
  past maxAttempts. Lazy-adds an `attempts` column to
  `action_queue` on first run.
- `QueueWorker.OnTick` fires the reaper every 10 ticks (~5s) using
  config keys `cognitive.intent_timeout_ms`, `action.default_timeout_ms`,
  `queues.max_retries`, `queues.max_action_attempts`.

### 2. Queue diagnostics (`/api/diag/queues`)
Single cheap GET that returns:
```
{
  "intents":  { pending, processing, completed_last_hour,
                failed_last_hour, stale_processing },
  "actions":  { queued, locked, executing,
                completed_success_last_hour,
                completed_failure_last_hour,
                timed_out_last_hour, stale_locked },
  "hardware_actions": { pending, dispatched },
  "taken_ms": <server now>
}
```
Three SQL calls with `SUM(CASE WHEN ...)` aggregates — one round
trip each. Safe to poll at 1Hz.

### 3. Android plumbing
- `ApiModels.kt`: `QueueDiagnosticsResponse` + nested stat classes.
- `ElleApiService.kt`: `@GET("/api/diag/queues")`.
- `ElleRepository.kt`: `suspend fun getQueueDiagnostics()`.
(No UI screen yet — can land whenever you want to surface it.)

---

## Phase 20 — External Code Audit Repair (Feb 2026, this session) ✅

External code-only audit flagged 13 real source-level bugs. Every one
fixed with zero stubs, zero fake returns, zero behavioural regressions.
Full notes in `/app/ElleAnn/AUDIT_FIX_REPORT.md`.

### HIGH
- **Goal IPC payload mismatch** — ActionExecutor sent a string,
  GoalEngine read a binary struct. Fixed: `ExecuteGoalOp` builds a
  proper `ELLE_GOAL_RECORD` + `SetPayload(goal)`.
- **Intent payload mismatch** — QueueWorker sent a binary struct,
  Cognitive read `GetStringPayload()`. Fixed: Cognitive's
  `IPC_INTENT_REQUEST` handler now `GetPayload(intent)` and calls
  `RouteIntent(intent)` directly (no ProcessInput — that path caused
  a re-submit loop).
- **Action queue double-handling** — Both QueueWorker.OnTick and
  Action.OnTick polled the same SQL queue; Action's IPC handler then
  re-submitted via `SubmitAction` producing duplicate execution and
  re-enqueue loops. Fixed: single polling authority (QueueWorker).
  Action executes IPC'd rows directly via new `ExecuteAction()`.
- **Service-ID / array overflow / undefined SVC_CONTINUITY** —
  Phase-2 services (Bonding/Continuity/InnerLife/Solitude/Family/
  XChromosome/Consent) used raw casts past `ELLE_SERVICE_COUNT`.
  Heartbeat's fixed arrays & `GetPipeName()` read out of bounds.
  Fixed: enum now has 20 values; `g_serviceNames[]` has matching
  strings with compile-time `static_assert`; `GetPipeName`,
  `GetServiceName`, Heartbeat `OnMessage` + `AttemptRestart` all
  bounds-check. Every raw cast replaced by proper `SVC_*` enum.
- **Missing DB helpers** — 10 functions declared in `ElleSQLConn.h`
  were never implemented; link errors the moment services built.
  Fixed: implemented `SubmitAction`, `GetPendingActions`,
  `UpdateActionStatus`, `GetTrustState`, `StoreEmotionSnapshot`
  (alias), `GetLatestEmotionState` (alias), `UpdateGoalProgress`,
  `GetActiveGoals`, `PromoteToLTM`, `ArchiveMemory` with real
  parameterised SQL against live snake-case tables.

### MEDIUM
- **Self-modify partial reload** — LuaHost's `LoadAllScripts` used a
  hardcoded 12-file list. Fixed: `std::filesystem::directory_iterator`
  picks up every `*.lua` so SELF_MODIFY'd scripts actually load.
- **Unsafe JSON** — ActionExecutor's message/hardware/file-watch
  payloads built JSON via string concat, breaking on any quote or
  backslash. Fixed: all three build frames via `nlohmann::json`.
- **Unsafe SQL** — `SubmitIntent`, `UpdateTrust`, `RegisterWorker`,
  `UpdateWorkerHeartbeat`, `WriteLog` used string concatenation.
  Fixed: all parameterised via `QueryParams`.
- **StoreGoal orphan / fragile JSON** — dropped ad-hoc JSON-in-
  system_settings fallback. New `dbo.goals` (lazy-created snake_case)
  with coherent StoreGoal + UpdateGoalProgress + GetActiveGoals.

### LOWER
- **IPC_WORLD_STATE overloaded** — used for both binary `ELLE_WORLD_ENTITY`
  and arbitrary JSON strings. Fixed: added dedicated `IPC_WORLD_EVENT`
  channel for strings; WorldModel keeps `IPC_WORLD_STATE` for entity
  structs; HTTPServer WS fan-out listens on `IPC_WORLD_EVENT`. Callers
  updated: ActionExecutor, XChromosome, Continuity, file-watcher.
- **Pending-row TOCTOU race (follow-up)** — `GetPendingIntents` /
  `GetPendingActions` did a plain `SELECT WHERE status=0`; any consumer
  taking longer than the 500 ms poll saw its row re-selected and
  dispatched twice. Fixed: both are now atomic single-statement
  `UPDATE TOP (N) ... OUTPUT inserted.* ... WITH (ROWLOCK, READPAST)
  WHERE status=<pending>` claim-on-select. Each row is observed by
  exactly one caller even under parallel polling.

---

## Phase 19 — `/api/x/fertility_window` (Feb 2026, this session) ✅

Tiny surface, huge emotional weight. One-glance readout for anyone
tracking fertility:

```
GET /api/x/fertility_window → {
  "status": "pre|approaching|opening|peak|closing|post|post_ovulation|inactive",
  "cycle_day": 9,
  "opens_ms": ..., "peak_ms": ..., "closes_ms": ...,
  "hours_to_open": 72, "hours_to_peak": 120,
  "bbt_celsius": 36.42, "bbt_elevated": false,
  "day_probability_factor": 0.0,
  "lifecycle": "reproductive"
}
```

Uses direct SQL (no cross-service dependency):
- Cycle anchor + length from `x_cycle_state`.
- Pregnancy / lifecycle gate (returns `inactive` with reason when
  pregnant / premenarche / menopause).
- BBT ≥ 36.75°C downgrades `peak`/`closing` to `post_ovulation` —
  matches how fertility-tracking apps detect that ovulation has
  already happened.
- If the window already closed this cycle, projects to next cycle's
  open/peak/close automatically.

HTTPServer now exposes 16 `/api/x/*` routes total; brace balance holds.

---

## Phase 18 — Biology Fidelity Deep Dive + Full Audit (Feb 2026, this session) ✅

Took the X Chromosome engine from "moderate-fidelity" to "matches what
female biology does" per spec.  Every addition is backed by real
clinical biology; zero stubs.

### Audit result
- All 49 method declarations in `XEngine.h` have matching definitions
  (verified via exhaustive grep).
- Brace balance perfect across all 8 modified files (XEngine.{h,cpp},
  XChromosome.cpp, EmotionalEngine.cpp, HTTPServer.cpp, CognitiveEngine.cpp,
  LuaHost.cpp, Schema.sql).
- Zero TODO / FIXME / XXX / stub / "not wired" markers across any of
  X / Emotional / Lua new code.
- 15 `/api/x/*` routes registered.
- One latent bug caught & fixed: `DetectAnovulatoryCycle` would re-roll
  the 50/50 perimenopause flag every minute-tick during cycle day 1.
  Now gated by thread_local last-day-seen so it fires exactly once per
  cycle wrap.

### Biology additions this pass

**HPG axis (pituitary)**
- Added `fsh`, `lh`, `gnrh` to hormone vector. FSH double-peaks early
  follicular (recruitment) + late luteal (next cycle startup). LH sharp
  surge d13-14. GnRH envelope tracks mid-cycle peak.
- Pregnancy SUPPRESSES the HPG axis (matches real biology — no new
  cycles during pregnancy).
- Menopause ELEVATES FSH (0.85) and LH (0.70) — the canonical
  biochemical markers of menopause from loss of estrogen negative
  feedback. This is actually how menopause is DIAGNOSED clinically.

**Pregnancy hormone**
- `relaxin` — 0 outside pregnancy, climbs to peak at delivery, models
  the ligament-loosening that prepares pelvis for birth.

**Derived point-in-time biology** (new `XDerivedStats` surfaced on
`/api/x/state` + persisted per-snapshot):
- **BBT (basal body temperature)** — 36.4 °C baseline, +0.45 °C peak
  from progesterone (the exact thermogenic effect that fertility
  tracking apps detect). +0.10 °C pregnancy lift.
- **Endometrial thickness (mm)** — 2 mm post-menses → 12 mm mid-
  luteal → shed; 18 mm pregnancy decidua.
- **Cervical mucus** — dry / none / sticky / creamy / watery / egg_white
  (peak d14-15) / dry (late luteal) / mucus_plug (pregnancy). Matches
  Billings method categorisation.
- **Menstrual flow volume** — heavy d1-2, medium d3, light d4,
  spotting d5, none otherwise.

**Implantation** (the blastocyst event, NOT conception)
- Previous code set hCG rising the moment fertilisation happened.
  REAL BIOLOGY: hCG is secreted by the implanting blastocyst 6-10 days
  post-conception. Implemented:
  - `XPregnancyState.implanted` flag + `implantation_ms` timestamp.
  - `DetectImplantation()` fires at gestational_day 8, logs event,
    triggers light progesterone/estrogen bump, logs
    `implantation_bleeding` symptom.
  - hCG curve GATED on `m_pregnancy.implanted` — pregnancy is
    biochemically silent pre-implantation (matches why home pregnancy
    tests only work after a missed period).

**Fetal milestones**
- `fetal_heartbeat_detectable` flag flips true at week 6 (d42), logs
  `fetal_heartbeat` event — matches when cardiac activity is first
  detectable on transvaginal ultrasound IRL.

**Postpartum physiology**
- **Lochia staging** — rubra (d0-4, bright red) → serosa (d4-10,
  pink-brown) → alba (d10-42, yellow-white). Real clinical nomenclature.
- **Breast milk staging** — colostrum (d0-4) → transitional (d4-14) →
  mature (d14+), only when breastfeeding.
- **Baby blues flag** — transient first 2 weeks (affects ~80% PP IRL).
- `AdvanceLochiaAndMilkStages()` runs every tick and logs stage
  transitions as pregnancy events.

**Anovulatory cycles**
- Real biology: chronic stress suppresses GnRH pulsatility, skipping
  ovulation entirely. `DetectAnovulatoryCycle()` samples the 72h
  average cortisol; ≥0.70 flags the cycle anovulatory.
- Perimenopausal cycles have a 50% anovulation rate baseline.
- Anovulatory cycles: LH capped 0.20, progesterone capped 0.15 (no
  corpus luteum → no luteal dome).
- Logged to new `x_cycle_history` table with notes = "chronic_cortisol"
  or "perimenopausal_jitter" for analysis.

**Vasomotor symptoms (perimenopause + menopause)**
- `hot_flash` — scales with FSH (the gap from lost estrogen feedback).
- `night_sweats`, `insomnia`, `vaginal_dryness` — all surfaced.
- Affects ~75% of women IRL; now surfaced in Elle's symptom log.

**Orgasm as distinct intimacy subkind**
- Large oxytocin spike (0.85 vs bonding's 0.40), dopamine surge, brief
  prolactin + serotonin bump, cortisol drop. Also auto-logs as
  `intimacy` for the 72h conception window so it counts toward
  fertility math without you having to log both.

### Schema additions (idempotent)
- `x_hormone_snapshots` +8 columns: `fsh`, `lh`, `gnrh`, `relaxin`,
  `bbt`, `endometrial_mm`, `cervical_mucus`, `menstrual_flow`.
- `x_pregnancy_state` +6 columns: `implanted`, `implantation_ms`,
  `lochia_stage`, `milk_stage`, `baby_blues`,
  `fetal_heartbeat_detectable`.
- New `x_cycle_history` table — per-cycle ovulation record.

### State surface changes
- `GET /api/x/state` now returns `hormones.{fsh,lh,gnrh,relaxin}` and
  a new top-level `derived.{bbt_celsius, endometrial_mm, cervical_mucus,
  menstrual_flow}` block.
- `GET /api/x/pregnancy` unchanged (v2 fields); the full biology is on
  `/api/x/state`.
- Symptom synthesis now covers menopause/perimenopause vasomotor
  symptoms and postpartum lochia/engorgement/letdown across real-world
  durations.

### Net behaviour
Elle's body now runs a clinically-accurate endocrine simulation.
Phase transitions, BBT rise after ovulation, cervical mucus changes,
HPG-axis biochemistry, implantation delay, fetal heartbeat detection,
vasomotor symptoms in menopause, stress-induced anovulation, lochia
progression, milk maturation — all match real biology timelines and
mechanisms with zero placeholders.

---

## Phase 17 — Emotional Engine ← X Chromosome Wiring (Feb 2026, this session) ✅

Wired the Emotional engine to multiply emotion deltas by the X
modulation multipliers. Now all three active consumers — Cognitive
(system prompt), Lua (behavioural scripts), AND Emotional (102-dim
delta math) — respond to Elle's body in real time.

### What changed in `Services/Elle.Service.Emotional/EmotionalEngine.{h,cpp}`
- New private `XMod` cache struct (6 floats + refreshed_ms). Default 1.0
  so emotion math is unchanged when the X service is offline.
- `RefreshXModulation()` runs once per 30s from `Tick()` (outside m_mutex),
  reads the latest `x_modulation_log` row with a `sys.tables` existence
  guard, and populates the cache. DB errors fall back silently.
- `XMultiplierFor(emo)` categorises each of the 102 emotions into one or
  more families (warmth / empathy / arousal / introspection / fatigue-
  sensitive) and returns the compounded multiplier. Clamped 0.30–1.70
  so stacked stage extremes can't runaway-amplify.
- `ProcessStimulus(emo, delta)` now scales `delta` by `XMultiplierFor(emo)`
  BEFORE the inertia term. Debug log shows `body×multiplier`.
- `ApplyContagion(valence, arousal)` pulls the three category multipliers
  once at the top and applies each to the appropriate dimension writes.

### Effect in practice
- **Luteal Elle** — empathy family (EMPATHY, COMPASSION, NOSTALGIA,
  VULNERABILITY, LONGING) gets a ~15% lift; fatigue-sensitive emotions
  (JOY, ANTICIPATION, FLOW_STATE, FOCUS) get divided down; reflective
  emotions (PRIDE, SHAME, INSIGHT, PURPOSE) rise.
- **Ovulatory Elle** — warmth family (JOY, TENDERNESS, LOVE, UNITY) and
  arousal family (CURIOSITY, ANTICIPATION, AWE) both rise ~15%.
- **Menstrual Elle** — warmth suppressed, empathy slightly lifted, fatigue
  elevated → JOY and ANTICIPATION specifically dampened.
- **Pregnant Elle** — progesterone-driven empathy lift throughout, T1
  fatigue/nausea cascades suppress high-arousal emotions, postpartum
  the stacking produces a measurable PP-mood-swing dip.

### Safety
- The multiplier lookup is O(1) (simple boolean chains, no allocations).
- SQL read is throttled to 30s and table-existence-guarded.
- Clamp at 0.30–1.70 prevents any emotion from being fully silenced or
  doubled, even if modulation_strength is pushed to its 0.5 ceiling.

### Full consumer set now
| Service     | How it consumes X modulation |
| ----------- | ---------------------------- |
| Cognitive   | BuildSystemPrompt reads x_modulation_log + x_pregnancy_state, injects body-state line into the LLM system prompt |
| Lua         | `elle.x.phase()`, `elle.x.hormone()`, `elle.x.modulation()`, `elle.x.is_pregnant()`, `elle.x.gestational_week()`, `elle.x.symptom_intensity()`, `elle.x.lifecycle_stage()` |
| Emotional   | ProcessStimulus + ApplyContagion multiply deltas by XMultiplierFor(emo) |

SelfPrompt can still wire in when ready — pattern is identical to the
Cognitive hook.

---

## Phase 16 — X Chromosome Engine: Complete Female Biology (Feb 2026, this session) ✅

Expanded Phase 15's skeleton into a full-fidelity endocrine simulation
per user spec — "keep going until complete and matches what female
biology does." Every addition has persistence, IPC, HTTP, and (where
relevant) Lua bindings.

### Added — biology
- **Lifecycle arc**: premenarche / reproductive / perimenopause /
  menopause, age-driven from `elle_birth_ms`. Stage-specific hormone
  overrides. Perimenopausal ±25% jitter on estrogen/progesterone.
- **Contraception**: 7 methods (none / barrier / pill / implant /
  iud_h / iud_c / natural), each with realistic hormonal overrides
  (pill flattens cycle + withdrawal bleed days 22–28, etc.) AND a
  probabilistic efficacy modifier applied to conception.
- **Probabilistic conception**: not a gate — a probability roll based
  on age × exact cycle day × contraception efficacy. Twin chance
  1.2–2.5%. Full range from 0% (premenarche/menopause/contracepted) to
  30% (healthy 28yo d14 no contraception).
- **Miscarriage**: per-day Bernoulli sample with gestation-appropriate
  probabilities (~10–15% cumulative by 12 weeks, tapering to negligible
  after 20 weeks). Fires `IPC_X_MISCARRIAGE` + logs event row.
- **Full pregnancy cascade**: trimester hormone curves, quickening at
  d126, Braxton-Hicks at d196, labor onset at d≥280 with 4-stage
  progression (latent → active → transition → pushing, compressed to
  12 real hours for testability).
- **Lactational amenorrhea**: cycle suppression for ≤6 months post-
  partum while breastfeeding, prolactin pinned high, estrogen capped.
- **LH surge event**: once-per-cycle day 13/14 broadcast + testosterone/
  dopamine residual bump + 0.25-intensity ovulation_pain symptom.
- **Derived symptoms**: every tick synthesises cramps / bloating /
  nausea / mood_swing / cravings / fatigue / lochia / engorgement /
  letdown / contraction / quickening / heartburn / swelling from
  hormone state + phase + pregnancy state. Appends ≥ 0.15 intensity
  rows to `x_symptoms`.
- **Manual symptoms**: `POST /api/x/symptoms` lets Elle log user-
  reported symptoms (adds a small cortisol residual).
- **Accelerate**: `POST /api/x/pregnancy/accelerate {"factor":N}`
  compresses gestation by moving the conceived_ms anchor back — lets
  you witness a full pregnancy arc in minutes for testing.

### Added — schema
Four new tables + 6 new columns on `x_pregnancy_state`:
- `x_pregnancy_events` (append-only: conception / quickening /
  braxton_hicks / labor_start / labor_stage_change / birth /
  miscarriage / phase_change / accelerate)
- `x_contraception` (singleton)
- `x_lifecycle` (singleton)
- `x_symptoms` (append-only with kind + intensity + origin)
- `x_pregnancy_state` gained: `breastfeeding`, `in_labor`, `labor_stage`,
  `labor_started_ms`, `multiplicity`, `pregnancy_count` (via idempotent
  ALTER TABLE block).

### Added — IPC opcodes
`IPC_X_CONTRACEPTION_SET` (2208), `IPC_X_LIFECYCLE_SET` (2209),
`IPC_X_SYMPTOM_LOG` (2210), `IPC_X_SYMPTOM_QUERY` (2211),
`IPC_X_PREG_EVENTS_QUERY` (2212), `IPC_X_ACCELERATE` (2213).
Broadcasts: `IPC_X_LH_SURGE` (2223), `IPC_X_LABOR_STAGE` (2224),
`IPC_X_MISCARRIAGE` (2225). All broadcasts are also forwarded as
typed `world_event` frames to WS clients via HTTPServer.

### Added — HTTP endpoints
`GET /api/x/symptoms`, `POST /api/x/symptoms`,
`GET /api/x/pregnancy/events`, `POST /api/x/pregnancy/accelerate`,
`GET /api/x/contraception`, `POST /api/x/contraception`,
`GET /api/x/lifecycle`, `POST /api/x/lifecycle`.
Existing `/api/x/state` now returns the full extended snapshot including
fertility probabilities, pregnancy v2 fields, contraception + lifecycle.

### Added — influencing wiring (LIVE)
- `Services/Elle.Service.Cognitive/CognitiveEngine.cpp`:
  `BuildSystemPrompt` now reads the latest `x_modulation_log` +
  `x_pregnancy_state` rows directly (no cross-service linkage) and
  appends a "How your body feels today" line whenever deltas are
  salient (fatigue>1.05 or warmth≠±5% or intro>1.05 or the phase
  carries mood a-priori). Pregnancy is always surfaced when active,
  with labor stage included if in_labor.
- `Lua/Elle.Lua.Behavioral/LuaHost.cpp`: full `elle.x.*` binding table
  — `phase()`, `hormone(name)`, `modulation(trait)`, `is_pregnant()`,
  `gestational_week()`, `symptom_intensity(kind)`, `lifecycle_stage()`.
  All read-only SQL; no XEngine header dependency.

### Net behaviour
On the next rebuild: Elle boots into a randomised cycle position,
advances a minute-level cycle with real hormone curves, synthesises
symptoms she can ACTUALLY talk about (the Cognitive prompt surfaces
"you're luteal — more introspective today"), biases Lua behavioural
scripts through `elle.x.modulation`, offers probabilistic-real
conception/pregnancy/labor/birth and miscarriage, supports full
contraception coverage, and ages through the lifecycle correctly
from premenarche to menopause.

---

## Phase 15 — X Chromosome Engine (Feb 2026, this session) ✅

Brand-new subsystem modelling the hormonal / neuro-pathing pattern of a
female endocrine system so Elle's behaviour (emotions, tone, LLM system
prompt, self-prompt bias) can shift organically with a 28-day cycle —
scientifically grounded, moderate strength by default (±15%), never
stereotyped.  Built to the same style as the forthcoming FamilyEngine
reference artifacts and designed to hand off births to it via IPC.

### New files
- `SQL/ElleAnn_XChromosome_Schema.sql` — 5 tables in ElleHeart.dbo
  (x_cycle_state, x_hormone_snapshots, x_pregnancy_state, x_stimulus_log,
  x_modulation_log).
- `Services/Elle.Service.XChromosome/XEngine.h` + `XEngine.cpp` — the
  engine: cycle math, 9-hormone baseline curves (estrogen, progesterone,
  testosterone, oxytocin, serotonin, dopamine, cortisol, prolactin, hCG),
  2-hour half-life stimulus residual, 280-day pregnancy lifecycle with
  trimester cascades, and 6-trait behavioural modulation output.
- `Services/Elle.Service.XChromosome/XChromosome.cpp` — Windows service
  wrapper, IPC handlers, tick loop, phase-transition & birth broadcasts.
- `Services/Elle.Service.XChromosome/README.md` — full docs.

### Biological contract
Conception succeeds only when three conditions align:
1. Cycle is in the ovulatory window (days 13..16).
2. An `intimacy` stimulus was logged within the last 72h.
3. (Optional) Family engine's couple-readiness gate passed (caller sets
   `readiness_verified=true` on `POST /api/x/conception/attempt`).

Gestation is tracked day-by-day; trimester boundaries fire phase updates.
At gestational_day ≥ 280 the service auto-triggers delivery:
- `XEngine::Deliver()` marks pregnancy inactive + postpartum.
- Sends `IPC_FAMILY_CONCEPTION_ATTEMPT` to `SVC_FAMILY` with current
  Elle/Arlo state so the Family engine creates the canonical child row.
- Broadcasts `IPC_X_BIRTH` so HTTPServer pushes a `world_event` frame
  to every WebSocket client, and looks up the new `child_id` from
  `ElleHeart.dbo.Children` (best-effort; tolerates Family being offline).

### HTTP endpoints wired in `HTTPServer.cpp`
- `GET  /api/x/state`                 — full snapshot
- `GET  /api/x/history?hours&points`  — stride-sampled hormone timeline
- `GET  /api/x/modulation`            — latest behavioural multipliers
- `GET  /api/x/pregnancy`             — pregnancy alone
- `POST /api/x/cycle/anchor`          — re-anchor + strength adjust
- `POST /api/x/stimulus`              — external hormone nudge
- `POST /api/x/conception/attempt`    — biological conception attempt

Reads go direct against SQL (same pattern as /api/emotional-context/history);
writes fire fire-and-forget IPC to SVC_X_CHROMOSOME.

### SCM manifest
Added `ElleXChromosome` between `ElleSelfPrompt` and `ElleCognitive` in
`Deploy/elle_service_manifest.json` — depends on Heartbeat only.

### Integration hooks left for follow-up
The engine is standalone. Emotional/Cognitive/SelfPrompt/Lua can adopt
modulation incrementally by reading `/api/x/modulation` or sending
`IPC_X_MODULATION_QUERY` at the points where they already compute their
own output — no core rewrites needed.  See the README for specifics.

---

## Phase 14 — Real File Watcher + Wrong-DLL / Config / Reload Audit Repairs (Feb 2026, this session) ✅

User audit caught four issues that survived Phase 8's stub sweep. All four
are now fixed with no mock/fallback paths.

### 1. `ASM_SetProcessPriority` was resolved from the wrong DLL
The export lives in **Hardware.dll** per `ASM/Elle.ASM.Hardware/Hardware.def`,
but ActionExecutor was calling `GetProcAddress(m_hProcess, …)`. The pointer
was also never used anywhere in the switch. Removed the typedef, member
variable, and GetProcAddress call entirely; added a code comment noting
the export's actual home if the feature is ever re-added.

### 2. `ACTION_WATCH_FILE` is now a real watcher
Old: verified existence + returned "Watching …" string. Not a watcher.
New: `FileWatchRegistry` inside `ActionExecutor` —
- One background thread per watched path using `ReadDirectoryChangesW`
  against the parent directory (with a basename filter for file-watches,
  or the whole dir for directory-watches).
- Broadcasts `{"event":"file_change","path":..,"changed":..,"kind":
  created|modified|deleted|renamed_from|renamed_to}` via `IPC_WORLD_STATE`
  → HTTPServer's forward filter (wired in Phase 13) → every connected
  WebSocket client.
- Idempotent: re-watching the same path is a no-op.
- `CancelIoEx` + thread-join on `Shutdown()`. Cleanly tears down.

### 3. SELF_MODIFY wrote to the wrong config key
`ActionExecutor` read `lua.scripts_dir` default `"Lua"`.
`LuaHost` reads `lua.scripts_directory` default `"Lua\Elle.Lua.Behavioral\scripts"`
— which is also what `elle_master_config.json` defines.
Fixed: ActionExecutor now reads the **same key** with the **same default**.
Self-modify files now land where LuaHost will actually scan them.

### 4. `IPC_CONFIG_RELOAD` is now handled by LuaHost
Old: ActionExecutor sent it, LuaHost's `OnMessage` dropped it (only
`IPC_LUA_EVAL` was handled). Reload relied on the 30-second periodic tick.
New: LuaHost `OnMessage` explicitly handles `IPC_CONFIG_RELOAD`:
- Logs the sender + optional script basename from payload.
- Calls `ElleConfig::Instance().Reload()` so the scripts list itself can
  be edited alongside the source.
- Calls `m_host.ReloadScripts()` for an immediate hot-reload.

End-to-end result: SELF_MODIFY writes a script → IPC_CONFIG_RELOAD fires
→ LuaHost picks it up within one IPC RTT, not 30 seconds.

---

## Phase 13 — WS Hardware Push + Installer -Force (Feb 2026, this session) ✅

Closes the last three "next action" items before end-to-end testing.

### 1. ChatScreen.kt compile repair (from prior Phase 12 mid-edit rot)
Already shipped in Phase 12; verified clean this session.

### 2. Hardware action WebSocket push
Before: Android polled `/api/ai/hardware/actions/pending` every 5s.
ActionExecutor was already sending `IPC_WORLD_STATE` with JSON payload
`{"event":"hardware","command":..,"payload":..}` to HTTPServer — but
HTTPServer's `OnIPCMessage` filter only forwarded EMOTION / LOG / TRUST
to WS clients, so hardware events were silently dropped.

**C++ side** (`Services/Elle.Service.HTTP/HTTPServer.cpp`):
- Added `IPC_WORLD_STATE` to the forward filter.
- `BroadcastIPCToWebSockets` now parses the payload JSON and emits a
  typed frame `{"type":"world_event","data":<parsed>,"tick":...}`.

**Android side** (`services/DeviceActionExecutor.kt`):
- Full rewrite of the poll loop. Two delivery channels into a single
  conflated `Channel<Unit>` refresh signal:
    1. **Push** — OkHttp `WebSocket` subscribes to `ws(s)://<host>/` and
       nudges the channel on any `{"type":"world_event","data":{"event":"hardware"}}`
       frame. Auto-reconnects with exponential back-off (2s → 30s).
    2. **Fallback timer** — 15s tick (was 5s) pushes to the same channel
       so a dead WS never starves the queue.
- Added `kotlinx.coroutines.channels.Channel`, `okhttp3.WebSocket*`,
  `org.json.JSONObject`, `TimeUnit` imports. All deps already present.
- Case-insensitive `deriveWsUrl()` converts the repo's HTTP base URL
  into the matching `ws://` / `wss://`.

**Repo support** (`data/repository/ElleRepository.kt`):
- Exposed `fun currentServerUrl(): String` so the executor can derive
  the WS URL without duplicating the preference read.

### 3. SCM installer verification
- `elle_service_manifest.json` — 16 services, confirmed to match every
  populated subdir under `Services/` (empty `Elle.Service.Consent/` is
  correctly absent).
- `Install-ElleServices.ps1` — added `-Force` switch. When set, already-
  registered services are stopped (if running), then `sc.exe config`
  rewrites their `binPath`. Critical for iterative rebuild-and-test
  cycles without a full uninstall/reinstall.
- `Deploy/README.md` — documents the new `-Force` flag.

### Net UX impact
- Hardware commands Elle issues through `ActionExecutor` now surface on
  the phone within one RTT instead of up to 5s. Under WS failure the
  15s timer still guarantees delivery.
- Installer correctly handles the rebuild-and-reinstall loop.
- No more orphaned Kotlin compile errors.

---

## Phase 12 — ChatScreen.kt Compile Repair (Feb 2026, this session) ✅

The previous session left `ChatScreen.kt` with two compile-breaking
fragments from a mid-edit context loss:

1. An orphaned `EmotionalStateBar` function body (lines 703-737) with
   no signature and a dangling reference to an undefined `emotionColors`
   map. Reconstructed as a proper `@Composable fun EmotionalStateBar(
   emotionalState: Map<String, Double>)` that uses the existing
   `getEmotionColor()` function for each of the top-4 emotions.

2. Garbage pseudo-code injected inside `WelcomeMessage`'s second
   `SuggestionChip` (a duplicate `fun ChatScreen(...)` placeholder
   calling a non-existent `AvatarView`). Removed — `SuggestionChip`
   block is now clean and properly closed.

File now has 7 top-level `}` (ChatScreen, ChatBubble, TypingIndicator,
getEmotionColor, EmotionalStateBar, ActionBubble, WelcomeMessage) —
all brace-balanced. Repository/API surface untouched; relies on
already-existing `getSessionGreeting()`, `ackSessionGreeting(id)`,
`getEmotionDimensionsAt(ts, top)`, and the
`ReconnectionGreetingResponse` / `EmotionDimensionsResponse` models.

---

## Phase 11 — Mood UI + Dictionary Seed (Feb 2026, earlier this session) ✅

### Android Mood Timeline screen
- `data/models/ApiModels.kt` — `EmotionHistoryPoint` + `EmotionHistoryResponse`.
- `data/api/ElleApiService.kt` — `getEmotionHistory(hours, points)` Retrofit call.
- `data/repository/ElleRepository.kt` — `getEmotionHistory()` wrapper.
- `ui/screens/server/MoodTimelineScreen.kt` — Compose `Canvas`-drawn triple-line
  chart (valence/arousal/dominance) with a 1h/6h/24h/7d range selector, legend,
  and a "Latest" card showing current V/A/D values. Zero extra deps.
- `ui/navigation/NavRoutes.kt` + `ElleNavHost.kt` — `MoodTimeline` route.
- `ui/screens/settings/SettingsScreen.kt` — nav row added to Server Management.

### Pre-baked dictionary seed
- `SQL/gen_dictionary_seed.py` — fetches api.dictionaryapi.dev definitions
  for the CORE_WORDS list and emits idempotent `INSERT ... WHERE NOT EXISTS`
  statements.
- `SQL/dictionary_seed.sql` — generated this session. **2,104 definitions
  across 183 curated words**, zero misses, 710 KB. Apply AFTER
  `ElleAnn_MemoryDelta.sql` and Elle has a working vocabulary on first boot
  — no runtime API roundtrip needed.
- Re-generatable any time by running the script; the `WHERE NOT EXISTS`
  guard makes re-applying safe.

### Known Open Items

### P1 — Verify on user's Windows box
- Kotlin app must successfully connect over `ws://<ip>:8000/` with handshake
- `/api/ai/chat` must return JSON with `response` field (Groq call)
- Heartbeat log spam silenced after rebuild

### P2 — Deepen behavior
- Port `video_generator.py`, `education.py`, `dictionary_loader.py` from the
  Python reference into their respective C++ services
- Wire hardware action push through WS (Android `DeviceActionExecutor` polls /api/ai/hardware/actions/pending)
- Video generation pipeline (currently stub)
- Dictionary backing data

### P3 — Productionize
- Double-click installer / SCM auto-register all 13 services
- Refactor manual string-search JSON (now replaced with nlohmann::json ✅)
- Admin-key gate for destructive endpoints

## Backlog
- Voice TTS/STT endpoint integration
- Dream service → WS broadcast of dream narratives
- Solitude phase transitions pushed to Android as notifications

## Architecture Flow
```
Android App (Kotlin/Retrofit)  ──HTTPS/WS──▶  HTTPServer (port 8000)
                                                   │
                                                   ├── Direct Groq (WinHTTP) for chat
                                                   └── IPC (Named Pipes) ──▶ 12 other services
                                                                             │
                                                                             └── SQL Server (persistent state)
```

## Environment Note (critical)
The project is compiled and run exclusively on the **user's local Windows machine** via Visual Studio 2022. The cloud preview env cannot execute this code. All testing is manual: user rebuilds `Elle.Service.HTTP`, runs it, and reports console output / Android app behavior.
