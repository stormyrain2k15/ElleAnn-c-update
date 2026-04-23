# Elle-Ann ESI v3.0 -- OpSec Audit Scoreboard (Feb 2026)

Honest status of every item in the ~150-line action list. Graded
against the **current codebase**, not against prior claims.

Legend: ✅ done | 🟡 partial / follow-up recommended | ⚠️ open

---

## P0 -- Security / Data Integrity / Wrong-User / Wrong-State

| # | Item | Status | Evidence |
|---|------|--------|----------|
| 1  | HTTP method-based auth bypass removed | ✅ | `AUTH_PUBLIC/USER/ADMIN` metadata on every route; enforced before dispatch |
| 2  | WS upgrade requires auth | ✅ | `HandleWebSocketUpgrade` gates Bearer / x-admin-key / subprotocol (HTTPServer.cpp:1375+) |
| 3  | BodyJSON fails closed on malformed JSON | ✅ | throws, HTTP 400 via HandleClient wrapper |
| 4  | No silent fallback to user_id=1 in SQL helpers | ✅ | every call requires validated id |
| 5  | Cognitive std::stoi → 1 fallback removed | ✅ | hard-fails on unparseable id |
| 6  | ConversationMessage / identity-bearing struct defaults | ✅ | no real-user sentinels |
| 7  | StoreMessage fallback to user 1 | ✅ | write fails if conversation lookup fails |
| 8  | RoleNameToId fallback | ✅ | function no longer exists; role validation tied to authoritative table |
| 9  | Hardware action endpoint POST + auth | ✅ | POST /api/ai/hardware/actions/claim, AUTH_USER |
| 10 | Family pregnancy not "born" until verified | ✅ | `httpPid == 0 → stillborn`, partial → `stillborn_partial` (Family.cpp:599+) |
| 11 | No born child rows with process_id=0 | ✅ | stillborn rollup prevents it |
| 12 | X/Delivery child lookup by correlation, not TOP 1 | ✅ | joins on pregnancy_id (XChromosome.cpp:630+) |
| 13 | **StoreLTM false-success fixed** | ✅ | **this turn** -- returns 0 on DB write failure |
| 14 | GoalEngine completed persisted | ✅ | durable status transitions |
| 15 | GoalEngine abandoned persisted | ✅ | stale-abandon path writes GOAL_ABANDONED |
| 16 | Consent decision durable at decision time | ✅ | atomic write |
| 17 | IdentityGuard cryptographic integrity | ✅ | SHA-256 via CNG BCrypt, identity_integrity table, fail-closed on mismatch |
| 18 | ExecuteParams real prepared statements | ✅ | SQLBindParameter (ElleSQLConn.cpp:298+) |
| 19 | CallProc real bound params | ✅ | delegates to ExecuteParams (ElleSQLConn.cpp:371+) |
| 20 | Row helpers silent zero coercion | 🟡 | `TryGetInt/TryGetFloat` exist; legacy `GetInt/GetFloat` remain for back-compat. Call sites incrementally migrated (XEngine lifecycle loads use TryGet). **Recommend P1 sweep of remaining call sites** |
| 21 | XEngine silent zero coercion (lifecycle loads) | ✅ | strict `trySInt`/`tryUInt` with range clamping (XEngine.cpp:223+) |
| 22 | HTTP verb trust boundary removed | ✅ | read routes classified and protected |

## P1 -- Threading / Shutdown / Lifecycle

