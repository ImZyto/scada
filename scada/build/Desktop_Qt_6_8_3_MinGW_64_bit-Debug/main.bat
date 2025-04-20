@echo off
setlocal

set BUILD_PATH=C:\Users\micha\Desktop\zaawansowane_C++_2025L\scada\scada\build\Desktop_Qt_6_8_3_MinGW_64_bit-Debug
cd /d "%BUILD_PATH%"

set SCADA_EXE=scada.exe
set SERVER_EXE=C:\Users\micha\Desktop\zaawansowane_C++_2025L\scada\server\build\Desktop_Qt_6_8_3_MinGW_64_bit-Debug\test_server.exe
set QT_PLUGIN_PATH=platforms

:: Uruchom serwer
start "" "%SERVER_EXE%"
timeout /t 2 > nul

:: Uruchom aplikację bez 'start', żeby log poszedł w dobry folder
"%SCADA_EXE%"

pause
endlocal
