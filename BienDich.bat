@echo off
REM ============================================================
REM Bien dich ban CONSOLE (menu nut bam chuot + ban phim tren Console)
REM Khong can thu vien ngoai. Ket qua: PBL2.exe
REM ============================================================
cd /d "%~dp0"
echo Dang bien dich PBL2.exe (ban Console)...
g++ -std=c++17 -Wall -O2 *.cpp -o PBL2.exe
if errorlevel 1 (
    echo.
    echo [LOI] Bien dich that bai!
    pause
    exit /b 1
)
echo [OK] Da tao PBL2.exe

