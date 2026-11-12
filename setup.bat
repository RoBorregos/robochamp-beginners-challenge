@echo off
rem Installs PlatformIO (if missing) and all firmware dependencies.
where python >nul 2>nul
if errorlevel 1 (
    echo Python not found, installing with winget...
    winget install -e --id Python.Python.3.12 --accept-package-agreements --accept-source-agreements || (echo Failed to install Python & exit /b 1)
    echo Python installed. Open a NEW terminal and run setup.bat again so PATH updates.
    exit /b 0
)
where pio >nul 2>nul
if errorlevel 1 (
    echo PlatformIO not found, installing with pip...
    python -m pip install -U platformio || (echo Failed to install PlatformIO & exit /b 1)
)
cd /d "%~dp0firmware" || exit /b 1
pio pkg install || (echo Dependency install failed & exit /b 1)
echo Done.