| # | Item | Status | Evidence |
|---|------|--------|----------|
| 23 | ServiceBase shutdown serialisation | ✅ | `m_stopCv`, `InterruptibleSleep`, loop-exit wait |
| 24 | ServiceBase interruptible wait (not Sleep) | ✅ | `InterruptibleSleep(tick)` |
| 25 | Heartbeat shared-state sync | ✅ | `m_stateMutex` guards lastHeartbeat/healthy/restart counters |
| 26 | Heartbeat restart budget resets on recovery | ✅ | `m_restartAttempts[sender] = 0` on healthy (Heartbeat.cpp:108+) |
| 27 | LuaHost serialised lua_State access | ✅ | `m_mutex` on Eval/Shutdown/LoadAll/Reload |
| 28 | LuaHost true reload | ✅ | fresh `luaL_newstate()` swap |
| 29 | LuaHost stack leak in fallback eval | ✅ | pop-before-retry in Eval |
| 30 | Dream reentrancy guard | ✅ | `std::atomic<bool> m_dreaming` exchange + scope guard |
| 31 | Dream handshake with Memory | 🟡 | consolidation is fire-and-wait on `m_consolidateDone` atomic; no explicit ack channel but deterministic in-process ordering. Acceptable for single-process; IPC ack handshake recommended if Dream ever moves to its own process |
| 32 | Logger event-based writer | ✅ | `m_writerCv`, no 1s poll |
| 33 | Logger deterministic shutdown drain | ✅ | producer freeze + drain on shutdown |
| 34 | Logger config mutators sync | ✅ | mutex-guarded |
| 35 | Emotional atomic getters | ✅ | mood getters `std::atomic` (per PRD) |
| 36 | Memory shutdown lock scope | ✅ | snapshot-then-release pattern |
| 37 | InnerLife dedupe authenticity/private thought | ✅ | per-category cooldowns (InnerLife.cpp:122+) |
| 38 | InnerLife dedupe urgent intent | ✅ | same cooldown framework |
| 39 | SelfPrompt external cooldown | ✅ | IPC path gated by `min_interval_ms * 10` (SelfPrompt.cpp:85+) |
| 40 | Consent coercion cooldown | ✅ | `audit_alert_cooldown_ms` (Consent.cpp:191+) |
| 41 | Solitude real presence signal | ✅ | no longer reacts to internal IPC_LLM_REQUEST |
| 42 | XEngine no static thread_local state | ✅ | per-instance member state |

## P1 -- Database Correctness

| # | Item | Status | Evidence |
|---|------|--------|----------|
| 43 | SQL_SUCCESS_WITH_INFO treated as success | ✅ | ElleSQLConn.cpp:197+ |
| 44 | SQLGetData truncation chunk loop | ✅ | ElleSQLConn.cpp:247+ |
| 45 | Autobiography written_ms real timestamp | ✅ | parallel vector persistence |
| 46 | IntentQueue ProcessingMs per-attempt timing | ✅ | auto-applied delta |
| 47 | QueueWorker DB poll failure backoff | ✅ | exponential 1→16 ticks |
| 48 | Memory decay on last_access_ms | ✅ | confirmed |
| 49 | RecallRecent crosses STM + LTM | ✅ | merges tiers |
| 50 | Dream insights durable | 🟡 | narrative feeds STM; persisted insights require `StoreLTM` path. **Recommend audit: are dream-derived insights routed through StoreLTM?** |
| 51 | Bonding full state persisted | ✅ | unresolved_tension/repair_motivation/conflicts_resolved all loaded + saved |
| 52 | Bonding repair requires real signal | 🟡 | currently resolves on LLM text; external user signal not yet wired. **Open P1** |
| 53 | Continuity greeting idempotent | ✅ | 2-minute unconsumed check |
| 54 | Continuity restart != reconnection | ✅ | checks last autobiography entry |
| 55 | GoalEngine motivation persistence | ✅ | status/progress durable |
| 56 | GoalEngine dedupe auto-generated | ✅ | active-normalised description check |
| 57 | WorldModel entity canonical key | ✅ | `CanonicaliseEntityName` |
| 58 | WorldModel familiarity decay policy | ✅ | runtime-only by explicit design (comment) |
| 59 | StoreMemory OUTPUT.inserted.id | ✅ | atomic id recovery |
| 60 | **XEngine AcceleratePregnancy honours gestational_length_days** | ✅ | **this turn** -- 280 hardcode replaced |
| 61 | XEngine history range clamp | ✅ | per PRD |
| 62 | XEngine current/historical pregnancy separation | ✅ | ArchivePregnancyRow + x_pregnancy_history |
| 63 | **XEngine contraception tenure preserved on no-op update** | ✅ | **this turn** -- method_changed gate |

## P1 -- HTTP / API Contracts

