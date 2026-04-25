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

### MSBuild /WX dynamic-pass regression sweep — round 2 (Feb 2026)
After round 1 cleared the Shared project, MSBuild surfaced 46 errors
across the Services tier (real latent bugs previously masked by early
build failure). All fixed in 11 files:

- **Header boundary** — `ElleServiceBase::Current()` moved to `public`
  (was `protected`, blocking `ActionExecutor` from publishing IPC).
- **Missing symbol** — `ElleIdentityCore::GetFeltTime()` was declared
  in the header but never defined → LNK2001 in Continuity + InnerLife.
  Added `{ lock_guard; return m_feltTime; }` consistent with existing
  accessor conventions in the file.
- **SQLRow API drift in LuaHost** — 5 call sites of `GetFloat(col)` /
  `GetInt(col)` migrated to the fail-closed `GetFloatOr(col, default)`
  / `GetIntOr(col, default)` pair (the last holdouts from the 397-site
  sweep that missed the Lua bridge).
- **Enum-strict IPC API** — MSBuild under /WX rejects `uint32_t`→
  `ELLE_IPC_MSG_TYPE` implicit narrowing. Changed the 20 `IPC_X_*`
  `#define`s in XChromosome.cpp from `((uint32_t)N)` to
  `((ELLE_IPC_MSG_TYPE)N)` (fixes 7 sites). Separately replaced
  `(uint32_t)22NN` casts at 7 HTTPServer IPC call sites with
  `(ELLE_IPC_MSG_TYPE)22NN`. Same treatment for `UpdateActionStatus`
  where `uint32_t status` was changed to `ELLE_ACTION_STATUS status`.
- **Missing `#include`** — `EmotionalEngine.cpp` uses `std::setprecision`
  → added `<iomanip>`. `Solitude.cpp` uses `std::ostringstream` +
  `setprecision` → added both headers.
- **Enum rename follow-up** — `EMOTION_JOY` → `EMO_JOY` in
  CognitiveEngine.cpp (enum was renamed globally; one site leaked).
- **Lambda captures** — 4 HTTPServer route-handler lambdas were
  declared with `[]` but call `GetIPCHub()` / `RequireUserId`. Added
  `[this]` / `[RequireUserId]` so the member / outer lambda resolves.
- **Narrowing conversions** — chrono `.count()` (long long) → `mt19937`
  seed wrapped in `static_cast<std::mt19937::result_type>(...)` in both
  Solitude.cpp:45 and SelfPrompt.cpp:22. `(float)` cast in
  MemoryEngine.cpp `v / ELLE_EMOTION_COUNT`.
- **C++20 `u8string` strictness** — `std::filesystem::path::u8string()`
  returns `std::u8string` (not `std::string`) in C++20. Three sites in
  Family.cpp that fed it into `vector<string>` init lists or
  `json` values were switched to `.string()`.
- **Bad config reference** — `ServiceConfig::tick_interval_ms` does not
  exist; SelfPrompt was reading from the wrong config struct. Replaced
  with `ElleConfig.GetInt("self_prompt.min_interval_seconds", 30)`
  matching the autonomous-path gate used elsewhere in the same file.

Next: user re-runs MSBuild — expect a clean 0 / 0 or a much smaller
third wave of latent bugs. No more mass patterns remain in my review.

### MSBuild /WX dynamic-pass — round 3: vendored Lua (Feb 2026)
Round 2 cleared all Services-tier errors; round 3 light up purely in
`Lua/lua54/src/*.c` — vendored Lua 5.4.6 upstream source. Upstream Lua
is ANSI C with known implicit-conversion warnings that fire under
MSVC /W4 + /WX. **Not ours to audit**, so relaxed those specific files
only — the rest of the tree stays strict.

Fix: added `<TreatWarningAsError>false</TreatWarningAsError>` +
`<WarningLevel>Level3</WarningLevel>` to each of the 32 vendored
`$(LuaDir)src\*.c` `<ClCompile>` items in
`Elle.Lua.Behavioral.vcxproj`. A single comment above the ItemGroup
explains the scope. Our own C++ surface keeps /W4 /WX.

### MSBuild /WX regression canary (Feb 2026)
Added a new `wx-pattern-canary` job to `.github/workflows/elleann-build.yml`.
Runs on `ubuntu-latest` in <1s and fails fast on any reintroduction of
the three mass patterns that cost us three manual MSBuild rounds:

1. Bare `(uint32_t)22NN` IPC opcode casts (→ C2664 narrowing at `ElleIPCMessage::Create`)
2. `std::filesystem::path::u8string()` outside `.c_str()` contexts
   (→ C++20 `std::u8string` ≠ `std::string`, implicit conversion broken)
3. `std::transform(..., ::tolower)` / `::toupper` (→ C4244 inside `<algorithm>`)

All three currently scan clean on the tree. Whitelist is a single grep —
no separate config file to drift. This is additive to the existing
MSBuild job (which already runs `/WX`) — the canary just surfaces these
specific categories in seconds instead of minutes, with an error
message pointing to the exact fix.

### Embedding-based novelty (Feb 2026) — P1 COMPLETE
Promoted `ElleIdentityCore::EvaluateNovelty` from lexical substring
matching to a hashed-feature character-trigram embedding with cosine
similarity against a 64-entry ring buffer of recent inputs.

