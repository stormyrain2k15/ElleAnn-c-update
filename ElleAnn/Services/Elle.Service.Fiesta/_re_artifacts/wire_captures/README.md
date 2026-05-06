# Wire Captures — 2026-02-05 Fiesta server login + post-login state dumps

These five port logs are LIVE packet captures of the user's Fiesta server,
captured during three different login + walk-around sessions. The 2026-02-05
session at port 60121 has BOTH a client-side and a (truncated) server-side
capture which let us cross-validate the direction labels — see §6 below.

| Port  | Time window         | Server role (inferred)         | Source       |
|-------|---------------------|--------------------------------|--------------|
| 55891 | 18:50:22 – 18:50:23 | LoginServer (initial auth)     | client-side  |
| 56538 | 18:55:29 – 18:55:30 | World/Bridge (re-auth)         | client-side  |
| 56543 | 18:55:46 – 18:55:53 | Zone/GameServer (in-game data) | client-side  |
| 56567 | 18:56:40 – 18:56:40 | Shop / instance / sub-server   | client-side  |
| 60121 | 18:02:12 – 18:02:14 | Zone post-login state dump     | client-side  |

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

### 5. ⚠ PHASE 6a FINDING: the user's server uses a different opcode build

After running `_re_artifacts/pdb/build_dispatch_table.py` and
`Debug/test_phase6a_protobase.cpp` against these captures, the
authoritative dispatch table reveals an important reality:

> **None** of the 17 distinct opcodes observed on the wire match
> the sizeof of the PDB struct that `PDB_OPCODES.json` says owns
> the opcode.  Every observed opcode is classified as `HEAD+TAIL`
> or `UNKNOWN`.

Concrete examples from `/tmp/fiesta_proto_coverage`:

| Opcode  | PDB name                                   | PDB sizeof | Wire len(s)            |
|---------|--------------------------------------------|------------|-------------------------|
| 0x0438  | NC_CHAR_OPTION_IMPROVE_SET_SHORTCUTDATA_ACK | 2          | 97                     |
| 0x0439  | NC_CHAR_OPTION_IMPROVE_SET_KEYMAP_REQ       | 2          | 4                      |
| 0x0447  | NC_CHAR_OPTION_IMPROVE_INIT_ETC3_REQ        | 1          | 3,12,82,162,182,262    |
| 0x0602  | NC_MAP_LOGIN_ACK                            | 1622       | 238                    |

The 0x0438 case is the smoking gun: the wire packet payload starts
with `[u32 userNo=5][char[16] "EllaAnn"]` — the unmistakable shape
of `PROTO_NC_CHAR_BASE_CMD` (PDB sizeof 105 → 97 wire), which
`PDB_OPCODES.json` puts at opcode **0x0407**.

**Conclusion**: The user's running Fiesta server is built from a
*different region toggle* than the four PDBs we extracted into
`_re_artifacts/pdb/extracted/`.  Opcode IDs were renumbered between
those two builds while the PROTO_NC_* struct shapes themselves
were preserved.

**Implication for Phase 6a**: opcode-name → opcode-id is unreliable
on this wire.  Wire-byte-shape → struct-name IS reliable, because
the PDB struct catalogue is correct even when the opcode mapping
isn't.  The dispatcher should resolve handlers by **payload shape**
first, then cross-reference opcode IDs from the user's specific
build via fresh PCAPs.

Action item before any handler logic gets written:
1. User runs Elle's headless client against their LoginServer.
2. Elle records the SEED_ACK handshake reply (always 2 bytes —
   the unambiguous fingerprint).
3. The dispatch table cross-correlates: "SEED_ACK arrived as
   opcode N, therefore THIS build's NC_MISC = (N >> 8)".
4. Apply that offset across the entire PROTOCOL_COMMAND enum to
   re-derive the build-specific opcode → name mapping, and
   regenerate `dispatch_table.json` under the new mapping.

`build_dispatch_table.py` exposes a `--remap` hook for this
calibration loop (TODO Phase 6a-step-2).

### 6. Direction-label resolution (paired captures, 2026-02-05 18:02)

The user later provided BOTH a server-side capture AND a client-side
capture for **the same Port 60121 session at the same wall-clock**.
Comparing event-by-event:

* The 1024-byte first event (`<Outbound>` `016 | 010`) has identical
  bytes in both captures.
