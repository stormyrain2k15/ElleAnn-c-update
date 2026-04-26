# Fiesta Online — Opcodes & Packet Layouts (extracted from server PDBs)

**Source binaries** (uploaded by user, mixed 2012 + 2016 builds):

| PDB                  | Size  | TPI ver | Parser              | Symbols | Type records |
|----------------------|-------|---------|---------------------|--------:|-------------:|
| `Login.pdb`          | 3.2 M | V80     | llvm-pdbutil-15     |   4 013 |       45 352 |
| `WorldManager.pdb`   | 1.4 M | V80     | llvm-pdbutil-15     |   8 885 |              |
| `Account.pdb`        | 4.3 M | V80     | llvm-pdbutil-15     |   7 119 |              |
| `AccountLog.pdb`     | 4.3 M | V80     | llvm-pdbutil-15     |   6 655 |              |
| `5ZoneServer2.pdb`   |  14 M | V70     | custom (`cv_pdb_dump.py`) | — |       24 216 |
| `Fiesta.pdb` (client) |  43 M | V80    | llvm-pdbutil-15    |  11 200 |      184 308 |

> "Fiesta was always one source — region branches just toggled features at compile-time"
> _-- user._ The 2016 PDBs therefore overlap heavily with the 2007–2014 source we already
> reverse-engineered from `.udd` files; offsets and opcode IDs are stable across versions.

---

## 1. Opcode encoding (CONFIRMED)

The 16-bit `NETCMDID` is constructed as:

```
NETCMDID = (cmdtype << 8) | cmdsubid
```

Stored on the wire **little-endian**, i.e. byte stream is `[cmdsubid][cmdtype]`.

`cmdtype` values come from the master enum **`PROTOCOL_COMMAND`** in `Login.pdb`
(field-list `0x28C1`).  Selected entries:

| Group          | Value | Group          | Value | Group              | Value |
|----------------|------:|----------------|------:|--------------------|------:|
| `NC_NULL`      |     0 | `NC_MAP`       |     6 | `NC_TRADE`         |    19 |
| `NC_LOG`       |     1 | `NC_BRIEFINFO` |     7 | `NC_GUILD`         |    29 |
| `NC_MISC`      |     2 | `NC_ACT`       |     8 | `NC_CHARGED`       |    36 |
| `NC_USER`      |     3 | `NC_BAT`       |     9 | `NC_USER_CONNECTION` | 45 |
| `NC_CHAR`      |     4 | `NC_OPTOOL`    |    10 | `NC_AUCTION`       |    46 |
| `NC_AVATAR`    |     5 | `NC_ITEM`      |    12 | `NC_PET`           |    53 |

Full list dumped to `pdb/extracted/PDB_OPCODES.json` (**2 687 entries** after
Zone-PDB merge — zero conflicts vs. Login/WM PDBs).

---

## 2. Login flow — verified opcode IDs

| Direction               | Opcode                                | Hex      |
|-------------------------|---------------------------------------|----------|
| **MISC handshake**      |                                       |          |
| Client → Login          | `NC_MISC_SEED_REQ`                    | `0x0206` |
| Login → Client          | `NC_MISC_SEED_ACK`                    | `0x0207` |
| Heartbeat (both ways)   | `NC_MISC_HEARTBEAT_REQ`/`_ACK`        | `0x0204`/`0x0205` |
| **Version check**       |                                       |          |
| Client → Login          | `NC_USER_CLIENT_VERSION_CHECK_REQ`    | `0x0365` |
| Login → Client (good)   | `NC_USER_CLIENT_RIGHTVERSION_CHECK_ACK` | `0x0367` |
| Login → Client (bad)    | `NC_USER_CLIENT_WRONGVERSION_CHECK_ACK` | `0x0366` |
| **Hop 1: Login (9010)** |                                       |          |
| Client → Login          | `NC_USER_LOGIN_REQ`                   | `0x0306` |
| Login → Client (ok)     | `NC_USER_LOGIN_ACK` (world list)      | `0x030A` |
| Login → Client (fail)   | `NC_USER_LOGINFAIL_ACK`               | `0x0309` |
| Client → Login          | `NC_USER_WORLDSELECT_REQ`             | `0x030B` |
| Login → Client          | `NC_USER_WORLDSELECT_ACK` (WM addr+token) | `0x030C` |
| **Hop 2: WorldManager (9110)** |                                |          |
| Client → WM             | `NC_USER_WILLLOGIN_REQ`               | `0x030D` |
| WM → Client             | `NC_USER_WILLLOGIN_ACK`               | `0x030E` |
| **Hop 3: Zone (9120)**  |                                       |          |
| Client → Zone           | `NC_USER_LOGINWORLD_REQ`              | `0x030F` |
| Client → Zone           | `NC_MAP_LOGIN_REQ`                    | `0x0601` |
| Zone → Client           | `NC_MAP_LOGINCOMPLETE_CMD`            | `0x0603` |
| **Logout**              |                                       |          |
| Client → Login/Zone     | `NC_USER_NORMALLOGOUT_CMD`            | `0x0318` |

