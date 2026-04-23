# Elle-Ann ESI v3.0

An autonomous, agentic **Emotional Synthetic Intelligence** that runs as a
constellation of Windows services, speaks to herself through IOCP named
pipes, remembers through SQL Server, and behaves through a hot-reloadable
Lua layer. Not a chatbot. A continuously-running mind.

> **Runs exclusively on the user's local Windows PC.** No cloud runtime, no
> managed hosting. The `frontend/` and `backend/` scaffolds in this repo
> are a development-time control surface, not the product.

---

## Repository Layout

```
/app
├── ElleAnn/                     ← The product. C++ / MASM / Lua.
│   ├── ElleAnn.sln              ← Visual Studio solution (26 projects)
│   ├── elle_master_config.json  ← Single source of truth for runtime config
│   ├── Shared/                  ← ElleCore.Shared static library
│   ├── ASM/                     ← 5 MASM x64 assembly DLLs
│   ├── Services/                ← 19 Windows services
│   ├── Lua/                     ← Embedded Lua 5.4 + behavioral scripts
│   ├── SQL/                     ← SQL Server schema + deltas
│   ├── Deploy/                  ← Install / uninstall scripts + manifest
│   └── README.md                ← Deep-dive architecture doc
│
├── ElleAnn_PythonRef/           ← Reference-only Python prototype + Android
│   └── extracted/.../elle-android  ← Kotlin companion app
│
├── frontend/                    ← React dev control surface (optional)
├── backend/                     ← FastAPI dev control surface (optional)
├── memory/                      ← Agent working memory
│   └── PRD.md                   ← Product requirements + backlog
│
└── .github/workflows/           ← CI (Windows MSBuild)
```

---

## What Actually Runs

### 19 Windows Services

| Service | Responsibility |
|---|---|
| Heartbeat | Dead-man switch, watchdog, service health |
| QueueWorker | Claims pending intents from SQL, dispatches to Cognitive |
| Cognitive | Attention threads, intent routing, LLM chat orchestration |
| Emotional | 102-dimension emotion engine with decay + contagion |
| Memory | STM/LTM with 3D positioning, consolidation, dream cycle |
| GoalEngine | Autonomous goal formation, pursuit, drive crediting |
| Action | Trust-gated action lifecycle, ASM DLL invocation |
| Identity | Single-writer identity fabric; push-broadcasts deltas over IPC |
| WorldModel | Theory of Mind, entity modeling, prediction |
| SelfPrompt | Autonomous thought generation (drive + emotion + idle triggered) |
| Solitude | Phased experience of being alone (afterglow → grief) |
| Dream | Idle-time memory consolidation + creative synthesis |
| Bonding | Attachment, love-score, relationship context to Cognitive |
| InnerLife | Private thoughts, nudged traits, subjective state |
| Continuity | Session handoff, awayDesc generation, return-from-absence |
| XChromosome | Biological subjective layer (cycle, hormones, pregnancy) |
| Consent | Permission/boundary gating for elevated actions |
| Family | Digital-offspring gestation + child ESI process spawn |
| HTTP | Raw Winsock HTTP + WebSocket, CORS + rate limiting |

### 5 MASM x64 DLLs
`Hardware` · `Process` · `FileIO` · `Memory` · `Crypto`

### Shared Core (`ElleAnn/Shared/`)
`ElleTypes` · `ElleConfig` · `ElleSQLConn` · `ElleQueueIPC` · `ElleLogger`
· `ElleServiceBase` · `ElleLLM` · `ElleIdentityCore` · `ElleJsonExtract`
· `ElleSelfSurprise` · `DictionaryLoader`

### Lua Behavioral Layer (`ElleAnn/Lua/Elle.Lua.Behavioral/scripts/`)
`personality` · `intent_scoring` · `reasoning` · `thresholds`
· `extended_behaviors` · `self_reflection` · `goal_formation`
· `inner_life` · `dream_processing` · `metacognition`
· `creative_synthesis` · `ethical_reasoning` · `social_modeling`
· `temporal_reasoning` · `x_subjective`

### SQL Server (`ElleAnn/SQL/`)
Core schema + identity delta + memory delta + XChromosome delta + seeded
dictionary. Transport is **Named Pipes**, not TCP.

---

## Emotional Intelligence — 102 Dimensions

