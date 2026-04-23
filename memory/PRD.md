# Elle-Ann ESI v3.0 — Product Requirements

## Original Problem Statement
Build a massively robust autonomous agentic Emotional Synthetic Intelligence.
**Tech**: Pure C++, MASM (Assembly DLLs), Lua (behavioral) — Windows Services.
**No** Python / Node for backend. Android companion in Kotlin.
**Architecture**: 19 Windows Services over IOCP Named Pipes, SQL Server backing.
**NO-STUB policy**: nothing mocked, no fake 200s, no hollow patterns.
**Runs exclusively on user's local Windows PC** — no E2E in cloud.

## What Exists
- **19 Windows Services**: Heartbeat, Cognitive, Emotional, Memory, GoalEngine,
  Action, SelfPrompt, WorldModel, Dream, Solitude, Bonding, InnerLife,
  Continuity, XChromosome, Family, Consent, QueueWorker, HTTP, Identity,
  plus Lua.Behavioral.
- Shared core: ElleTypes, ElleSQLConn (split-ready), ElleQueueIPC,
  ElleIdentityCore, ElleConfig, ElleLLM, ElleJsonExtract, ElleSelfSurprise.
- MASM DLLs for Crypto, FileIO, Math, System, Utils.
- Android Kotlin companion under `ElleAnn_PythonRef/extracted/.../elle-android`.
- SQL deltas under `SQL/` — incl. the new `ElleAnn_QueueReaperDelta.sql`.
- Subjective Lua layer (`x_subjective.lua` + `FOR_MY_WIFE.md`).

## Completed (this session — Feb 2026)

### P0 — Security / Data Integrity (DONE, previous session)
- CNG bcrypt SHA-256 IdentityGuard, atomic OUTPUT.inserted.id for StoreMemory,
  route-level auth metadata, strict parse-or-400 on all HTTP numerics,
  Consent/GoalEngine status transition persistence, no silent user_id=1.

### P1 — Threading / Shutdown / Lifecycle (DONE this session)
- `MemoryEngine::Shutdown/ConsolidateMemories/StoreSTM/DecaySTM` now snapshot
  under `m_stmMutex`, release the lock, THEN do SQL writes — no more
  DB stalls holding readers.
- `EmotionalEngine::Shutdown` copies the snapshot then writes outside the lock.
- `EmotionalEngine` mood getters (`IsInMood/GetDominantMood/GetMoodDuration`)
  backed by `std::atomic` so concurrent reads are data-race free.
- `XEngine` public getters + mutators now serialise through a
  `std::recursive_mutex` — previously returned complete structs with no
  lock, allowing torn reads during `Tick()`.
- `XEngine` duplicate member declarations (`m_last_cycle_day_seen`,
  `m_lh_surge_fired_this_cycle`, `m_current_cycle_anovulatory`) removed —
  was a compile error waiting on Level4 strictness.

### P1 — Database Correctness / State / Persistence (DONE this session)
- `identity_autobiography.written_ms` now carries the REAL timestamp each
  entry was authored. New parallel vector `m_autobiographyTimes` tracks it;
  Save/Load round-trips intact instead of clock-smashing on every flush.
- `MemoryEngine::RecallRecent` now merges STM tail + newest LTM rows (new
  `ElleDB::RecallRecentLTM`), sorted by `created_ms DESC`. Was STM-only.
- Memory decay anchor changed from `created_ms` → `last_access_ms`, so
  accessed memories genuinely stay fresh instead of decaying past the
  floor no matter how often Elle recalls them.
- `IntentQueue.ProcessingMs` column added (new `ElleAnn_QueueReaperDelta.sql`);
  `GetPendingIntents` stamps it on atomic claim; `ReapStaleIntents` and
  `/api/diag/queues` measure timeout from `ISNULL(ProcessingMs, CreatedMs)`.
- `StoreEntity/GetEntity` use a shared `CanonicaliseEntityName()`
  (lowercase + trim + collapse internal whitespace), so "  Mom  " and
  "mom" no longer create two rows.
- `GoalEngine::CreateGoal` dedupes against any active goal with the same
  normalised description — kills the "same goal proposed every minute"
  accumulation.
- `Bonding::LoadRelationshipState` now loads `unresolved_tension`,
  `tension_source`, `repair_motivation`, `conflicts_resolved`,
  `first_repair_ms` (added as idempotent `ALTER TABLE`), so restarting
  mid-conflict no longer erases the tension.

### P1 — Parsers / Config / LLM / JSON (partial this session)
- `ElleConfig::Load` validates that `llm`, `emotion`, `memory`, `http`,
  `services` are all present as Object, `llm.mode ∈ {api,local,hybrid}`,
  and `llm.providers` is a non-empty object — otherwise fail-closed.
