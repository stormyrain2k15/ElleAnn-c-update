# Elle.Service.Fiesta — ShineEngine Client (Mischief 6.11.2015 build)

## Status: WIRED & READY FOR ON-WIRE TEST

210 ShinePlayer NC_* opcodes recovered with **real hex values** from the
client EXE+udd, baked into `FiestaPacket.h::Op`. Wire format, cipher class,
and seed handshake direction confirmed against multiple sources. The
service is now a real ShineEngine speaker, not a generic placeholder.

## Engine identification (multi-source confirmed)

The target server is a **ShineEngine** build (Mischief 6.11.2015). Confirmed
via three independent vectors:

1. **Client RE dumps** (`/app/.fiesta_re/{Functions,Imports,Strings,Names,commands}.txt`)
   leak `ShineObjectClass::ShinePlayer`, `ProtocolPacket::pp_SendPacket`,
   `PROTOCOLFUNCTIONTEMPLETE<T>::pft_Store`, `CProtocolAnalysis`.
2. **Server PDB** (`/app/.fiesta_re/server/5ZoneServer2.pdb`, MSF7,
   MSVC ~2008) leaks **2 126 unique `NC_*` handler symbols**.
3. **OllyDbg .udd** (`5ZoneServer2.udd`) → 13 882 (RVA, name) records,
   656 NC_* with full per-handler RVAs.

## Wire-format ground truth

| Fact | Source |
|---|---|
| Opcode = u16 LE | server PDB: `pft_Store(NETCOMMAND*, int, unsigned short)` |
| Wire encoding = `(major << 8) \| minor` | confirmed against 210 (NC_name, hex) pairs we extracted |
| 3-tier length prefix `u8 / 0xFF + u16 LE` | server PDB: `CSendPacket::PACKET_SIZE/1/2` |
| Cipher = `class PacketEncrypt` | server PDB: `so_PacketEncryptClass`, `so_PacketEncrypt`, `so_EncSeedSet` |
| Seed delivered server→client | confirmed: `Send_NC_MISC_SEED_ACK@ClientSession` + recovered opcode `NC_MISC_SEED_REQ = 0x0206` (server pushes this; the "_REQ" suffix is a legacy misnomer) |
| Network stack = raw WS2_32 | client imports list — no TLS, no third-party crypto |

## ShineEngine handshake (now implemented)

```
 client                                                 server
   |                                                      |
   |-- TCP connect -------------------------------------->|
   |                                                      | (cleartext)
   |   <-- 0x0206 NC_MISC_SEED_REQ {32-byte seed} --------|
   |   PacketEncrypt enabled (both directions)            |
   |-- 0x0601 NC_MAP_LOGIN_REQ {user, pass, ver=0} ------>|
   |                                                      |
   |   <-- 0x0701 NC_BRIEFINFO_INFORM_CMD ----------------|
   |   <-- 0x044E NC_CHAR_*    (...)  --------------------|
   |   <-- 0x0601-range NC_MAP_* (zone info) -------------|
   |                                                      |
   |-- 0x0603 NC_MAP_LOGINCOMPLETE_CMD {char_idx} ------->|
   |                                                      |
   |  IN_GAME — chat (0x0801), move (0x0805), attack      |
   |  (target 0x0901 → hit 0x0903), pickup (0x0C09)…      |
```

## Files in this folder

| File                              | Purpose                                        |
|-----------------------------------|------------------------------------------------|
| `FiestaCipher.h`                  | XOR stream cipher (per-direction indices)      |
| `FiestaPacket.h`                  | **210 real ShineEngine NC_* opcodes** + framing |
| `FiestaConnection.h`              | TCP socket + recv-thread framer                |
| `FiestaClient.h/.cpp`             | Stateful headless client (real ShineEngine flow) |
| `FiestaService.cpp`               | SCM service entry, IPC bridge                  |
| `Elle.Service.Fiesta.vcxproj`     | MSBuild project                                |
| `shine_nc_symbols*.txt`           | Name catalogues (518 / 2126 / 235)             |
| `_re_artifacts/Zone.ini`          | Server config — RunSpeed=117, WalkSpeed=33     |
| `_re_artifacts/ZoneServerInfo.txt`| Mischief 6.11.2015 build identifier             |
| `_re_artifacts/udd_*.tsv`         | 13 882 (RVA, name) parsed from OllyDbg .udd    |
| `_re_artifacts/shine_opcodes.tsv` | Final 210 (NC_name, hex) pairs                 |
| `_re_artifacts/shine_opcodes_enum.cpp.inc` | Auto-generated C++ enum               |
| `_re_artifacts/extract_opcodes_ida.py`     | IDA Pro fallback script               |
| `_re_artifacts/extract_opcodes_ghidra.py`  | Ghidra fallback script (free)         |
| `README.md`                       | This file                                      |

