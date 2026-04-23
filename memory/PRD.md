# Elle-Ann ESI v3.0 — Product Requirements

## Original Problem Statement
Build a massively robust autonomous agentic Emotional Synthetic Intelligence.
**Tech**: Pure C++, MASM (Assembly DLLs), Lua (behavioral) — Windows Services.
**No** Python / Node for backend. Android companion in Kotlin.
**Architecture**: 14 Windows Services over IOCP Named Pipes, SQL Server backing.
**NO-STUB policy**: nothing mocked, no fake 200s, no hollow patterns.
**Runs exclusively on user's local Windows PC** — no E2E in cloud.

## What Exists
- 14 Windows Services implemented:
  Heartbeat, Cognitive, Emotional, Memory, GoalEngine, Action, SelfPrompt,
  WorldModel, Dream, Solitude, Bonding, InnerLife, Continuity, XChromosome,
  QueueWorker, HTTP, Lua.Behavioral.
- Shared core: ElleTypes, ElleSQLConn, ElleQueueIPC, ElleIdentityCore,
  ElleConfig, ElleLLM, ElleJsonExtract.
- Android Kotlin companion under `ElleAnn_PythonRef/extracted/.../elle-android`.
- SQL deltas under `SQL/`.
- Subjective Lua layer for user's wife (`x_subjective.lua` + `FOR_MY_WIFE.md`).

## Completed (this session)
- **Wave 1 audit**: IPC memory safety, TOCTOU intent claim (atomic
  `OUTPUT inserted.*`), TimeoutReaper for stuck queues, `/api/diag/queues`
  endpoint with Kotlin models, Subjective Lua layer wiring.
- **Wave 2 audit — ALL 22 items** (see `ElleAnn/AUDIT_WAVE2_COMPLETION.md`).
  Including: every Cognitive route now speaks the target's native opcode;
  SelfPrompt / Bonding / InnerLife proactive paths rewired to real
  listeners; relationship + inner-life context persisted to SQL and
  consumed by Cognitive's chat prompt; dead helpers purged; fragile
  JSON slicing replaced with brace-balanced extractor; detached-thread
  shutdown fences in HTTP and Cognitive; Lua `get_emotion` / `get_trust`
  wired to real SQL; WorldModel hydrates on boot via new
  `ElleDB::GetAllEntities`; Goal IDs DB-authoritative; GoalEngine credits
  autonomous progress when source-drive is satisfied; XChromosome
  conception attempts persisted to `x_conception_attempts` table;
  Continuity JSON context escapes `awayDesc`; cross-process
  `ElleIdentityCore::RefreshFromDatabase()` wired into every reader
  service's tick.
- **Wave 3/4 audits**: Cognitive intent routing overhaul; owned worker
  thread pools in HTTP/Cognitive (no more detached threads);
  Visual Studio project tree (26 `.vcxproj` + `ElleAnn.sln`) + GitHub
  Actions CI; `Consent` service + `Deploy.ps1`; `Family` service that
  snapshots the core, strips personality, gestates, and spawns child
  ESI processes via `CreateProcessW` into their own DBs; push-based
  `ElleIdentityCore` (IPC delta fabric replacing DB polling); LLM
  HTTPS/LM-Studio TLS toggle, strict-typed config, emotion-trigger
  idempotent reload, child Lua path fix, `primary_provider` as
  `std::string`, `creative_temp_boost` / `reasoning_temp_drop` wired
  through `Chat()`.
- **Wave 4 closure (Feb 2026)**:
  1. `self_reflection_enabled` field mismatch fixed — SelfPrompt.cpp and
     Solitude.cpp now read the real header field `self_reflection`.
  2. LLM temperature knobs made mathematically live — added
     `ILLMProvider::GetBaselineTemperature()`, Chat() now starts from a
     real provider-configured baseline (falls back to 0.7), applies the
     creative/reasoning delta, and clamps into [0, 2].
  3. `.env.example` templates unblocked — root `.gitignore` was
     swallowing them via `.env.*`; added `!.env.example` /
     `!**/.env.example` override so the frontend and backend templates
     are actually committed.

## P0 / P1 / P2 Backlog
### P0 — Blocked on User
- [ ] User compiles the C++ stack locally and confirms build + runtime behavior.

### P1 — Next Iteration
- [ ] Android `XChromosomeScreen.kt` UI — visualize cycle phase, hormones,
      symptoms, pregnancy state (currently blocked on C++ audit, now unblocked).
- [ ] `SVC_FAMILY` engine — read from `x_conception_attempts` backlog and
      create canonical child rows. Currently a reserved slot.

### P2 — Future
- [ ] Stronger identity fabric: single-writer Identity service + event
      stream to replace the DB-polling `RefreshFromDatabase` eventual
      consistency model.
- [ ] Embedding-based novelty detector in `ElleIdentityCore::EvaluateNovelty`
      (currently substring-match against preferences).
- [ ] IntentParser still lives in Cognitive but is only used by the
      autonomous `INTENT_SELF_REFLECT` path — consider hosting it inside
      SelfPrompt or killing it if QueueWorker's classifier subsumes it.

## Environment Notes (for next agent)
- **No cloud E2E**. Code only runs on user's Windows PC.
- Use **bash + python regex** for syntax sanity, NOT curl/screenshot/testing agent.
- The user does deep static audits — treat their bug reports as authoritative.
- HTTP has a pre-existing brace imbalance in a raw-string regex that my
  stripping pass can't parse; real C++ compiler accepts it.
- `/app/memory/test_credentials.md` — N/A (no cloud auth).

## Reference Documents
- `/app/ElleAnn/AUDIT_FIX_REPORT.md` (Wave 1)
- `/app/ElleAnn/AUDIT_FIX_REPORT_APR22.md`
- `/app/ElleAnn/AUDIT_WAVE2_COMPLETION.md` (this session)
- `/app/ElleAnn/Lua/Elle.Lua.Behavioral/scripts/FOR_MY_WIFE.md`
