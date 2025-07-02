@echo off
setlocal enabledelayedexpansion

echo.
echo ================================
echo      Customizable Logger
echo ================================
echo.

:: --- Step 0: Check for CMake ---
where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] CMake is not found in your system PATH.
    echo Please install CMake and ensure it is added to the PATH.
    echo You can download it from: https://cmake.org/download/
    pause
    exit /b 1
)

:: --- Step 1: Clean previous Build ---
echo [INFO] Cleaning previous Build...
if exist Build (
    rmdir /s /q Build
)
mkdir Build
cd Build

:: --- Step 2: Run CMake configuration ---
echo [INFO] Running CMake configuration...
cmake .. || (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

:: --- Step 3: Build the project ---
echo [INFO] Building the project...
cmake --build . || (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

:: --- Step 4: Locate the executable ---
echo [INFO] Locating the compiled executable...

:: Try common paths
set EXE_PATH=""
if exist "bin\Customizable_Logger.exe" (
    set EXE_PATH=bin\Customizable_Logger.exe
) else if exist "bin\Debug\Customizable_Logger.exe" (
    set EXE_PATH=bin\Debug\Customizable_Logger.exe
) else if exist "Debug\Customizable_Logger.exe" (
    set EXE_PATH=Debug\Customizable_Logger.exe
) else if exist "Customizable_Logger.exe" (
    set EXE_PATH=Customizable_Logger.exe
)

:: If executable found, copy it to bin/
if not "!EXE_PATH!"=="" (
    echo [INFO] Found executable at: !EXE_PATH!
    if not exist ..\bin (
        mkdir ..\bin
    )
    copy /Y "!EXE_PATH!" "..\bin\Customizable_Logger.exe" >nul
    echo [INFO] Copied executable to: ..\bin\Customizable_Logger.exe
) else (
    echo [ERROR] Executable not found after build.
    pause
    exit /b 1
)

cd ..

:: --- Step 5: Run the logger ---
echo.
echo ================================
echo      Running Logger Output
echo ================================
echo.

.\bin\Customizable_Logger.exe

echo.
echo ================================
echo        Execution Complete
echo ================================
echo.
pause
exit /b 0