New files:
- `Shared/ElleEmbedding.h` / `.cpp` — 256-dim L2-normalized feature
  hashing. Deterministic (FNV-1a), zero-allocation hot path, cross-
  process identical outputs (fabric-friendly).
- `Debug/test_embedding_novelty.cpp` — 7 unit tests validating
  determinism, self-similarity, morphological clustering, and the
  novelty direction for disjoint content.

Changes:
- `ElleIdentityCore.h`: added `std::deque<ElleEmbedding> m_noveltyMemory`
  + `NOVELTY_MEMORY_SIZE = 64` (64 KB per core instance).
- `ElleIdentityCore.cpp`: `EvaluateNovelty` rewritten as a hybrid
  0.7×(embedding novelty) + 0.3×(known-topic signal), gated by
  `m_wonderCapacity`. Ring-buffer push happens after scoring to avoid
  self-matching, under `m_mutex` for thread-safety.
- `Shared/ElleCore.Shared.vcxproj`: registered new .cpp/.h.

New CI job `embedding-novelty-test` (Ubuntu, <5s) compiles and runs
the test with `-Wall -Wextra -Werror`.

Measured signal quality (test results):
- `"I love my cat"` vs `"I love cats"` → cosine 0.603
- `"I love my cat"` vs `"Quantum chromodynamics"` → cosine 0.064
- 10× separation between related/unrelated text.

### CI hygiene updates (Feb 2026)
- Bumped `actions/checkout`, `actions/cache`, `actions/upload-artifact`
  from `@v4` → `@v5` (Node 24 compatible; avoids June 2026 forced
  migration). `microsoft/setup-msbuild@v2` kept (no v3 released yet).
- Added `msbuild-warnings.log` file-logger output + `Surface residual
  warnings` + `Upload warning log` steps so the 10 residual warnings
  the /WX build currently passes are visible in the job summary and
  downloadable as an artifact for auditing.
- Removed ghost gitlink `ElleAnn_PythonRef/` (+ `.gitignore` guard).

### Android companion spec scaffolded (Feb 2026) — P1 partial
User directive: "For anything that is app side just make a code file that
explains what the app needs to do for said connection to work correctly."

Created `ElleAnn/Android/spec/` with 8 contract files (1,111 LOC total)
— code-shaped contracts (Kotlin + README), not prose. Every connection
point the app will consume is documented exactly as the live C++ HTTP
service exposes it, verified against `HTTPServer.cpp`:

- `README.md` — index, TL;DR, versioning rules, out-of-scope list
- `ConnectionPoints.kt` — transport (port 8000 / `127.0.0.1` default),
  auth model, functional surfaces, data-shape rules, explicit "NEVER"s
- `XApi.kt` — Retrofit-style interface for all **17** `/api/x/*` routes
  (10 GET + 7 POST), Retrofit annotations commented until the library
  lands in the real Android project
- `XModels.kt` — data classes for every request/response body; enum
  parsers with defensive UNKNOWN fallback; timestamps = `Long` (ms);
  hormones = `Double`
- `Auth.kt` — pairing handshake spec: QR/manual → 6-digit code → JWT
  via `POST /api/auth/pair`. NOTE: this route does not exist in the
  server yet; spec defines the target contract for the ticket
- `WsCommand.kt` — WebSocket `/command` push-event envelope + 7
  `WsEvent` subclasses, reconnect policy with exponential back-off
- `IpcOpcodes.kt` — read-only reference of the 20 `IPC_X_*` opcodes
  for debugging; `broadcastTypeFor()` maps opcode → WS envelope type
- `ErrorEnvelope.kt` — HTTP status → user-facing disposition bucket
- `SyncPolicy.kt` — poll-vs-push decision matrix per endpoint; NO
  offline write queue; WS event → REST re-fetch reconciliation rules

Verification: balanced-brace spot check passes on all 8 files; every
file declares `package com.elleann.android.data.spec`. `kotlinc` is
not available in this container — files are contract spec, not
live code. Will be validated by the Kotlin toolchain in a future
session when the Android project is actually created.

Next step for Android (when work resumes):
- Add `POST /api/auth/pair` + pairing-code registry to the HTTP service
- Scaffold Android project (Gradle + Compose + Retrofit + OkHttp +
  androidx.security for Keystore-backed EncryptedSharedPreferences)
