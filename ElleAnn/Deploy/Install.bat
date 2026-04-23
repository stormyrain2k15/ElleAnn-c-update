@echo off
REM Double-click wrapper -- elevates and runs Install-ElleServices.ps1.
REM Propagates the underlying PowerShell exit code so a failed install
REM never prints "Done." OpSec audit (Feb 2026, item #132): prior
REM version unconditionally said "Done." and paused, masking failures.

setlocal

REM Elevate if not already admin.
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Requesting Administrator privileges...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b %errorLevel%
)

set "SCRIPT_DIR=%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%Install-ElleServices.ps1" %*
set "PS_RC=%errorLevel%"

echo.
if "%PS_RC%"=="0" (
    echo ================================================================
    echo  SUCCESS. To uninstall later, run Uninstall.bat from this folder.
    echo ================================================================
) else (
    echo ================================================================
    echo  FAILED. Install-ElleServices.ps1 exited with code %PS_RC%.
    echo  Check the PowerShell output above for the specific error.
    echo ================================================================
)

pause
exit /b %PS_RC%
