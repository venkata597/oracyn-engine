@echo off
setlocal enabledelayedexpansion

:: =========================================================
:: 0. Self-elevate to Administrator
:: =========================================================
net session >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [INFO] Requesting administrator privileges...
    powershell -NoProfile -Command "Start-Process cmd -ArgumentList '/k \"\"%~f0\"\"' -Verb RunAs"
    exit /b 0
)

set "NEEDS_RERUN=0"

:: Delete broken local vcpkg configuration if present
if exist "vcpkg-configuration.json" del /f /q "vcpkg-configuration.json"

:: =========================================================
:: 1. Ensure winget is available
:: =========================================================
where winget >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ERROR] winget was not found on this machine.
    pause
    exit /b 1
)

:: =========================================================
:: 2. Ensure Git, CMake, Ninja, and PowerShell are installed
:: =========================================================
where git >nul 2>&1 || (
    echo [INFO] Installing Git...
    winget install --id Git.Git -e --source winget --accept-source-agreements --accept-package-agreements --skip-dependencies
    set "NEEDS_RERUN=1"
)

where cmake >nul 2>&1 || (
    echo [INFO] Installing CMake...
    winget install --id Kitware.CMake -e --source winget --accept-source-agreements --accept-package-agreements --skip-dependencies
    set "NEEDS_RERUN=1"
)

where ninja >nul 2>&1 || (
    echo [INFO] Installing Ninja...
    winget install --id Ninja-build.Ninja -e --source winget --accept-source-agreements --accept-package-agreements --skip-dependencies
    set "NEEDS_RERUN=1"
)

where pwsh >nul 2>&1 || (
    echo [INFO] Installing PowerShell 7...
    winget install --id Microsoft.PowerShell -e --source winget --accept-source-agreements --accept-package-agreements --skip-dependencies
    set "NEEDS_RERUN=1"
)

:: =========================================================
:: 3. Ensure MSVC C++ Build Tools & Windows SDK are installed
:: =========================================================
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS_PATH="
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_PATH=%%i"
    )
)

if not defined VS_PATH (
    echo [INFO] MSVC C++ Build Tools & Windows SDK not found - installing via winget...
    winget install --id Microsoft.VisualStudio.2022.BuildTools -e --source winget ^
        --accept-source-agreements --accept-package-agreements --skip-dependencies ^
        --override "--quiet --wait --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.Windows11SDK.22000 --includeRecommended"
    set "NEEDS_RERUN=1"
) else (
    echo [INFO] MSVC Build Tools found at: !VS_PATH!
)

if "%NEEDS_RERUN%"=="1" (
    echo.
    echo [INFO] Tools installed. Please close this window and run build.bat.
    pause
    exit /b 0
)

:: =========================================================
:: 4. Verify MSVC Environment
:: =========================================================
call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
where cl.exe >nul 2>&1 || (
    echo [ERROR] cl.exe not found after vcvars64.bat.
    pause
    exit /b 1
)

:: =========================================================
:: 5. Install or update vcpkg
:: =========================================================
set "VCPKG_ROOT=C:\vcpkg"

if not exist "%VCPKG_ROOT%\.git" (
    echo [INFO] Installing vcpkg to %VCPKG_ROOT%...
    git clone --depth 1 https://github.com/microsoft/vcpkg.git "%VCPKG_ROOT%"
) else (
    echo [INFO] Updating vcpkg...
    pushd "%VCPKG_ROOT%"
    git pull --depth 1
    popd
)

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    call "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics
)

setx VCPKG_ROOT "%VCPKG_ROOT%" >nul
setx VCPKG_DISABLE_METRICS "1" >nul

echo [SUCCESS] Setup complete.
pause