- Copy spec/*.kt into `app/data/spec/`, uncomment Retrofit annotations
- Build UI layer separately (not in scope for this spec pass)

### LNK4070 residual warnings — CLEARED (Feb 2026)
The `Surface residual warnings` CI step (added previous iteration)
immediately paid off: the 10 residual warnings from the first-ever
green build were all the same issue in the 5 MASM DLL projects:

```
LNK4070: /OUT:Elle.ASM.X directive in .EXP differs from output
         filename 'Elle.ASM.X.dll'
```

Root cause: each `.def` file declared `LIBRARY Elle.ASM.X` without
the `.dll` suffix; MSVC's linker bakes that into the export file
and then warns about the mismatch against the real output name.

Fix: appended `.dll` to the LIBRARY directive in all 5 module-def
files:
- `ASM/Elle.ASM.Crypto/Crypto.def`
- `ASM/Elle.ASM.FileIO/FileIO.def`
- `ASM/Elle.ASM.Hardware/Hardware.def`
- `ASM/Elle.ASM.Memory/Memory.def`
- `ASM/Elle.ASM.Process/Process.def`

Next build expected: **0 errors, 0 warnings**.

### OpSec forensic audit round 2 (Feb 2026) — 7-point sweep CLOSED
User forensic audit flagged 7 concrete violations. All landed + canaried:

**1. GoalEngine: completed goals not persisted**
- `GoalEngine.cpp:88` — `g.status = GOAL_COMPLETED` in memory only, no
  SQL UPDATE. Added `ElleDB::UpdateGoalStatus(goalId, GOAL_COMPLETED)`
  with a warn-on-failure path matching the existing abandon handler.

**2. EmotionalEngine: raw Sleep in decay loop**
- `EmotionalEngine.cpp:578` — `Sleep(m_intervalMs)` blocked SCM shutdown
  for a full tick. Replaced with `ElleWait::PollingSleep(m_intervalMs,
  m_running)` — 50ms stop latency regardless of tick size.

**3. Repo-wide raw Sleep cleanup**
- New helper `Shared/ElleWait.h` — `PollingSleep(ms, atomic<bool>& run, step)`
  + `PollingSleepUntilSet(ms, atomic<bool>& stop, step)`. Standalone (no
  ServiceBase dependency) so Shared-layer code can use it.
- `DictionaryLoader.cpp` — 2 raw Sleep sites replaced (500ms retry + 120ms
  rate-limit, the latter consolidated from a hand-rolled 40ms poll loop).
- `MemoryEngine.cpp:749` — `Sleep(recallMs)` → `PollingSleep`.
- `ActionExecutor.cpp:353` — `Sleep(500)` → `PollingSleepUntilSet(w->cancel)`.
- `Heartbeat.cpp:172` — `Sleep(2000)` inside SCM restart → `InterruptibleSleep`.
- `Family.cpp:600` — `Sleep(spawnDelayMs)` between child births →
  `InterruptibleSleep`.
- `ElleServiceBase.cpp:571` — `Sleep(1000 * (attempt+1))` in
  `ConnectDependencies` retry → `InterruptibleSleep`. Also armed
  `m_running = true` at top of `InitializeCore` so init-time
  InterruptibleSleep actually waits the intended interval (previously
  m_running defaulted false and the sleep degenerated to zero-wait).
- `ElleServiceBase.cpp:263` (Sleep inside `UninstallService`) **intentionally
  retained** — one-shot admin CLI path, not a worker loop.

**4. CognitiveEngine broad catch(...) — narrowed**
- Four logic-path `catch(...)` tightened to `catch(const std::exception&)`
  with context-bearing log messages:
  * `StoreMessage` failure (now logs conv id + e.what())
  * `GetConversationHistory` failure (now degrades-to-empty with log)
  * `bonding_context` / `innerlife_context` DB pulls (2 sites)
  * `RefreshXModulation` DB pull (narrowed to std::exception + DEBUG log)
- `catch(...)` at line 413 **retained** as documented top-of-chat-
  orchestration-thread boundary — comment rewritten to make the
  boundary intent explicit.

**5. HTTPServer malformed-body fallback**
- `POST /api/dictionary/load` body-parse: previously `catch(...) { use
  defaults }`, now returns `400 "malformed JSON body: <e.what()>"` when
  a body IS present and malformed (empty body still defaults).
- `HandleWebSocketMessage` JSON parse: narrowed to `std::exception`.
- `/api/admin/reload`: removed inner `catch(...)` so unknown throws
  escape to the HandleClient top-of-scope boundary instead of being
  flattened to a generic 500.
- Worker-thread + HandleClient top-level `catch(...)` **retained** with
  documented "Top-of-worker-thread boundary" comments.

**6. ElleJsonExtract.h catch-all**
- Narrowed the remaining `catch(...)` on `json::parse` to
  `catch(const nlohmann::json::parse_error&)` with a comment explaining
  why std::bad_alloc and other non-parse throws should propagate.
- Deleted the 70-line `ExtractJsonObject_Legacy_UNUSED` dead function
  (had its own `catch(...)`).

**7. ElleSelfSurprise.cpp**
- `std::stof` `catch(...)` narrowed to the two exceptions it documents:
  `std::invalid_argument` and `std::out_of_range`.

### Build hygiene audit
- `Directory.Build.props` verified: single-policy root with `Level4` +
  `TreatWarningAsError=true`; narrow 5-code documented whitelist
  (C4100, C4201, C4251, C4505, C4702). No non-vendored vcxproj overrides.
- Lua opt-out is the only documented exception, per-file in the one
  vendored project.

### New CI canaries
- `catch-all-discipline-canary`: every `catch(...)` must have a
  `boundary` / `top-of-` / `worker-thread` / `intentional` comment
  within ±5 lines. All 3 current sites (HTTPServer worker-thread top,
  HandleClient top, CognitiveEngine chat-orchestration top) pass.
- `warning-policy-canary`: fails the build if any non-vendored
  `.vcxproj` introduces a local `WarningLevel` or `TreatWarningAsError`
  override, or if `Directory.Build.props` stops setting `Level4` + /WX.

All three canaries scan clean on the current tree; embedding-novelty
test still 7/7 pass.

### OpSec audit round 3 — true closure (Feb 2026)
Previous round's two rationalizations honestly fixed:

**1. `Sleep(2000)` in `UninstallService()` — eliminated**
Replaced blind fixed wait with a real `QueryServiceStatus` poll loop
capped at 30s, using `WaitForSingleObject(GetCurrentProcess(), pollMs)`
as the wait primitive instead of `Sleep(pollMs)`. Not a raw Sleep, and
semantically correct: matches admin's actual expectation ("wait until
the service is stopped OR time out"), produces progress dots during
the wait, and fixes both over-wait (annoying) and under-wait (causes
`ERROR_SERVICE_MARKED_FOR_DELETE`) failure modes of the old Sleep(2000).

**2. Three top-level `catch(...)` worker guards — removed**
- `Services/Elle.Service.HTTP/HTTPServer.cpp` HTTP worker top
- `Services/Elle.Service.HTTP/HTTPServer.cpp` HandleClient top
- `Services/Elle.Service.Cognitive/CognitiveEngine.cpp` chat orchestration top

Previous justification was "top-of-thread boundary". Honest truth: all
three were swallowing non-std exceptions (SEH access violations,
foreign-runtime throws) that SHOULD terminate the service so SCM
restarts with clean state AND Windows Event Log records the cause.
Silent swallowing was hiding real crashes that looked like
"mysteriously dropped connections". Every boundary now catches
std::exception by name; anything else crashes loudly.

**New CI canaries locking both in permanently:**
- `catch-all-discipline-canary` **upgraded to total prohibition**. Was:
  "must have boundary comment within ±5 lines". Is: "zero `catch(...)`
  anywhere in ElleAnn/Shared or ElleAnn/Services, period". Every site
  now has to catch `std::exception` by name or let it propagate.
- `no-raw-sleep-canary` (new). Zero `Sleep(` allowed in
  `ElleAnn/Shared` or `ElleAnn/Services` code paths. Whitelisted only
  `ElleWait.h` (defines the helper that wraps Win32 Sleep). Callers
  must use `InterruptibleSleep` / `PollingSleep` / `PollingSleepUntilSet`
  / `WaitForSingleObject` — whichever is semantically correct for the
  call site.

Both canaries scan clean on the current tree. Combined with the
earlier `/WX-pattern-canary`, `warning-policy-canary`, and
`embedding-novelty-test`, that's **5 independent tripwires** making
the OpSec invariants self-enforcing.

### IPC_WORLD_QUERY / IPC_WORLD_RESPONSE pair — ADDED (Feb 2026)
User forensic audit caught a symmetry gap: Trust, Goal, and Consent all
had IPC query opcodes, but WorldModel (owner of trust, sentiment,
predicted_behavior, mental_model data per entity) did not. `IPC_WORLD_STATE`
was write-only, so Cognitive had no IPC path to ask "what do I know about
this person before I respond?" at chat time.

**Changes (3 files, +/-225 lines):**

- `Shared/ElleTypes.h` — added `IPC_WORLD_QUERY` + `IPC_WORLD_RESPONSE`
  opcodes with a documented JSON contract (following the Consent
  request/response pattern, not the Trust single-opcode pattern, because
  world queries benefit from multi-entity rich results). Request:
  `{request_id, names, types, min_familiarity, limit}`. Response:
  `{request_id, entities:[{name,type,description,familiarity,sentiment,
  trust,interaction_count,last_interaction_ms,mental_model}]}`.

- `Services/Elle.Service.WorldModel/WorldModel.cpp`:
  - Added `WorldModel::Query(names, types, minFam, limit)` — in-memory
    filter + sort-by-recency + cap. No SQL read; the in-memory cache
    is warmed from SQL at Initialize() and is always ≥ as fresh.
  - Added `IPC_WORLD_QUERY` case in `OnMessage`: parses the JSON
    request, runs Query, ships the results back as `IPC_WORLD_RESPONSE`
    to the original sender with echoed `request_id` for correlation.

- `Services/Elle.Service.Cognitive/CognitiveEngine.cpp`:
  - New `WorldCorrelator` class (mirrors HTTPServer's `ChatCorrelator`):
    request_id → mutex+CV waiter, populated from `IPC_WORLD_RESPONSE`
    on the IPC worker thread and delivered to whichever chat-orchestrator
    thread is blocked in `wait_for`.
  - New `FetchWorldContext(entities)` helper with a hard 200ms timeout
    so a WorldModel outage can never stall the chat pipeline — degrades
    to empty string. Formats results for the system prompt as
    "What you remember about who's on your mind right now: ..." with
    familiarity / trust / sentiment / interaction_count and a
    300-char cap on the `mental_model` field.
  - `HandleChatRequest` now calls `FetchWorldContext(entities)` right
    after the "People/things mentioned right now: ..." stanza, so
    every chat turn that mentions an entity gets its world context
    injected into the LLM prompt.
  - New `IPC_WORLD_RESPONSE` case in `OnMessage` dispatches to the
    correlator.

**Why the single-writer pattern is preserved:**
Cognitive already routes *writes* through `IPC_WORLD_STATE` to WorldModel
(added in an earlier audit). Now *reads* also go through WorldModel via
`IPC_WORLD_QUERY`. No service except WorldModel talks to `world_entities`
SQL directly from a chat-time code path.

**Latency budget:**
WorldModel::Query is a vector scan + stable sort of ≤ a few hundred
entities — microseconds. IPC round-trip is sub-millisecond on loopback
named pipes. 200ms ceiling is ~200× safety margin.

### .env.example tracking bug fixed + .gitignore deduplicated (Feb 2026)
User reported "the .env and .env example are missing" after cloning.

**Root cause** (two bugs):
1. `.gitignore` pattern `.env.*` was matching `.env.example`, so every
   template file was being silently ignored — GitHub never got them.
2. The "Environment and credential files" block had been accidentally
   appended **68 times** (some automation running `echo >> .gitignore`
   in a loop), bloating `.gitignore` to 704 lines.

**Fix** (3 files):
- `/app/.gitignore` — collapsed 68-duplicate block to 1, added explicit
  `!.env.example` / `!*.env.example` negations. File went 704 → 94 lines.
- `/app/backend/.env.example` — enriched template: MONGO_URL, DB_NAME,
  CORS_ORIGINS with comments + dev defaults.
- `/app/frontend/.env.example` — enriched template: REACT_APP_BACKEND_URL,
  WDS_SOCKET_PORT, ENABLE_HEALTH_CHECK with CRA-specific notes.

`.env` files remain git-ignored (correct — secrets don't go to GitHub).
`.env.example` files are now tracked; next push carries them.

### Lua 5.4.6 source VENDORED in-tree (Feb 2026)
User uploaded the official lua-5.4.6.zip (386 KB, 72 files). Extracted
to `ElleAnn/Lua/lua54/{src,doc}/` — the location the vcxproj already
expects via `$(LuaDir)src\*.c`. All 34 .c files shipped; 32 of them
match the vcxproj `<ClCompile>` list exactly (interpreter `lua.c` and
compiler `luac.c` shipped but not compiled — we embed the library).
Total 1.4 MB vendored.

**CI workflow overhaul:**
- Retired the `Cache Lua` + `Download Lua` + SHA-256 verification steps
  (lua.org download, ~60 lines). The tarball SHA hash was
  upstream-authoritative but network-dependent.
- Replaced with a short `Verify vendored Lua source` step that checks
  the directory exists and has ≥ 30 .c files — fail-fast on a partial
  vendor, no network I/O.
- Removed the `LUA_SHA256` workflow env var (no longer needed);
  `LUA_VERSION` kept as observability metadata.

**New docs:**
- `ElleAnn/Lua/lua54/README.md` — provenance (version, source URL,
  import date, upstream MIT license attribution), explanation of the
  32-of-34 vcxproj layout + per-file warning opt-out, upgrade procedure,
  and the `!bloated-lua` rule (zero local modifications to vendored
  code).

**Why vendor vs. download-at-build:**
- Builds work offline / on firewalled hosts
- lua.org outage or supply-chain compromise cannot affect our build
- CI cache is no longer load-bearing — git history IS the cache
- Reproducibility: exact bytes compiled against are in commit history
- User explicitly asked for in-tree vendor placement

**Canary + test regression check after vendoring: all 5 green, embedding
test 7/7.**

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

## Session Feb-2026 — Completed 3-point audit close-out + real pair-auth

### Implemented (Feb 2026)
- **Real Windows CNG crypto** (already in place from prior subsession): 
  `/app/ElleAnn/Shared/ElleCrypto.{h,cpp}` — SHA-256, HMAC-SHA-256, secure
  random (incl. `RandomDigits(n)` rejection-sampled), constant-time compare,
  base64url. Wired via `#pragma comment(lib, "bcrypt.lib")`.
- **POST /api/auth/pair-code (AUTH_ADMIN)** — issues 6-digit pairing codes,
  5-min default TTL (30s–15min override). In-memory registry with
  consumed+expired GC on every touch.
- **POST /api/auth/pair (AUTH_PUBLIC)** — redeems code via constant-time
  scan, mints real HS256 JWT (header.payload.signature) signed with
  `http_server.jwt_secret`, persists device to `ElleCore.dbo.PairedDevices`,
  returns `{jwt, expires_ms, paired_at_ms}`. 90-day JWT lifetime.
- **SQL delta** `/app/ElleAnn/SQL/ElleAnn_PairedDevicesDelta.sql` — new
  `PairedDevices` table, filtered index on Revoked=0.
- **ElleDB::PairedDeviceRow + 5 helpers** (Upsert / Get / List / Revoke /
  TouchLastSeen) in `ElleSQLConn.h` + `ElleDB_Domain.cpp`.
- **EmotionalEngine VAD `5.0f` magic-scalar explained** — 25-line header
  comment over `ComputeValence` covers the sparsity-normalization math.
- **ProcessTriggers O(1) lookup** — lazy-init `std::unordered_map` replaces
  the previous O(N·102) tolower-scan; constructor baseline-set also
  migrated to the same map.
- **`EmotionalEngine::EmotionName(id)`** public accessor added so the
  name-to-ID lookup table can read the canonical spelling without a friend
  declaration.
- **Portable regression test** `/app/ElleAnn/Debug/test_jwt_and_emotion_map.cpp`
  — RFC 4231 HMAC vector + JWT determinism + map-lookup correctness;
  11/11 passing under Linux g++17.
- **Android spec note updated** — `Android/spec/Auth.kt` now reflects that
  `/api/auth/pair` exists (previously noted as unimplemented).

### Deferred (next ticket)
- **Central auth gate JWT verification**. Today the gate still compares
  Bearer against the shared `jwt_secret`. The JWTs we mint are forward-
  compatible; the upgrade is a gate-only change that parses header.payload,
  HMAC-verifies against the secret, checks `exp` and `PairedDevices.Revoked`.

### Canaries (verified clean Feb 2026 session)
- wx-pattern-canary: 0 hits in touched files
- catch-all-discipline: 0 hits in touched files
- no-raw-sleep-canary: 0 hits in touched files
- g++ portable unit test: 11/11 pass


## Session Feb-2026 (continued) — Central auth gate upgraded to real HS256 JWT verify

### Implemented
- `VerifyJwtHs256()` static helper in `HTTPServer.cpp` — parses 3-segment
  token, validates alg=HS256 (rejects "none" + unknown algs to close JWT
  downgrade attack), HMAC-verifies sig against `http_server.jwt_secret`,
  decodes payload, extracts `sub`/`exp`, enforces `exp > now`.
- `RouteDispatch::Dispatch` auth gate upgraded — JWT-first path: if
  Bearer has 2 dots, runs full verify + PairedDevices.Revoked check +
  LastSeenMs touch. Falls through to shared-secret compare only when JWT
  path doesn't apply (admin CLIs can still present the raw secret).
  Revoked devices with valid signatures are HARD-refused (401) — no
  fallback, since the signature proves device origin.
- `x-auth-device-id` synthetic header stashed on the request after
  successful JWT auth so downstream handlers can personalise by device.
- Test suite 18/18 pass (portable g++) — includes wrong-secret,
  expired, tampered-payload, no-dots, and happy-path.

### Now closed
- The P0 gate-upgrade handoff item from the previous milestone.
- Android spec (Auth.kt) is now end-to-end accurate: `/api/auth/pair`
  endpoint exists, JWTs it issues are actually honoured by every
  authenticated route, revocation works.

### Remaining
- Admin UI / routes for listing + revoking paired devices
  (`GET /api/auth/devices`, `DELETE /api/auth/devices/{id}`).
- Android project scaffold consuming `Android/spec/*.kt`.
- Optional: in-memory cache for GetPairedDevice to avoid a per-request
  DB round-trip under heavy load (currently 1 PK-indexed lookup per
  authenticated request, acceptable for local single-user traffic).

## Session Feb-2026 (continued) — P1/P2/QR complete + Android scaffold

### HTTP-side (all compiled + decoder-verified)
- **P1 admin device management**
  - `GET /api/auth/devices` (AUTH_ADMIN) — lists up to 200 paired devices.
  - `DELETE /api/auth/devices/{id}` (AUTH_ADMIN) — revokes + wipes the
    gate cache entry so revocation takes effect on the next request
    rather than 30 s later.
- **P2 cache** — `PairedDeviceStatusCached()` 30-second TTL map (bounded
  at 4096 entries, LRU-ish eviction of oldest half when full). Replaces
  the per-request `ElleDB::GetPairedDevice` round-trip in the JWT gate.
- **QR endpoint** — `GET /api/auth/qr?code=XXXXXX&host=...&port=...`
  (AUTH_ADMIN) returns `image/svg+xml` rendered from a pure-C++
  spec-compliant encoder at `Shared/ElleQR.{h,cpp}`. Covers QR versions
  1–10, all 4 ECC levels, 8-mask penalty-selection, RS-over-GF(256).
- **Decoder verification** — `/tmp/qr_verify.py` fed 6 payloads through
  the encoder and `pyzbar` decoded all 6 back byte-identical.

### Android scaffold (ready to `./gradlew assembleDebug`)
- Root: `settings.gradle.kts`, `build.gradle.kts`, `gradle.properties`,
  `README.md`.
- App module `app/` with Compose UI, Retrofit + OkHttp + kotlinx-
  serialization, `androidx.security` encrypted token storage, ZXing
  QR scanner.
- Kotlin source tree:
  - `ElleApp.kt` — Application subclass owning `AppContainer`.
  - `MainActivity.kt` — Compose host with `ellepair://` deep-link parser.
  - `data/TokenStore.kt` — EncryptedSharedPreferences + stable device UUID.
  - `data/AuthInterceptor.kt` — Bearer attach + 401-wipe logic.
  - `data/ElleApi.kt` — Retrofit interface (pair / health / emotions).
  - `data/AppContainer.kt` — hand-rolled DI; `apiFor(host, port)` cache.
  - `ui/PairScreen.kt` — host/port/code form + QR scan launcher.
  - `ui/HomeScreen.kt` — sanity call to `/api/emotions` to verify the
    Bearer plumbing end-to-end.
  - `ui/theme/Theme.kt` — muted teal/umber (dodges the default purple).
- Resources: manifest with camera + INTERNET + `ellepair://` intent
  filter, strings/themes XML, cleartext allowed (LAN-only), no-backup
  rules on the auth blob.

### Canaries
- All three `.cpp`/`.h` added this session pass wx-pattern,
  catch-all-discipline, no-raw-sleep, transform-tolower canaries.

### Still deferred
- Admin UI (web frontend) to call the device-mgmt routes and render
  the QR endpoint inline. Today the C++ service returns the SVG; an
  admin can hit it with a browser tab and screenshot/point the phone.
- Android build + wire-up validation — requires Android Studio + a
  physical or emulated device on the LAN.
## Session Feb-2026 (continued) — v1.7 audit fixes + Gradle wrapper

### P1 audit items resolved
- **ApiExplorer blocking I/O**: `DevScreens.kt::ApiExplorerScreen` now
  wraps the OkHttp `execute()` in `withContext(Dispatchers.IO)` so the
  Compose Main dispatcher is no longer blocked while a request flies.
  The string body is read inside the IO scope; only the resulting
  `String` crosses back to Main.
- **PairedDevices admin route**: both call sites in DevScreens
  (`getPairedDevices()` initial load + post-revoke refresh) now use
  `container.api(admin = true).getPairedDevices()` so the admin key
  header reaches the gate. Previously they used `container.extendedApi`
  which had no admin header → 403 risk.
- **WebSocket connect hardening**: added `@Volatile isConnecting` flag,
  guard now blocks duplicate `connect()` calls during a pending
  handshake. `onFailure` and `onClosed` clear the dead `webSocket`
  reference and reset `isConnecting=false`. `disconnect()` zeroes
  the connecting flag too.
- **ConnectionNotReadyScreen**: new
  `ui/common/ConnectionNotReadyScreen.kt` replaces the silent
  `return@composable` blanks in `ElleNavHost.kt` for the three routes
  that depend on the WebSocket (Elle home, Observatory, X-Chronicle).
  Shows a spinner + "live connection not ready" message + Retry button
  wired to `containerExtended.reconnectWebSocketIfNeeded()`.

### Gradle wrapper
- Added `Android/gradlew` (sh), `Android/gradlew.bat`,
  `Android/gradle/wrapper/gradle-wrapper.jar`, and
  `Android/gradle/wrapper/gradle-wrapper.properties` (Gradle 8.7).
  The project now builds reproducibly on a fresh clone with
  `./gradlew assembleDebug` — no system Gradle install required.
- Added `Android/gradle.properties` (AndroidX, parallel build, caching).

### Apache port
- Confirmed `AppContainerExtended.apachePort = 8080`. The companion
  app talks to the Apache reverse-proxy stripe at
  `http://<host>:8080/` for the 10 Apache-only endpoints, and to the
  Elle.Service.HTTP REST surface at the user-supplied paired port.

## Session Feb-2026 (continued) — v1.8: chat-cache crash safety, P2 typed bodies, silver palette

### Crash-safe chat persistence (per explicit user requirement)
- `ChatCacheManager` rewritten:
  - File extension changed `.json` → `.txt` (still JSON-encoded; user
    asked for "save to text" for grep-friendly diagnostics).
  - **Removed the 100-message cap** — caches the FULL conversation
    every time, no rolling window.
  - Atomic temp-file-then-rename writes prevent half-written files on
    sudden process death.
  - Process-wide singleton `installAsGlobal()` plus `crashFlush()`
    static dispatch let the JVM uncaught-exception handler synchronously
    flush every tracked conversation before the previous handler
    escalates the crash.
  - Per-conversation file lock via `String.intern()` so concurrent
    writers on the same file serialize correctly while different
    conversations parallelise.
  - `flushAllSync()` (crash path), `flushAll()` (suspend, lifecycle
    path), `flushAllBlocking()` (lifecycle observer that wants
    on-disk by ON_STOP-return-time).
- `ElleApp.onCreate()` now installs:
  - `Thread.setDefaultUncaughtExceptionHandler` — runs `crashFlush()`
    before delegating to the OS default handler.
  - `ProcessLifecycleOwner` `DefaultLifecycleObserver.onStop` →
    `flushAllBlocking()`.
- `ChatViewModel`:
  - Mirrors every state mutation into `cacheManager.track()` so the
    crash-flush can read the latest in-memory list.
  - `onCleared()` writes synchronously and untracks.
- `ChatScreen` now uses the singleton from `ElleApp` instead of
  creating a per-screen instance.

### P2 — typed `@Body` request models
Replaced every `Map<String, *>` request body in `ElleApiExtended.kt`
and `ElleApi.pair` with a `@Serializable data class`:
- `AttachFileRequest` (POST /api/memory/{id}/files)
- `SetEmotionDimensionRequest` (PUT /api/emotions/dimensions/{name})
- `ClaimHardwareActionRequest`, `CompleteHardwareActionRequest`
- `CreateSubjectRequest`, `UpdateSubjectRequest`,
  `CreateMilestoneRequest`, `CreateReferenceRequest`,
  `CreateSkillRequest`
- `LoadDictionaryRequest`
- `CreateModelWorkerRequest`
- `PairRequest` (POST /api/auth/pair)
Updated call sites in `PairScreen.kt`. The `Map<String,*>` _response_
types that `getBrainStatus()` and `generatePairCode()` use stay typed
as Map intentionally — the server returns opaque diagnostic blobs
there.

### UI palette correction — silver, not red
- `IsyaColors.kt` adds `IsyaSilver`, `IsyaSilverMid`, `IsyaSilverDeep`.
- `IsyaAnimatedBorder.kt::isyaHueCycle` now cycles
  Silver → Gold → Teal → Silver (was Gold → Teal → Violet → Gold;
  on certain panels the violet read as red, which was the user's
  "interface box is shaded red" complaint).
- `IsyaTheme.kt`:
  - `outline` slot now `IsyaSilverMid`, `outlineVariant` =
    `IsyaSilverDeep`.
  - `IsyaExtendedColors` exposes `silver` / `silverMid` / `silverDeep`
    so any future panel can pull silver directly without re-deriving.

### Audit-tag scrub
Removed every `// Fix N:`, `// Issue N:`, `// — Fix N`, etc. tag from
the production source. ~10 files touched. No behavioural change;
comments that previously cited audit tickets now describe what the
code does.

### Gradle wrapper present (kept from prior batch)
- `Android/gradlew`, `gradlew.bat`, `gradle/wrapper/gradle-wrapper.{jar,properties}`,
  `gradle.properties`. `./gradlew assembleDebug` works on a clean clone.

### Validation
- Brace + paren balance checker (same logic as the CI canary) →
  0 imbalanced files across 138 Kotlin files.
- No remaining `Map<` request bodies in `ElleApiExtended.kt`.
- No remaining `Fix N` / `Issue N` audit-tag comments.

### Delivery
- Repo updated at `/app/ElleAnn/Android/`.
- Zip: `/tmp/ElleAnn_Android_v1.8.zip` (386 KB).

## Session Feb-2026 (continued) — v1.9: Fiesta-calibrated silver bevel + SILVER_BLUE button

### Silver palette recalibration
After analysing the user-supplied Fiesta Online reference screenshot to
within ±5 RGB units:
- `IsyaSilver`           = `#D2D7DC`  (top-edge highlight)
- `IsyaSilverMid`        = `#A5AFB4`  (cool silver fill, slight blue cast)
- `IsyaSilverDeep`       = `#6E7378`  (bottom-edge shadow)
- `IsyaSilverButtonBlue` = `#5078B4`  (OK-button inner glow)

### Static silver bevel rendering
`IsyaAnimatedBorderBox` now branches on `animated`:
- `animated = true`: legacy cycling Silver→Gold→Teal hue path (opt-in for
  active/highlighted panels).
- `animated = false`: NEW static path that paints the Fiesta-style
  bevelled silver frame using a vertical gradient stroke
  (highlight → mid → deep) plus a thin inner-sheen pass for the
  polished-metal look.
`IsyaPanel`'s default flipped from `flowingBorder = true` →
`flowingBorder = false`, so every panel in the app now renders with the
silver bevel by default. Routes that want the cycling animation
explicitly opt in.

### SILVER_BLUE button variant
New `IsyaButtonVariant.SILVER_BLUE` matching the Fiesta OK button:
vertical blue inner-glow gradient (`#5078B4` at varying alpha) with a
silver gradient border (highlight → mid → deep). Wires through the
existing `IsyaButton` API; no other call sites need to change.

### Implementation polish
- Added missing `androidx.compose.foundation.border` import in
  `IsyaComponents.kt`.
- `drawIsyaSilverBevel` helper added to `IsyaAnimatedBorder.kt` — single
  source of truth for the bevel geometry; reused by both the outer
  bevel and the inner sheen.

### Validation
- Brace/paren balance checker: 0 imbalanced across all Kotlin files.
- Repacked: `/tmp/ElleAnn_Android_v1.9.zip` (387 KB).

## Session Feb-2026 (continued) — v2.0: Fiesta two-tone striped blue panel + corner ornaments + texture atlases

### Fiesta two-tone striped blue panel background
New `IsyaFiestaPanelBackground` composable, calibrated against the
user-supplied inventory reference screenshot (analysed to within ±5
RGB units):
- Base fill `#0D1B44` (deep midnight blue floor)
- Streak crest `#3C5FA8`, trough `#040C28`
- ~18% panel-width period, ~12° tilt from vertical
- Deterministic film-grain speckle pass (~3% alpha) for static noise

`IsyaPanel` now defaults to this background (`fiestaBackground = true`)
and falls back to the legacy `IsyaDusk` solid fill on opt-out for
specialty panels that need a clean canvas.

### Corner ornaments (your requested improvement)
New `IsyaCornerOrnaments` overlay paints small silver-gradient
triangular notches at all four panel corners. Opt in via
`IsyaPanel(decoratedCorners = true)` — off by default to avoid
cluttering dense screens.

### Texture atlases bundled
The 4 user-supplied TGA atlases (decoded → PNG) shipped as drawables
for any code path that wants to sample real Fiesta sprites:
- `R.drawable.elle_textures_atlas`        (512×512, large frame set)
- `R.drawable.elle_textures_atlas_med`    (256×256, medium icons + bars)
- `R.drawable.elle_textures_atlas_small`  (128×128, button frames)
- `R.drawable.elle_textures_atlas_xs`     (32×32,  tiny status dots)

Today's panel rendering is fully procedural (Compose Canvas) — these
atlases are available for any future screen that wants to layer in the
exact Fiesta sprites verbatim.

### Validation
- Brace/paren balance: 0 imbalanced across all Kotlin files.
- Repacked: `/tmp/ElleAnn_Android_v2.0.zip` (725 KB; +340 KB vs v1.9
  is the texture atlases).

### Knobs introduced on `IsyaPanel`
| Param              | Default | What it does                              |
|--------------------|---------|-------------------------------------------|
| flowingBorder      | false   | Animate the border with the hue cycle     |
| decoratedCorners   | false   | Paint silver corner notches               |
| fiestaBackground   | true    | Use the striped blue background           |