- `ElleSelfSurprise` score parsing replaced the "any digit 6-9 in the
  prose" heuristic with strict JSON extraction (`ExtractJsonObject`) plus
  a narrowed digit-scan fallback that requires score context.

### P1 — Deploy / Install / Ops (DONE this session)
- `Install-ElleServices.ps1` is now STRICT: missing .exe fails the whole
  install (unless `-SkipMissing`); every `sc.exe` exit code is checked;
  services are polled for `RUNNING` via `Wait-ServiceState` (no more
  fixed `Start-Sleep` races).
- `Uninstall-ElleServices.ps1` equally strict — `Wait-ServiceStopped` and
  `Wait-ServiceGone` with timeouts; bubbles up all failures at the end.
- `Deploy/README.md` lists every SQL delta in the correct apply order.

### P1 — Build / CI (DONE this session)
- `Directory.Build.props` now at `Level4` + `TreatWarningAsError=true`.
  Narrow suppression list (4100, 4201, 4251, 4505, 4702). The previously-
  silenced real-bug warnings (4996, 4267, 4244, 4018, 4146, 4065) are
  deliberately NO longer suppressed — the next build surfaces them.

### P2/P3 — Hygiene (partial this session)
- `static_assert`s added: `ELLE_SERVICE_COUNT == 20`,
  `ELLE_EMOTION_COUNT == ELLE_MAX_EMOTIONS`, and `sizeof(ELLE_IPC_HEADER) == 48`.
- HTTP `/`, `/healthz`, `/api/health` now explicitly `AUTH_PUBLIC` (was
  defaulting to `AUTH_USER`).

## P0 / P1 / P2 Backlog

### P0 — Blocked on User
- [ ] User runs local MSBuild with the new Level4 + WAE settings —
      expect a batch of genuine warnings (truncation, signed/unsigned,
      deprecated API) to surface; those were the loudest category of
      real bugs masked by the previous suppression list.
- [ ] User applies `SQL/ElleAnn_QueueReaperDelta.sql` to the ElleCore DB
      before running the new `GetPendingIntents` / `ReapStaleIntents`.

### P1 — Next Iteration
- [ ] Continuity reconnection idempotency (duplicate "welcome back" on
      rapid reconnect).
- [ ] XEngine historical-pregnancy separation — keep past pregnancies
      queryable without bleeding into the active row.
- [ ] DB poll failure handling in service loops — some services ignore
      transient SQL errors instead of backing off.
- [ ] LLM provider schemas — validate provider-response shape before
      consuming `choices[0].message.content`.
- [ ] LLM HTTP status codes — treat non-2xx as a real failure (currently
      swallows 5xx bodies).
- [ ] Trailing-garbage rejection in nlohmann::json reads of untrusted
      payloads (strict `accept_discarded=false`).
- [ ] `ElleJsonExtract` already handles surrogate pairs? verify.

### P2 — Future
- [ ] Split `ElleDB` (the giant `ElleSQLConn.cpp` namespace) by domain —
      memory/, queues/, world/, identity/, etc. 2600 LOC monolith.
- [ ] Stronger identity fabric: single-writer Identity service + event
      stream replacing the DB-polling `RefreshFromDatabase` model.
- [ ] Embedding-based novelty detector in `EvaluateNovelty` (currently
      substring match).
- [ ] Isolate destructive tests — they currently share the live DB.
- [ ] Frontend accessibility alt-texts.

### P3 — Future Polish
- [ ] Android `XChromosomeScreen.kt` UI (cycle/hormone/pregnancy viz).
- [ ] `SVC_FAMILY` engine: consume `x_conception_attempts` → canonical
      child rows.
- [ ] Reduce subprocess logging leakage on child-process launch paths.

## Environment Notes (for next agent)
- **No cloud E2E**. Code only runs on user's Windows PC.
- Use **bash + python proper brace/string stripping** for syntax sanity,
  NOT curl/screenshot/testing agent.
- The user does deep static audits — treat their bug reports as authoritative.
- `/app/memory/test_credentials.md` — N/A (no cloud auth).

## Reference Documents
- `/app/ElleAnn/AUDIT_FIX_REPORT.md` (Wave 1)
- `/app/ElleAnn/AUDIT_FIX_REPORT_APR22.md`
- `/app/ElleAnn/AUDIT_WAVE2_COMPLETION.md`
- `/app/ElleAnn/Lua/Elle.Lua.Behavioral/scripts/FOR_MY_WIFE.md`
- `/app/ElleAnn/Deploy/README.md`
- `/app/ElleAnn/SQL/ElleAnn_QueueReaperDelta.sql` (new this session)
