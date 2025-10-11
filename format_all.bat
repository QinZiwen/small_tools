@echo off
REM Format all tracked C/C++ files according to .gitignore

REM Check if clang-format exists
where clang-format >nul 2>nul
if errorlevel 1 (
    echo [error] clang-format not found. Please install it first.
    pause
    exit /b 1
)

REM Recursively format all tracked *.cpp and *.h files
for /f "delims=" %%f in ('git ls-files "*.cpp" "*.cc" "*.h" "*.hpp"') do (
    echo Formatting %%f
    clang-format -i "%%f"
)

echo Done.
pause