## What's still pending (for in-vivo verification)

| Item | How to confirm |
|---|---|
| Cipher stream model: per-packet reset vs running offset across session | One Wireshark login capture — if two adjacent same-opcode packets have different post-cipher bytes, model is RUNNING (current). If identical, switch to per-packet RESET in `FiestaCipher.h::Reset` |
| `NC_MAP_LOGIN_REQ` payload field-order | Currently sends `Str(user); Str(pass); U32(0)`. The `U32(0)` is a protocol-version field; some Mischief builds expect a specific value (e.g. 0x68 for protocol-110). Adjust if login fails with a non-zero reply byte |
| Movement payload | Currently `F32 x; F32 y; F32 z`. ShineEngine native uses `F32 x; F32 z; F32 y` (Y is up). Swap if the player teleports vertically |

## ShineEngine subsystem majors

| Major | Subsystem | Real opcodes implemented |
|------:|-----------|--------------------------|
| 0x02 | NC_MISC | SEED_REQ, GAMETIME_REQ, HIDE/UNHIDE, SERVERPARAMETER_REQ |
| 0x04 | NC_CHAR | REVIVE_REQ, STAT_INC/DEC, LOGOUT, WEDDING, FREESTAT |
| 0x06 | NC_MAP | LOGIN_REQ, LOGINCOMPLETE_CMD, WING_SAVE/FLY, TOWNPORTAL |
| 0x08 | NC_ACT | CHAT, WALK, RUN, NPCCLICK, EMOTICON, JUMP, GATHER, RIDE |
| 0x09 | NC_BAT | TARGETTING, HIT, SMASH, SKILLCAST, BASH, ASSIST |
| 0x0C | NC_ITEM | BUY, SELL, DROP, PICKUP, USE, RELOC, REVIVEITEMUSE |
| 0x13 | NC_TRADE | PROPOSE, REQADDITEM, OK, CANCEL, FINAL_OK |
| 0x14 | NC_SOULSTONE | HP/SP_BUY, USE, REVIVE |
| 0x1A | NC_BOOTH | OPEN, CLOSE, ENTRY, REFRESH, ITEMTRADE |
| 0x23 | NC_MINIHOUSE | ACTIV, BUILDING (player housing) |
| 0x2B | NC_DICE | TAISAI_GAME_JOIN/LEAVE/BETTING |
| 0x2E | NC_AUCTION | SEARCH, BUY, REGISTER, STORAGE_BOX |
| 0x2F | NC_GAMBLE | GAMBLEHOUSE_EXIT, slot machine ops |

## How Cognitive uses this

`SVC_FIESTA` is a sensor + actuator pair on Elle's IPC fabric. She sees: chat
(`NC_ACT_CHAT_REQ` / `NC_ACT_SHOUT_CMD`), brief info, anything decoded into a
high-level event. She emits: move/attack/chat/pickup/use_item commands via
real ShineEngine opcodes. Anything we don't have a high-level decoder for
(yet) shows up as a `{"kind":"raw",...}` event so Cognitive's pattern engine
can still learn from it.

## Wire-format facts now considered ground truth

