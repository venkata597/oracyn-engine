@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

echo =========================================================
echo Oracyn - Pre-Build Environment Setup
echo =========================================================
echo.

:: =========================================================
:: 0. Require Administrator Privileges
:: =========================================================

net session >nul 2>&1

if errorlevel 1 (
    echo [INFO] Administrator privileges required.
    echo [INFO] Requesting elevation...

    powershell -NoProfile -ExecutionPolicy Bypass -Command ^
        "Start-Process cmd.exe -Verb RunAs -ArgumentList '/c ""%~f0""'"

    exit /b 0
)

:: =========================================================
:: 1. Check for winget
:: =========================================================

echo [INFO] Checking winget...

where winget >nul 2>&1

if errorlevel 1 (
    echo [ERROR] winget was not found.
    echo Install Microsoft App Installer and run this script again.
    pause
    exit /b 1
)

echo [OK] winget found.

:: =========================================================
:: 2. Install Git
:: =========================================================

echo.
echo [INFO] Checking Git...

where git >nul 2>&1

if errorlevel 1 (
    echo [INFO] Git not found.
    echo [INFO] Installing Git...

    winget install ^
        --id Git.Git ^
        -e ^
        --source winget ^
        --accept-source-agreements ^
        --accept-package-agreements

    if errorlevel 1 (
        echo [ERROR] Failed to install Git.
        pause
        exit /b 1
    )

    echo [OK] Git installed.
) else (
    echo [OK] Git already installed.
)

:: =========================================================
:: 3. Install CMake
:: =========================================================

echo.
echo [INFO] Checking CMake...

where cmake >nul 2>&1

if errorlevel 1 (
    echo [INFO] CMake not found.
    echo [INFO] Installing CMake...

    winget install ^
        --id Kitware.CMake ^
        -e ^
        --source winget ^
        --accept-source-agreements ^
        --accept-package-agreements

    if errorlevel 1 (
        echo [ERROR] Failed to install CMake.
        pause
        exit /b 1
    )

    echo [OK] CMake installed.
) else (
    echo [OK] CMake already installed.
)

:: =========================================================
:: 4. Install Ninja
:: =========================================================

echo.
echo [INFO] Checking Ninja...

where ninja >nul 2>&1

if errorlevel 1 (
    echo [INFO] Ninja not found.
    echo [INFO] Installing Ninja...

    winget install ^
        --id Ninja-build.Ninja ^
        -e ^
        --source winget ^
        --accept-source-agreements ^
        --accept-package-agreements

    if errorlevel 1 (
        echo [ERROR] Failed to install Ninja.
        pause
        exit /b 1
    )

    echo [OK] Ninja installed.
) else (
    echo [OK] Ninja already installed.
)

:: =========================================================
:: 5. Locate Visual Studio Installer / vswhere
:: =========================================================

echo.
echo =========================================================
echo Visual Studio 2022
echo =========================================================
echo.

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS_PATH="
set "VS_TEMP=%TEMP%\oracyn_vs_path.txt"

echo [INFO] Looking for Visual Studio Installer...

if not exist "%VSWHERE%" (
    echo [INFO] vswhere.exe was not found.
    echo [INFO] Installing Visual Studio 2022 Build Tools...

    winget install ^
        --id Microsoft.VisualStudio.2022.BuildTools ^
        -e ^
        --source winget ^
        --accept-source-agreements ^
        --accept-package-agreements ^
        --override "--quiet --wait --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.Windows11SDK.22000 --includeRecommended"

    if errorlevel 1 (
        echo [ERROR] Visual Studio 2022 installation failed.
        pause
        exit /b 1
    )

    echo [OK] Visual Studio 2022 installation completed.
)

:: =========================================================
:: 6. Locate VS 2022 with MSVC
:: =========================================================

echo [INFO] Searching for Visual Studio 2022 with MSVC...

if exist "%VSWHERE%" (

    "%VSWHERE%" ^
        -version "[17.0,18.0)" ^
        -products * ^
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
        -property installationPath ^
        > "%VS_TEMP%"

    if exist "%VS_TEMP%" (
        set /p VS_PATH=<"%VS_TEMP%"
        del /q "%VS_TEMP%" >nul 2>&1
    )
)

:: =========================================================
:: 7. Install VS 2022 if no valid installation was found
:: =========================================================

if not defined VS_PATH (

    echo [INFO] Visual Studio 2022 with MSVC was not found.
    echo [INFO] Installing/configuring Visual Studio 2022...

    winget install ^
        --id Microsoft.VisualStudio.2022.BuildTools ^
        -e ^
        --source winget ^
        --accept-source-agreements ^
        --accept-package-agreements ^
        --override "--quiet --wait --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.Windows11SDK.22000 --includeRecommended"

    if errorlevel 1 (
        echo [ERROR] Failed to install/configure Visual Studio 2022.
        pause
        exit /b 1
    )

    echo [INFO] Re-checking Visual Studio installation...

    if exist "%VSWHERE%" (

        "%VSWHERE%" ^
            -version "[17.0,18.0)" ^
            -products * ^
            -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
            -property installationPath ^
            > "%VS_TEMP%"

        if exist "%VS_TEMP%" (
            set /p VS_PATH=<"%VS_TEMP%"
            del /q "%VS_TEMP%" >nul 2>&1
        )
    )
)

:: =========================================================
:: 8. Verify VS_PATH
:: =========================================================

if not defined VS_PATH (
    echo.
    echo [ERROR] Visual Studio 2022 with MSVC C++ tools was not found.
    echo.
    echo Expected components:
    echo.
    echo   Visual Studio 2022 Build Tools
    echo   Desktop development with C++
    echo   MSVC v143 build tools
    echo   Windows 10/11 SDK
    echo.
    pause
    exit /b 1
)

