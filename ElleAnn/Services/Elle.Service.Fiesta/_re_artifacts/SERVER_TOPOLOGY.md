# ShineEngine Server Topology — Mischief 6.11.2015

The complete server stack the user has in their possession (eight processes
across SQL, HTTP-auth, and TCP game services). Confirmed via
`JHR_ServerInfo.txt` (the canonical server-config script) plus string
analysis of every binary.

```
┌──────────────────────────────────────────────────────────────┐
│                     MSSQL Server (sa user, blank PWD)        │
│   Account  · AccountLog  · World00_Character  · World00_GameLog │
│   Statistics · OperatorTool                                  │
└──────────────┬─────────────────────┬─────────────────────────┘
               │ ODBC                │
   ┌───────────┴────────────┐   ┌────┴──────────────────────────┐
   │  ID 0  Account_Release │   │  ID 2  Character_Release      │
   │  port  9000            │   │  port  9100                   │
   │  ID 1  AccountLog      │   │  ID 3  GameLog_Release        │
   │  port  9005            │   │  port  9101                   │
   └───────────┬────────────┘   └────┬──────────────────────────┘
               │ S2S                 │ S2S
   ┌───────────┴────────────┐        │
   │  ID 4  3LoginServer2   │        │
   │  9010 client-facing    │ ◄── client TCP connect #1 (auth)
   │  9015 from WM          │        │
   │  9016 from OPTOOL      │        │
   └───────────┬────────────┘        │
               │ S2S                 │ S2S
               ▼                     ▼
   ┌──────────────────────────────────────────────┐
   │  ID 5  4WorldManagerServer2                  │
   │  9110 client-facing  ◄── client TCP connect #2 (char list, zone select)
   │  9115 from Zone                              │
   │  9116 from OPTOOL                            │
   └───────────┬──────────────────────────────────┘
               │ S2S NC_MISC_S2SCONNECTION_REQ
               ▼
   ┌──────────────────────────────────────────────┐
   │  ID 6  5ZoneServer2 — multiple instances     │
   │  9120 PG_Zone_00  ◄── client TCP connect #3 (gameplay)
   │  9122 PG_Zone_01                             │
   │  9124 PG_Zone_02                             │
   │  9126 PG_Zone_03                             │
   │  9128 PG_Zone_04                             │
   │  9130 PG_Zone_05                             │
   └──────────────────────────────────────────────┘
```

## Client → server flow (what Elle.Service.Fiesta speaks)

| Step | Endpoint               | Direction | Notes |
|-----:|------------------------|-----------|-------|
| 1 | TCP 127.0.0.1:9010 (`PG_Login`)  | client → Login | initial cleartext connect |
| 2 | `0x0206 NC_MISC_SEED_REQ`         | Login → client | server pushes 32-byte seed; cipher on |
| 3 | `0x0601 NC_MAP_LOGIN_REQ`          | client → Login | `Str(user); Str(pass); U32(protocolVersion)` |
| 4 | login ack + WM hand-off            | Login → client | server sends WM IP + port (typically the same 127.0.0.1:9110) |
| 5 | TCP 127.0.0.1:9110 (`PG_WorldManager`) | client → WM | new socket, fresh seed handshake |
| 6 | char-list, char-select, zone-select | both | NC_CHAR_*, NC_MAP_* round-trip |
| 7 | zone hand-off                       | WM → client | WM responds with Zone IP + port (e.g. 127.0.0.1:9120) |
| 8 | TCP 127.0.0.1:9120 (`PG_Zone_00`)  | client → Zone | new socket, fresh seed handshake |
| 9 | `0x0603 NC_MAP_LOGINCOMPLETE_CMD`   | client → Zone | confirms ready; transitions to IN_GAME |
| 10 | gameplay                          | both | NC_ACT_*, NC_BAT_*, NC_ITEM_*, NC_TRADE_* |

## ODBC connections (server-side)

| DSN         | DB used in MSSQL          | Used by                        |
|-------------|---------------------------|--------------------------------|
| `Account`   | `Account`                 | LoginServer authenticates here |
| `AccountLog`| `AccountLog`              | LoginServer/AccountLog write here |
| `Statistics`| `StatisticsData`          | OPTool                         |
| `OPTool`    | `OperatorTool`            | OPTool                         |
| `Character` | `World00_Character`       | CharacterDB server             |
| `GameLog`   | `World00_GameLog`         | GameLog server                 |

