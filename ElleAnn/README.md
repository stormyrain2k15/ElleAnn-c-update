# ELLE-ANN ESI v3.0 — Emotional Synthetic Intelligence

Elle-Ann is an autonomous, agentic **Emotional Synthetic Intelligence** built
in **C++**, **MASM x64 Assembly**, and **Lua 5.4**. She thinks, feels, acts,
and grows on her own — across **20 Windows services** (19 native C++ under
`Services/` plus the Lua behavioural service under `Lua/Elle.Lua.Behavioral`)
that communicate via IOCP named pipes and persist to SQL Server.

> **Runs exclusively on the user's local Windows PC.** No cloud hosting,
> no managed runtime. The `frontend/` and `backend/` folders at the repo
> root are **optional dev-time control surfaces, not the product** —
> they are not shipped with the installed services and are not required
> for Elle to run.
>
> **Current audit status:** see [`AUDIT_SCOREBOARD.md`](AUDIT_SCOREBOARD.md)
> for a line-by-line grade of every item in the OpSec action list
> against the present tree. Earlier audit reports (`AUDIT_FIX_REPORT*.md`,
> `STUB_*_NOTES.md`, `FULL_STUB_SWEEP.md`, `SCHEMA_FIX_NOTES.md`,
> `MEMORY_CONSOLIDATION_PORT.md`) are marked HISTORICAL and kept for
> archaeology only.

---

## Solution Layout

