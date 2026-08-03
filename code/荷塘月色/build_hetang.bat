@echo off
setlocal

gcc hetang_yuese.c -std=c11 -O2 -Wall -Wextra -mwindows -lgdi32 -o hetang_yuese.exe

if errorlevel 1 (
    echo.
    echo Build failed. Make sure MinGW-w64 GCC is installed and gcc is in PATH.
    pause
    exit /b 1
)

echo Build succeeded: hetang_yuese.exe
start "" hetang_yuese.exe

endlocal