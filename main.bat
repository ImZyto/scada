@echo off
setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"

set "SCADA_BUILD=%SCRIPT_DIR%scada\build\Desktop_Qt_6_8_3_MinGW_64_bit-Debug"
set "SERVER_BUILD=%SCRIPT_DIR%server\build\Desktop_Qt_6_8_3_MinGW_64_bit-Debug"

set "SCADA_EXE=%SCADA_BUILD%\scada.exe"
set "SERVER_EXE=%SERVER_BUILD%\test_server.exe"
set "QT_PLUGIN_PATH=%SCADA_BUILD%\platforms"

if not exist "%SCADA_EXE%" (
    echo [Error] File not found: %SCADA_EXE%
    pause
    exit /b
)

if not exist "%SERVER_EXE%" (
    echo [Error] File not found: %SERVER_EXE%
    pause
    exit /b
)

set "QT_QPA_PLATFORM_PLUGIN_PATH=%QT_PLUGIN_PATH%"
echo [Info] QT_QPA_PLATFORM_PLUGIN_PATH set to: %QT_QPA_PLATFORM_PLUGIN_PATH%

cd /d "%SCADA_BUILD%"

echo [Info] Starting server...
start "" "%SERVER_EXE%"
timeout /t 1 > nul

echo [Info] Starting SCADA...
"%SCADA_EXE%"

echo.
echo [Info] SCADA operation terminated.
pause

endlocal
