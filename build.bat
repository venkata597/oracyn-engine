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
:: 3. Verify Visual Studio & MSVC
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
:: 5. Verify Vulkan SDK & OpenGL Environment
:: =========================================================

echo.
echo [INFO] Verifying Graphics SDKs (Vulkan & OpenGL)...

if defined VULKAN_SDK (
    if exist "%VULKAN_SDK%\Include\vulkan\vulkan.h" (
        echo [OK] Vulkan SDK:
        echo      "%VULKAN_SDK%"
    ) else (
        echo [WARNING] VULKAN_SDK environment variable is set, but vulkan.h was not found.
    )
) else (
    echo [INFO] VULKAN_SDK is not explicitly set; CMake will attempt to locate system Vulkan drivers/headers.
)

where opengl32.lib >nul 2>&1
if errorlevel 1 (
    if exist "%WindowsSdkDir%Lib\%WindowsSDKLibVersion%um\x64\opengl32.lib" (
        echo [OK] OpenGL 32-bit/64-bit import library located in Windows SDK.
    ) else (
        echo [INFO] OpenGL library will be resolved via MSVC / Windows SDK defaults.
    )
) else (
    echo [OK] OpenGL library found in PATH/SDK.
)

:: =========================================================
:: 6. Verify Ninja
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
:: 7. Clean build if requested
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
:: 8. Configure with CMake
:: =========================================================

echo.
echo =========================================================
echo Configuring Oracyn (OpenGL + Vulkan)
echo =========================================================
echo.

set "CMAKE_VULKAN_ARG="
if defined VULKAN_SDK (
    set "CMAKE_VULKAN_ARG=-DVULKAN_SDK=""%VULKAN_SDK%"""
)

cmake ^
    -S . ^
    -B build ^
    -G Ninja ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-windows ^
    %CMAKE_VULKAN_ARG%

if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

:: =========================================================
:: 9. Build
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
:: 10. Success
:: =========================================================

echo.
echo =========================================================
echo [SUCCESS] Oracyn built successfully.
echo =========================================================
echo.

pause
exit /b 0