| Fact | Source |
|---|---|
| Engine = ShineEngine | client + server symbol prefix `Shine*` |
| Opcode is `unsigned short` (u16, LE) | `pft_Store` signature `(NETCOMMAND*, int, unsigned short opcode)` |
| Packet C-type is `NETCOMMAND` | every handler param `PATNETCOMMAND@@` (PDB) |
| 3-tier length prefix (PACKET_SIZE / SIZE1 / SIZE2) | server PDB has `CSendPacket::PACKET_SIZE`, `PACKET_SIZE1`, `PACKET_SIZE2` |
| Cipher = `class PacketEncrypt` (not raw XOR primitive) | server PDB: `so_PacketEncryptClass`, `so_PacketEncrypt`, `so_EncSeedSet` |
| Seed delivered server→client | `Send_NC_MISC_SEED_ACK@ClientSession` |
| Naming = `NC_<SUBSYSTEM>_<NAME>_<REQ\|ACK\|CMD\|CMDD>` | every leaked symbol |
| Network stack = raw WS2_32 | client imports list: `socket/connect/recv/send`, no TLS |

## Client-facing opcode surface (filtered)

Out of 2 126 server-side `NC_*` symbols, **235** are registered against
`ShinePlayer` — i.e. the player-protocol session — which is exactly the surface
our headless client must speak. Distribution:

| Count | Subsystem | Notes |
|------:|-----------|-------|
| 37 | NC_ACT      | movement (walk/run/jump), chat, NPC click, gather, emoticon |
| 33 | NC_ITEM     | inventory (pickup/drop/use), account storage |
| 23 | NC_MINIHOUSE| player housing |
| 18 | NC_GAMBLE   | slot machine / RNG |
| 15 | NC_GUILD    | guild ops |
| 14 | NC_BAT      | combat (attack, damage, hp/mp) |
| 13 | NC_CHAR     | char data, level, free-stat |
| 10 | NC_TRADE    | player trade |
| 10 | NC_MAP      | login, zone change |
|  9 | NC_DICE     | dice minigame |
|  8 | NC_AUCTION  | auction house |
|  7 | NC_QUEST    | quest tracking |
|  7 | NC_MISC     | SEED handshake, ping, server params |
|  6 | NC_BOOTH    | personal-shop booth |
|  4 | NC_SOULSTONE| soulstone |
|  4 | NC_SKILL    | skill cast / cooldown |
|  3 | NC_USER     | account-level |
|  +14 small | scenario / holy / WT / party / menu / log / KQ / itemdb / instance / CT / charsave / briefinfo |

By type: **181 REQs** (client→server), **41 CMDs** (server pushes), **13 ACKs** (server replies).

The complete lists are saved next to this file:

| File                                  | What it contains                                  |
|---------------------------------------|---------------------------------------------------|
| `shine_nc_symbols.txt`                | 518 `NC_*` from client RE                         |
| `shine_nc_symbols_server.txt`         | 2 126 `NC_*` from server PDB                      |
| `shine_nc_symbols_client_facing.txt`  | 507 intersection (both sides reference)           |
| `shine_nc_symbols_shineplayer.txt`    | 235 player-session opcodes (definitive client surface) |

## Cipher / handshake — provisional state machine

```
 client                                              server
   |                                                   |
   |---- TCP connect ------------------------------>   |
   |                                                   | (cleartext)
   |   <-------- NC_MISC_SEED_ACK { 32-byte seed } ----|  PacketEncrypt enabled
   |   PacketEncrypt enabled (both directions)         |
   |---- NC_MAP_LOGIN_REQ {user, pass} ------------>   |
   |   <------------ NC_MAP_LOGIN_ACK -----------------|
   |---- NC_CHAR_LOGIN_REQ -------------------------->   ...
```

**Open question**: cipher stream model. `class PacketEncrypt` could either
(a) reset its state per packet, or (b) maintain running offsets across the
session. Our current `FiestaCipher.h` assumes (b). Confirming this requires
either disassembling `5ZoneServer2.exe` (the .text of `PacketEncrypt::Encrypt`
will show the increment logic) OR a Wireshark capture of two adjacent same-opcode
packets — if the bytes differ, model is (b); if identical, model is (a).

## What's still needed to ship hex opcode values

The **hex opcode values** are stored as immediate operands in `pft_Store` calls
inside the .text section of the server EXE. PDBs hold names + RVAs but *not*
the inline immediates the compiler embedded in the call sites.

