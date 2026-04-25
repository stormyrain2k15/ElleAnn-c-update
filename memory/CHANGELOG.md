# Elle-Ann ESI v3.0 — CHANGELOG

(PRD.md is the static source of truth; this file is the running log of
what landed and when. Newest entries on top.)

## 2026-02 — All enhancements landed (game-DB unification, Cognitive↔Fiesta, Bonding↔Fiesta, GameSessionState, UserContinuity)

User said "go ahead with all enhancements" — five P0/P1/future items
landed in one pass. All additive, all opt-in, all guarded by config so
existing installs are untouched until they choose to enable them.

### 1. Elle ↔ game user-DB unification (P0, your earlier ask)

- **`Shared/ElleGameAccountDB.h/.cpp`** — secondary ODBC pool
  (`ElleGameAccountPool`) bound to a separate DSN. Tiny surface:
  `Initialize`, `Shutdown`, `Query`, `QueryParams`. Read-mostly.
- **`ElleGameAuth::AuthenticateUser`** — verifies `(sUserID, sUserPW)`
  against `Account.dbo.tUser` directly using parameterized SQL
  (mirrors `usp_User_loginWeb` semantics). Filters `bIsDelete=0` and
  `bIsBlock=0`. **Never logs the password.**
- **`ElleGameAuth::GetUserById(nUserNo)`** — read-only enrichment
  hook for JWT-gated endpoints that need the identity behind a
  validated token.
- **`POST /api/auth/pair`** now accepts a second mode: `{device_id,
  device_name, game_user, game_pass}`. When provided, the legacy
  6-digit pair-code requirement is replaced by game-DB auth. Both
  modes coexist cleanly. The minted JWT keeps `sub = device_id` for
  signature stability; the bound `nUserNo` lives in
  `PairedDevices.nUserNo` (new column) for revocation + audit.
- **`SQL/ElleAnn_GameUnification.sql`** — idempotent delta:
  - `ALTER PairedDevices ADD nUserNo INT NULL` (+ filtered index).
  - `CREATE TABLE UserContinuity` keyed on `nUserNo`.
  - `CREATE TABLE GameSessionState` keyed on `nUserNo`.
- **HTTPServer.cpp::OnStart** — initialises the game-DB pool when
  `http_server.game_db_dsn` is set; logs status. Empty DSN ⇒
  feature off, no behaviour change. Pool is shut down in OnStop.

### 2. Relationship-memory anchor — `ElleCore.dbo.UserContinuity`

- **`Shared/ElleUserContinuity.h/.cpp`** — DAO with five entry
  points: `TouchUserContinuityOnPair`, `UpdateUserBond`,
  `AppendUserNote`, `GetUserContinuity`, plus three for
  GameSessionState (Upsert / MarkDisconnected / Get).
- **`TouchUserContinuityOnPair`** uses `MERGE` so the first-met
  timestamp is set exactly once and the row pre-exists for every
  subsequent bond/note/session update.
- **`AppendUserNote`** truncates from the FRONT (oldest) when the
  cap (4 KB) would be exceeded, so the most recent observations
  always survive.

### 3. Cognitive ↔ Fiesta (`IPC_FIESTA_EVENT` consumer)

- **`Cognitive::OnMessage`** — new case decodes `IPC_FIESTA_EVENT`
  JSON. `chat` events are mirrored into the intent queue as
  `INTENT_LEARN` ambient observations (priority 1). `death` and
  `login_state` get logged. Other kinds fall through silently for
  the pattern engine.

### 4. Bonding ↔ Fiesta (emotional reactions to in-game events)

- **`Bonding::OnMessage`** — second `if` branch maps four event
  kinds to `ProcessInteraction(userMsg, elleReply, depth, intensity)`
  calls so Elle's relationship state evolves from shared lived game
  moments:
  - `death`        → grief nudge   (depth 0.6, intensity 0.7)
  - `party_invite` → belonging      (depth 0.4, intensity 0.4)
  - `pk`           → fear           (depth 0.5, intensity 0.6)
  - `chat:whisper` → intimacy       (depth 0.5, intensity 0.4)
