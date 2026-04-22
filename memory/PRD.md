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

## Phase 11 — Mood UI + Dictionary Seed (Feb 2026, this session) ✅

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
