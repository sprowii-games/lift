@echo off
rem LIFT web: локальный сервер (браузер не грузит .wasm/.data через file://)
chcp 65001 >nul
cd /d "%~dp0"

where python >nul 2>nul
if %errorlevel%==0 (
    set "PY=python"
) else (
    where py >nul 2>nul
    if %errorlevel%==0 (
        set "PY=py"
    ) else (
        echo Python не найден. Установи Python с python.org и запусти этот файл снова.
        pause
        exit /b 1
    )
)

echo.
echo   LIFT web: http://localhost:8080
echo   Остановить: Ctrl+C
echo.
start "" http://localhost:8080
%PY% -m http.server 8080
