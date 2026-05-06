# Wire Captures — 2026-02-05 Fiesta server login sequence

These four port logs are LIVE packet captures from the user's Fiesta server,
captured at roughly the same login session walking through the server chain:

| Port  | Time window         | Server role (inferred)         |
|-------|---------------------|--------------------------------|
| 55891 | 18:50:22 – 18:50:23 | LoginServer (initial auth)     |
| 56538 | 18:55:29 – 18:55:30 | World/Bridge (re-auth)         |
| 56543 | 18:55:46 – 18:55:53 | Zone/GameServer (in-game data) |
| 56567 | 18:56:40 – 18:56:40 | Shop / instance / sub-server   |

(Times suggest the user logged in to LoginServer, then character-selected,
then entered world. Port assignments aren't documented here yet — they need
to be cross-checked against the live `0oneServerInfo.txt` and `JHR_ServerInfo.txt`
files in this same `_re_artifacts/` directory.)

## Format

Each line:
```
[timestamp]    (n)    <Direction>    ==    XXX | YYY    HH HH HH ...
```
- `XXX` = some 3-digit field that's identical (`004`) for all small inbound
  packets and varies (`016`, `026`) for bigger encrypted/outbound packets.
  This appears to be the **packet-format-id** or **flag byte**.
- `YYY` = 3-digit decimal opcode. **This is the NETCOMMAND value.**
- The bytes after are the packet payload (NOT including the length prefix
  or the opcode itself — those are already stripped by the capture tool).

## Critical observations

### 1. Outbound (server→client) is encrypted, Inbound is plaintext

The very first event in each port log is a 1024-byte outbound packet that's
clearly XOR499-ciphered (high-entropy bytes). Every inbound packet after
that is plain text. Two interpretations:

a. The capture tool decrypts incoming on its way in and the outgoing buffer
   is dumped pre-encrypt — but only sometimes (the first packet shows raw
   ciphertext while subsequent outbound is missing → the first IS the
   handshake key blob, and after that outbound was filtered out).
b. Only the first packet (the seed/handshake) is encrypted, and once the
   session is established both directions go plaintext.

This needs to be verified by Elle against her FiestaNetCrypto.h — but
either way, **the inbound (client→server-arriving) flow is fully readable,
which is exactly what the headless client needs**.

### 2. Elle's character name "EllaAnn" appears in the auth payload

In every port log, opcode 056 (4|056) carries:
```
05 00 00 00    45 6C 6C 65 41 6E 6E 00 00 00 00 00 00 00 00 00    96 00 ...
[userNo=5    ] [user/char string "EllaAnn" + nul-pad to 16 bytes ] [...]
```

Combined with port 56538's variant where the name is `52 6F 75 4E` ("RouN"
truncated — probably "RouN..." ), this confirms the format is:
- 4 bytes:  userNo (uint32)
- 16 bytes: nullable ASCII char/user ID
- ...:      remaining auth/session payload

### 3. Opcode 061 is a HUGE skill/spell list

Port 56538 + 56543 both show opcode 061 carrying ~1100 bytes of repeating
12-byte records:
```
DF 01 E8 03 00 00 00 00 00 00 00 00 00
DF 01 E9 03 00 00 00 00 00 00 00 00 00
DF 01 EA 03 ...
```
Pattern: `[u16 owner_id][u16 skill_id][8 bytes zero/cooldown]`. With the
PDB cross-reference, opcode 061 is almost certainly
`PROTO_NC_CHAR_USEINFO_SKILL_INFO` or similar — Elle has 1099 entries
which matches a "send character's full skill list on login" payload.

### 4. The opcode pool

Across all 4 captures, the inbound opcodes seen are:
```
056 057 058 059 061 062 206 215 ... and many more
```
shine_opcodes.tsv has 221 documented opcodes. Cross-referencing the captured
opcodes against the .tsv will tell us which are documented vs which are NEW
finds — every undocumented opcode in real traffic is a structural data
point Phase 6a's parser fills in.

## What this unlocks for Phase 6a

With these captures, the headless-client unit tests no longer need a live
server. Specifically:

1. **Cipher round-trip test** — feed the 1024-byte outbound from Port 55891
   into FiestaNetCrypto::Decrypt with the seed Elle's code computes and
   confirm the result starts with a recognizable handshake header.

2. **Opcode-to-struct dispatch tests** — for every captured opcode, look up
   the PROTO_NC_* struct of matching sizeof in MERGED_protos.json and
   verify the byte layout fits. Opcodes whose payload size doesn't match
   any known struct are flagged as needing reverse-engineering.

3. **Replay tests** — Phase 6b's world-model state machine can be fed the
   inbound stream byte-for-byte and we can assert what ends up in the world
   model (e.g. "after Port 55891 capture, Elle's known userNo == 5,
   character name == 'EllaAnn', skill_count == 1099").

This is a free regression suite — no server needed, deterministic, runs
in <100ms. It will catch every layout drift the future hits.

## Action items for Phase 6a kickoff

1. Build `_re_artifacts/wire_captures/parse_capture.py` — turn the text
   logs into a list of `(direction, opcode, payload_bytes)` tuples.
2. Cross-reference observed opcodes against shine_opcodes.tsv to size up
   the documentation gap.
3. For each (opcode, observed_payload_length), find the matching
   PROTO_NC_* struct in MERGED_protos.json and emit a coverage report.
4. Build the round-trip test harness as described above.

Estimated effort: 1-2 days from a clean baseline.
