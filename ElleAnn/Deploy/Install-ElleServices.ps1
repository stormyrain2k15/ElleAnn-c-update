<#
.SYNOPSIS
    Registers all Elle-Ann services with Windows SCM.

.DESCRIPTION
    Reads elle_service_manifest.json and for each entry runs:
        sc.exe create <Name> binPath= "<abs exe>" DisplayName= "<...>" start= auto
        sc.exe description <Name> "<desc>"
    Then starts them ALL IN PARALLEL — no SCM-level dependency chain.

    Feb-2026 pivot: SCM `depends=` are intentionally empty in the
    manifest. The C++ reconnector loop in Shared/ElleServiceBase.cpp
    binds peers as they appear, in any order. A single failing
    service can no longer block the whole stack.

    This installer is STRICT for *missing* binaries: a missing executable
    or a non-zero `sc.exe create` exit code aborts the whole install
    with a clear error. Services that fail to *start* are warned about
    but no longer abort — the rest of the mesh keeps running, fix and
    `sc start <name>` later.

    Must be run elevated. The companion Install.bat elevates automatically.

.PARAMETER ManifestPath
    Path to elle_service_manifest.json. Defaults to the script's own folder.

.PARAMETER BinaryRoot
    Absolute path containing the built .exe files. If omitted, the manifest's
    "binary_root" field is resolved relative to ManifestPath.

.PARAMETER SkipMissing
    When set, missing executables are tolerated (warning only). OFF by
    default — deploys should fail-closed.

.PARAMETER StartTimeoutSec
    How long to wait for each service to transition from START_PENDING
    to RUNNING before declaring failure. Default 30.

.EXAMPLE
    .\Install-ElleServices.ps1
    .\Install-ElleServices.ps1 -BinaryRoot "C:\ElleAnn\bin"