echo [OK] Visual Studio 2022 found:
echo      "%VS_PATH%"

:: =========================================================
:: 9. Verify MSVC installation
:: =========================================================

set "VCVARS=%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"

echo.
echo [INFO] Checking MSVC environment...

if not exist "%VCVARS%" (
    echo [ERROR] vcvars64.bat was not found:
    echo.
    echo "%VCVARS%"
    echo.
    echo Visual Studio was detected, but the MSVC workload is incomplete.
    echo.
    pause
    exit /b 1
)

if not exist "%VS_PATH%\VC\Tools\MSVC" (
    echo [ERROR] MSVC toolset directory was not found:
    echo.
    echo "%VS_PATH%\VC\Tools\MSVC"
    echo.
    pause
    exit /b 1
)

echo [OK] MSVC toolset found.
echo      "%VCVARS%"

:: =========================================================
:: 10. Initialize MSVC environment
:: =========================================================

echo.
echo [INFO] Initializing MSVC environment...

call "%VCVARS%"

if errorlevel 1 (
    echo [ERROR] Failed to initialize MSVC environment.
    pause
    exit /b 1
)

where cl.exe >nul 2>&1

if errorlevel 1 (
    echo [ERROR] cl.exe is unavailable after initializing MSVC.
    pause
    exit /b 1
)

echo [OK] MSVC compiler:
where cl.exe
:: =========================================================
:: 11. Locate / Install vcpkg
:: =========================================================

echo.
echo =========================================================
echo vcpkg
echo =========================================================
echo.

set "VCPKG_ROOT=%LOCALAPPDATA%\vcpkg"

echo [INFO] vcpkg root:
echo        "%VCPKG_ROOT%"

:: =========================================================
:: 12. Clone vcpkg if necessary
:: =========================================================

if not exist "%VCPKG_ROOT%\.git" (

    if exist "%VCPKG_ROOT%" (
        echo [INFO] Removing incomplete vcpkg directory...
        rmdir /s /q "%VCPKG_ROOT%"

        if exist "%VCPKG_ROOT%" (
            echo [ERROR] Could not remove existing vcpkg directory.
            pause
            exit /b 1
        )
    )

    echo [INFO] Cloning current vcpkg...

    git clone https://github.com/microsoft/vcpkg.git "%VCPKG_ROOT%"

    if errorlevel 1 (
        echo [ERROR] Failed to clone vcpkg.
        pause
        exit /b 1
    )

    echo [OK] vcpkg cloned.

) else (
    echo [OK] vcpkg repository already exists.
)

:: =========================================================
:: 13. Update vcpkg
:: =========================================================

pushd "%VCPKG_ROOT%"

echo [INFO] Updating vcpkg...

git fetch origin

if errorlevel 1 (
    popd
    echo [ERROR] Failed to fetch latest vcpkg.
    pause
    exit /b 1
)

git checkout master >nul 2>&1

if errorlevel 1 (
    popd
    echo [ERROR] Failed to checkout vcpkg master branch.
    pause
    exit /b 1
)

git pull --ff-only origin master

if errorlevel 1 (
    popd
    echo [ERROR] Failed to update vcpkg.
    pause
    exit /b 1
)

for /f "tokens=*" %%i in ('git rev-parse HEAD') do (
    set "VCPKG_SHA=%%i"
)

popd

echo [OK] vcpkg commit:
echo      !VCPKG_SHA!

:: =========================================================
:: 14. Bootstrap vcpkg
:: =========================================================

if not exist "%VCPKG_ROOT%\vcpkg.exe" (

    echo.
    echo [INFO] Bootstrapping vcpkg...

    call "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics

    if errorlevel 1 (
        echo [ERROR] vcpkg bootstrap failed.
        pause
        exit /b 1
    )

    echo [OK] vcpkg bootstrapped.

) else (
    echo [OK] vcpkg.exe already exists.
)

:: =========================================================
:: 15. Configure vcpkg environment
:: =========================================================

set "VCPKG_DISABLE_METRICS=1"

:: =========================================================
:: 16. Generate env_setup.bat
:: =========================================================

echo.
echo [INFO] Generating env_setup.bat...

(
    echo @echo off
    echo set "VCPKG_ROOT=%VCPKG_ROOT%"
    echo set "VCPKG_DISABLE_METRICS=1"
    echo set "ORACYN_VS_PATH=%VS_PATH%"
) > "%~dp0env_setup.bat"

if errorlevel 1 (
    echo [ERROR] Failed to create env_setup.bat.
    pause
    exit /b 1
)

echo [OK] env_setup.bat created.

:: =========================================================
:: 17. vcpkg baseline
:: =========================================================

echo.
echo [INFO] Checking vcpkg baseline...

if not exist "%~dp0vcpkg.json" (
    echo [ERROR] vcpkg.json not found.
    pause
    exit /b 1
)

echo [OK] vcpkg baseline is managed by vcpkg.json.
echo [INFO] The setup script will NOT modify it.

:: =========================================================
:: 18. Final Verification
:: =========================================================

echo.
echo =========================================================
echo Environment Verification
echo =========================================================
echo.

echo [INFO] Visual Studio:
echo        "%VS_PATH%"

echo.
echo [INFO] MSVC compiler:
where cl.exe

echo.
echo [INFO] CMake:
cmake --version

echo.
echo [INFO] Ninja:
ninja --version

echo.
echo [INFO] vcpkg:
"%VCPKG_ROOT%\vcpkg.exe" version

echo.
echo =========================================================
echo [SUCCESS] Pre-build setup completed successfully.
echo =========================================================
echo.
echo Run:
echo.
echo     build.bat
echo.
echo to configure and build Oracyn.
echo.

pause
exit /b 0