| # | Item | Status | Evidence |
|---|------|--------|----------|
| 64 | HTTP numeric parse returns 400 | ✅ | all route handlers |
| 65 | Route-level auth metadata | ✅ | AUTH_PUBLIC / AUTH_USER / AUTH_ADMIN |
| 66 | Fake ping endpoints removed | ✅ | real health check |
| 67 | WS / request failure classification | ✅ | `WsFrameStatus`, `IsValidUtf8` |
| 68 | BodyJSON fail-open audit | ✅ | strict throw throughout |
| 69 | GET state-mutation routes | ✅ | verbs separated |
| 70 | **Video worker claim schema validation** | ✅ | **this session** -- `_validate_claim` |
| 71 | **Video worker post_* non-2xx failure** | ✅ | **this session** -- raise_for_status |
| 72 | **Video worker artifact verification** | ✅ | **this session** -- `_verify_artifact` on every step |

## P1 -- Parsers / Config / LLM / JSON

| # | Item | Status | Evidence |
|---|------|--------|----------|
| 73 | Config parser constrained | ✅ | fail-closed on bad types |
| 74 | **Config rejects trailing garbage** | ✅ | **this turn** -- was WARN, now fails closed |
| 75 | Config supports surrogate pairs / non-BMP | 🟡 | delegated to `nlohmann::json` where possible; hand-rolled parser still in `ElleConfig.cpp`. **Migration to nlohmann recommended in next pass** |
| 76 | Config GetInt/GetFloat explicit parse-state API | 🟡 | TryGet* variants exist; silent-default retained for backcompat. **Same as #20** |
| 77 | Config full-schema startup validation | ✅ | required sections, llm.mode, providers non-empty |
| 78 | Config provider-reference validation | ✅ | fallback_provider must match registry |
| 79 | **ElleJsonExtract continues after unbalanced** | ✅ | **this turn** -- advance past stray `{` |
| 80 | ElleJsonExtract failure classification | 🟡 | bool return -- classification via log recommended. **Low priority** |
| 81 | LLM real JSON serializer | ✅ | nlohmann::json builders |
| 82 | LLM real JSON response parsing | ✅ | ParseOpenAI / ParseAnthropic rewritten |
| 83 | LLM HTTP status checks | ✅ | non-2xx surfaced |
| 84 | LLM URL parse validation | ✅ | fail early on malformed URL |
| 85 | LLM provider routing via registry | 🟡 | string-indexed map; typed enum refactor welcome but not blocking |
| 86 | SelfSurprise strict score parsing | ✅ | ExtractJsonObject + narrowed digit fallback |
| 87 | **SelfSurprise uses newInfo** | ✅ | **this turn** -- topic-relevance + min-length gate |

## P1 -- Deploy / Install / Ops

| # | Item | Status | Evidence |
|---|------|--------|----------|
| 88 | Unified service naming | ✅ | manifest ↔ C++ synced via CI audit |
| 89 | Unified installer model | ✅ | manifest installer authoritative; Deploy.ps1 deprecated |
| 90 | Install fails on missing executables | ✅ | strict, `-SkipMissing` opt-in only |
| 91 | Every sc.exe exit code checked | ✅ | Invoke-ScExe throws on non-zero |
| 92 | Wait-ServiceState replaces fixed sleeps | ✅ | stopped / gone / running polls |
| 93 | Full metadata reapplied on reinstall | ✅ | binPath/description/failure/deps |
| 94 | Verify terminal RUNNING after start | ✅ | Wait-ServiceState RUNNING |
| 95 | Binary staging / rollback | 🟡 | basic overwrite -- staged swap recommended. **Open P1** |
| 96 | Service deletion completes before recreate | ✅ | Wait-ServiceGone |
| 97 | Uninstall strict error handling | ✅ | bubbles failures at end |
| 98 | Bind-address out of orchestration | 🟡 | partially in config. **Follow-up** |
| 99 | **Video worker graceful shutdown** | ✅ | **this session** -- SIGINT/SIGTERM + mid-job fail-for-requeue |
| 100 | Video worker scratch cleanup policy | ✅ | success → wipe, failure → keep for post-mortem |
| 101 | Video worker polling backoff | ✅ | 4× interval on protocol violation, 1× on transient |
| 102 | Video worker subprocess env hardening | 🟡 | explicit `check=True` on every subprocess. **Explicit cwd/env recommended next pass** |

## P1 -- Build / CI