All values cross-validated against existing constants in
`Elle.Service.Fiesta/FiestaPacket.h` (e.g. `NC_MISC_SEED_REQ = 0x0206`,
`NC_MAP_LOGIN_REQ = 0x0601`, `NC_USER_NORMALLOGOUT_CMD = 0x0318`).

---

## 3. Verified packet layouts (from PDB type stream)

### `PROTO_NC_MISC_SEED_ACK`  (Login → Client) — **2 bytes**

| Offset | Field   | Type                   |
|-------:|---------|------------------------|
|      0 | `seed`  | `unsigned short` (u16) |

This single u16 is the **XOR cipher seed** consumed by `FiestaCipher`.

### `PROTO_NC_USER_CLIENT_VERSION_CHECK_REQ`  (Client → Login) — **64 bytes**

| Offset | Field         | Type          |
|-------:|---------------|---------------|
|      0 | `sVersionKey` | `wchar_t[32]` (64 bytes) |

### `PROTO_NC_USER_LOGIN_REQ`  (Client → Login) — **272 bytes**

| Offset | Field      | Type           |
|-------:|------------|----------------|
|      0 | `user`     | `char[256]` (Name256Byte) |
|    256 | `password` | `char[16]`  (Name4)       |

### `PROTO_NC_USER_WORLDSELECT_REQ`  (Client → Login) — **1 byte**

A single u8 selecting which world from `LOGIN_ACK::WorldInfo[]` the client wants.

### `PROTO_NC_USER_WORLDSELECT_ACK`  (Login → Client) — **83 bytes**

| Offset | Field          | Type                       | Note                       |
|-------:|----------------|----------------------------|----------------------------|
|      0 | `worldstatus`  | `u8`                       | 0=down, 1=normal, 2=busy…  |
|      1 | `ip`           | `char[16]` (Name4)         | dotted-quad ASCIZ for WM   |
|     17 | `port`         | `u16`                      | WM listen port (0x9110)    |
|     19 | `validate_new` | `u16[32]` (64 bytes)       | **WM authentication blob** |

This 64-byte blob **must be echoed verbatim** to WM in
`PROTO_NC_USER_WILLLOGIN_REQ.validate_new` — it is the per-session token.

### `PROTO_NC_USER_WILLLOGIN_REQ`  (Client → WM, hop 2) — **347 bytes**

| Offset | Field             | Type                  | Note                                  |
|-------:|-------------------|-----------------------|---------------------------------------|
|      0 | `netpacketheader` | 7-byte WM frame head  | (PDB only retains 2 named bytes)      |
|      7 | `userid`          | `char[256]` (Name256Byte) | Same string used in LOGIN_REQ.user |
|    263 | `validate_new`    | `u16[32]`             | Token from WORLDSELECT_ACK            |
|    327 | `spawnapps`       | `char[20]` (Name5)    | OS / launcher fingerprint             |

### `PROTO_NC_USER_WILLLOGIN_ACK`  (WM → Client) — **348 bytes**

Wraps `WILLLOGIN_REQ` (347 B) with a 1-byte status prefix; on success WM
returns the Zone server `(ip, port)` and a fresh `validate_new` for hop 3.

### `PROTO_NC_USER_LOGIN_ACK`  (Login → Client) — variable

