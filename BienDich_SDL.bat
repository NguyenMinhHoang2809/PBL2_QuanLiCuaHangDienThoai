@echo off
REM ============================================================
REM Bien dich ban DO HOA SDL2 (menu trong cua so, nhap lieu o Console)
REM Thu vien SDL2 + SDL2_ttf (ban MinGW 64-bit) nam san trong ThuVien\SDL2
REM Ket qua: PBL2_SDL.exe + SDL2.dll + SDL2_ttf.dll (PHAI de cung thu muc)
REM ============================================================
cd /d "%~dp0"
set SDL=ThuVien\SDL2

if not exist "%SDL%\include\SDL2\SDL.h" (
    echo [LOI] Khong tim thay %SDL%\include\SDL2\SDL.h
    echo Hay tai SDL2-devel-x.y.z-mingw.zip va SDL2_ttf-devel-x.y.z-mingw.zip
    echo tu https://github.com/libsdl-org roi chep thu muc x86_64-w64-mingw32
    echo vao %SDL% ^(include\SDL2, lib, bin^).
    pause
    exit /b 1
)

echo Dang bien dich PBL2_SDL.exe (ban SDL2)...
g++ -std=c++17 -Wall -O2 -DDUNG_SDL *.cpp -o PBL2_SDL.exe ^
    -I"%SDL%\include" -I"%SDL%\include\SDL2" -L"%SDL%\lib" ^
    -lSDL2 -lSDL2_ttf
if errorlevel 1 (
    echo.
    echo [LOI] Bien dich that bai!
    pause
    exit /b 1
)

REM Chuong trinh can 2 file DLL nam canh file .exe de chay
copy /Y "%SDL%\bin\SDL2.dll" . >nul
copy /Y "%SDL%\bin\SDL2_ttf.dll" . >nul
echo [OK] Da tao PBL2_SDL.exe (kem SDL2.dll, SDL2_ttf.dll)

