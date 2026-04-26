# Fiesta CN2012 Server Distribution — Local Inventory

Cloned from `https://github.com/FiestaHeroes/CN2012` (April 2026, depth=1).
Stored at `/tmp/CN2012/`. Selected artifacts copied to this folder.

## Server topology (verified from `9Data/ServerInfo/ServerInfo.txt`)

| Service               | Type id | Public port | Notes                                |
|-----------------------|--------:|------------:|--------------------------------------|
| Login                 | 4       |        9010 | Hop 1                                |
| WorldManager (W00)    | 5       |        9013 | Hop 2                                |
| Zone W00_Z00..Z03     | 6       | 9016/9019/9022/9025 | Hop 3 (4 zones in world 0)   |
| AccountDB             | 0       |        9028 | Login back-end                       |
| AccountLogDB          | 1       |        9029 | Audit / IP-block back-end            |
| CharDB W00_CDB        | 2       |        9030 | Char persistence (used by Zones)     |
| GameLogDB W00_GLDB    | 3       |        9031 | Combat/economy log back-end          |

All public ports also expose two LOCALHOST mirrors (`PUBLIC_IP+1/+2`) for
internal mesh traffic.

## ODBC connection strings (test SQL credentials, captured for reference only)

```
SQL Server: SQLEXPRESS, UID=sa, PWD=V63WsdafLJT9NDAn
Databases : Account, AccountLog, World00_Character, World00_GameLog,
            StatisticsData, OperatorTool
```

> ⚠️ These are *the public CN2012 dev defaults* — do **not** ship them.
> Elle's `ElleSQLConn` already takes credentials from `.env` only.

## PDBs available

| PDB                  | Size  | Parser status                              |
|----------------------|-------|--------------------------------------------|
| `Login.pdb`          | 3.2 M | ✅ llvm-pdbutil-15 (full)                  |
| `WorldManager.pdb`   | 1.4 M | ✅ llvm-pdbutil-15 (full)                  |
| `Account.pdb`        | 4.3 M | ✅ llvm-pdbutil-15 (full)                  |
| `AccountLog.pdb`     | 4.3 M | ✅ llvm-pdbutil-15 (full)                  |
| `Zone00.pdb`         |  14 M | ⚠️ TPI-stream version unsupported by llvm-pdbutil — fell back to `strings(1)` over `Zone.exe` (518 opcodes recovered) |

The Zone PDB uses a newer TPI version that crashes both llvm-pdbutil-15
and llvm-pdbutil-19. Cross-server PDBs (WM/Login) already reference Zone
opcodes since WM forwards them, so coverage is preserved:

* PDB-resolved opcodes: **2 054**
* Zone.exe string opcodes: **776** (covers movement/combat/auction)
* Intersection (cross-validated): **462** ✅ no conflicts found

## Database backups

`Server/Databases/` contains seven `.bak` files, all are MS-Tape format
(see `_re_artifacts/SQL_SCHEMA.md`). The login-relevant `Account.bak` is
the same one already analyzed in this project.

## Anti-cheat / launcher

No launcher binary in this drop, so no Pangya-style or Xtrap challenge
is required for our headless client. The CN2012 server **does not check
client integrity** beyond opcode validity and the `validate_new` token.
