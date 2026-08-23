@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

echo.
echo =========================================================
echo    ORACYN - PRE-BUILD ENVIRONMENT SETUP
echo =========================================================
echo.
echo    Preparing the system for building Oracyn.
echo.

:: =========================================================
:: 0. Require Administrator Privileges
:: =========================================================

echo ---------------------------------------------------------
echo [1/9] Administrator privileges
echo ---------------------------------------------------------
echo.

net session >nul 2>&1

if errorlevel 1 (
    echo [INFO] Administrator privileges are required.
    echo [INFO] Requesting elevation...
    echo.

    powershell -NoProfile -ExecutionPolicy Bypass -Command ^
        "Start-Process cmd.exe -Verb RunAs -ArgumentList '/c ""%~f0""'"

    exit /b 0
)

echo [OK] Administrator privileges confirmed.
echo.

:: =========================================================
:: 1. Check for winget
:: =========================================================

echo ---------------------------------------------------------
echo [2/9] Windows Package Manager
echo ---------------------------------------------------------
echo.

echo [INFO] Checking winget...

where winget >nul 2>&1

if errorlevel 1 (
    echo [ERROR] winget was not found.
    echo.
    echo Install Microsoft App Installer and run this script again.
    echo.
    pause
    exit /b 1
)

echo [OK] winget found.
echo.

:: =========================================================
:: 2. Install / Verify Git
:: =========================================================

echo ---------------------------------------------------------
echo [3/9] Git
echo ---------------------------------------------------------
echo.

echo [INFO] Checking Git...

where git >nul 2>&1

if errorlevel 1 (
    if exist "%ProgramFiles%\Git\cmd\git.exe" (
        set "GIT_EXE=%ProgramFiles%\Git\cmd\git.exe"
        set "PATH=%ProgramFiles%\Git\cmd;%PATH%"
        echo [OK] Git found at default installation path.
    ) else (
        echo [INFO] Git was not found.
        echo [INFO] Installing Git...
        echo.

        winget install ^
            --id Git.Git ^
            -e ^
            --source winget ^
            --accept-source-agreements ^
            --accept-package-agreements

        if errorlevel 1 (
            echo.
            echo [ERROR] Failed to install Git.
            echo.
            pause
            exit /b 1
        )

        if exist "%ProgramFiles%\Git\cmd\git.exe" (
            set "GIT_EXE=%ProgramFiles%\Git\cmd\git.exe"
            set "PATH=%ProgramFiles%\Git\cmd;%PATH%"
        ) else (
            where git >nul 2>&1

            if errorlevel 1 (
                echo [ERROR] Git was installed but could not be located.
                echo [ERROR] Restart Windows and run this script again.
                echo.
                pause
                exit /b 1
            )

            set "GIT_EXE=git"
        )

        echo.
        echo [OK] Git installed.
    )
) else (
    set "GIT_EXE=git"
    echo [OK] Git already installed.
)

echo.

:: =========================================================
:: 3. Install / Verify CMake
:: =========================================================

echo ---------------------------------------------------------
echo [4/9] CMake
echo ---------------------------------------------------------
echo.

echo [INFO] Checking CMake...

where cmake >nul 2>&1

if errorlevel 1 (
    if exist "%ProgramFiles%\CMake\bin\cmake.exe" (
        set "CMAKE_EXE=%ProgramFiles%\CMake\bin\cmake.exe"
        set "PATH=%ProgramFiles%\CMake\bin;%PATH%"
        echo [OK] CMake found at default installation path.
    ) else (
        echo [INFO] CMake was not found.
        echo [INFO] Installing CMake...
        echo.

        winget install ^
            --id Kitware.CMake ^
            -e ^
            --source winget ^
            --accept-source-agreements ^
            --accept-package-agreements

        if errorlevel 1 (
            echo.
            echo [ERROR] Failed to install CMake.
            echo.
            pause
            exit /b 1
        )

        if exist "%ProgramFiles%\CMake\bin\cmake.exe" (
            set "CMAKE_EXE=%ProgramFiles%\CMake\bin\cmake.exe"
            set "PATH=%ProgramFiles%\CMake\bin;%PATH%"
        ) else (
            where cmake >nul 2>&1

            if errorlevel 1 (
                echo [ERROR] CMake was installed but could not be located.
                echo [ERROR] Restart Windows and run this script again.
                echo.
                pause
                exit /b 1
            )

            set "CMAKE_EXE=cmake"
        )

        echo.
        echo [OK] CMake installed.
    )
) else (
    set "CMAKE_EXE=cmake"
    echo [OK] CMake already installed.
)

echo.

:: =========================================================
:: 4. Install / Verify Ninja
:: =========================================================

echo ---------------------------------------------------------
echo [5/9] Ninja
echo ---------------------------------------------------------
echo.

echo [INFO] Checking Ninja...

where ninja >nul 2>&1

