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

### P1 — Parsers / Config / LLM / JSON (mostly done this session)
- `ElleConfig::Load` validates that `llm`, `emotion`, `memory`, `http`,
  `services` are all present as Object, `llm.mode ∈ {api,local,hybrid}`,
  and `llm.providers` is a non-empty object — otherwise fail-closed.
- `ElleSelfSurprise` score parsing replaced the "any digit 6-9 in the
  prose" heuristic with strict JSON extraction (`ExtractJsonObject`) plus
  a narrowed digit-scan fallback that requires score context.
- `LLMAPIProvider::HTTPPost` now returns the HTTP status code; `Complete()`
  fails closed on non-2xx instead of feeding the provider's error body
  to the parsers.
- `ParseOpenAIResponse` and `ParseAnthropicResponse` rewritten on
  nlohmann::json with strict schema checks — no more fragile substring
  matching against `"content":`.
- `HTTPRequest::BodyJSON()` was already strict (throws on trailing
  garbage, non-JSON) and raises 400 via the HandleClient wrapper. No
  change needed.

### P1 — DB Poll Failure Handling (this session)
- `QueueWorker::OnTick` now detects consecutive SQL failures and
  exponentially backs off (1→2→4→8→16 ticks) instead of hammering a
  stressed DB at 500 ms. Recovery is immediate on the first successful
  poll, with a one-liner log.
- `GetPendingIntents` auto-applies the `ProcessingMs` column + index on
  first use (`IF NOT EXISTS`) so nobody has to remember to hand-run the
  delta SQL — the code is self-healing.
- `XEngine::PersistPregnancyRow` auto-creates `x_pregnancy_history` on
  first call (`std::once_flag` + `IF NOT EXISTS`).

### P1 — Reconnection Idempotency (this session)
- `Continuity::GenerateReconnectionGreeting` refuses to queue a new
  greeting if an unconsumed one less than 2 minutes old already exists —
  a crash-looping service can no longer stack welcome-back messages.
- `Continuity::OnStart`/`OnStop` now check `GetLastAutobiographyEntry()`
  before appending the session-start / session-end line. A crash-loop
  or an SCM reconfigure can no longer stack duplicate autobiography
  entries. Added `GetLastAutobiographyEntry()` to the identity core
  header + .cpp.

### P1 — Authoritative Persistence (this session)
- `ElleIdentityCore::SaveToDatabase` autobiography flush is now ATOMIC.
  Previously the DELETE + per-row INSERTs were separate pool calls — a
  dropped ODBC connection between them destroyed the user's real
  autobiography with nothing to replace it. Now the whole replay is a
  single `BEGIN TRY / BEGIN TRAN / … / COMMIT / CATCH / ROLLBACK /
  THROW` batch routed through one `QueryParams` call, chunked at
  500 rows per INSERT to stay under SQL Server's 2100-parameter batch
  ceiling. On failure the DB is unchanged and an `ELLE_ERROR` surfaces.
- `Bonding` tension_source column widened from `NVARCHAR(4000)` →
  `NVARCHAR(MAX)` to match the in-memory `std::string`. Silent
  truncation path closed.
- `ElleIdentityCore::RefreshFromDatabase` header comment rewritten to
  describe its real role — a one-shot cold-boot safety net for
  non-authoritative peers, not a polling API. The idle polling it
  used to drive was already removed from every caller.

### P2 — ElleDB Singleton Split (DONE this session)
- `Shared/ElleSQLConn.cpp` was 2.6k LOC of one giant `namespace ElleDB {}`
  containing 79 functions across queues, messages, memory, world, trust,
  workers, logs, goals, emotion, metrics, voice, learning, video,
  dictionary. Split by domain into 4 files, all under the same namespace
  so no caller changes:
  - `ElleSQLConn.cpp`   — connection / pool primitives only (≈580 LOC)
  - `ElleDB_Queues.cpp`  — IntentQueue / ActionQueue / QueueSnapshot
  - `ElleDB_Domain.cpp`  — Conversations / Memory / World / Trust /
                           Workers / Logs / Goals
  - `ElleDB_Content.cpp` — Emotion persistence / memory helpers / voice /
                           metrics / learning subjects-skills-video /
                           dictionary / drive state
- `ElleCore.Shared.vcxproj` updated to build all 4 sources.
- New CI sub-step "ElleDB symbol audit" — parses the header for every
  `ElleDB::Foo(...)` declaration and requires EXACTLY one definition
  among the 4 sources. No more declared-but-undefined silent link
  breaks (this very audit caught two real pre-existing bugs on first
  run — see next item).