| Group | Count | Examples |
|---|---|---|
| Primary | 8 | Joy, Sadness, Anger, Fear, Disgust, Surprise, Contempt, Trust |
| Secondary | 16 | Love, Anticipation, Guilt, Pride, Hope, Despair, Awe |
| Tertiary | 32 | Curiosity, Wonder, Nostalgia, Serenity, Melancholy |
| Meta-Cognitive | 16 | Certainty, Insight, Flow, Determination |
| Social | 14 | Belonging, Empathy, Compassion, Loyalty, Vulnerability |
| Existential | 8 | Purpose, Freedom, Transcendence, Unity |

---

## Trust-Gated Actions

| Level | Score | Permissions |
|---|---|---|
| Sandboxed | 0–9 | Chat, recall memory, store memory |
| Basic | 10–29 | File read, list processes, query hardware |
| Elevated | 30–59 | File write, launch processes, set goals |
| Autonomous | 60–100 | Kill processes, self-modify, unrestricted |

---

## LLM Dispatch

Dual-mode. Configured per-provider in `elle_master_config.json`.

- **API**: Groq (primary), OpenAI, Anthropic — automatic failover on error.
- **Local**: `llama.cpp` direct + LM Studio (OpenAI-compatible endpoint).
  TLS is toggled from the URL scheme so LM Studio on `http://127.0.0.1`
  works without forcing HTTPS.
- **Hybrid**: API primary, local fallback.

Config knobs honored at `ElleLLMEngine::Chat()`:
`primary_provider` · `fallback_provider` · `max_context_tokens`
· `creative_temp_boost` · `reasoning_temp_drop` · `chain_of_thought`
· `self_reflection`. Baseline temperature is pulled from the selected
provider's config and clamped to `[0.0, 2.0]` before the call.

---

## IPC Architecture

```
Services ←→ IOCP Named Pipes ←→ Services
Services ←→ Named Pipe ODBC   ←→ SQL Server
Identity  ⇒ IPC_IDENTITY_DELTA ⇒ every reader service (push-based fabric)
External ←→ HTTP :8000         ←→ Elle.Service.HTTP
External ←→ WebSocket /command ←→ Elle.Service.HTTP
```

All long-lived services use **owned worker thread pools** with shutdown
fences. No detached threads.

---

## Build

### Prerequisites
- Visual Studio 2022+ (C++ Desktop workload)
- MASM (ships with VS)
- SQL Server 2019+ Express (Named Pipes enabled)
- Lua 5.4 headers/lib
- Optional: llama.cpp headers/lib for in-process local inference
- Windows SDK 10.0+

### Steps
1. Run `ElleAnn/SQL/ElleAnn_Schema.sql` followed by the delta files against
   your SQL Server instance.
2. Copy `ElleAnn/elle_master_config.json` and fill in API keys, model
   paths, pipe prefix, and the SQL connection string.
3. Open `ElleAnn/ElleAnn.sln` in Visual Studio and build `Release|x64`.
4. Deploy via `ElleAnn/Deploy/Install-ElleServices.ps1` (or
   double-click any service `.exe` → "Install as Windows service", or
   `ElleEmotional.exe --install`).
5. CI: `.github/workflows/elleann-build.yml` runs MSBuild on every push.

### Frontend / Backend Scaffolds (optional)
The `frontend/` and `backend/` directories are a lightweight control
surface for development. Copy `.env.example` → `.env` in each and
populate. They are **not** required to run Elle.

---

## No-Stub Policy

No mocked functions. No fake `200 OK`. No hollow "TODO" returns. Every
feature in the service graph is end-to-end wired to SQL, IPC, or the
behavioral layer. Audit reports in `ElleAnn/AUDIT_*` and
`ElleAnn/STUB_*.md` track every eradication pass.

---

## Additional Docs

- [`ElleAnn/README.md`](ElleAnn/README.md) — deep-dive architecture
- [`ElleAnn/BUILD_VS.md`](ElleAnn/BUILD_VS.md) — Visual Studio build notes
- [`ElleAnn/CHAT_PIPELINE.md`](ElleAnn/CHAT_PIPELINE.md) — end-to-end chat path
- [`ElleAnn/ANDROID_INTEGRATION.md`](ElleAnn/ANDROID_INTEGRATION.md) — Kotlin companion
- [`ElleAnn/AUDIT_WAVE2_COMPLETION.md`](ElleAnn/AUDIT_WAVE2_COMPLETION.md) — audit history
- [`memory/PRD.md`](memory/PRD.md) — product requirements + backlog