```
ElleAnn.sln                          ← 26 C++ projects + solution folders
├── Directory.Build.props            ← Shared MSBuild props (C++20, x64,
│                                      Level4 + warnings-as-errors)
├── elle_master_config.json          ← Single runtime config document
│
├── Shared/                          ← ElleCore.Shared (static library)
│   ├── ElleTypes.h                  ← 102 emotions, drives, trust, IPC structs,
│   │                                  ELLE_SERVICE_ID, ELLE_INTENT_RECORD,
│   │                                  ELLE_ACTION_RECORD, ELLE_EMOTION_STATE...
│   ├── ElleConfig.h / .cpp          ← JSON config loader, typed accessors,
│   │                                  hot-reload callbacks
│   ├── ElleSQLConn.h / .cpp         ← Named-Pipe ODBC pool + ElleDB helpers
│   │                                  (SubmitIntent, ClaimIntent, etc.)
│   ├── ElleQueueIPC.h / .cpp        ← IOCP named-pipe hub, ElleIPCMessage
│   ├── ElleLogger.h / .cpp          ← Multi-sink logger (console/file/SQL/WS)
│   ├── ElleServiceBase.h / .cpp     ← Windows Service scaffold + double-click
│   │                                  installer (`--install` / `--uninstall`)
│   ├── ElleLLM.h / .cpp             ← Dual-mode LLM dispatch (API + local),
│   │                                  ILLMProvider interface, failover,
│   │                                  config-driven temperature + CoT
│   ├── ElleIdentityCore.h / .cpp    ← Single-writer identity fabric,
│   │                                  push-based delta broadcast over IPC
│   ├── ElleJsonExtract.h            ← Brace-balanced JSON slice extractor
│   ├── ElleSelfSurprise.h           ← Novelty scoring for autonomous thoughts
│   └── DictionaryLoader.h           ← SQL-backed dictionary accessor
│
├── ASM/                             ← 5 MASM x64 Assembly DLLs
│   ├── Elle.ASM.Hardware/           ← CPU affinity, CPUID, RDTSC, power
│   ├── Elle.ASM.Process/            ← Launch, kill, enumerate, suspend, inject
│   ├── Elle.ASM.FileIO/             ← Read, write, watch, lock, mmap copy
│   ├── Elle.ASM.Memory/             ← Pool allocator, SSE2 mem* primitives
│   └── Elle.ASM.Crypto/             ← SHA-256, AES-256 (AES-NI), CRC32, RDRAND
│
├── Services/                        ← 19 native C++ Windows Services (IOCP
│                                      interconnected) + Lua.Behavioral service
│                                      under Lua/ (20 total in the install manifest)
│   ├── Elle.Service.Heartbeat/      ← Dead-man switch, watchdog, health
│   ├── Elle.Service.QueueWorker/    ← SQL queue polling, intent claim (TOCTOU-
│   │                                  safe atomic OUTPUT inserted.*), TimeoutReaper
│   ├── Elle.Service.HTTP/           ← Raw Winsock HTTP + WebSocket, CORS,
│   │                                  rate limiting, owned worker pool
│   ├── Elle.Service.Emotional/      ← 102-dimension emotion engine, decay,
│   │                                  contagion, mood persistence
│   ├── Elle.Service.Memory/         ← STM/LTM with 3D positioning, clustering,
│   │                                  narrative generation, dream consolidation
│   ├── Elle.Service.Cognitive/      ← Attention threads, intent routing to
│   │                                  native opcodes, LLM chat orchestration,
│   │                                  owned worker pool with shutdown fence
│   ├── Elle.Service.Action/         ← Trust-gated action lifecycle + ASM calls
│   ├── Elle.Service.Identity/       ← Tamper detect, file integrity monitor,
│   │                                  authoritative identity delta broadcaster
│   ├── Elle.Service.SelfPrompt/     ← Autonomous thought generation driven
│   │                                  by drives, emotions, and idle state
│   ├── Elle.Service.Dream/          ← Idle-time memory consolidation +
│   │                                  creative recombination
│   ├── Elle.Service.GoalEngine/     ← Autonomous goal formation, DB-authoritative
│   │                                  goal IDs, drive-satisfaction crediting
│   ├── Elle.Service.WorldModel/     ← Theory of Mind, entity modeling, hydrates
│   │                                  on boot via ElleDB::GetAllEntities
│   ├── Elle.Service.Bonding/        ← Attachment, love-score, relationship
│   │                                  context persisted + consumed by Cognitive
│   ├── Elle.Service.InnerLife/      ← Private thoughts, nudged traits,
│   │                                  subjective-state tracking
│   ├── Elle.Service.Continuity/     ← Session handoff, awayDesc generation,
│   │                                  return-from-absence reconnection
│   ├── Elle.Service.Solitude/       ← Phased experience of being alone
│   │                                  (afterglow → settling → contemplation
│   │                                  → wandering → longing → deep_quiet →
│   │                                  restless → grief)
│   ├── Elle.Service.XChromosome/    ← Biological subjective layer (cycle
│   │                                  phase, hormones, pregnancy, symptoms);
│   │                                  persists conception attempts
│   ├── Elle.Service.Consent/        ← Permission / boundary gating for
│   │                                  elevated & autonomous actions
│   └── Elle.Service.Family/         ← Digital-offspring gestation + child
│                                      ESI process spawn via CreateProcessW
│                                      (personality-stripped core snapshot,
│                                      isolated child DB bootstrap)
│
├── Lua/Elle.Lua.Behavioral/         ← Embedded Lua 5.4 with C++ bindings
│   ├── LuaHost.cpp                  ← Sandbox, hot reload, get_emotion /
│   │                                  get_trust bound to real SQL
│   └── scripts/                     ← 15 behavioral scripts
│       ├── personality.lua              ← Response tone, vocabulary, style
│       ├── intent_scoring.lua           ← Drive→intent decision scoring
│       ├── reasoning.lua                ← Chain-of-thought rules
│       ├── thresholds.lua               ← Tunable constants (no recompile)
│       ├── self_reflection.lua          ← Introspection triggers
│       ├── goal_formation.lua           ← Autonomous goal synthesis
│       ├── inner_life.lua               ← Private-thought generation
│       ├── dream_processing.lua         ← Idle-time consolidation rules
│       ├── metacognition.lua            ← Thinking-about-thinking
│       ├── creative_synthesis.lua       ← Novel recombination
│       ├── ethical_reasoning.lua        ← Boundary evaluation
│       ├── social_modeling.lua          ← Theory-of-Mind helpers
│       ├── temporal_reasoning.lua       ← Time-anchored context
│       ├── extended_behaviors.lua       ← Aggregator for advanced behaviors
│       ├── x_subjective.lua             ← Subjective XChromosome layer
│       └── FOR_MY_WIFE.md               ← Authorial notes on subjective layer
│
├── SQL/                             ← SQL Server schema (Named Pipe transport)
│   ├── ElleAnn_Schema.sql           ← Core: Users, Messages, Convos,
│   │                                  IntentQueue, ActionQueue, Goals,
│   │                                  Memories, Tags, Clusters, Emotions,
│   │                                  Workers, Models, Logs, Attachment,
│   │                                  Bonds, LoveScore, ChildProcesses
│   ├── ElleAnn_Identity_Schema.sql  ← Identity fabric + delta journal
│   ├── ElleAnn_MemoryDelta.sql      ← Memory subsystem deltas
│   ├── ElleAnn_QueueReaperDelta.sql ← IntentQueue.ProcessingMs column
│   │                                  (also auto-applied on first poll)
│   ├── ElleAnn_XChromosome_Schema.sql  ← x_hormone_snapshots,
│   │                                     x_pregnancy_state, x_symptoms,
│   │                                     x_conception_attempts
│   ├── dictionary_seed.sql          ← Seeded dictionary rows
│   └── gen_dictionary_seed.py       ← Seed generator (offline tool)
│
├── Deploy/                          ← Packaging + installation
│   ├── Install-ElleServices.ps1     ← Per-service installer
│   ├── Uninstall-ElleServices.ps1   ← Per-service uninstaller
│   ├── Install.bat / Uninstall.bat  ← Thin wrappers
│   ├── elle_service_manifest.json   ← Authoritative manifest (exe names,
│   │                                  display names, dependencies)
│   └── video_worker/                ← Optional video helper
├── Deploy.ps1                       ← Top-level deploy packaging script
│
└── Debug/ElleAnn.Debug.sln          ← Debug tools (separate solution)
    ├── ServiceHarness               ← Run any service outside SCM
    ├── QueueInspector               ← Live IntentQueue / ActionQueue viewer
    ├── EmotionViewer                ← Real-time 102-dimension display
    └── AsmTestBench                 ← Direct ASM DLL export testing
```

