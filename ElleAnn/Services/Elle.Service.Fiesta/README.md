# Elle.Service.Fiesta — ShineEngine Intel & TODO

## Engine identification (confirmed Feb 2026)

The target server is a **ShineEngine** build (the engine under Fiesta
Online / Isya / JFOL / CN-Fiesta). Confirmation vectors from the
user-supplied client RE artifacts (`/app/.fiesta_re/*.txt`):

- Class names in error strings: `ShineObjectClass::ShinePlayer`,
  `ProtocolPacket::pp_SendPacket`, `PROTOCOLFUNCTIONTEMPLETE<T>::pft_Store`,
  `CProtocolAnalysis::LogWriteAllNetCommand`.
- Network commands follow the canonical `NC_<SUBSYSTEM>_<NAME>_<TYPE>`
  convention where `TYPE ∈ {REQ, ACK, CMD, CMDD}`.
- 518 unique `NC_*` identifiers leaked through error format strings —
  full list in `shine_nc_symbols.txt`.

## Cipher / handshake (partial confirmation)

- Symbols `sp_NC_MISC_SEED_REQ` and `sp_NC_MISC_SEED_REQ : No PacketEncrypt`
  confirm the engine uses a **SEED-based** XOR stream cipher: the client
  sends `NC_MISC_SEED_REQ`, server replies with the 32-byte seed, both
  sides flip the cipher on for all subsequent traffic.
- The size-prefix byte stays cleartext (else the receiver couldn't read
  it before knowing the key).
- The exact stream model (per-packet reset vs running offset across
  packets) is **NOT** yet confirmed — `FiestaCipher.h` currently uses
  the running-offset model. If the PDB or a packet capture proves
  per-packet reset, flip `m_inIdx`/`m_outIdx` to reset on each call to
  `DecryptIn`/`EncryptOut` (one-line change).

## Subsystem coverage of leaked NC_* symbols

```
  82  NC_ITEMDB     — item database / catalog operations
  55  NC_CHAR       — character data, stats, level, wedding, PK toggle
  39  NC_GUILD      — guild membership, ranks, wars
  37  NC_ACT        — actions / animations
  31  NC_ITEM       — inventory pickup/drop/use, account storage
  29  NC_GAMBLE     — slot machine / dice / RNG events
  24  NC_QUEST      — quest tracking / rewards
  22  NC_MINIHOUSE  — player housing
  17  NC_AUCTION    — auction house
  16  NC_MISC       — SEED handshake, ping, server params, hide/show
  15  NC_BAT        — combat (Battle): attacks, damage, hp/mp
  14  NC_DICE       — dice rolling minigame
  13  NC_CHARSAVE   — character save persistence
  12  NC_MAP        — login/logout, zone transfer
  11  NC_SKILL      — skill cast, cooldown
  11  NC_RAID       — raid/dungeon
  11  NC_PARTY      — party invite/leave/info
  10  NC_TRADE      — player-to-player trading
   8  NC_USER       — account-level commands
   8  NC_KQ         — Kingdom Quest
   7  NC_MID        — middleware
   7  NC_CT         — CharTitle?
   6  NC_BOOTH      — personal shop / merchant booth
   5  NC_WT         — world tournament?
   5  NC_HOLY       — holy event
```

## What we still need to ship a real, online client

### CRITICAL — to populate hex opcode values
Either of:
1. **Client `*.pdb`** — xrefs to `pft_Store(opcode, handler)` give the
   complete `(NC_name → hex)` map.
2. **Server `Protocol/NC.h`** (or equivalent) — the source of truth
   defining every `NC_*` constant by hex value. Server is in your
   possession; this file is typically under
   `Source\Server\Protocol\` or `\Common\NC.h`.

Either source unblocks: combat, inventory, party, guild, trade, quest,
auction — i.e., real gameplay.

### NICE TO HAVE
- A 2–3 packet capture from your live login flow (Wireshark, dump
  raw bytes). Confirms cipher stream model (per-packet vs running)
  and validates our framing in vivo.
- Server-side login schema (account table) to confirm if password is
  hashed before sending or done server-side post-receive.

## Files in this folder

| File                              | Purpose                                        |
|-----------------------------------|------------------------------------------------|
| `FiestaCipher.h`                  | XOR stream cipher (per-direction indices)      |
| `FiestaPacket.h`                  | Wire framing + opcode constants                |
| `FiestaConnection.h`              | TCP socket + recv-thread framer                |
| `FiestaClient.h/.cpp`             | Stateful headless client                       |
| `FiestaService.cpp`               | SCM service entry, IPC bridge                  |
| `Elle.Service.Fiesta.vcxproj`     | MSBuild project                                |
| `shine_nc_symbols.txt`            | 518 leaked `NC_*` identifiers (RE artifact)    |
| `README.md`                       | This file                                      |

## How Cognitive uses this

`SVC_FIESTA` is a sensor + actuator pair on Elle's IPC fabric. She
sees: chat, hp/mp deltas, entity spawns, login state. She emits:
move/attack/chat/pickup/use_item commands. The packet stream that
`FiestaPacket.h` doesn't decode yet shows up as `{"kind":"raw",...}`
events — Cognitive's pattern engine can learn from those and you can
upgrade the decoder one opcode at a time.