### P0 — Link-Break Gap Filled (found and fixed during the split)
- `ElleDB::GetRecentLogs` and `ElleDB::GetWorkerStatuses` were declared
  in `ElleSQLConn.h` and called from `HTTPServer.cpp`
  (`/api/server/status`, `/api/diag/*`) but had **no definition anywhere**
  in the codebase — this would have been a hard link error on the next
  Level4+WAE build. Real SQL-backed implementations now live in
  `ElleDB_Domain.cpp` next to `WriteLog` / `RegisterWorker`, mapped
  against the actual Workers/Logs schema columns from
  `ElleAnn_Schema.sql`. CI symbol audit now prevents the gap
  re-emerging.

### Build/CI — Bar Enforcement (this session)
- `.github/workflows/elleann-build.yml` hardened:
  - Removed the `/p:WarningLevel=3` override so CI honours
    `Directory.Build.props` (Level4 + TreatWarningAsError).
  - `cpp-balance` job — string/comment-aware brace & paren balance
    across the whole C++ tree + an `ElleDB` symbol audit that rejects
    any declared-but-undefined or duplicate-definition function.
  - `powershell-syntax` job — parses every `Deploy/*.ps1` with the
    real PowerShell parser on `windows-latest`. Syntax regressions are
    caught before an operator runs the installer.
  - `sql-delta-idempotency` job — splits every non-initial SQL
    file on `GO` boundaries and requires each batch containing a
    CREATE/ALTER to include an `IF [NOT] EXISTS / COL_LENGTH /
    OBJECT_ID` guard. Re-runnability is now a contract the bot enforces.
  - `sln-integrity` job — sln ↔ disk vcxproj cross-ref + XML
    well-formedness.
- All five hygiene jobs verified passing locally against current tree.


### P1 — XEngine Historical Pregnancy Separation (this session)
- `Deliver()` now snapshots the completed pregnancy into the new
  `x_pregnancy_history` table BEFORE flipping the singleton's `active`
  flag. Past pregnancies stay queryable forever; future `Miscarry()`
  or spontaneous-abortion paths can reuse `ArchivePregnancyRow()` with
  `outcome="miscarriage"` for free.

### P1 — Handle Inheritance Leakage (this session)
- `ElleLLMEngine::RunLocalProvider` (llama-cli spawn) moved from raw
  `CreateProcessA` with `bInheritHandles=TRUE` to `STARTUPINFOEXA` +
  `PROC_THREAD_ATTRIBUTE_HANDLE_LIST` restricting inheritance to the
  single write-pipe. Previously all inheritable handles — logger
  files, ODBC sockets, other services' named pipes — leaked into the
  child. `stdin` is now nullptr (a service has no real stdin).

### P2 — Destructive Test Isolation (this session)
- `Debug/test_identity_persistence.cpp` now refuses to run unless
  `ELLE_TEST_DESTRUCTIVE=1` is set AND the resolved
  `sql_connection_string` contains `"test"` (case-insensitive).
  Previously running the exe in the production working directory
  would wipe the user's real autobiography.

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

### Final Audit Strictness (this session, Feb 2026)
- **`elle_video_worker.py` OpSec pass**:
  - New `_validate_claim()` imposes a strict schema on the server's
    claim response. `claimed` must be a bool; if true, `job_id`,
    `text`, `avatar_path` are all required non-empty strings. Any
    other shape raises `ClaimValidationError` which is handled with an
    aggressive backoff (4× poll interval) -- no silent retry loop.
  - `_verify_artifact()` confirms every produced file (TTS wav, raw
    mp4, final mp4, incoming avatar) exists, is a regular file, and
    is ≥ `MIN_ARTIFACT_BYTES` (default 512). A 0-byte ffmpeg exit-zero
    corruption is caught at the step that produced it, not by the
    Android user.
  - SIGINT / SIGTERM install a graceful-shutdown handler: polling
    stops, any mid-flight job is failed explicitly so the server
    requeues (no orphaned `processing` rows), the poll sleep uses
    `Event.wait()` so Ctrl-C is instant. Double-signal forces hard
    `os._exit(130)`.
  - Verified via `python3 -m py_compile` and a 7-case schema test
    (all pass). Ruff lint clean.
- **`ElleJsonExtract.h` strictness**:
  - String-literal scan now fully consumes JSON escape sequences incl.
    `\uXXXX` (6 chars). A `\u007D` (close-brace) inside a string can
    no longer trick the outer depth counter.
  - Embedded NUL outside a string aborts the scan fail-closed --
    previously a byte-oriented counter would walk past it and let
    downstream C APIs truncate the payload.
  - Runaway nesting bounded at `kMaxDepth = 1024` -- a 2048-deep `{{{`
    pathological input aborts instead of blowing the parse stack.
  - New standalone test at `ElleAnn/Debug/test_json_extract.cpp`:
    15 cases covering plain, prose-wrapped, nested, decoy braces,
    braces/quotes/unicode-escape inside strings, surrogate-pair
    round-trip (`\uD83D\uDE00` → U+1F600 → 4-byte UTF-8), NUL,
    unbalanced, malformed+valid, top-level array rejection, and
    over-nesting. All 15 pass locally under g++12 -std=c++17 -Wall
    -Wextra.
