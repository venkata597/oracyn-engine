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
    echo [ERROR] Install "App Installer" from the Microsoft Store, then re-run:
    echo [ERROR]   https://apps.microsoft.com/detail/9nblggh4nns1
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

where pwsh >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [INFO] Installing PowerShell 7...
    winget install --id Microsoft.PowerShell -e --source winget --accept-source-agreements --accept-package-agreements --skip-dependencies

    where pwsh >nul 2>&1
    if !ERRORLEVEL! neq 0 (
        echo [INFO] winget source install failed or timed out - trying Microsoft Store source...
        winget install --id 9MZ1SNWT0N5D --source msstore --accept-source-agreements --accept-package-agreements
    )

    set "NEEDS_RERUN=1"

    :: If Zed is installed on this machine, installing pwsh alongside the
    :: existing Windows PowerShell 5.1 is known to break Zed's terminal
    :: ("can't spawn terminal") because its default-shell resolution gets
    :: confused. Pin Zed's terminal shell explicitly to avoid that.
    if exist "%APPDATA%\Zed" (
        echo [INFO] Zed detected - pinning its terminal shell to avoid a known conflict...
        for /f "usebackq delims=" %%p in (`where pwsh 2^>nul`) do set "PWSH_PATH=%%p"
        if defined PWSH_PATH (
            pwsh -NoProfile -Command ^
                "$settingsPath = '%APPDATA%\Zed\settings.json'; $pwsh = '%PWSH_PATH%'.Replace('\','\\'); if (Test-Path $settingsPath) { $json = Get-Content $settingsPath -Raw | ConvertFrom-Json -AsHashtable } else { $json = @{} }; if (-not $json.ContainsKey('terminal')) { $json['terminal'] = @{} }; $json['terminal']['shell'] = @{ program = $pwsh }; New-Item -ItemType Directory -Force -Path (Split-Path $settingsPath) | Out-Null; $json | ConvertTo-Json -Depth 10 | Set-Content $settingsPath"
        )
    )
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
    echo [INFO] One or more tools were just installed.
    echo [INFO] Please close this window and run pre_build.bat again so the
    echo [INFO] new PATH entries are picked up correctly.
    pause
    exit /b 0
)

:: =========================================================
:: 4. Verify MSVC Environment
:: =========================================================
where cl.exe >nul 2>&1
if %ERRORLEVEL% neq 0 (
    call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
)

where cl.exe >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ERROR] cl.exe still not found on PATH after loading vcvars64.bat.
    echo [ERROR] Something is wrong with the Visual Studio C++ install.
    pause
    exit /b 1
)
echo [INFO] MSVC compiler found:
where cl.exe

:: =========================================================
:: 5. Install or update vcpkg (lives at C:\vcpkg, shared by
::    all projects that use this script)
:: =========================================================
set "VCPKG_ROOT=C:\vcpkg"

if not exist "%VCPKG_ROOT%\.git" (
    echo [INFO] Installing vcpkg to %VCPKG_ROOT%...
    git clone --depth 1 https://github.com/microsoft/vcpkg.git "%VCPKG_ROOT%"
    if !ERRORLEVEL! neq 0 (
        echo [ERROR] git clone of vcpkg failed. Check your internet connection.
        pause
        exit /b 1
    )
) else (
    echo [INFO] Updating vcpkg...
    pushd "%VCPKG_ROOT%"
    git pull --depth 1
    popd
)

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [INFO] Bootstrapping vcpkg...
    call "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics

    if not exist "%VCPKG_ROOT%\vcpkg.exe" (
        echo [INFO] Bootstrap failed - trying curl fallback...
        where curl.exe >nul 2>&1
        if !ERRORLEVEL! equ 0 (
            powershell -NoProfile -Command ^
                "$r = Invoke-RestMethod -Uri 'https://api.github.com/repos/microsoft/vcpkg-tool/releases/latest'; $a = $r.assets | Where-Object { $_.name -eq 'vcpkg.exe' }; Invoke-WebRequest -Uri $a.browser_download_url -OutFile '%VCPKG_ROOT%\vcpkg.exe'"
        )
    )
)

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [ERROR] vcpkg.exe could not be installed automatically.
    echo [ERROR] This machine's network may be blocking GitHub release downloads.
    echo [ERROR] Manually download it from:
    echo [ERROR]   https://github.com/microsoft/vcpkg-tool/releases/latest
    echo [ERROR] and place it at %VCPKG_ROOT%\vcpkg.exe
    echo [ERROR] Then re-run this script.
    pause
    exit /b 1
)

setx VCPKG_ROOT "%VCPKG_ROOT%" >nul
setx VCPKG_DISABLE_METRICS "1" >nul
echo [INFO] Using VCPKG_ROOT: %VCPKG_ROOT%

echo.
echo [SUCCESS] Pre-build setup complete.
echo [INFO] Git, CMake, Ninja, PowerShell 7, MSVC Build Tools, and vcpkg are ready.
echo [INFO] Run build.bat from the project root to configure and build.
pause
