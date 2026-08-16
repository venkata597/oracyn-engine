@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

echo =========================================================
echo Oracyn - Cleanup
echo =========================================================
echo.
echo This utility cleans files generated or modified by
echo the Oracyn build environment.
echo.
echo It will NEVER uninstall software without asking you.
echo.
echo =========================================================
echo.

:: =========================================================
:: 1. Clean project-generated files
:: =========================================================

echo =========================================================
echo Project Cleanup
echo =========================================================
echo.

echo The following project files/directories can be safely
echo removed:
echo.
echo   build\
echo   env_setup.bat
echo   vcpkg-configuration.json
echo   CMakeCache.txt
echo   CMakeFiles\
echo   cmake_install.cmake
echo   compile_commands.json
echo.

choice /C YN /N /M "Remove project-generated files? [Y/N]: "

if errorlevel 2 (
    echo [SKIP] Project files were not removed.
    goto :system_tools
)

echo.
echo [INFO] Cleaning project files...

:: build/
if exist "%~dp0build" (
    rmdir /s /q "%~dp0build"

    if exist "%~dp0build" (
        echo [ERROR] Failed to remove build\
    ) else (
        echo [OK] Removed build\
    )
)

:: env_setup.bat
if exist "%~dp0env_setup.bat" (
    del /f /q "%~dp0env_setup.bat"

    if exist "%~dp0env_setup.bat" (
        echo [ERROR] Failed to remove env_setup.bat
    ) else (
        echo [OK] Removed env_setup.bat
    )
)

:: vcpkg-configuration.json
if exist "%~dp0vcpkg-configuration.json" (
    del /f /q "%~dp0vcpkg-configuration.json"

    if exist "%~dp0vcpkg-configuration.json" (
        echo [ERROR] Failed to remove vcpkg-configuration.json
    ) else (
        echo [OK] Removed vcpkg-configuration.json
    )
)

:: CMakeCache.txt
if exist "%~dp0CMakeCache.txt" (
    del /f /q "%~dp0CMakeCache.txt"
    echo [OK] Removed CMakeCache.txt
)

:: CMakeFiles/
if exist "%~dp0CMakeFiles" (
    rmdir /s /q "%~dp0CMakeFiles"
    echo [OK] Removed CMakeFiles\
)

:: cmake_install.cmake
if exist "%~dp0cmake_install.cmake" (
    del /f /q "%~dp0cmake_install.cmake"
    echo [OK] Removed cmake_install.cmake
)

:: compile_commands.json
if exist "%~dp0compile_commands.json" (
    del /f /q "%~dp0compile_commands.json"
    echo [OK] Removed compile_commands.json
)

echo.
echo [OK] Project cleanup complete.

:: =========================================================
:: 2. vcpkg
:: =========================================================

:system_tools

echo.
echo =========================================================
echo vcpkg
echo =========================================================
echo.

set "VCPKG_ROOT=%LOCALAPPDATA%\vcpkg"

if exist "%VCPKG_ROOT%" (

    echo vcpkg is currently installed at:
    echo.
    echo   "%VCPKG_ROOT%"
    echo.
    echo WARNING:
    echo This vcpkg installation may be useful to other projects.
    echo.
    echo Removing it will require vcpkg to be downloaded again
    echo the next time another project needs it.
    echo.

    choice /C YN /N /M "Remove the local vcpkg installation? [Y/N]: "

    if errorlevel 2 (
        echo [KEEP] vcpkg was kept.
    ) else (
        echo.
        echo [INFO] Removing vcpkg...

        rmdir /s /q "%VCPKG_ROOT%"

        if exist "%VCPKG_ROOT%" (
            echo [ERROR] Failed to completely remove vcpkg.
        ) else (
            echo [OK] vcpkg removed.
        )
    )

) else (
    echo [INFO] No local vcpkg installation found.
)

:: =========================================================
:: 3. Persistent environment variables
:: =========================================================

echo.
echo =========================================================
echo Environment Variables
echo =========================================================
echo.

echo The following user environment variables may have been
echo created by older versions of Oracyn's setup script:
echo.
echo   VCPKG_ROOT
echo   VCPKG_DISABLE_METRICS
echo   ORACYN_VS_PATH
echo.

choice /C YN /N /M "Remove these user environment variables? [Y/N]: "

if errorlevel 2 (
    echo [KEEP] Environment variables were kept.
) else (

    echo.
    echo [INFO] Removing old Oracyn environment variables...

    reg delete "HKCU\Environment" /v VCPKG_ROOT /f >nul 2>&1

    if errorlevel 1 (
        echo [SKIP] VCPKG_ROOT not found.
    ) else (
        echo [OK] Removed VCPKG_ROOT.
    )

    reg delete "HKCU\Environment" /v VCPKG_DISABLE_METRICS /f >nul 2>&1

    if errorlevel 1 (
        echo [SKIP] VCPKG_DISABLE_METRICS not found.
    ) else (
        echo [OK] Removed VCPKG_DISABLE_METRICS.
    )

    reg delete "HKCU\Environment" /v ORACYN_VS_PATH /f >nul 2>&1

    if errorlevel 1 (
        echo [SKIP] ORACYN_VS_PATH not found.
    ) else (
        echo [OK] Removed ORACYN_VS_PATH.
    )

    echo.
    echo [INFO] Environment cleanup complete.
    echo [INFO] Open terminals will retain their old environment.
    echo [INFO] New terminals will use the updated environment.
)