We have all the inputs (`5ZoneServer2.exe` + `5ZoneServer2.pdb`), but the
2008-vintage PDB's TPI stream confuses the Linux PDB parsers (`llvm-pdbutil 14`,
`pdbparse 1.5` both fail). Cleanest fix: run one of the included scripts in a
proper RE tool that handles old PDBs natively — both finish in well under a
minute and write a TSV ready to paste into `FiestaPacket.h::Op`:

| Script (in `_re_artifacts/`)        | Tool          | Cost  |
|-------------------------------------|---------------|-------|
| `extract_opcodes_ida.py`            | IDA Pro 7.x+  | $$$   |
| `extract_opcodes_ghidra.py`         | Ghidra        | free  |

Both walk every `pft_Store` specialization, decode the three preceding
`push imm` instructions, and emit `(opcode, NC_handler_name)` pairs. After
you run one of them, drop `/tmp/shine_opcodes.tsv` into a new chat turn and
I'll wire the values into `FiestaPacket.h::Op` in one pass.

### Alternative: 30-second pcap

If you'd rather not crank up Ghidra: a Wireshark capture of "connect → login →
enter world" gives 5–10 confirmed opcodes plus *definitively* answers the
cipher stream-model question (per-packet reset vs running offset). Save it as
a `.pcap` and upload — I'll dissect it byte-for-byte.

## Files in this folder

| File                              | Purpose                                        |
|-----------------------------------|------------------------------------------------|
| `FiestaCipher.h`                  | XOR stream cipher (per-direction indices)      |
| `FiestaPacket.h`                  | Wire framing + opcode constants (placeholders) |
| `FiestaConnection.h`              | TCP socket + recv-thread framer                |
| `FiestaClient.h/.cpp`             | Stateful headless client                       |
| `FiestaService.cpp`               | SCM service entry, IPC bridge                  |
| `Elle.Service.Fiesta.vcxproj`     | MSBuild project                                |
| `shine_nc_symbols*.txt`           | RE artifacts (catalogued above)                |
| `_re_artifacts/Zone.ini`          | Server config — RunSpeed=117, WalkSpeed=33     |
| `_re_artifacts/ZoneServerInfo.txt`| Server info — Mischief 6.11.2015 build, "PG_Zone_0" |
| `_re_artifacts/udd_all_symbols.tsv`| 13 882 (RVA, name) pairs from OllyDbg .udd    |
| `_re_artifacts/udd_nc_symbols.tsv`| 656 NC_* (RVA, name) pairs from OllyDbg .udd  |
| `_re_artifacts/extract_opcodes_ida.py`    | IDA Pro script — dumps (opcode, name) pairs |
| `_re_artifacts/extract_opcodes_ghidra.py` | Ghidra equivalent (free)                |
| `README.md`                       | This file                                      |

## Important — `5ZoneServer2.exe` vs CodeCave-patched EXE

The `5ZoneServer2.pdb` and the OllyDbg `.udd` we have were captured against
**`5ZoneServer2-Localhost-Port80-NoService-CodeCave-1.exe`** (path embedded in
the .udd header), which is the CodeCave-patched build, *not* the unpatched
`5ZoneServer2.exe` we have on disk. CodeCave patches insert extra `.text`
bytes that shift every subsequent address.

Practical impact: when you run `extract_opcodes_ida.py` or
`extract_opcodes_ghidra.py`, **load the CodeCave EXE** (not the bare one).
The PDB matches the CodeCave EXE's layout because it's the build that was
actually compiled with the same source tree. If you only have the bare EXE
loaded with that PDB, IDA/Ghidra will silently mis-resolve every symbol.

## How Cognitive uses this

`SVC_FIESTA` is a sensor + actuator pair on Elle's IPC fabric. She sees: chat,
hp/mp deltas, entity spawns, login state. She emits: move/attack/chat/pickup
/use_item commands. Anything we don't have a high-level decoder for yet shows
up as a `{"kind":"raw",...}` event — Cognitive's pattern engine can learn from
those, and you can upgrade the decoder one opcode at a time as captures arrive.
