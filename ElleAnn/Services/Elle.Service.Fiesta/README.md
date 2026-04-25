# Elle.Service.Fiesta — ShineEngine Intel & TODO

## Engine identification (Feb 2026, multi-source confirmed)

The target server is a **ShineEngine** build (Fiesta Online / Isya / JFOL / CN-Fiesta).
Confirmation comes from three independent artifacts:

1. **Client RE dumps** (`/app/.fiesta_re/{Functions,Imports,Strings,Names,commands}.txt`)
   leak class names: `ShineObjectClass::ShinePlayer`, `ProtocolPacket::pp_SendPacket`,
   `PROTOCOLFUNCTIONTEMPLETE<T>::pft_Store`, `CProtocolAnalysis`.
2. **Server PDB** (`/app/.fiesta_re/server/5ZoneServer2.pdb`, MSF7, MSVC ~2008)
   leaks **2 126 unique `NC_*` handler symbols** including the demangled MSVC
   forms `?sp_NC_MISC_SEED_REQ@ShinePlayer@ShineObjectClass@@QAEXPATNETCOMMAND@@HG@Z`.
3. **WorldManager IDA-export** (`4WorldManagerServer2.h`) supplies CRT / Win32
   typedefs (PROTOCOL types not in the export).

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
| `_re_artifacts/extract_opcodes_ida.py`    | IDA Pro script — dumps (opcode, name) pairs |
| `_re_artifacts/extract_opcodes_ghidra.py` | Ghidra equivalent (free)                |
| `README.md`                       | This file                                      |

## How Cognitive uses this

`SVC_FIESTA` is a sensor + actuator pair on Elle's IPC fabric. She sees: chat,
hp/mp deltas, entity spawns, login state. She emits: move/attack/chat/pickup
/use_item commands. Anything we don't have a high-level decoder for yet shows
up as a `{"kind":"raw",...}` event — Cognitive's pattern engine can learn from
those, and you can upgrade the decoder one opcode at a time as captures arrive.