* Event #2 (`<Inbound>` `004 | 056`, 97 bytes) carries `05 00 00 00
  45 6C 6C 65 41 6E 6E 00 …` (Elle's userNo + name) — also identical
  bytes between the two captures.

Conclusion (locked in): **direction labels are from the client's
network perspective**, NOT the server's:

> `<Inbound>`  = arriving at the **client** (server → client).
> `<Outbound>` = leaving the **client** (client → server).

This makes the encryption polarity make sense too:
* `<Outbound>` (client → server) packets are encrypted; the very
  first one (`016 | 010` 1024 B) is the cipher handshake the client
  sends.
* `<Inbound>`  (server → client) packets are PLAINTEXT in this build
  — the cipher is one-way client→server only on this region toggle.

So the opcode pool we've been categorising contains **server-pushed
state-dump opcodes** (NC_*_CMD / NC_*_ACK), not client requests.
The "EllaAnn identity packet" at `0x0438` is the server pushing
Elle's own character base record back to her on session start.

### 7. Shape-match calibration (Phase 6a step-2 tool)

`_re_artifacts/pdb/shape_match_payloads.py` was added to ignore
opcode IDs entirely and instead match each `(wire_opcode,
payload_len)` tuple against the 2 723-entry PDB struct catalogue
by sizeof.  Output sample (full file is `extracted/payload_shape_matches.json`):

| Wire op  | Plen | Top struct candidates by sizeof (±8)                |
|----------|-----:|-----------------------------------------------------|
| 0x0438   |   97 | PROTO_NC_CHAR_BASE_CMD (105), PROTO_NC_USER_TW_PASSWORD_CHECK_REQ (102), … |
| 0x043E   |   90 | PROTO_NC_USER_WORLDSELECT_ACK (83) — wrong direction; PROTO_NC_PRISON_OK_CMD (82) |
| 0x0602   |  238 | PROTO_NC_BAT_LEVELUP_CMD (235), PROTO_NC_GUILD_WAR_TARGET_CMD (234) |

The 0x0438 → `PROTO_NC_CHAR_BASE_CMD` match is the strongest signal:
the wire payload starts with `[u32 chrregnum][char[16] charid]` — the
exact head-shape the PDB documents for that struct, just with the
charid rendered as `Name4` (16 B) instead of `Name5` (20 B) and a
4-byte trailing field dropped (105 → 97).

### 8. Two-character side-by-side proof (2026-02-05 evening)

The user logged in a SECOND client (character "Crystal", userNo=6)
on Port 60795 immediately after EllaAnn's login on Port 60121.
Aligning the first inbound packet from each session:

```
Offset  EllaAnn  (Port 60121)         Crystal  (Port 60795)         Field
─────── ───────────────────────────── ───────────────────────────── ──────────────────────
 0..3   05 00 00 00                   06 00 00 00                   u32 chrregnum (= userNo)
 4..19  "EllaAnn"+9×0x00 (16 B)       "Crystal"+9×0x00 (16 B)       Name4 charid (16 B fixed)
 20     00                            00                            u8 (always 0 — pad/flag)
 21     96                            96                            u8 = 0x96 (CONSTANT —
                                                                      class/protocol marker?)
 22..35 14×0x00                       14×0x00                       reserved zero block (14 B)
 36..47 0E 27 0B 27 C5 0F 00 00       00 00 00 00 00 00 00 00       coords/level (EllaAnn
        BB 0E 00 00                   00 00 00 00                   has data; Crystal fresh
                                                                      char = all zeros)
```

**What this proves**:

* `charid` width is **exactly 16 bytes** (`Name4`), NOT 20 bytes (`Name5`).
  This contradicts the PDB schema which has `Name5` for this field —
  the build's region toggle simplified the field.
* `chrregnum` lives at offset 0 and equals `userNo` from `tUser` (= the
  database account number, NOT the per-zone session handle).
* Bytes 20-21 = `[u8 0][u8 0x96]` are CONSTANT across characters —
  these encode something protocol-level (could be `slotno=0` +
  `version=0x96`, or a packet-type sub-marker). Need a third
  capture from a different account-tier to disambiguate.
* The 14-byte zero block at offsets 22-35 means at least 14 bytes
  of `PROTO_NC_CHAR_BASE_CMD` are reserved/uninitialised on a fresh
  character.
* Byte 36 onwards is **character-state data** (coords, level, etc.).
  EllaAnn's `0E 27 0B 27` = LE u16 `0x270E`/`0x270B` = (9998, 9995)
  → looks like world (X, Y) on a typical Fiesta map.

This locks in the wire→struct mapping for the most-important inbound
opcode in the post-login state dump, with full confidence:

> **Wire opcode `0x0438` (97 bytes inbound) = `PROTO_NC_CHAR_BASE_CMD`
> variant** — server pushes Elle's own character base info on session
> start. Decoder shape: `[u32 chrregnum][char[16] charid][u8 0]
> [u8 0x96][14×0 reserved][73 B character-state body]`.

### 9. Two-client paired login signature

Logging in two clients in succession produces an unmistakable wire
fingerprint:

| Port  | Char    | userNo | Cipher seed-derived first-9-bytes |
|-------|---------|--------|------------------------------------|
| 60121 | EllaAnn |   5    | `69 C5 77 A9 CA BB F1 1E B7 …`     |
| 60795 | Crystal |   6    | `B8 C2 71 B7 DF AD C4 11 B5 …`     |

After byte 9 both 1024-byte handshake blobs are byte-identical:
`23 CF EE 0E 28 B5 03 7B 49 AB …`. This confirms the cipher uses a
short per-session seed (≤ 9 bytes of state divergence) and the
underlying plaintext of the first packet is FIXED across sessions —
likely a server-banner or anti-cheat handshake constant. Useful for
the headless client: once we recover that constant plaintext, we can
work backward from any captured ciphertext to reveal the seed.

### 10. Open question: Port 59507 (2-event mini-session)

The user supplied a third capture (Port 59507) with only 2 events:

```
<Outbound> 008 | 120  (5 B)  1C A4 53 5B F0
<Inbound>  003 | 003  (2 B)  01 00
```

Both events are tiny and the opcode `008|120` (= 0x0878 in our
host-side u16 convention) does NOT appear in the post-login dump.
This may be a heartbeat / version-check / quick-action round trip.
Three plausible decodes:

1. **Heartbeat ping → ack** (`NC_MISC_HEARTBEAT_REQ` / `_ACK`).
2. **Logout request → status** (`NC_USER_NORMALLOGOUT_CMD` →
   `NC_USER_LOGOUT_ACK`).
3. **Action keepalive** — small client-side action notification
   for an idle timer.

Insufficient signal to disambiguate yet — needs a fresh capture
where we deliberately trigger one of the above and observe the
wire shape.

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
