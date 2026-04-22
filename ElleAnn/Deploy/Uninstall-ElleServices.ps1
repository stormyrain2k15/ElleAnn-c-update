<#
.SYNOPSIS
    Stops and removes all Elle-Ann services registered by Install-ElleServices.ps1.
#>
param(
    [string] $ManifestPath = (Join-Path $PSScriptRoot 'elle_service_manifest.json'),
    [switch] $KeepData
)

$ErrorActionPreference = 'Continue'

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

# Remove in reverse dependency order (HTTP first, Heartbeat last).
$reversed = $manifest.services | Sort-Object { [Array]::IndexOf($manifest.services, $_) } -Descending

foreach ($svc in $reversed) {
    $status = (sc.exe query $svc.name 2>&1) -join "`n"
    if ($status -match 'does not exist|FAILED 1060') {
        Write-Host "[$($svc.name)] not installed — skipping."
        continue
    }
    if ($status -match 'RUNNING') {
        Write-Host "[$($svc.name)] stopping..." -ForegroundColor Yellow
        & sc.exe stop $svc.name | Out-Null
        Start-Sleep -Milliseconds 600
    }
    Write-Host "[$($svc.name)] deleting..." -ForegroundColor Red
    & sc.exe delete $svc.name | Out-Null
}

if (-not $KeepData) {
    Write-Host "`nData preserved (SQL DB + logs untouched). Use -KeepData:$false to discard too." -ForegroundColor Cyan
}

Write-Host "Uninstall complete." -ForegroundColor Cyan
