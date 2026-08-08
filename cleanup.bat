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

echo =========================================================
echo  This will remove everything pre_build.bat installed:
echo    - C:\vcpkg (and all cached/installed packages)
echo    - Git, CMake, Ninja, PowerShell 7 (via winget)
echo    - Visual Studio Build Tools (via winget)
echo    - VCPKG_ROOT / VCPKG_DISABLE_METRICS environment variables
echo    - This project's local build\ folder
echo  Each step will ask for confirmation before doing anything.
echo =========================================================
echo.
pause

:: =========================================================
:: 1. Remove local build\ folder (project root, this script's dir)
:: =========================================================
if exist "build" (
    set /p "CONFIRM=Delete .\build\ folder? [y/N] "
    if /i "!CONFIRM!"=="y" (
        rmdir /s /q "build"
        echo [INFO] build\ folder removed.
    ) else (
        echo [INFO] Skipped build\ folder.
    )
) else (
    echo [INFO] No build\ folder found - skipping.
)

:: =========================================================
:: 2. Remove vcpkg (C:\vcpkg)
:: =========================================================
if exist "C:\vcpkg" (
    set /p "CONFIRM=Delete C:\vcpkg entirely? This removes ALL cached vcpkg packages, not just this project's. [y/N] "
    if /i "!CONFIRM!"=="y" (
        rmdir /s /q "C:\vcpkg"
        echo [INFO] C:\vcpkg removed.
    ) else (
        echo [INFO] Skipped C:\vcpkg.
    )
) else (
    echo [INFO] C:\vcpkg not found - skipping.
)

:: =========================================================
:: 3. Uninstall winget-installed tools
:: =========================================================
echo.
echo [INFO] The following steps uninstall tools via winget.
echo [INFO] Skip any of these if you use them for other projects.
echo.

where git >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set /p "CONFIRM=Uninstall Git? [y/N] "
    if /i "!CONFIRM!"=="y" (
        winget uninstall --id Git.Git -e --source winget
    )
)

where cmake >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set /p "CONFIRM=Uninstall CMake? [y/N] "
    if /i "!CONFIRM!"=="y" (
        winget uninstall --id Kitware.CMake -e --source winget
    )
)

where ninja >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set /p "CONFIRM=Uninstall Ninja? [y/N] "
    if /i "!CONFIRM!"=="y" (
        winget uninstall --id Ninja-build.Ninja -e --source winget
    )
)

where pwsh >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set /p "CONFIRM=Uninstall PowerShell 7? [y/N] "
    if /i "!CONFIRM!"=="y" (
        winget uninstall --id Microsoft.PowerShell -e --source winget
        winget uninstall --id 9MZ1SNWT0N5D --source msstore >nul 2>&1
    )
)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_PATH=%%i"
    )
)
if defined VS_PATH (
    echo [INFO] Found Visual Studio Build Tools at: !VS_PATH!
    set /p "CONFIRM=Uninstall Visual Studio Build Tools? This is a large, slow operation. [y/N] "
    if /i "!CONFIRM!"=="y" (
        winget uninstall --id Microsoft.VisualStudio.2022.BuildTools -e --source winget
    )
)

:: =========================================================
:: 4. Remove persisted environment variables
:: =========================================================
set /p "CONFIRM=Remove VCPKG_ROOT and VCPKG_DISABLE_METRICS environment variables? [y/N] "
if /i "!CONFIRM!"=="y" (
    reg delete "HKCU\Environment" /F /V "VCPKG_ROOT" >nul 2>&1
    reg delete "HKCU\Environment" /F /V "VCPKG_DISABLE_METRICS" >nul 2>&1
    echo [INFO] Environment variables removed.
    echo [INFO] You may need to sign out/in for this to fully take effect everywhere.
) else (
    echo [INFO] Skipped environment variable cleanup.
)

echo.
echo [SUCCESS] Cleanup complete.
echo [INFO] Note: this does not revert any changes cleanup.bat's own
echo [INFO] winget uninstalls make to shared system state (e.g. other
echo [INFO] projects relying on the same Git/CMake/Ninja install).
pause