:: =========================================================
:: 4. Git
:: =========================================================

echo.
echo =========================================================
echo Git
echo =========================================================
echo.

where git >nul 2>&1

if errorlevel 1 (
    echo [INFO] Git is not installed.
) else (
    echo Git is installed and available in PATH.
    echo.
    echo WARNING:
    echo Git is commonly used by other development projects.
    echo.

    choice /C YN /N /M "Uninstall Git? [Y/N]: "

    if errorlevel 2 (
        echo [KEEP] Git was kept.
    ) else (
        echo.
        echo [INFO] Uninstalling Git...

        winget uninstall --id Git.Git -e

        if errorlevel 1 (
            echo [ERROR] Failed to uninstall Git.
        ) else (
            echo [OK] Git uninstalled.
        )
    )
)

:: =========================================================
:: 5. CMake
:: =========================================================

echo.
echo =========================================================
echo CMake
echo =========================================================
echo.

where cmake >nul 2>&1

if errorlevel 1 (
    echo [INFO] CMake is not installed.
) else (
    echo CMake is installed and available in PATH.
    echo.
    echo WARNING:
    echo CMake is commonly used by other development projects.
    echo.

    choice /C YN /N /M "Uninstall CMake? [Y/N]: "

    if errorlevel 2 (
        echo [KEEP] CMake was kept.
    ) else (
        echo.
        echo [INFO] Uninstalling CMake...

        winget uninstall --id Kitware.CMake -e

        if errorlevel 1 (
            echo [ERROR] Failed to uninstall CMake.
        ) else (
            echo [OK] CMake uninstalled.
        )
    )
)

:: =========================================================
:: 6. Ninja
:: =========================================================

echo.
echo =========================================================
echo Ninja
echo =========================================================
echo.

where ninja >nul 2>&1

if errorlevel 1 (
    echo [INFO] Ninja is not installed.
) else (
    echo Ninja is installed and available in PATH.
    echo.
    echo WARNING:
    echo Ninja is commonly used by other development projects.
    echo.

    choice /C YN /N /M "Uninstall Ninja? [Y/N]: "

    if errorlevel 2 (
        echo [KEEP] Ninja was kept.
    ) else (
        echo.
        echo [INFO] Uninstalling Ninja...

        winget uninstall --id Ninja-build.Ninja -e

        if errorlevel 1 (
            echo [ERROR] Failed to uninstall Ninja.
        ) else (
            echo [OK] Ninja uninstalled.
        )
    )
)
:: =========================================================
:: 7. Visual Studio
:: =========================================================

echo.
echo =========================================================
echo Visual Studio
echo =========================================================
echo.

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo [INFO] Visual Studio Installer not found.
    goto :finished
)

set "VS_PATH="
set "VS_TEMP=%TEMP%\oracyn_cleanup_vs.txt"

"%VSWHERE%" ^
    -products * ^
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
    -property installationPath ^
    > "%VS_TEMP%"

if exist "%VS_TEMP%" (
    set /p VS_PATH=<"%VS_TEMP%"
    del /q "%VS_TEMP%" >nul 2>&1
)

if not defined VS_PATH (
    echo [INFO] No Visual Studio installation with MSVC was found.
    goto :finished
)

echo Visual Studio with MSVC was found at:
echo.
echo   "%VS_PATH%"
echo.
echo WARNING:
echo Visual Studio is a large development environment and may
echo be required by many other C++ projects.
echo.

choice /C YN /N /M "Uninstall Visual Studio Build Tools? [Y/N]: "

if errorlevel 2 (
    echo [KEEP] Visual Studio was kept.
) else (
    echo.
    echo [INFO] Uninstalling Visual Studio Build Tools...
    echo.

    winget uninstall --id Microsoft.VisualStudio.2022.BuildTools -e

    if errorlevel 1 (
        echo [ERROR] Failed to uninstall Visual Studio Build Tools.
        echo You may need to use Visual Studio Installer manually.
    ) else (
        echo [OK] Visual Studio Build Tools uninstalled.
    )
)

:: =========================================================
:: 8. Finished
:: =========================================================

:finished

echo.
echo =========================================================
echo Cleanup Complete
echo =========================================================
echo.

echo Project-generated files have been processed.
echo.
echo Any system-wide tools you chose to keep remain installed.
echo.
echo If you removed environment variables, restart your terminal
echo before checking them again.
echo.

echo =========================================================
echo Oracyn cleanup finished.
echo =========================================================
echo.

pause
exit /b 0
