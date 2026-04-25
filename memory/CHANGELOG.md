# Elle-Ann ESI v3.0 — CHANGELOG

(PRD.md is the static source of truth; this file is the running log of
what landed and when. Newest entries on top.)

## 2026-02 — Elle.Service.Fiesta — Headless Game Client Complete (P0)

Service count: **20 → 21** (`SVC_FIESTA = 20`). New end-to-end MMO bridge
so Elle can perceive and act inside a Fiesta-style world directly from
the cognitive engine.

**Files added:**
- `Services/Elle.Service.Fiesta/FiestaCipher.h`     — 32-byte XOR stream cipher, dual in/out indices.
- `Services/Elle.Service.Fiesta/FiestaPacket.h`     — size-prefix framing (u8 / 0xFF+u16), Writer/Reader LE primitives, opcode registry (login/world/move/combat/chat/inventory/entity).
- `Services/Elle.Service.Fiesta/FiestaConnection.h` — single-socket TCP conn, recv-thread framer, send mutex, cipher boundary respected.
- `Services/Elle.Service.Fiesta/FiestaClient.h/.cpp` — full state machine (DISCONNECTED → LOGIN → WORLD_LIST → WORLD_HANDOFF → IN_GAME), opcode dispatcher emits high-level JSON events + raw fallback for unknown opcodes, heartbeat thread.
- `Services/Elle.Service.Fiesta/FiestaService.cpp`     — **NEW THIS SESSION.** SCM-registered `ElleServiceBase` subclass `ElleFiestaService`. Wires `Fiesta::Client` into the IPC fabric: receives `IPC_FIESTA_COMMAND` from Cognitive (login/select_world/select_char/move/attack/chat/pickup/use_item/respawn/raw/disconnect), broadcasts `IPC_FIESTA_EVENT` (high-level + raw inbound) to `SVC_COGNITIVE` and `SVC_HTTP_SERVER`, runs a config-driven exponential-backoff reconnect watchdog (5s → 60s).
- `Services/Elle.Service.Fiesta/Elle.Service.Fiesta.vcxproj` — **NEW.** GUID `B1000000-…-000000000014`, mirrors Bonding's structure, references `ElleCore.Shared`.
- `Deploy/Configs/fiesta.json` — **NEW.** Config skeleton (`fiesta.host`, `fiesta.port`, `fiesta.username`, `fiesta.password`, `fiesta.auto_login`, `fiesta.reconnect_min_ms`, `fiesta.reconnect_max_ms`). Credentials must be filled locally; defaults stay empty so a missing config can never silently connect somewhere wrong.
- `backend/tests/fiesta_smoke.cpp` — Linux/g++ -Wall -Wextra -Werror smoke test of the portable layer (cipher round-trip, disabled-passthrough, writer/reader primitives, short+long packet framing, full encrypt-and-parse round-trip). All 5 assertions pass.

**Files modified:**
- `ElleAnn.sln` — project entry, Debug/Release|x64 configs, `NestedProjects` slot under the Services solution folder.
- `Deploy/elle_service_manifest.json` — `ElleFiesta` registered with `start: "manual"` and `depends: [ElleHeartbeat, ElleCognitive]`.

**Already in place from prior session (not touched again):**
- `Shared/ElleTypes.h`     — `SVC_FIESTA` enum slot (20), `IPC_FIESTA_COMMAND`, `IPC_FIESTA_EVENT` IPC types with full JSON schema docstrings.
- `Shared/ElleQueueIPC.cpp` — `g_serviceNames[]` extended with `"Fiesta"`, `static_assert` against `ELLE_SERVICE_COUNT` still passes.

**Testing status**
- ✅ Linux portable smoke test passes (cipher correctness, framing, round-trip).
- ⏳ Windows MSBuild/SCM integration must run on the user's local PC — Windows headers / winsock / SQL Server are unavailable in this Linux container by design.

**Constraints observed**
- No copyrighted Fiesta game assets downloaded or embedded.
- No-stub policy: every IPC op is wired to real `Fiesta::Client` calls, no fake 200s.
- `/WX`-clean by inspection; no raw `Sleep()` in tick loop (uses `SetTickInterval(1000)`); no broad `catch(...)` (the sole `catch (const std::exception& e)` matches the existing Bonding pattern for JSON parse).
