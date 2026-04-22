# Elle-Ann Deployment — Windows SCM Installer

## Contents
- `elle_service_manifest.json` — ordered list of all 16 services with dependencies
- `Install-ElleServices.ps1`   — registers + starts every service
- `Uninstall-ElleServices.ps1` — stops + deletes them (reverse order)
- `Install.bat`   — double-click wrapper (auto-elevates)
- `Uninstall.bat` — same for removal

## Prerequisites
1. Visual Studio build produced `.exe` files for all 16 services.
   By default the manifest expects them in `..\x64\Release\` relative to this folder.
   If your build output is elsewhere, edit `binary_root` in the JSON OR pass
   `-BinaryRoot "<abs path>"` to the PowerShell script.
2. SQL Server is reachable at the connection string in
   `elle_master_config.json` AND the delta in `SQL/ElleAnn_MemoryDelta.sql`
   has been applied.
3. ASM DLLs (Elle.ASM.Hardware.dll etc.) sit next to the .exe files OR on the
   system PATH — the Action service dynamically loads them via LoadLibrary.

## Install
Double-click `Install.bat` (or right-click → Run as Administrator).

Manual PowerShell run if you prefer:
```powershell
cd .\Deploy
.\Install-ElleServices.ps1                           # register + start
.\Install-ElleServices.ps1 -NoStart                  # register only
.\Install-ElleServices.ps1 -BinaryRoot "C:\Ann\bin"  # custom bin path
.\Install-ElleServices.ps1 -WhatIf                   # dry run
```

## What it does per service
```
sc.exe create <Name> binPath= "<exe>" DisplayName= "..." start= auto depend= <deps>
sc.exe description <Name> "Elle-Ann ESI — <display>"
sc.exe failure    <Name> reset= 86400 actions= restart/60000/restart/60000/restart/60000
sc.exe start      <Name>
```

Already-registered services are skipped on create but still started if stopped.

## Uninstall
Double-click `Uninstall.bat` (or `.\Uninstall-ElleServices.ps1`).

Stops + deletes in reverse dependency order (HTTP first, Heartbeat last).
Your SQL data and logs are **not** touched — delete the DB manually if you
want a full wipe.

## Verifying the install
```powershell
sc.exe query ElleHTTP           # should report RUNNING
curl http://localhost:8000/api/health
```

## Service order (dependency graph)
```
  Heartbeat
     ├── QueueWorker ─── Action
     ├── Memory      ─── WorldModel
     │                 ├── Identity
     │                 ├── Continuity
     │                 ├── GoalEngine
     │                 └── Dream
     ├── Emotional   ─── InnerLife
     │                 ├── Solitude
     │                 ├── Bonding
     │                 └── SelfPrompt
     └── (Memory + Emotional) ── Cognitive ── HTTP
```