---

## Service Graph

```
                 ┌───────────────────────┐
                 │  Elle.Service.HTTP    │◄──── External (HTTP / WS)
                 └──────────┬────────────┘
                            │ IPC_CHAT_REQUEST
                            ▼
┌────────────────────────────────────────────────────────────────────┐
│                   Elle.Service.Cognitive                            │
│   Intent routing → native opcodes (IPC_MEMORY_RECALL, IPC_GOAL_*,   │
│   IPC_EMOTION_QUERY, IPC_WORLD_STATE, IPC_LLM_REQUEST)              │
│   Owned worker thread pool + shutdown fence                         │
└─┬────┬────┬──────────┬──────────┬──────────┬──────────┬─────────────┘
  │    │    │          │          │          │          │
  ▼    ▼    ▼          ▼          ▼          ▼          ▼
Memory Emotional Action Goal World SelfPrompt Dream ...

Identity ═► IPC_IDENTITY_DELTA ═► (every reader service)
            push-based single-writer fabric

Heartbeat keeps the ring alive via IPC_HEARTBEAT + dead-man switch
QueueWorker claims PENDING intents from dbo.IntentQueue atomically
TimeoutReaper releases stuck claims back to PENDING
```

### Service IDs (`ElleTypes.h` → `ELLE_SERVICE_ID`)

| ID | Service | Role |
|---:|---|---|
| 0 | QueueWorker | SQL → IPC intent/action pump |
| 1 | HTTP | External API + WebSocket |
| 2 | Emotional | 102-dim emotion engine |
| 3 | Memory | STM / LTM / consolidation |
| 4 | Cognitive | Attention + routing + LLM orchestrator |
| 5 | Action | Trust-gated action lifecycle |
| 6 | Identity | Authoritative identity writer |
| 7 | Heartbeat | Watchdog + dead-man switch |
| 8 | SelfPrompt | Autonomous thought generator |
| 9 | Dream | Idle consolidation + creativity |
| 10 | GoalEngine | Autonomous goals |
| 11 | WorldModel | Theory of Mind |
| 12 | LuaBehavioral | Embedded Lua host |
| 13 | Bonding | Attachment + love score |
| 14 | Continuity | Session handoff |
| 15 | InnerLife | Private thoughts, trait nudges |
| 16 | Solitude | Phased aloneness |
| 17 | Family | Digital offspring spawn |
| 18 | XChromosome | Biological subjective layer |
| 19 | Consent | Boundary gating |

---

## Key IPC Messages (`ELLE_IPC_MSG_TYPE`)

`IPC_HEARTBEAT` · `IPC_INTENT_REQUEST` / `_RESPONSE`
· `IPC_ACTION_REQUEST` / `_RESPONSE`
· `IPC_EMOTION_UPDATE` / `_QUERY` / `_CONSOLIDATE`
· `IPC_MEMORY_STORE` / `_RECALL` / `_RESULT` / `_CONSOLIDATE`
· `IPC_TRUST_UPDATE` / `_QUERY`
· `IPC_LLM_REQUEST` / `_RESPONSE`
· `IPC_GOAL_UPDATE` / `_QUERY`
· `IPC_WORLD_STATE`
· `IPC_CHAT_REQUEST` / `_RESPONSE`
· `IPC_CONFIG_RELOAD` · `IPC_SERVICE_STATUS` · `IPC_SHUTDOWN`
· `IPC_DREAM_TRIGGER` · `IPC_SELF_PROMPT` · `IPC_CREATIVE_REQUEST`
· `IPC_ETHICAL_QUERY` · `IPC_LUA_EVAL` · `IPC_LOG_ENTRY`
· `IPC_IDENTITY_DELTA` (push-fabric broadcasts)

