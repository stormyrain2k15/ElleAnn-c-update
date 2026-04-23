@echo off
REM Double-click wrapper -- elevates and runs Uninstall-ElleServices.ps1.
REM Propagates the underlying PowerShell exit code (audit item #132).

setlocal

net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Requesting Administrator privileges...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b %errorLevel%
)

set "SCRIPT_DIR=%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%Uninstall-ElleServices.ps1" %*
set "PS_RC=%errorLevel%"

echo.
if "%PS_RC%"=="0" (
    echo ================================================================
    echo  Uninstall complete.
    echo ================================================================
) else (
    echo ================================================================
    echo  FAILED. Uninstall-ElleServices.ps1 exited with code %PS_RC%.
    echo  Check the PowerShell output above for the specific error.
    echo ================================================================
)

pause
exit /b %PS_RC%
