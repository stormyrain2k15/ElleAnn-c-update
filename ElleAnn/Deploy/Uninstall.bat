@echo off
REM Double-click wrapper — elevates and runs Uninstall-ElleServices.ps1.

setlocal

net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Requesting Administrator privileges...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)

set "SCRIPT_DIR=%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%Uninstall-ElleServices.ps1" %*

pause