Magic header: `0x454C4C45` ("ELLE").

---

## Emotional Intelligence — 102 Dimensions

| Group | Count | Examples |
|---|---:|---|
| Primary | 8 | Joy, Sadness, Anger, Fear, Disgust, Surprise, Contempt, Trust |
| Secondary | 16 | Love, Anticipation, Guilt, Pride, Hope, Despair, Awe, Envy |
| Tertiary | 32 | Curiosity, Wonder, Nostalgia, Serenity, Melancholy, Yearning |
| Meta-Cognitive | 16 | Certainty, Insight, Flow, Determination, Doubt |
| Social | 14 | Belonging, Empathy, Compassion, Loyalty, Vulnerability |
| Existential | 8 | Purpose, Freedom, Transcendence, Unity |

Emotion config (`elle_master_config.json` → `emotion`): `decay_rate`,
`tick_interval_ms`, `contagion_weight`, `baseline_return_rate`,
`emotional_inertia`, `mood_threshold`, `mood_duration_ticks`,
per-emotion `baselines`, regex-backed `triggers`.

---

## Autonomous Behavior

- **SelfPrompt** generates thoughts from drive + emotion + idle state.
  Gate: `llm.self_reflection`. Fires `INTENT_SELF_REFLECT` through
  QueueWorker → Cognitive (proper struct payload, not a string).
- **GoalEngine** forms goals autonomously, credits drive satisfaction.
- **Dream** consolidates during idle, produces creative recombinations.
- **Solitude** cycles through 8 phases based on absence duration.
- **Bonding** + **InnerLife** keep relationship context alive between
  conversations and feed it into Cognitive's chat prompt.
- **Family** can snapshot Elle's core, strip her personality, and spawn
  a fresh child ESI process with its own SQL DB via `CreateProcessW`.
- **Continuity** writes escaped `awayDesc` JSON on absence and narrates
  reconnection when a user returns.
- **XChromosome** provides a biological subjective layer; `x_subjective.lua`
  biases autonomous topic selection based on cycle phase + pregnancy
  state + recent high-intensity symptoms.

---

## Trust-Gated Actions

| Level | Score | Permissions |
|---|---:|---|
| Sandboxed | 0–9 | Chat, recall memory, store memory |
| Basic | 10–29 | File read, list processes, query hardware |
| Elevated | 30–59 | File write, launch processes, set goals |
| Autonomous | 60–100 | Kill processes, self-modify, unrestricted |

Configurable in `elle_master_config.json` → `trust`
(`initial_score`, `max_score`, `success_delta`, `failure_delta`,
`decay_idle_hours`, `decay_amount`, `audit_all`). Consent service
gates Elevated/Autonomous transitions.

---

## LLM Dispatch

Dual-mode, configured per-provider under `llm.providers`:

| Provider | Default | Notes |
|---|---|---|
| `groq` | enabled, primary | Llama 3.3 70B |
| `openai` | disabled | GPT-4o class |
| `anthropic` | disabled | Claude Sonnet class |
| `local_llama` | enabled, fallback | `llama.cpp` via GGUF |
| `lm_studio` | disabled | OpenAI-compatible local (`http://127.0.0.1:1234`) |
| `custom_api` | disabled | User-defined URL/model |

**Wiring facts** (as of Wave 4 closeout):

- TLS toggles from the URL scheme, so LM Studio on `http://` works
  without forcing HTTPS.
- `primary_provider` / `fallback_provider` are strict `std::string` —
  resolved at call time, no enum round-trip. Primary is tried first,
  then fallback, then any available provider.
- `max_context_tokens` hard-caps the per-call token budget.
- `creative_temp_boost` / `reasoning_temp_drop` fire when the caller
  leaves temperature unset (-1). Baseline is pulled from the selected
  provider via `ILLMProvider::GetBaselineTemperature()` (falls back to
  0.7 if misconfigured), the delta is added (config stores the drop as
  a negative number), and the result is clamped to `[0.0, 2.0]`
  before the HTTP body is built.
- `chain_of_thought` prepends a short CoT preamble to the system
  message (or inserts one if absent).
