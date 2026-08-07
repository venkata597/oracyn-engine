@echo off

set "VCPKG_ROOT=C:\vcpkg"

:: Remove problem configuration if present
if exist "vcpkg-configuration.json" del /f /q "vcpkg-configuration.json"

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [ERROR] vcpkg not found at %VCPKG_ROOT%. Run pre_build.bat first.
    pause
    exit /b 1
)

:: Locate Visual Studio
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS_PATH="
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_PATH=%%i"
    )
)

if not defined VS_PATH (
    echo [ERROR] No Visual Studio C++ workload found. Run pre_build.bat first.
    pause
    exit /b 1
)

:: Initialize MSVC environment
call "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" x64

:: Locate Ninja executable
set "NINJA_EXE="
where ninja >nul 2>&1
if %ERRORLEVEL% equ 0 (
    for /f "tokens=*" %%i in ('where ninja') do set "NINJA_EXE=%%i"
) else if exist "%VS_PATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe" (
    set "NINJA_EXE=%VS_PATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
)

if not defined NINJA_EXE (
    echo [ERROR] Ninja executable not found.
    pause
    exit /b 1
)

:: Clean prior failed build state
if exist "build" rmdir /s /q "build"

:: Configure
cmake -B build -S . -G "Ninja" ^
  -DCMAKE_MAKE_PROGRAM="%NINJA_EXE%" ^
  -DCMAKE_BUILD_TYPE=Debug ^
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
  -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
  -DVCPKG_TARGET_TRIPLET=x64-windows

if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configure failed.
    pause
    exit /b %ERRORLEVEL%
)

:: Build
cmake --build build
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed.
    pause
    exit /b %ERRORLEVEL%
)

echo [SUCCESS] Build completed successfully.
pause