| Offset | Field        | Type                 | Note                       |
|-------:|--------------|----------------------|----------------------------|
|      0 | `numofworld` | `u8`                 | count of WorldInfo entries |
|      1 | `worldinfo`  | `WorldInfo[numofworld]` | array of 20-byte records |

`WorldInfo` (20 bytes per record): `worldno:u8 + worldname:char[?] +
status:u8 + …` (see `PROTO_NC_USER_LOGIN_ACK::WorldInfo`,
field-list `0x25B7`).

---

## 4. Cross-server frame header (Login ↔ AccountDB / WM ↔ ZoneDB)

`NETPACKETHEADER` (server-to-server only, **not** client-facing):

| Offset | Field          | Type   | Notes                                   |
|-------:|----------------|--------|-----------------------------------------|
|      0 | `clienthandle` | `u16`  | logical session id within the dest svc  |

The PDB reports `sizeof = 2`; in WILLLOGIN_REQ a 7-byte header is observed,
i.e. there is an unnamed 5-byte tail (probably `[seq:u32][cmdtype:u8]` or
`[ts:u32][error:u8]`). This single gap is the only remaining structural
unknown that PCAPs would resolve.

---

## 5. State machine fingerprints

`CLoginClientSession::SessionMode` enum (from PDB `0x277B`):

```
L_CLIENT_SESSION_MODE_BEFORE_NC_USER_LOGIN_REQ_FOR_EACH_VER  = 1
L_CLIENT_SESSION_MODE_AFTER_NC_USER_LOGIN_REQ_FOR_EACH_VER   = 2
```

Login session class member layout (Login.pdb `CLoginClientSession`):

| Offset | Field                | Use                              |
|-------:|----------------------|----------------------------------|
|    268 | `m_PacketEncryptor`  | XOR cipher state machine         |
|    272 | `m_UserNo`           | u32 — `tUser.nUserNo`            |
|    276 | `m_UserID`           | Name256Byte — login string       |
|    532 | `m_UserSwpawnApps`   | Name5 — fingerprint              |
|    552 | `m_UserVaidation`    | u16 — current `validate_new`     |
|    554 | `m_strSerialKey`     | (region-specific) — serial blob  |

The presence of `Send_NC_MISC_SEED_ACK` as a method on this class confirms
the cipher seed is sent **once, immediately after TCP accept** and
**before any application packets** — i.e. the server-side cipher is keyed
by the seed it generates and sends, and the client must adopt the same
seed on reception. Per-packet running-offset model (not per-packet reset).

---

## 6. New opcodes added by these PDBs (not previously in `FiestaPacket.h`)

The 470 dispatchers found in the PDBs cover ~360 opcodes that our existing
header (sourced from Zone `.udd` files) does not have. Notably:

* **CharDB family** (162 dispatchers, `NC_CHAR_*_CMD` / `NC_AVATAR_*_ACK`)
  — character creation, save/load, guild, friend, item DB.
* **Zone announce** (`NC_BAT_WORLD_MOB_KILL_ANNOUNCE_CMD`, etc.).
* **OPTool** (38 dispatchers) — admin/audit channel.

These are surfaced in `pdb/extracted/PDB_OPCODES.json` and individual
struct layouts in `pdb/extracted/{Login,WorldManager,Account,AccountLog}_protos.md`.

---

## 7. Zone-side packets (5ZoneServer2.pdb, parsed via custom V70 reader)

The Zone PDB uses TPI v7.0 which llvm-pdbutil rejects. A custom CodeView
reader (`pdb/cv_pdb_dump.py`, ~280 LOC, NO external deps) walks the TPI
stream sequentially and emits the same JSON shape as the llvm-driven
parser. Result: **24 216 type records, 1 123 PROTO_NC_* structs,
2 432 enumerators**, all cross-validated with WM/Login PDB enums (zero
conflicts).

### Movement / world primitives (verified bit-layouts)

```c
struct SHINE_XY_TYPE       { uint32_t x; uint32_t y; };               // 8B
struct SHINE_COORD_TYPE    { SHINE_XY_TYPE xy; uint8_t  dir; };       // 9B
struct NETPACKETZONEHEADER { uint16_t clienthandle;
                             uint32_t charregistnumber; };            // 6B
struct NETCOMMAND          { uint16_t protocol; };                    // 2B
```

