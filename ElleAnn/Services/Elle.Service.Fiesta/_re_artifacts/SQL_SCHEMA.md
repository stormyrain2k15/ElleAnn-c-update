# Mischief Account Database Schema (extracted from `Account.bak`)

Recovered Feb 2026 by parsing the SQL Server backup file (`SQL_Database.7z` →
`Account.bak`, MSSQL MTF format). Stored procedures were leaked verbatim by
the backup as text; the table schemas are inferred from the procedure
definitions.

## TUSER — primary account table

Two parallel naming conventions exist in the same DB (legacy + modern web
panel). Both refer to the **same physical row**.

| Column        | Type            | Notes                                |
|---------------|------------------|--------------------------------------|
| `NUSERNO`     | `INT IDENTITY`   | PK; auto-increment                   |
| `SUSERID`     | `NVARCHAR(30)`   | Login name (e.g. `Klurr`)            |
| `SUSERPW`     | `NVARCHAR(20)`   | **Password — stored AS-IS (not hashed)** |
| `SUSERNAME`   | `NVARCHAR(10)`   | Display name                         |
| `SUSERIP`     | `VARCHAR(?)`     | Last-known IP                        |
| `JF`          | `INT`            | Points / karma                       |
| `QX`          | `CHAR(1)`        | Permission level (`A`=user, `B`=premium, `C`=admin) |
| `TIME`        | `DATETIME`       | Premium-expiry / last-login          |
| `QUESTION`    | `VARCHAR(50)`    | Security question                    |
| `ANSWER`      | `VARCHAR(50)`    | Security answer                      |
| `COMMEND`     | `VARCHAR(30)`    | Referrer code                        |
| `BISDELETE`   | `BIT`            | Soft-delete flag (filter `=0`)       |
| `BISBLOCK`    | `BIT`            | Banned flag                          |
| `NAUTHID`     | `INT`            | Auth class                           |

## TUSEROPTION — extended profile

| Column      | Type            |
|-------------|------------------|
| `NUSERNO`   | `INT` FK to TUSER|
| `SEMAIL`    | `NVARCHAR(90)`   |
| `SJUMINNO`  | `VARCHAR(?)`     |
| `SPHONENO`  | `VARCHAR(?)`     |
| `NZIPNO`    | `INT`            |
| `SADDRESS`  | `VARCHAR(?)`     |
| `BISMAIL`   | `BIT`            |

## Authentication semantics (CRITICAL)

The `usp_User_loginWeb` and `TUSER_LOG` stored procedures both compare
`sUserPW` directly against the supplied parameter — the password is stored
**plaintext** in the DB. The PHP `UserAuthentication.php` we received does
MD5(salt+pass) on the user input but only checks against three hardcoded
strings — it is **NOT** the production auth path. Production auth is the
TCP `NC_MAP_LOGIN_REQ` against port 9010, which the LoginServer validates
via `usp_User_loginWeb` (or `TUSER_LOG`) directly.

```sql
-- Production login query (reverse-engineered):
SELECT nUserNo, sUserID, sUserPW, sUserName, bIsBlock, nAuthID
  FROM tUser WITH(NOLOCK)
 WHERE sUserID = @userID
   AND sUserPW = @userPW
   AND bIsDelete = 0
```

## Other tables in the Account DB

| Table                 | Purpose                                           |
|-----------------------|---------------------------------------------------|
| `TCHARACTER`          | (lives in World00_Character) — character roster   |
| `TITEM`               | Cash-shop catalog                                 |
| `TITEMCATEGORY`       | Cash-shop categories                              |
| `TCHARGEITEM`         | Cash-shop ownership                               |
| `TORDER`              | Cash-shop transaction log                         |
| `WEB_NEWS`            | News posts                                        |
| `WEB_MESSAGE`         | In-DB user-to-user messages                       |
| `WEB_DOWNLOAD`        | Downloads list                                    |
| `WEB_QUESTION`        | FAQ                                               |
| `WEB_ADMIN`           | Web-panel admin accounts                          |
| `WEB_DDB`             | Daily-deal DB                                     |
| `WEB_BUYMONEYCLASS`   | Cash-shop pricing tiers                           |

## Other databases in the backup

| File                        | Purpose                                     |
|-----------------------------|---------------------------------------------|
| `Account.bak` (11 MB)       | THIS — accounts, cash shop, web panel       |
| `AccountLog.bak` (2 MB)     | Login history, security log                 |
| `OperatorTool.bak` (1.5 MB) | GM-tool operations log                      |
| `StatisticsData.bak` (2 MB) | Aggregate gameplay stats                    |
| `World00_Character.bak` (6 MB) | Per-world char data: TCHARACTER, inventory, skills, quests, guilds |
| `World00_GameLog.bak` (2 MB) | World-specific gameplay log                 |

## Implications for Elle.Service.HTTP unification

To honour the user's earlier ask ("change the Elle user tables to use the
same user db as the game"):

1. **Add an ODBC-driven auth path** to `Elle.Service.HTTP::POST /api/auth/pair`.
   Connect to `DSN=Account; UID=sa; PWD=` and run:

   ```sql
   SELECT nUserNo, sUserID, sUserName, bIsBlock
     FROM tUser WITH(NOLOCK)
    WHERE sUserID = ? AND sUserPW = ? AND bIsDelete = 0 AND bIsBlock = 0
   ```

2. **Issue Elle JWT** with `sub=NUSERNO` (the int identity) so the JWT
   subject is stable even if SUSERID changes. Embed `display=sUserName`
   for friendly UI.

3. **Drop `ElleCore.dbo.PairedDevices`** in favour of a thin
   `ElleCore.dbo.UserPairing` table that joins on `NUSERNO`:

   ```sql
   CREATE TABLE ElleCore.dbo.UserPairing (
     nUserNo     INT NOT NULL,           -- FK → Account.dbo.tUser.nUserNo
     deviceId    VARCHAR(64) NOT NULL,
     pairCode    VARCHAR(8)  NOT NULL,   -- 8-char QR code
     pairedAt    DATETIME    NOT NULL,
     lastSeen    DATETIME    NULL,
     PRIMARY KEY (nUserNo, deviceId)
   );
   ```

4. **Cross-reference with Fiesta**. When `SVC_FIESTA` connects to the game
   using the same `(SUSERID, SUSERPW)`, Elle's reasoning loop sees the
   same `NUSERNO` in both contexts — Elle's "I think this is the same
   person" check becomes trivial.

**Caveat — security**: The game stores plaintext passwords. If Elle is
going to forward those, they must NEVER hit any persistent log line, and
the ODBC connection must stay on `127.0.0.1` (or be tunnelled through
SSH/IPSec). The game already sends them over a XOR-cipher TCP socket with
no real crypto, so the trust model is "the box is the perimeter".
