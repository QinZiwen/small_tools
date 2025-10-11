@echo off
setlocal enabledelayedexpansion

echo -------------------------------------------------
echo Installing clang-format pre-commit hook
echo -------------------------------------------------

:: Check if inside a Git repository
if not exist ".git" (
    echo [error] This is not a Git repository. Please run this script in the project root.
    pause
    exit /b 1
)

:: Check Git existence
where git >nul 2>nul
if errorlevel 1 (
    echo [error] Git is not installed or not in PATH. Please install Git: https://git-scm.com/download/win
    pause
    exit /b 1
)

:: Check clang-format existence
where clang-format >nul 2>nul
if errorlevel 1 (
    echo [warning] clang-format not found. Starting automatic installation...

    :: Try scoop first
    where scoop >nul 2>nul
    if not errorlevel 1 (
        echo [info] Scoop detected. Installing LLVM via scoop...
        scoop install llvm
    ) else (
        :: Use official installer
        echo [warning] Scoop not found. Downloading LLVM installer...
        set LLVM_URL=https://gitlab.navinfo.com/qinziwen11262/llvm-project/-/raw/main/release/LLVM-18.1.8-win64.exe
        set LLVM_INSTALLER=%TEMP%\llvm-installer.exe
        echo LLVM_URL: !LLVM_URL!
        echo LLVM_INSTALLER: !LLVM_INSTALLER!
        powershell -Command "Write-Host 'Downloading LLVM...'; Invoke-WebRequest -Uri '!LLVM_URL!' -OutFile '!LLVM_INSTALLER!'"
        echo Starting silent installation...
        "!LLVM_INSTALLER!" /S
        del "!LLVM_INSTALLER!"

        :: Add LLVM to PATH
        set "LLVM_DIR=C:\Program Files\LLVM\bin"
        if exist "!LLVM_DIR!\clang-format.exe" (
            echo Adding LLVM to system PATH...
            for /f "tokens=2* delims= " %%a in ('reg query "HKCU\Environment" /v PATH 2^>nul') do set "OLD_PATH=%%b"
            echo !OLD_PATH! | find /i "!LLVM_DIR!" >nul
            if errorlevel 1 (
                setx PATH "!OLD_PATH!;!LLVM_DIR!" >nul
                set "PATH=%PATH%;!LLVM_DIR!"
                echo [info] LLVM path added to PATH.
            ) else (
                echo [info] LLVM path already exists in PATH.
            )
        ) else (
            echo [error] "!LLVM_DIR!\clang-format.exe" not found. Please verify installation.
        )
    )

    :: Verify clang-format installation
    where clang-format >nul 2>nul
    if errorlevel 1 (
        echo [error] clang-format installation failed. Please install LLVM manually: https://releases.llvm.org/download.html
        pause
        exit /b 1
    ) else (
        for /f "delims=" %%i in ('clang-format --version') do set "CF_VER=%%i"
        echo [info] clang-format installed: !CF_VER!
    )
) else (
    for /f "delims=" %%i in ('clang-format --version') do set "CF_VER=%%i"
    echo [info] clang-format detected: !CF_VER!
)

:: Choose hook mode
echo.
echo =================================================
echo Select pre-commit hook mode:
echo   [1] Strict mode (block commit if format issues)
echo   [2] Auto-fix mode (format automatically before commit)
echo =================================================
set /p choice=Enter choice [1/2] and press Enter:

if "!choice!"=="2" (
    set MODE=auto-fix
) else (
    set MODE=strict
)

:: Ensure hooks directory exists
if not exist ".git\hooks" mkdir ".git\hooks"

:: Download pre-commit hook from remote repository
set HOOK_PATH=.git\hooks\pre-commit
set HOOK_URL=https://gitlab.navinfo.com/qinziwen11262/llvm-project/-/raw/main/release/hook/pre-commit.auto-fix?inline=false
if "!MODE!"=="strict" (
    echo MODE: strict
    set HOOK_URL=https://gitlab.navinfo.com/qinziwen11262/llvm-project/-/raw/main/release/hook/pre-commit.strict?inline=false
) else (
    echo MODE: auto-fix
)

echo Downloading pre-commit hook from: !HOOK_URL!

powershell -Command "Invoke-WebRequest -Uri '!HOOK_URL!' -OutFile '!HOOK_PATH!' -UseBasicParsing -ErrorAction Stop" || (
    echo [error] Failed to download hook from !HOOK_URL!
    pause
    exit /b 1
)

if not exist "!HOOK_PATH!" (
    echo Error: Failed to create pre-commit hook.
    pause
    exit /b 1
)

:: Ensure Git uses the hooks path
icacls ".git\hooks\pre-commit" /grant:r "%USERNAME%":(F) >nul 2>&1
git config core.hooksPath .git/hooks >nul 2>nul

:: Final output
echo.
echo =================================================
if "!MODE!"=="strict" (
    echo [info] Strict mode pre-commit hook installed.
    echo Commits will be blocked if C/C++ formatting issues are detected.
) else (
    echo [info] Auto-fix mode pre-commit hook installed.
    echo C/C++ code will be automatically formatted before commit.
)
echo =================================================
pause

endlocal
exit /b 0