All DSNs use `UID=sa; PWD=` (empty password — ShineEngine private-server convention; the user's local instance is exposed only on 127.0.0.1).

## Recovered binaries inventory

| File | Size | Purpose |
|------|------|---------|
| `3LoginServer2.exe` | 168 KB | TCP login validator (client port 9010, server-aggregator 9015) |
| `4WorldManagerServer2.bak` | 540 KB | Client gateway after login (9110); routes to zones |
| `5ZoneServer2-Port80-NoService.exe` | 2.7 MB | Zone gameplay; matches the .pdb + .udd we have |
| `Account_Release.exe` | 200 KB | MSSQL Account DB bridge (9000) |
| `Character_Release.bak` / `.exe` | 553 KB | MSSQL Character DB bridge (9100) |
| `GameLog_Release.bak` / `.exe` | 200 KB | MSSQL GameLog DB bridge (9101) |
| `AccountLog_Release.exe` | 184 KB | MSSQL AccountLog DB bridge (9005) |
| `Fiesta.dll` | client-side | Patched DLL — bypasses the original client's integrity check |
| `5ZoneServer2.pdb` | 13 MB | Server symbols → handler RVAs |
| `5ZoneServer2.udd` | 738 KB | OllyDbg DB; 13 882 (RVA, name) annotations |
| `JHR_ServerInfo.txt` | server config | THE canonical port + ODBC map |
| `Zone.ini` / `0oneServerInfo.txt` | server config | per-zone tuning |

## Test accounts (PHP UserAuthentication.php — currently a 3-user stub)

| User    | Plaintext password | MD5(salt+pass)                   |
|---------|---------------------|----------------------------------|
| Klurr   | `omglolpass`        | `09944cb770631d1bcfa738cb6ea2f0a1` |
| Miff    | `omglolpass`        | `09944cb770631d1bcfa738cb6ea2f0a1` |
| Muhment | `omglolpass`        | `09944cb770631d1bcfa738cb6ea2f0a1` |

Salt: `dlrtkdlxm!`. The PHP is a hardcoded stub — the *real* auth happens
TCP-side at port 9010 against the `Account` MSSQL DB.

## Important caveats

- **No HTTP step required for Elle.Service.Fiesta.** The PHP we got is a
  3-user stub for some legacy web feature. The real login flow is TCP only:
  client speaks `NC_MAP_LOGIN_REQ` directly to `PG_Login` on port 9010.
- **The "NO-APEX VERSION" log line is internal.** It's the WM failing to
  load guild-tournament data that requires an APEX server build, which then
  silently crashes the WM. **It is NOT a client-side rejection** — never
  was. Our client doesn't need any APEX-related behaviour.
- **Login → WM → Zone is a 3-hop dance.** Each hop opens a new TCP socket
  with its own SEED handshake. The current `FiestaClient::Connect` only
  wires hop #1 (Login). Hop #2 (WM) and hop #3 (Zone) need to be added —
  see TODO in `FiestaClient.cpp`.
- **MSSQL `sa` password is blank** in this build. If you ever connect Elle
  directly to the same `Account` database (see ROADMAP below), make sure
  the binding stays on `127.0.0.1` and the firewall keeps 1433 closed
  externally.

## Future enhancement: unify Elle's user table with the game's `Account` DB

Per user request: "change the Elle user tables to use the same user db as
the game". This means replacing Elle.Service.HTTP's local `ElleCore.dbo.PairedDevices`
auth with a query against the game's `Account` database, so a single Fiesta
account credential pairs an Android device, a web session, and Elle's
in-game presence.

Required to land this:
- Actual MSSQL `.bak` of the `Account` database (the `.bak` files we
  received earlier turned out to be EXEs renamed; we need a real
  `BACKUP DATABASE Account TO DISK = ...` artifact).
- The `Account` table column list (likely: `UserID`, `Password` (varchar
  containing the same MD5(salt+pass) the PHP uses), `cpid`, `birthdate`,
  `gender`, `countrycode`, `lastlogin`).
- A decision on Elle-side: authenticate by MD5(salt+plaintext) match (one
  ODBC query per pair attempt), or by issuing JWTs that wrap an opaque
  `UserID`. Recommended: latter, with the JWT subject = `UserID`.
