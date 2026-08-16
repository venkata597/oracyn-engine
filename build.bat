@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

echo =========================================================
echo Oracyn - Build
echo =========================================================
echo.

:: =========================================================
:: 1. Load environment
:: =========================================================

if not exist "%~dp0env_setup.bat" (
    echo [ERROR] env_setup.bat not found.
    echo Run pre_build.bat first.
    pause
    exit /b 1
)

call "%~dp0env_setup.bat"

if not defined VCPKG_ROOT (
    echo [ERROR] VCPKG_ROOT is not defined.
    echo Run pre_build.bat first.
    pause
    exit /b 1
)

if not defined ORACYN_VS_PATH (
    echo [ERROR] ORACYN_VS_PATH is not defined.
    echo Run pre_build.bat first.
    pause
    exit /b 1
)

:: =========================================================
:: 2. Verify vcpkg
:: =========================================================

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [ERROR] vcpkg.exe not found:
    echo "%VCPKG_ROOT%\vcpkg.exe"
    echo.
    echo Run pre_build.bat first.
    pause
    exit /b 1
)

echo [OK] vcpkg:
echo      "%VCPKG_ROOT%\vcpkg.exe"

:: =========================================================
:: 3. Verify Visual Studio
:: =========================================================

set "VS_PATH=%ORACYN_VS_PATH%"
set "VCVARS=%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"

echo.
echo [INFO] Visual Studio:
echo        "%VS_PATH%"

if not exist "%VCVARS%" (
    echo [ERROR] vcvars64.bat was not found:
    echo.
    echo "%VCVARS%"
    echo.
    echo Run pre_build.bat again.
    pause
    exit /b 1
)

echo [OK] MSVC environment:
echo      "%VCVARS%"

:: =========================================================
:: 4. Initialize MSVC
:: =========================================================

echo.
echo [INFO] Initializing MSVC environment...

call "%VCVARS%"

if errorlevel 1 (
    echo [ERROR] Failed to initialize MSVC.
    pause
    exit /b 1
)

where cl.exe >nul 2>&1

if errorlevel 1 (
    echo [ERROR] cl.exe was not found after initializing MSVC.
    pause
    exit /b 1
)

echo [OK] MSVC compiler:
where cl.exe

:: =========================================================
:: 5. Verify Ninja
:: =========================================================

echo.
echo [INFO] Checking Ninja...

where ninja.exe >nul 2>&1

if errorlevel 1 (
    echo [ERROR] Ninja was not found.
    echo Run pre_build.bat first.
    pause
    exit /b 1
)

echo [OK] Ninja:
where ninja.exe

:: =========================================================
:: 6. Clean build if requested
:: =========================================================

if /i "%~1"=="clean" (
    echo.
    echo [INFO] Removing build directory...

    if exist "build" (
        rmdir /s /q "build"
    )

    if exist "build" (
        echo [ERROR] Failed to remove build directory.
        pause
        exit /b 1
    )

    echo [OK] Build directory removed.
)

:: =========================================================
:: 7. Configure with CMake
:: =========================================================

echo.
echo =========================================================
echo Configuring Oracyn
echo =========================================================
echo.

cmake ^
    -S . ^
    -B build ^
    -G Ninja ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-windows

if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

:: =========================================================
:: 8. Build
:: =========================================================

echo.
echo =========================================================
echo Building Oracyn
echo =========================================================
echo.

cmake --build build --parallel

if errorlevel 1 (
    echo.
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

:: =========================================================
:: 9. Success
:: =========================================================

echo.
echo =========================================================
echo [SUCCESS] Oracyn built successfully.
echo =========================================================
echo.

pause
exit /b 0