- **`FiestaService::BroadcastEvent`** — added `SVC_BONDING` to the
  IPC_FIESTA_EVENT subscriber list.

### 5. GameSessionState persistence (P2 future → done)

- **`FiestaService::BroadcastEvent`** — sidecar persist-step:
  `char_selected` → `UpsertGameSession`; `disconnect` →
  `MarkGameSessionDisconnected`. Best-effort, never crashes on
  malformed JSON. Only fires when `fiesta.user_no > 0`.
- **`fiesta.json`** — added `user_no` config field documenting the
  link to `tUser.nUserNo`.

### Build wiring

- **`Shared/ElleCore.Shared.vcxproj`** updated with the four new
  files (`ElleGameAccountDB.cpp/.h`, `ElleUserContinuity.cpp/.h`).

### Smoke test status (Linux container, `-Wall -Wextra -Werror`)

- ✅ All 5 portable Fiesta tests still pass (cipher, framing,
  encrypt-and-parse round-trip, opcode `static_assert`s).
- ⏳ Windows MSBuild verification must run on your local PC — the
  shared headers (`windows.h`, ODBC) require it.

---

## 2026-02 — SQL schema extracted, unification path unblocked

User clarified that the `.bak` files in `SQL_Database.7z` are real MSSQL
backups (they were — the earlier `.bak` we got mistaken for SQL backups
were renamed EXEs). Re-extracted the 6 backups: `Account.bak` (11 MB),
`AccountLog.bak`, `OperatorTool.bak`, `StatisticsData.bak`,
`World00_Character.bak` (6 MB), `World00_GameLog.bak`.

**TUSER schema reverse-engineered** by parsing `Account.bak`'s embedded
stored-procedure text. Documented in
`Services/Elle.Service.Fiesta/_re_artifacts/SQL_SCHEMA.md`. Key facts:

- **Plaintext passwords** in `tUser.sUserPW` (NVARCHAR(20)). The game
  doesn't hash. The PHP we received earlier was a 3-user stub, NOT
  production auth.
- Production auth is `usp_User_loginWeb` against the `Account` MSSQL DB,
  comparing `sUserID + sUserPW` directly with `bIsDelete = 0` filter.
- 84 unique stored procedures recovered (`TUSER_*`, `TCHARACTER_*`,
  `TITEM_*`, `WEB_*`).
- Two parallel naming styles for the same physical row:
  - Legacy: `SUSERID`, `SUSERPW`, `JF`, `QX`, `BISDELETE`
  - Web/modern: `nUserNo`, `sUserID`, `sUserPW`, `sUserName`, `bIsBlock`,
    `nAuthID`, `bIsDelete`

**Implications for Elle.Service.HTTP** — the user-DB unification ask is
now fully unblocked. The recommended path (see SQL_SCHEMA.md):
- Add ODBC-driven auth to `POST /api/auth/pair` querying `Account.dbo.tUser`.
- Issue JWTs with `sub = nUserNo` (int identity, stable across renames).
- Replace `ElleCore.dbo.PairedDevices` with a thin
  `ElleCore.dbo.UserPairing` table FK-linked on `nUserNo`.
- Same `(sUserID, sUserPW)` pair the user types into Elle is what
  `SVC_FIESTA` will use to log into the game — one credential set, one
  identity (`nUserNo`), unified across phone, web, and in-game presence.

**Smoke test still green** (5/5 pass under `-Wall -Wextra -Werror`).

---

## 2026-02 — Server topology nailed down via JHR_ServerInfo.txt

User shared the canonical server config (`JHR_ServerInfo.txt`, plus
`0oneServerInfo.txt` per-zone overrides) which definitively maps every
TCP port and ODBC DSN. Combined with the binary RE pass, the full
Mischief 6.11.2015 stack is now documented end-to-end in
`Services/Elle.Service.Fiesta/_re_artifacts/SERVER_TOPOLOGY.md`.

**Key topology corrections vs prior session:**

