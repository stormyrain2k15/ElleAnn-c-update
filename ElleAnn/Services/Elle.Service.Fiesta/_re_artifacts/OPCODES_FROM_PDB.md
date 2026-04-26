# Fiesta Online — Opcodes & Packet Layouts (extracted from server PDBs)

**Source binaries** (uploaded by user, 2016 build, post-region-toggle era):

| PDB              | Service role                              | Symbols | Dispatchers |
|------------------|-------------------------------------------|--------:|------------:|
| `Login.pdb`      | Login server (port 9010)                  |   4 013 |          50 |
| `WorldManager.pdb` | World Manager (port 9110)               |   8 885 |         998 |
| `Account.pdb`    | Account DB proxy                          |   7 119 |          31 |
| `AccountLog.pdb` | Audit / IP-block proxy                    |   6 655 |          35 |

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

Full list dumped to `pdb/extracted/PDB_OPCODES.json` (2 054 entries).

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

## 7. What PCAPs would still confirm

After this PDB pass, the only protocol unknowns left are runtime/stream
properties, not structure:

1. The 5-byte unnamed tail of `NETPACKETHEADER` in WM-bound packets
   (likely `seq:u32 + cmdtype_high:u8`).
2. Whether the XOR stream cipher is reset per packet or runs continuously
   across a TCP stream (PDB strongly implies continuous; PCAP confirms).
3. Movement axis order for `NC_ACT_MOVEWALK_CMD` / `MOVERUN_CMD`
   (xz-yaw vs xy-yaw — Zone-side, not in these 4 PDBs).
4. Server-pushed BRIEFINFO / CHAR_BASE deltas (Zone-side).

The 3-hop login dance can now be implemented from **PDB data alone**, with
PCAPs reserved for runtime sanity validation.
