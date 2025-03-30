@echo off
setlocal

:: Ścieżki
set SCADA_BUILD=C:\Users\micha\Desktop\zaawansowane_C++_2025L\scada\scada\build\Desktop_Qt_6_8_3_MinGW_64_bit-Debug
set SERVER_BUILD=C:\Users\micha\Desktop\zaawansowane_C++_2025L\scada\server\build\Desktop_Qt_6_8_3_MinGW_64_bit-Debug

set SCADA_EXE=%SCADA_BUILD%\scada.exe
set SERVER_EXE=%SERVER_BUILD%\test_server.exe
set QT_PLUGIN_PATH=%SCADA_BUILD%\platforms

:: Sprawdzenie plików
if not exist "%SCADA_EXE%" (
    echo [Błąd] Nie znaleziono pliku scada.exe!
    pause
    exit /b
)

if not exist "%SERVER_EXE%" (
    echo [Błąd] Nie znaleziono pliku test_server.exe!
    pause
    exit /b
)

:: Ustawienie zmiennej Qt dla pluginów
set QT_QPA_PLATFORM_PLUGIN_PATH=%QT_PLUGIN_PATH%
echo [Info] QT_QPA_PLATFORM_PLUGIN_PATH ustawiono na: %QT_QPA_PLATFORM_PLUGIN_PATH%

:: Przejdź do folderu z EXE, żeby katalog roboczy się zgadzał
cd /d "%SCADA_BUILD%"

:: Uruchom serwer
echo [Info] Uruchamianie serwera...
start "" "%SERVER_EXE%"
timeout /t 1 > nul

:: Uruchom aplikację bez "start", żeby logi działały
echo [Info] Uruchamianie SCADA...
"%SCADA_EXE%"

:: Zostaw konsolę otwartą po zamknięciu aplikacji
echo.
echo [Info] Zakończono działanie SCADA.
pause

endlocal
