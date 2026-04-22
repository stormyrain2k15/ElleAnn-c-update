@echo off
REM Double-click wrapper — elevates and runs Install-ElleServices.ps1
REM Usage: just double-click this file.

setlocal

REM Elevate if not already admin
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Requesting Administrator privileges...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)

set "SCRIPT_DIR=%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%Install-ElleServices.ps1" %*

echo.
echo ================================================================
echo  Done. To uninstall later, run Uninstall.bat from this folder.
echo ================================================================
pause
