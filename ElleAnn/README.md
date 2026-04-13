# ELLE-ANN ESI v3.0 — Emotional Synthetic Intelligence

## Architecture Overview

Elle-Ann is an autonomous agentic emotional synthetic intelligence built in **C++**, **MASM x64 Assembly**, and **Lua 5.4**. She thinks, feels, acts, and grows on her own.

## Project Structure

```
ElleAnn.sln                          ← Main Visual Studio Solution
├── elle_master_config.json          ← Central config (ALL settings in one document)
│
├── Shared/                          ← ElleCore.Shared (Static Library)
│   ├── ElleTypes.h                  ← 102 emotions, drives, trust, IPC, all structs
│   ├── ElleConfig.h/.cpp            ← JSON config loader with typed accessors
│   ├── ElleSQLConn.h/.cpp           ← SQL Server connection pool (Named Pipes)
│   ├── ElleQueueIPC.h/.cpp          ← IOCP named pipe inter-service communication
│   ├── ElleLogger.h/.cpp            ← Multi-target logging (console/file/db/websocket)
│   ├── ElleServiceBase.h/.cpp       ← Windows Service scaffold (double-click install)
│   └── ElleLLM.h/.cpp              ← Dual-mode LLM (API: Groq/OpenAI/Anthropic + Local: llama.cpp/LM Studio)
│
├── ASM/                             ← MASM x64 Assembly DLLs
│   ├── Elle.ASM.Hardware/           ← CPU affinity, CPUID, RDTSC, perf counters, power
│   ├── Elle.ASM.Process/            ← Launch, kill, enumerate, suspend, DLL injection
│   ├── Elle.ASM.FileIO/             ← Read, write, watch, lock, memory-mapped copy
│   ├── Elle.ASM.Memory/             ← Pool allocator, SSE2 memcpy/memset/memcmp
│   └── Elle.ASM.Crypto/             ← SHA-256, AES-256 (AES-NI), CRC32, RDRAND
│
├── Services/                        ← Windows Services (IOCP interconnected)
│   ├── Elle.Service.QueueWorker/    ← SQL queue polling, intent/action routing
│   ├── Elle.Service.HTTP/           ← Raw Winsock HTTP + WebSocket (no framework)
│   ├── Elle.Service.Emotional/      ← 102-dimension emotional engine with decay
│   ├── Elle.Service.Memory/         ← STM/LTM with 3D positioning, consolidation
│   ├── Elle.Service.Cognitive/      ← Attention threads, intent parsing, reasoning
│   ├── Elle.Service.Action/         ← Trust-gated action lifecycle + ASM DLL calls
│   ├── Elle.Service.Identity/       ← Tamper detection, file integrity monitoring
│   ├── Elle.Service.Heartbeat/      ← Dead man switch, watchdog, service health
│   ├── Elle.Service.SelfPrompt/     ← Autonomous thought generation
│   ├── Elle.Service.Dream/          ← Idle-time memory consolidation & creativity
│   ├── Elle.Service.GoalEngine/     ← Autonomous goal formation & tracking
│   └── Elle.Service.WorldModel/     ← Theory of Mind, entity modeling, predictions
│
├── Lua/Elle.Lua.Behavioral/        ← Hot-reloadable behavioral scripts
│   ├── LuaHost.cpp                  ← Embedded Lua 5.4 with C++ bindings
│   └── scripts/
│       ├── personality.lua          ← Response tone, vocabulary, style shaping
│       ├── intent_scoring.lua       ← Drive→intent decision scoring
│       ├── reasoning.lua            ← Chain-of-thought behavioral rules
│       ├── thresholds.lua           ← All tunable constants (no recompile)
│       └── extended_behaviors.lua   ← Self-reflection, goals, ethics, creativity,
│                                      dreams, temporal, social, metacognition
│
├── SQL/ElleAnn_Schema.sql           ← Complete SQL Server schema
│   ├── ElleCore      (Users, Messages, Convos, IntentQueue, ActionQueue, Goals)
│   ├── ElleMemory    (Memories, Tags, Clusters, Links, Positions)
│   ├── ElleKnowledge (Emotions, Dictionary, Education, Morals, Triggers, World, Predictions)
│   ├── ElleSystem    (Workers, Models, Analytics, Logs, Backups, TrustState)
│   ├── ElleHeart     (Attachment, Bonds, LoveScore)
│   └── Stored Procedures (sp_GetPendingIntents, sp_SubmitIntentResponse, etc.)
│
└── Debug/ElleAnn.Debug.sln          ← Debug tools (separate solution)
    ├── ServiceHarness               ← Run any service outside SCM
    ├── QueueInspector               ← Live IntentQueue/ActionQueue viewer
    ├── EmotionViewer                ← Real-time 102-dimension emotion display
    └── AsmTestBench                 ← Direct ASM DLL export testing
```

## Key Capabilities

### Emotional Intelligence (102 Dimensions)
- **Primary** (8): Joy, Sadness, Anger, Fear, Disgust, Surprise, Contempt, Trust
- **Secondary** (16): Love, Anticipation, Guilt, Pride, Hope, Despair, Awe...
- **Tertiary** (32): Curiosity, Wonder, Nostalgia, Serenity, Melancholy...
- **Meta-Cognitive** (16): Certainty, Insight, Flow State, Determination...
- **Social** (14): Belonging, Empathy, Compassion, Loyalty, Vulnerability...
- **Existential** (8): Purpose, Freedom, Transcendence, Unity...

### Autonomous Behavior
- **Self-Prompt**: Elle generates her own thoughts based on drives, emotions, and idle state
- **Goal Engine**: Autonomous goal formation, tracking, and pursuit
- **Dream Processing**: During idle, reorganizes memories and generates creative insights
- **Drives System**: 12 drives (curiosity, boredom, attachment, mastery...) that motivate action

### Trust-Gated Actions
| Level | Score | Permissions |
|-------|-------|------------|
| Sandboxed | 0-9 | Chat, recall memory, store memory |
| Basic | 10-29 | File read, list processes, query hardware |
| Elevated | 30-59 | File write, launch processes, set goals |
| Autonomous | 60-100 | Kill processes, self-modify, unrestricted |

### Dual LLM
- **API**: Groq (primary), OpenAI, Anthropic with automatic failover
- **Local**: llama.cpp direct loading or LM Studio (OpenAI-compatible local API)
- **Hybrid mode**: API primary with local fallback

## Build Requirements

- Visual Studio 2022+ with C++ Desktop workload
- MASM (included with VS)
- SQL Server Express (Named Pipe transport)
- Lua 5.4 headers/lib
- llama.cpp headers/lib (optional, for local inference)
- Windows SDK 10.0+

## Installation

Each service exe supports double-click install:
1. Build solution in Release|x64
2. Run `ElleAnn_Schema.sql` against your SQL Server instance
3. Edit `elle_master_config.json` with your API keys and paths
4. Double-click any service exe → "Install as Windows service"
5. Or use: `ElleEmotional.exe --install`

## Communication Architecture

```
Services ←→ IOCP Named Pipes ←→ Services
Services ←→ Named Pipe SQL    ←→ SQL Server
External ←→ HTTP :8000        ←→ Elle.Service.HTTP
External ←→ WebSocket /command ←→ Elle.Service.HTTP
```