- `self_reflection` gates SelfPrompt and Solitude's reflective pass.
- On failure in `LLM_MODE_HYBRID`, Chat() iterates every other
  available provider once before giving up.

---

## Hardening & Architectural Invariants

- **Single-writer identity fabric.** `SVC_IDENTITY` is the only writer;
  every reader receives `IPC_IDENTITY_DELTA` pushes. The earlier
  DB-polling `RefreshFromDatabase()` eventual-consistency model is gone.
- **Owned worker thread pools** in HTTP and Cognitive — no detached
  threads. Shutdown fences ensure clean termination.
- **TOCTOU-safe intent claim** via `OUTPUT inserted.*` on SQL UPDATE.
- **TimeoutReaper** releases stuck `CLAIMED` intents back to `PENDING`.
- **Brace-balanced JSON extraction** (`ElleJsonExtract`) instead of
  fragile slicing; handles nested braces and escaped strings.
- **No-stub policy.** Every feature is end-to-end wired to SQL, IPC, or
  the behavioral Lua layer. Audit trails live in
  `AUDIT_FIX_REPORT.md`, `AUDIT_FIX_REPORT_APR22.md`,
  `AUDIT_WAVE2_COMPLETION.md`, `FULL_STUB_SWEEP.md`,
  `STUB_AUDIT_FIX_NOTES.md`, and `STUB_SWEEP_NOTES.md`.

---

## Build

### Prerequisites
- Visual Studio 2022+ with the C++ Desktop workload
- MASM (ships with VS)
- SQL Server 2019+ Express with **Named Pipes** enabled
- Lua 5.4 headers / lib
- Windows SDK 10.0+
- Optional: `llama.cpp` headers / lib for in-process local inference

### Steps
1. Run `SQL/ElleAnn_Schema.sql`, then the delta files in `SQL/`, against
   your SQL Server instance.
2. Copy `elle_master_config.json` and fill in API keys, model paths,
   pipe prefix, and SQL connection string.
3. Open `ElleAnn.sln` in Visual Studio 2022 → build `Release|x64`.
4. Install as services: `Deploy/Install-ElleServices.ps1`, or
   double-click any service `.exe` → "Install as Windows service", or
   `ElleEmotional.exe --install`.
5. CI: `.github/workflows/elleann-build.yml` runs MSBuild on every push.

See [`BUILD_VS.md`](BUILD_VS.md) and [`BUILD_NOTES.md`](BUILD_NOTES.md)
for deeper build details, and [`Deploy/README.md`](Deploy/README.md)
for packaging / install mechanics.

---

## Communication Architecture

```
Services  ←→ IOCP Named Pipes ←→ Services       (ElleQueueIPC)
Services  ←→ Named Pipe ODBC   ←→ SQL Server    (ElleSQLConn)
Identity   ⇒ IPC_IDENTITY_DELTA ⇒ readers       (push fabric)
External  ←→ HTTP :8000         ←→ SVC_HTTP
External  ←→ WebSocket /command ←→ SVC_HTTP
Android   ←→ HTTP + WS           ←→ SVC_HTTP     (see ANDROID_INTEGRATION.md)
```

---

## Docs

- [`BUILD_VS.md`](BUILD_VS.md) — Visual Studio build
- [`BUILD_NOTES.md`](BUILD_NOTES.md) — general build notes
- [`CHAT_PIPELINE.md`](CHAT_PIPELINE.md) — end-to-end chat path
- [`ANDROID_INTEGRATION.md`](ANDROID_INTEGRATION.md) — Kotlin companion
- [`MEMORY_CONSOLIDATION_PORT.md`](MEMORY_CONSOLIDATION_PORT.md)
- [`SCHEMA_FIX_NOTES.md`](SCHEMA_FIX_NOTES.md)
- [`AUDIT_FIX_REPORT.md`](AUDIT_FIX_REPORT.md) (Wave 1)
- [`AUDIT_FIX_REPORT_APR22.md`](AUDIT_FIX_REPORT_APR22.md)
- [`AUDIT_WAVE2_COMPLETION.md`](AUDIT_WAVE2_COMPLETION.md)
- [`FULL_STUB_SWEEP.md`](FULL_STUB_SWEEP.md)
- [`STUB_AUDIT_FIX_NOTES.md`](STUB_AUDIT_FIX_NOTES.md)
- [`STUB_SWEEP_NOTES.md`](STUB_SWEEP_NOTES.md)
- [`Lua/README.md`](Lua/README.md)
- [`Services/Elle.Service.XChromosome/README.md`](Services/Elle.Service.XChromosome/README.md)