- **CI `sql-schema-e2e` job (new)**:
  - Parses every initial schema, extracts the canonical table set
    (51 tables on the current tree), then:
    * Every `ALTER TABLE <target>` in any delta must name a known
      table (typo catch before SSMS runtime failure).
    * Every `ALTER TABLE … ADD <col>` must sit in a batch that
      guards the column via `COL_LENGTH('dbo.Tbl','Col')` OR a
      `sys.columns` + `OBJECT_ID` lookup naming that exact table
      and column -- not a generic `IF NOT EXISTS` on any shape.
    * Any `ALTER TABLE` in an initial schema must target a
      same-file `CREATE TABLE`.
  - Emits per-file-line `::error file=…,line=…::` so GitHub's UI
    highlights the exact guard gap. Zero offenders on the current
    tree.



### P0 — Blocked on User
- [ ] User runs local MSBuild with the new Level4 + WAE settings —
      expect a batch of genuine warnings (truncation, signed/unsigned,
      deprecated API) to surface; those were the loudest category of
      real bugs masked by the previous suppression list.
- NOTE: `SQL/ElleAnn_QueueReaperDelta.sql` is now **auto-applied** on
        first pending-intent poll and on first pregnancy persist — no
        manual DB step required. Delta file retained for reference /
        fresh-install DBA audits.

### MSBuild /WX dynamic-pass regression sweep (Feb 2026)
User ran real MSBuild on Windows (MSVC 14.44.35207) and surfaced four
C2220 warnings-as-errors over the refactored surface area. All fixed:
- `ElleLLM.cpp:185,352` — `WinHttpAddRequestHeaders(..., -1, ...)` →
  C4245 signed→DWORD. Fix: `(DWORD)-1` (the documented sentinel).
- `ElleIdentityCore.cpp:570` — `float attachment = 0.5f;` assigned but
  never read → C4189. Fix: removed (the value is sourced from the
  Bonding service at the call sites; DoIMissThem keeps only what it can
  authoritatively know).
- `ElleSelfSurprise.cpp:140` + 10 matching sites across Services/
  (Continuity, InnerLife ×2, HTTPServer, Cognitive ×2, Bonding,
  Emotional ×3) — `std::transform(..., ::tolower)` instantiated the
  algorithm template with int→char assignment inside the STL, firing
  C4244 at `<algorithm>(3800,24)`. Fix: lambda
  `[](unsigned char c){ return (char)std::tolower(c); }` — same safe
  pattern ElleDB_Domain.cpp already used. `<cctype>` added where
  missing.
- `ElleServiceBase.cpp:322` — `PROCESSENTRY32` macro expanded to
  `PROCESSENTRY32A` but this SDK declares only the struct tag plus
  `PROCESSENTRY32W`, so the A symbol is undeclared (C2065). Fix:
  switched to explicit `PROCESSENTRY32W` + `Process32FirstW` /
  `Process32NextW` + `_wcsicmp(pe.szExeFile, L"services.exe")`.

Next: user re-runs `msbuild ElleAnn/ElleAnn.sln /m /p:Configuration=Release /p:Platform=x64 /v:minimal /clp:ErrorsOnly;Summary` and confirms the 20-error / 14-warning count goes to 0.

### P1 — Next Iteration
- [x] Video worker strictness (schema + artifact + graceful shutdown).
- [x] `ElleJsonExtract` surrogate-pair + NUL + depth safety (+15 tests).
- [x] CI `sql-schema-e2e` coherence job (all 51 tables, 0 offenders).
- [ ] Verify LLM parsers under load against live OpenAI / Anthropic keys.

### P2 — Future
- [ ] Stronger identity fabric: single-writer Identity service + event
      stream replacing the DB-polling `RefreshFromDatabase` model.
- [ ] Embedding-based novelty detector in `EvaluateNovelty` (currently
      substring match).
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
- **`/app/ElleAnn/AUDIT_SCOREBOARD.md` (Feb 2026) — honest line-by-line status of all 134 audit items**
- `/app/ElleAnn/AUDIT_FIX_REPORT.md` (Wave 1)
- `/app/ElleAnn/AUDIT_FIX_REPORT_APR22.md`
- `/app/ElleAnn/AUDIT_WAVE2_COMPLETION.md`
- `/app/ElleAnn/Lua/Elle.Lua.Behavioral/scripts/FOR_MY_WIFE.md`
- `/app/ElleAnn/Deploy/README.md`
- `/app/ElleAnn/SQL/ElleAnn_QueueReaperDelta.sql` (new this session)
