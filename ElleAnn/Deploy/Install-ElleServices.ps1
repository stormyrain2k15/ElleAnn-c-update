<#
.SYNOPSIS
    Registers all Elle-Ann services with Windows SCM.

.DESCRIPTION
    Reads elle_service_manifest.json and for each entry runs:
        sc.exe create <Name> binPath= "<abs exe>" DisplayName= "<...>" start= auto depend= <deps>
        sc.exe description <Name> "<desc>"
    Then starts them in manifest order (dependency-respecting).

    Must be run elevated. The companion Install.bat elevates automatically.

.PARAMETER ManifestPath
    Path to elle_service_manifest.json. Defaults to the script's own folder.

.PARAMETER BinaryRoot
    Absolute path containing the built .exe files. If omitted, the manifest's
    "binary_root" field is resolved relative to ManifestPath.

.EXAMPLE
    .\Install-ElleServices.ps1
    .\Install-ElleServices.ps1 -BinaryRoot "C:\ElleAnn\bin"
#>
param(
    [string] $ManifestPath = (Join-Path $PSScriptRoot 'elle_service_manifest.json'),
    [string] $BinaryRoot   = '',
    [switch] $NoStart,
    [switch] $WhatIf,
    [switch] $Force
)

$ErrorActionPreference = 'Stop'

# --- elevation check ------------------------------------------------------
$principal = New-Object Security.Principal.WindowsPrincipal(
    [Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Error "This script must run elevated (Run as Administrator)."
    exit 1
}

if (-not (Test-Path -LiteralPath $ManifestPath)) {
    Write-Error "Manifest not found: $ManifestPath"
    exit 1
}
$manifest = Get-Content -LiteralPath $ManifestPath -Raw | ConvertFrom-Json

if (-not $BinaryRoot) {
    $BinaryRoot = Join-Path $PSScriptRoot $manifest.binary_root
}
$BinaryRoot = (Resolve-Path -LiteralPath $BinaryRoot).Path
Write-Host "Using binary root: $BinaryRoot" -ForegroundColor Cyan

# --- register each service -----------------------------------------------
foreach ($svc in $manifest.services) {
    $exePath = Join-Path $BinaryRoot $svc.exe
    if (-not (Test-Path -LiteralPath $exePath)) {
        Write-Warning "Missing exe for $($svc.name): $exePath (skipped)"
        continue
    }

    $existing = (sc.exe query $svc.name 2>&1) | Out-String
    if ($existing -notmatch 'FAILED|does not exist') {
        if ($Force) {
            Write-Host "[$($svc.name)] -Force: reconfiguring binPath → $exePath" -ForegroundColor Yellow
            if (-not $WhatIf) {
                # Stop before reconfiguring so the EXE lock is released.
                if ($existing -match 'RUNNING') {
                    & sc.exe stop $svc.name | Out-Null
                    Start-Sleep -Milliseconds 800
                }
                $configBin = 'binPath= "' + $exePath + '"'
                & sc.exe config $svc.name $configBin | Out-Null
            }
        }
        else {
            Write-Host "[$($svc.name)] already registered — skipping create (use -Force to reconfigure)." -ForegroundColor DarkYellow
        }
    }
    else {
        $binArg   = 'binPath= "' + $exePath + '"'
        $dispArg  = 'DisplayName= "' + $svc.display + '"'
        $startArg = 'start= ' + $svc.start
        $createArgs = @('create', $svc.name, $binArg, $dispArg, $startArg)
        if ($svc.depends -and $svc.depends.Count -gt 0) {
            $createArgs += 'depend= ' + ($svc.depends -join '/')
        }

        if ($WhatIf) {
            Write-Host "WhatIf: sc.exe $($createArgs -join ' ')"
        }
        else {
            Write-Host "[$($svc.name)] registering..." -ForegroundColor Green
            & sc.exe @createArgs | Out-Null
            & sc.exe description $svc.name "Elle-Ann ESI — $($svc.display)" | Out-Null
            # crash-recovery — restart 3 times with 60-sec delays
            & sc.exe failure $svc.name reset= 86400 actions= restart/60000/restart/60000/restart/60000 | Out-Null
        }
    }
}

# --- start in manifest order ---------------------------------------------
if (-not $NoStart) {
    Write-Host "`nStarting services in dependency order..." -ForegroundColor Cyan
    foreach ($svc in $manifest.services) {
        if ($WhatIf) {
            Write-Host "WhatIf: sc.exe start $($svc.name)"
            continue
        }
        $status = (sc.exe query $svc.name) -join "`n"
        if ($status -match 'RUNNING') {
            Write-Host "[$($svc.name)] already running."
            continue
        }
        Write-Host "[$($svc.name)] starting..." -ForegroundColor Green
        & sc.exe start $svc.name | Out-Null
        Start-Sleep -Milliseconds 600
    }
}

Write-Host "`nDone. Run 'sc.exe query ElleHTTP' to check the API service." -ForegroundColor Cyan