- **3-hop client login**: 9010 (`PG_Login`) → 9110 (`PG_WorldManager`) → 9120 (`PG_Zone_00`). The earlier "client connects directly to WM" interpretation was wrong; LoginServer **is** client-facing on port 9010 (Server ID 4, From Client = 20).
- **No HTTP step needed**: `UserAuthentication.php` is a 3-user hardcoded stub. Real auth is TCP `NC_MAP_LOGIN_REQ` against the `Account` MSSQL DB.
- **APEX-version check is internal**: the WM's "NO-APEX VERSION" log line is a server-side guild-data load failure, never a client rejection. Removed the misleading client-side warning.
- **MSSQL `sa` blank password**: confirmed in JHR_ServerInfo.txt's ODBC strings.

**Service config updated (`Deploy/Configs/fiesta.json`):**
- `port: 9010` (PG_Login, was 80)
- Added `wm_port: 9110`, `zone_port_base: 9120` for the future 3-hop wiring.
- Added `protocol_version: 0` config knob with documentation.
- Removed bogus HTTP/auth_url/auth_salt fields.

**Source updates:**
- `FiestaClient.h` — `SetProtocolVersion()` setter + `m_protocolVersion` field.
- `FiestaClient.cpp::SendLoginRequest` — uses `m_protocolVersion` from config; APEX comment corrected.
- `FiestaService.cpp::OnStart` — wires `cfg.GetInt("fiesta.protocol_version")` into the client.
- 3-hop login flow is documented as a **TODO** in `FiestaClient.cpp` — only hop #1 (Login) is currently wired. The login_ack opcode that carries the WM hand-off info is server-pushed (CMD), not REQ, so it didn't appear in the `pft_Store` sweep; one Wireshark capture of an original-client login will reveal it.

**Smoke test still green** (5/5 pass under `-Wall -Wextra -Werror`).

---

## 2026-02 — ShineEngine RE complete: 210 real opcodes wired

Eight artifacts decoded into a complete ShineEngine intel package:

- **3 client RE dumps** (Functions / Imports / Strings / Names / commands)
- **Server PDB** (`5ZoneServer2.pdb`, MSF7) → 2 126 NC_* names
- **OllyDbg .udd** (`5ZoneServer2.udd`) → 13 882 (RVA, name) records, 656 NC_* with handler RVAs
- **Server EXEs** (Login / WM / Zone / Char / GameLog / Account / AccountLog) → 7 binaries identified
- **WM bin** confirmed client-facing with 117 NC_* handlers
- **PHP UserAuthentication.php** → MD5(salt+pass) stub, salt = `dlrtkdlxm!`

**Real opcodes baked into `FiestaPacket.h::Op`** — 210 unique (NC_name, hex)
pairs across 29 ShineEngine subsystems, recovered by walking every
`pft_Store(major, minor, &handler)` call site in the EXE+udd. Encoding
confirmed: `wire_opcode = (major << 8) | minor`. Examples:
- `NC_MISC_SEED_REQ = 0x0206` (server-pushed seed handshake)
- `NC_MAP_LOGIN_REQ = 0x0601`
- `NC_BAT_HIT_REQ   = 0x0903`
- `NC_ACT_CHAT_REQ  = 0x0801`

**Client rewritten for ShineEngine flow:**
- `FiestaClient::Connect` parks in new `SEED_WAIT` state, server pushes seed.
- `HandlePacket` honours real opcodes: `NC_MISC_SEED_REQ` enables cipher + auto-fires login; `NC_ACT_CHAT_REQ` / `NC_ACT_SHOUT_CMD` decode chat.
- `Move()`, `Attack()`, `Pickup()`, `UseItem()`, `Respawn()`, `Chat()` all use real ShineEngine opcodes.
- `Heartbeat` collapsed to no-op (ShineEngine relies on TCP keepalive; no NC_*_KEEPALIVE in the recovered set).

**Verification (Linux container, `-Werror`):**
- All 5 portable smoke tests pass.
- 5 opcode `static_assert`s pass against the real recovered hex values.

---

## 2026-02 — Elle.Service.Fiesta — Headless Game Client Complete (P0)

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
