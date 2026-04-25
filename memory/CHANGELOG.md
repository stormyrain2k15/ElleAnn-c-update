# Elle-Ann ESI v3.0 — CHANGELOG

(PRD.md is the static source of truth; this file is the running log of
what landed and when. Newest entries on top.)

## 2026-02 — ShineEngine RE pass for Elle.Service.Fiesta

User shared client RE artifacts (`Functions.txt`, `Imports.txt`,
`Strings.txt`, `Names.txt`, `commands.txt`), the server PDB
(`5ZoneServer2.pdb`), the server EXE (`5ZoneServer2.exe`), the server
config (`Zone.ini` — RunSpeed=117, WalkSpeed=33), `ZoneServerInfo.txt`
(Mischief 6.11.2015 build, server tag `PG_Zone_0`), and `Fiesta.dll`
(client security-check bypass).

**Engine identified**: ShineEngine (Fiesta Online / Isya / CN-Fiesta).
Confirmed via three independent vectors — client RTTI strings, server
PDB demangled symbols, and class-name patterns (`ShineObjectClass::ShinePlayer`,
`ProtocolPacket::pp_SendPacket`, `PROTOCOLFUNCTIONTEMPLETE<T>::pft_Store`).

**Catalogue extracted** to `Services/Elle.Service.Fiesta/`:
- `shine_nc_symbols.txt` — 518 `NC_*` from client.
- `shine_nc_symbols_server.txt` — 2 126 `NC_*` from server PDB.
- `shine_nc_symbols_client_facing.txt` — 507 intersection.
- `shine_nc_symbols_shineplayer.txt` — **235** opcodes registered
  against `ShinePlayer` — the precise client surface (181 REQ + 41 CMD
  + 13 ACK across 28 subsystems: ACT, ITEM, MINIHOUSE, GAMBLE, GUILD,
  BAT, CHAR, TRADE, MAP, etc.).

**Wire format upgraded to ground truth** (was: documented JFOL/CN
guesses; now: confirmed from PDB):
- u16 LE opcode (was assumed; PDB `pft_Store(NETCOMMAND*, int, unsigned short)` confirms).
- 3-tier length prefix `u8 / 0xFF + u16` (server PDB: `CSendPacket::PACKET_SIZE`,
  `PACKET_SIZE1`, `PACKET_SIZE2`).
- Cipher class `PacketEncrypt` (server PDB: `so_PacketEncryptClass`,
  `so_PacketEncrypt`, `so_EncSeedSet`).
- Seed delivered server→client via `NC_MISC_SEED_ACK` (PDB:
  `Send_NC_MISC_SEED_ACK@ClientSession`).

**Hex opcode values still pending** — they live as immediate operands in
the EXE's `.text` section; recovering them needs IDA-Pro / Ghidra-grade
type-aware analysis. The 2008-vintage PDB's TPI stream confuses
`llvm-pdbutil 14` and `pdbparse 1.5`. Two ready-to-run extraction scripts
delivered:
- `_re_artifacts/extract_opcodes_ida.py` (IDA 7.x+)
- `_re_artifacts/extract_opcodes_ghidra.py` (Ghidra, free)

Both walk every `pft_Store` specialization, decode the three preceding
`push imm` instructions, and emit a TSV of `(opcode, NC_handler_name)`
pairs ready to paste into `FiestaPacket.h::Op`.

**Doc & header updates** in `Services/Elle.Service.Fiesta/`:
- `README.md` — fully rewritten, multi-source intel + handshake state
  diagram + "what's still needed" + extraction-script paths.
- `FiestaPacket.h` — header docstring rewritten to flag ShineEngine
  identification, accurate wire-format facts, and the
  status-of-opcode-values caveat.

**Smoke test still green** under `-Wall -Wextra -Werror`:
```
[ok] cipher roundtrip
[ok] cipher disabled passthrough
[ok] writer/reader primitives
[ok] build packet (short + long framing)
[ok] full encrypt-and-parse round trip
```

---

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
