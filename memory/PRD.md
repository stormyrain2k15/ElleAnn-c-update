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

## Known Open Items

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
