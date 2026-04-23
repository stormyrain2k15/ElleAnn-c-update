<#
.SYNOPSIS
    Registers all Elle-Ann services with Windows SCM.

.DESCRIPTION
    Reads elle_service_manifest.json and for each entry runs:
        sc.exe create <Name> binPath= "<abs exe>" DisplayName= "<...>" start= auto depend= <deps>
        sc.exe description <Name> "<desc>"
    Then starts them in manifest order (dependency-respecting).

    This installer is STRICT: a missing executable, a non-zero `sc.exe`
    exit code, or a service that fails to reach RUNNING within
    `-StartTimeoutSec` will abort the whole install and emit a clear
    error. Previously the install would happily print a warning and
    continue — deploys shipped to production with broken services
    until an operator noticed at runtime.

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
            [void](Invoke-Sc -Arguments @('config', $svc.name, ('binPath= "' + $exePath + '"')))
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
        [void](Invoke-Sc -Arguments @('start', $svc.name))
        if (-not (Wait-ServiceState -Name $svc.name -DesiredState 'RUNNING' -TimeoutSec $StartTimeoutSec)) {
            throw "[$($svc.name)] failed to reach RUNNING within $StartTimeoutSec seconds."
        }
    }
}

Write-Host "`nDone. Run 'sc.exe query ElleHTTPServer' to check the API service." -ForegroundColor Cyan