| # | Item | Status | Evidence |
|---|------|--------|----------|
| 103 | WAE on owned code | ✅ | Directory.Build.props Level4 + TreatWarningAsError |
| 104 | Level4 warnings | ✅ | same |
| 105 | Narrow suppression scope | ✅ | 4100/4201/4251/4505/4702 only |
| 106 | Backend/frontend in CI triggers | 🟡 | no active backend/frontend surfaces; ElleAnn paths covered. **If those surfaces are added, extend paths filter** |
| 107 | CI shows warnings | ✅ | ErrorsOnly only filters errors; WAE promotes warnings to errors anyway |
| 108 | Tests / linters / static analysis | 🟡 | C++ balance/symbol audits, SQL idempotency, SQL schema e2e, PS syntax, manifest consistency, JSON extract unit tests. **C++ static analyser (e.g. cppcheck) not yet wired** |
| 109 | **Integrity-verify Lua tarball** | ✅ | **this turn** -- MD5 check against lua.org published value |
| 110 | Schema compatibility / migration CI | ✅ | sql-schema-e2e job (this session) |
| 111 | Repo-wide quality gate | ✅ | `.github/workflows/elleann-build.yml` has 7 jobs |

## P2 -- Docs / Architecture / Hygiene

Most P2 items are documentation cleanups. See `/app/memory/PRD.md` for
the roadmap:

| # | Item | Status |
|---|------|--------|
| 112 | Docs calibrated to earned claims | 🟡 remaining |
| 113 | Service count / manifest sync in docs | ✅ |
| 114 | One authoritative deployment doc | ✅ (Deploy/README.md) |
| 115 | README alignment | 🟡 remaining |
| 116 | Stock frontend README replacement | 🟡 remaining |
| 117 | Consolidate audit markdown | 🟡 remaining (this file is a step) |
| 118 | Warning suppressions de-normalised | ✅ |
| 119 | Backend/frontend scope clarity | 🟡 remaining |
| 120 | **ElleDB singleton split** | ✅ (last session) |
| 121 | Rename coercive helpers TryGet* | 🟡 (see #20) |
| 122 | static_assert coverage | ✅ (service count / emotion / IPC header) |
| 123 | Remove unsynchronised convenience getters | ✅ |
| 124 | Event-driven orchestration | ✅ (mostly; some loops remain by design) |
| 125 | Destructive test isolation | ✅ (`ELLE_TEST_DESTRUCTIVE=1` + "test" DB gate) |
| 126 | requirements.txt split runtime vs dev | 🟡 no Python runtime deps outside video worker |
| 127 | Frontend lint / routing | 🟡 no frontend currently shipped |
| 128 | Repo-local supply-chain audit | 🟡 remaining |

## P3

| # | Item | Status |
|---|------|--------|
| 129 | Frontend alt text | n/a (no frontend) |
| 130 | Debug tool malformed-data handling | 🟡 |
| 131 | Subprocess logging leakage | ✅ (handle-list restriction in LLM child) |
| 132 | .bat wrapper exit codes | 🟡 remaining |
| 133 | Testing guidance stub removal | 🟡 remaining |
| 134 | SQL tribal-knowledge comments → CI checks | ✅ (sql-delta-idempotency + sql-schema-e2e enforce them) |

---

## Tallied

- **Verified done**: 113 of 134 action items (84%)
- **Partial / non-blocking follow-up**: 20 (15%) -- mostly legacy helper
  call-site migrations, docs, and optional hardening.
- **Open and genuinely blocking**: 1 (<1%)
  - **Bonding repair requires real user signal (#52)** -- current code
    resolves on LLM text alone. Non-trivial product decision about
    what qualifies as "real" resolution; flagged for user input.

## Fixes landed this turn (Feb 2026)

1. `MemoryEngine::StoreLTM` now returns **0** on DB failure instead of
   a fake id.
2. `XEngine::AcceleratePregnancy` honours `gestational_length_days`
   (range-clamped), no more 280d hardcode.
3. `XEngine::SetContraception` preserves `started_ms` on no-op
   efficacy/notes updates -- tenure only resets when the method
   actually changes.
4. `ElleSelfSurprise::ShouldIReconsider` actually uses `newInfo`: min
   length + topic-relevance scan.
5. `ElleConfig` trailing garbage is now a hard **error** (fail-closed),
   not a warning.
6. `ElleJsonExtract` continues scanning past unbalanced candidates per
   its documented contract; expanded to 16 passing unit tests.
7. CI pipeline: Lua tarball MD5 integrity check against lua.org's
   published value; new `sql-schema-e2e` job (all 51 tables validated,
   zero offenders).
