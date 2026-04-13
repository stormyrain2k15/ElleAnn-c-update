# ELLE-ANN ESI v3.0 — Product Requirements Document

## Original Problem Statement
User requested building a massive autonomous agentic emotional synthetic intelligence system based on a detailed topology. The system should be built in C++, MASM (x64 assembly), and Lua — no Python, no web frontend. The AI must support both API-based LLM (Groq) and local inference (llama.cpp/LM Studio). Services communicate via IOCP named pipes, SQL Server via named pipes, and each service exe should be double-click installable as a Windows Service.

## Architecture
- **Language**: C++ (services/shared lib), MASM x64 (DLLs), Lua 5.4 (behavioral scripts)
- **Database**: SQL Server (5 databases: ElleCore, ElleMemory, ElleKnowledge, ElleSystem, ElleHeart)
- **IPC**: IOCP named pipes between services
- **SQL Transport**: Named pipes
- **LLM**: Dual-mode (API: Groq/OpenAI/Anthropic + Local: llama.cpp/LM Studio)

## User Personas
- **Developer/Creator**: Builds and configures Elle-Ann, sets trust levels, manages goals
- **End User**: Interacts with Elle through chat, voice, hardware commands (Android companion app — future)

## Core Requirements (Static)
1. 102-dimension emotional state machine with decay and contagion
2. STM/LTM memory system with 3D positioning and consolidation
3. Autonomous thought generation (self-prompt)
4. Trust-gated action system (4 levels: Sandboxed → Autonomous)
5. 12-drive motivation system
6. Dual LLM (API + local)
7. IOCP inter-service communication
8. Named pipe SQL Server connection
9. Double-click service installation
10. Hot-reloadable Lua behavioral scripts
11. Dream processing and creative synthesis
12. Theory of Mind / World Model
13. Ethical reasoning framework
14. Central configuration document

## What's Been Implemented (Jan 2026)

### Shared Library (6 headers + 6 implementations)
- ElleTypes.h — 102 emotions, 12 drives, all structs, ASM DLL imports
- ElleConfig — JSON parser, typed config accessors, hot-reload
- ElleSQLConn — ODBC connection pool, ElleDB namespace helpers
- ElleQueueIPC — IOCP server/client/hub, message serialization
- ElleLogger — Multi-target logging with rotation
- ElleServiceBase — Windows Service scaffold with double-click install
- ElleLLM — Groq/OpenAI/Anthropic API + llama.cpp local + failover

### MASM DLLs (5 DLLs, 10 files)
- Hardware (CPUID, RDTSC, CPU affinity, power, perf counters)
- Process (launch, kill, enumerate, suspend, DLL injection)
- FileIO (read, write, watch, lock, memory-mapped copy)
- Memory (pool allocator, SSE2 memcpy/memset/memcmp)
- Crypto (SHA-256, AES-256 AES-NI, CRC32, RDRAND)

### Services (12 services)
- QueueWorker, HTTP/WebSocket, Emotional, Memory, Cognitive
- Action, Identity, Heartbeat, SelfPrompt, Dream, GoalEngine, WorldModel

### Lua Scripts (5 files)
- personality, intent_scoring, reasoning, thresholds, extended_behaviors

### SQL Schema (1 comprehensive file)
- 5 databases, 20+ tables, 6 stored procedures, seed data

### Debug Solution (4 tools)
- ServiceHarness, QueueInspector, EmotionViewer, AsmTestBench

### Configuration
- elle_master_config.json — ALL settings in one document

## Prioritized Backlog

### P0 (Critical)
- Visual Studio .vcxproj files for each project
- Compile and link testing on Windows
- llama.cpp actual integration (scaffold provided)

### P1 (High)
- Android Kotlin companion app
- Voice call system (TTS/STT)
- Embedding-based memory search (replace keyword similarity)
- Full WebSocket protocol implementation

### P2 (Medium)
- Drives engine as separate service
- Memory visualization UI
- Performance profiling
- Encrypted IPC channel

### P3 (Low)
- Multi-user support
- Plugin system for custom actions
- Neural network-based emotion detection
- Distributed service deployment

## Next Tasks
1. Generate .vcxproj files for all projects
2. Test compilation on Windows with Visual Studio 2022
3. Integrate actual llama.cpp for local inference
4. Build Android companion app (Kotlin/Jetpack Compose)
5. Implement WebSocket frame encoding/decoding in HTTP service
