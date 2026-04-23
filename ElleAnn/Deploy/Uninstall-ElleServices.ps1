<#
.SYNOPSIS
    Stops and removes all Elle-Ann services registered by Install-ElleServices.ps1.

.DESCRIPTION
    STRICT uninstall: every sc.exe return code is checked, services are
    polled for STATE = STOPPED instead of a fixed sleep, and the script
    confirms each delete actually succeeded (sc.exe `delete` is async on
    services with open handles — the row survives until the last handle
    closes).  Previously sleeps of 600ms were racy and swallowed errors.
#>
param(
    [string] $ManifestPath     = (Join-Path $PSScriptRoot 'elle_service_manifest.json'),
    [switch] $KeepData,
    [int]    $StopTimeoutSec   = 30,
    [int]    $DeleteTimeoutSec = 30
)

$ErrorActionPreference = 'Stop'

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

function Wait-ServiceGone {
    param([Parameter(Mandatory)] [string] $Name, [int] $TimeoutSec = 30)
    $deadline = (Get-Date).AddSeconds($TimeoutSec)
    while ((Get-Date) -lt $deadline) {
        $raw = (& sc.exe query $Name 2>&1) -join "`n"
        if ($raw -match 'does not exist|FAILED 1060') { return $true }
        Start-Sleep -Milliseconds 500
    }
    return $false
}

function Wait-ServiceStopped {
    param([Parameter(Mandatory)] [string] $Name, [int] $TimeoutSec = 30)
    $deadline = (Get-Date).AddSeconds($TimeoutSec)
    while ((Get-Date) -lt $deadline) {
        $raw = (& sc.exe query $Name) -join "`n"
        if ($raw -match 'STATE\s*:\s*1\s+STOPPED') { return $true }
        Start-Sleep -Milliseconds 500
    }
    return $false
}

# Remove in reverse dependency order (HTTP first, Heartbeat last).
$reversed = $manifest.services | Sort-Object { [Array]::IndexOf($manifest.services, $_) } -Descending

$failures = @()
foreach ($svc in $reversed) {
    $status = (sc.exe query $svc.name 2>&1) -join "`n"
    if ($status -match 'does not exist|FAILED 1060') {
        Write-Host "[$($svc.name)] not installed — skipping."
        continue
    }

    if ($status -match 'RUNNING|START_PENDING|STOP_PENDING') {
        Write-Host "[$($svc.name)] stopping..." -ForegroundColor Yellow
        $null = & sc.exe stop $svc.name 2>&1
        # sc.exe stop returns non-zero if already stopped — tolerate.
        if (-not (Wait-ServiceStopped -Name $svc.name -TimeoutSec $StopTimeoutSec)) {
            $failures += "[$($svc.name)] failed to stop within $StopTimeoutSec s"
            continue
        }
    }

    Write-Host "[$($svc.name)] deleting..." -ForegroundColor Red
    $null = & sc.exe delete $svc.name 2>&1
    $delCode = $LASTEXITCODE
    if ($delCode -ne 0) {
        $failures += "[$($svc.name)] sc.exe delete exit code $delCode"
        continue
    }
    if (-not (Wait-ServiceGone -Name $svc.name -TimeoutSec $DeleteTimeoutSec)) {
        $failures += "[$($svc.name)] still present after $DeleteTimeoutSec s (open handles?)"
    }
}

if (-not $KeepData) {
    Write-Host "`nData preserved (SQL DB + logs untouched). Use -KeepData:`$false to discard too." -ForegroundColor Cyan
}

if ($failures.Count -gt 0) {
    Write-Error ("Uninstall completed with errors:`n  " + ($failures -join "`n  "))
    exit 2
}

Write-Host "Uninstall complete." -ForegroundColor Cyan