if errorlevel 1 (
    if exist "%ProgramFiles%\Ninja\ninja.exe" (
        set "NINJA_EXE=%ProgramFiles%\Ninja\ninja.exe"
        set "PATH=%ProgramFiles%\Ninja;%PATH%"
        echo [OK] Ninja found at default installation path.
    ) else (
        echo [INFO] Ninja was not found.
        echo [INFO] Installing Ninja...
        echo.

        winget install ^
            --id Ninja-build.Ninja ^
            -e ^
            --source winget ^
            --accept-source-agreements ^
            --accept-package-agreements

        if errorlevel 1 (
            echo.
            echo [ERROR] Failed to install Ninja.
            echo.
            pause
            exit /b 1
        )

        if exist "%ProgramFiles%\Ninja\ninja.exe" (
            set "NINJA_EXE=%ProgramFiles%\Ninja\ninja.exe"
            set "PATH=%ProgramFiles%\Ninja;%PATH%"
        ) else (
            where ninja >nul 2>&1

            if errorlevel 1 (
                echo [ERROR] Ninja was installed but could not be located.
                echo [ERROR] Restart Windows and run this script again.
                echo.
                pause
                exit /b 1
            )

            set "NINJA_EXE=ninja"
        )

        echo.
        echo [OK] Ninja installed.
    )
) else (
    set "NINJA_EXE=ninja"
    echo [OK] Ninja already installed.
)

echo.

:: =========================================================
:: 5. Locate Visual Studio Installer / vswhere
:: =========================================================

echo ---------------------------------------------------------
echo [6/9] Visual Studio 2022
echo ---------------------------------------------------------
echo.

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS_PATH="
set "VS_TEMP=%TEMP%\oracyn_vs_path.txt"

echo [INFO] Looking for Visual Studio Installer...

if not exist "%VSWHERE%" (
    echo [INFO] vswhere.exe was not found.
    echo [INFO] Installing Visual Studio 2022 Build Tools...
    echo.

    winget install ^
        --id Microsoft.VisualStudio.2022.BuildTools ^
        -e ^
        --source winget ^
        --accept-source-agreements ^
        --accept-package-agreements ^
        --override "--quiet --wait --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"

    if errorlevel 1 (
        echo.
        echo [ERROR] Visual Studio 2022 installation failed.
        echo.
        pause
        exit /b 1
    )
)

if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe is still missing after Visual Studio installation.
    echo.
    echo Expected:
    echo     "%VSWHERE%"
    echo.
    pause
    exit /b 1
)

echo [OK] Visual Studio Installer located.
echo.

echo [INFO] Searching for Visual Studio 2022 with MSVC...

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

if not defined VS_PATH (
    echo.
    echo [ERROR] Visual Studio 2022 with MSVC C++ tools was not found.
    echo.
    pause
    exit /b 1
)

echo [OK] Visual Studio 2022 found:
echo      "%VS_PATH%"
echo.

set "VCVARS=%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"

if not exist "%VCVARS%" (
    echo [ERROR] vcvars64.bat was not found: "%VCVARS%"
    echo.
    pause
    exit /b 1
)

echo [INFO] Initializing MSVC environment...
call "%VCVARS%" >nul

if errorlevel 1 (
    echo [ERROR] Failed to initialize MSVC environment.
    echo.
    pause
    exit /b 1
)

echo [OK] MSVC compiler initialized.
echo.

:: =========================================================
:: 6. Locate / Install vcpkg
:: =========================================================

echo ---------------------------------------------------------
echo [7/9] vcpkg
echo ---------------------------------------------------------
echo.

set "VCPKG_ROOT=%LOCALAPPDATA%\vcpkg"

if not exist "%VCPKG_ROOT%\.git" (
    if exist "%VCPKG_ROOT%" (
        rmdir /s /q "%VCPKG_ROOT%"
    )

    echo [INFO] Cloning vcpkg...
    "%GIT_EXE%" clone https://github.com/microsoft/vcpkg.git "%VCPKG_ROOT%"

    if errorlevel 1 (
        echo [ERROR] Failed to clone vcpkg.
        pause
        exit /b 1
    )
) else (
    echo [OK] vcpkg repository already exists.
)

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [INFO] Bootstrapping vcpkg...
    call "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics

    if errorlevel 1 (
        echo [ERROR] vcpkg bootstrap failed.
        pause
        exit /b 1
    )
) else (
    echo [OK] vcpkg.exe already exists.
)

echo.

:: =========================================================
:: 7. Generate env_setup.bat
:: =========================================================

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
echo.

:: =========================================================
:: 8. Final Verification
:: =========================================================

echo ---------------------------------------------------------
echo [8/9] Verification
echo ---------------------------------------------------------
echo.

echo [INFO] Visual Studio: "%VS_PATH%"
echo [INFO] Git:
"%GIT_EXE%" --version
echo [INFO] CMake:
"%CMAKE_EXE%" --version
echo [INFO] Ninja:
"%NINJA_EXE%" --version
echo.

:: =========================================================
:: 9. Complete
:: =========================================================

echo ---------------------------------------------------------
echo [9/9] Setup complete
echo ---------------------------------------------------------
echo.
echo =========================================================
echo     ORACYN ENVIRONMENT READY
echo =========================================================
echo.
echo Run build.bat to configure and build Oracyn.
echo.

pause
exit /b 0