### Verified Zone-side opcodes

| Opcode                         | Hex      | Layout                                       |
|--------------------------------|----------|----------------------------------------------|
| `NC_ACT_WALK_REQ`              | `0x0803` | `from:XY` + `to:XY` (16 B)                   |
| `NC_ACT_RUN_REQ`               | `0x0805` | _(not parsed in this build)_                 |
| `NC_ACT_STOP_REQ`              | `0x0812` | _(not parsed in this build)_                 |
| `NC_ACT_MOVEWALK_CMD`          | `0x0817` | `from:XY + to:XY + moveattr:u8` (17 B)       |
| `NC_ACT_MOVERUN_CMD`           | `0x0819` | _(not parsed in this build)_                 |
| `NC_ACT_NPCCLICK_CMD`          | `0x080A` | `npchandle:u16` (2 B)                        |
| `NC_ACT_CHAT_REQ`              | `0x0801` | **`itemLinkDataCount:u8 + len:u8 + content[len]`** ⭐ |
| `NC_ACT_SHOUT_CMD`             | `0x081E` | same shape as CHAT_REQ                       |
| `NC_ACT_WHISPER_REQ`           | `0x080C` | `?:u8 + receiver:Name5(21B) + content[len]`  |
| `NC_ACT_JUMP_CMD`              | `0x0824` | (Zone-only)                                  |
| `NC_BRIEFINFO_LOGINCHARACTER_CMD` | `0x0706` | **`handle:u16 + charid:char[16]` + 203B body** ⭐ |
| `NC_BRIEFINFO_BRIEFINFODELETE_CMD` | `0x070E` | **`hnd:u16` (2 B)** ⭐                  |
| `NC_BRIEFINFO_INFORM_CMD`      | `0x0701` | `nMyHnd:u16 + ReceiveNetCommand:u16 + hnd:u16` (6 B) |
| `NC_BRIEFINFO_REGENMOB_CMD`    | `0x0708` | full mob spawn (124 B incl. animation, flags) |
| `NC_CHAR_BASE_CMD`             | `0x0407` | **`chrregnum:u32 + charid:char[16]` + 77B body** ⭐ |
| `NC_CHAR_LOGIN_ACK`            | `0x0403` | `zoneip:Name4 + zoneport:u16` (18 B)         |
| `NC_MAP_LOGIN_REQ`             | `0x0601` | `chardata:CHARDATA_REQ + checksum:Name8[120]` (978 B) |
| `NC_MAP_LOGINCOMPLETE_CMD`     | `0x0603` | `checksumDoorBlock:Name8[4]` (32 B)          |

⭐ = recovered/refined from the **client `Fiesta.pdb`** in the second
extraction round (TPI v8.0, 184 308 type records). The CHAT layout
was previously assumed to be `[u8 len + content]` based on the
zone-server PDB's forward-decl; the client PDB exposes the full
fieldlist `0x159AB` showing the actual `itemLinkDataCount + len +
content` shape that ShineEngine uses to embed clickable item-tooltip
links inside chat lines.

> "This build" = the CN2012 5ZoneServer2.exe; some opcodes have only
> declarations or are conditionally compiled out of THIS region toggle
> set, but the IDs are still in the master enum, so we still hold them.

### Zone-side discovery summary

* Movement uses **u32 fixed-point world coordinates** (not floats) plus a
  u8 facing direction. `SHINE_XY_TYPE.x/y` are absolute, server-validated.
* `NC_ACT_*_REQ` are **client → server**, `NC_ACT_*_CMD` are
  **server → broadcast**. The CMD variants carry a sender handle prefix.
* `NC_MAP_LOGIN_REQ` carries a 960-byte checksum array (`Name8[120]` =
  120 × 8-byte digests) — these are MD5/SHA fragments of the client's
  data files, used by anti-cheat on the **server** side. We will fill
  them with zero blocks, which is what the CN2012 build accepts (no
  matching server-side validation on this region toggle set).