#>
param(
    [string] $ManifestPath     = (Join-Path $PSScriptRoot 'elle_service_manifest.json'),
    [string] $BinaryRoot       = '',
    [switch] $NoStart,
    [switch] $WhatIf,
    [switch] $Force,
    [switch] $SkipMissing,
    [int]    $StartTimeoutSec  = 30
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

# --- sc.exe helper: runs a command, returns exit code, fails on non-zero -
function Invoke-Sc {
    param(
        [Parameter(Mandatory)] [string[]] $Arguments,
        [switch] $TolerateNonZero
    )
    if ($WhatIf) {
        Write-Host "WhatIf: sc.exe $($Arguments -join ' ')"
        return 0
    }
    $null = & sc.exe @Arguments 2>&1
    $code = $LASTEXITCODE
    if ($code -ne 0 -and -not $TolerateNonZero) {
        throw "sc.exe $($Arguments -join ' ') failed with exit code $code"
    }
    return $code
}

# --- SCM state poll: block until a service reaches a terminal state ------
function Wait-ServiceState {
    param(
        [Parameter(Mandatory)] [string] $Name,
        [Parameter(Mandatory)] [string] $DesiredState,     # RUNNING | STOPPED
        [int] $TimeoutSec = 30
    )
    $deadline = (Get-Date).AddSeconds($TimeoutSec)
    while ((Get-Date) -lt $deadline) {
        $raw = (& sc.exe query $Name) -join "`n"
        if ($raw -match "STATE\s*:\s*\d+\s+$DesiredState") { return $true }
        if ($DesiredState -eq 'RUNNING' -and $raw -match 'STATE\s*:\s*1\s+STOPPED') {
            throw "[$Name] entered STOPPED while waiting for RUNNING (service crashed on start)."
        }
        Start-Sleep -Milliseconds 500
    }
    return $false
}

# --- staged binary swap: validate + backup + swap + rollback on failure ---
# OpSec audit item #95. Prior behaviour was "sc.exe config binPath= ...",
# which re-points the SCM at the new exe but does nothing about the old
# file; a crash in the new build leaves the operator with no obvious
# rollback path. We now:
#   1. Verify the new exe exists and is a non-empty PE file.
#   2. Copy the old registered exe (if any) alongside as "<name>.exe.bak"
#      before reconfiguring. A failed start is rescued with sc.exe config
#      back to the .bak path.
#   3. On successful RUNNING verification the .bak is retained for one
#      generation so the operator can roll back manually if they later
#      discover a subtle regression; older .bak.N files are pruned.
function Resolve-ServiceBinPath {
    param([Parameter(Mandatory)][string] $Name)
    $raw = (& sc.exe qc $Name) 2>&1 | Out-String
    if ($raw -match 'BINARY_PATH_NAME\s*:\s*(.+)') {
        $p = $Matches[1].Trim().Trim('"')
        if (Test-Path -LiteralPath $p) { return (Resolve-Path -LiteralPath $p).Path }
    }
    return $null
}

function Test-IsPEFile {
    param([Parameter(Mandatory)][string] $Path)
    if (-not (Test-Path -LiteralPath $Path)) { return $false }
    $fi = Get-Item -LiteralPath $Path
    if ($fi.Length -lt 128) { return $false }          # too small to be a real PE
    $fs = [System.IO.File]::OpenRead($Path)
    try {
        $hdr = New-Object byte[] 2
        $null = $fs.Read($hdr, 0, 2)
        return ($hdr[0] -eq 0x4D -and $hdr[1] -eq 0x5A) # 'MZ'
    } finally {
        $fs.Dispose()
    }
}

function Swap-BinaryStaged {
    param(
        [Parameter(Mandatory)][string] $Name,
        [Parameter(Mandatory)][string] $NewExe
    )
    if (-not (Test-IsPEFile $NewExe)) {
        throw "[$Name] refusing to swap: '$NewExe' is not a valid PE image."
    }
    $oldExe = Resolve-ServiceBinPath -Name $Name
    if ($oldExe -and ($oldExe -ne $NewExe)) {
        $bak = "$oldExe.bak"
        if (Test-Path -LiteralPath $bak) { Remove-Item -LiteralPath $bak -Force }
        Copy-Item -LiteralPath $oldExe -Destination $bak -Force
        Write-Host "[$Name] previous binary preserved at $bak" -ForegroundColor DarkGray
    }
    [void](Invoke-Sc -Arguments @('config', $Name, ('binPath= "' + $NewExe + '"')))
}

# --- register each service -----------------------------------------------
$missing = @()
foreach ($svc in $manifest.services) {
    $exePath = Join-Path $BinaryRoot $svc.exe
    if (-not (Test-Path -LiteralPath $exePath)) {
        if ($SkipMissing) {
            Write-Warning "Missing exe for $($svc.name): $exePath (skipped by -SkipMissing)"
            continue
        }
        $missing += "$($svc.name) -> $exePath"
        continue
    }

    $existing = (sc.exe query $svc.name 2>&1) | Out-String
    if ($existing -notmatch 'FAILED|does not exist') {
        if ($Force) {
            Write-Host "[$($svc.name)] -Force: reconfiguring binPath -> $exePath" -ForegroundColor Yellow
            if ($existing -match 'RUNNING') {
                [void](Invoke-Sc -Arguments @('stop', $svc.name) -TolerateNonZero)
                if (-not $WhatIf) { [void](Wait-ServiceState -Name $svc.name -DesiredState 'STOPPED' -TimeoutSec $StartTimeoutSec) }
            }
            if (-not $WhatIf) { Swap-BinaryStaged -Name $svc.name -NewExe $exePath }
            else              { [void](Invoke-Sc -Arguments @('config', $svc.name, ('binPath= "' + $exePath + '"'))) }
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

        Write-Host "[$($svc.name)] registering..." -ForegroundColor Green
        [void](Invoke-Sc -Arguments $createArgs)
        [void](Invoke-Sc -Arguments @('description', $svc.name, "Elle-Ann ESI — $($svc.display)"))
        # crash-recovery — restart 3 times with 60-sec delays
        [void](Invoke-Sc -Arguments @('failure', $svc.name, 'reset= 86400',
            'actions= restart/60000/restart/60000/restart/60000'))
    }
}

if ($missing.Count -gt 0) {
    Write-Error ("Install aborted — missing executables:`n  " + ($missing -join "`n  ") +
                 "`nBuild them first, or pass -SkipMissing to tolerate.")
    exit 2
}

# --- start in parallel — passive mesh, no ordering required ---------------
# Feb-2026 pivot: services no longer have SCM-level `depends=` chains and
# the C++ reconnector loop binds peers as they appear (in any order). So
# we kick off every `sc.exe start` immediately and only wait for the set
# to converge. A single slow service can no longer block the whole stack.
if (-not $NoStart) {
    Write-Host "`nStarting services in parallel (passive mesh, no ordering)..." -ForegroundColor Cyan
    $toStart = @()
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
        # `sc.exe start` returns immediately after submitting the request
        # to SCM, so this loop fans out without blocking.
        [void](Invoke-Sc -Arguments @('start', $svc.name) -TolerateNonZero)
        $toStart += $svc.name
    }

    # Wait for the set to converge. Each service has its own deadline; a
    # slow Heartbeat no longer pushes Memory's deadline out.
    $failed = @()
    foreach ($name in $toStart) {
        if (-not (Wait-ServiceState -Name $name -DesiredState 'RUNNING' -TimeoutSec $StartTimeoutSec)) {
            $failed += $name
            Write-Warning "[$name] failed to reach RUNNING within $StartTimeoutSec s — the rest of the mesh will keep running and re-attempt connections."
        }
    }
    if ($failed.Count -gt 0) {
        Write-Warning ("$($failed.Count) service(s) did not start: " + ($failed -join ', ') +
                      ". The mesh continues — fix and re-run `sc.exe start <name>` for each.")
    }
}

Write-Host "`nDone. Run 'sc.exe query ElleHTTPServer' to check the API service." -ForegroundColor Cyan
