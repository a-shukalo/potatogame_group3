@echo off
REM =============================================================================
REM Modular Architecture Build Script for Windows
REM =============================================================================

echo =============================================================================
echo Building Brotato Game - Modular Architecture
echo =============================================================================

REM Clean up any existing build directory
if exist build (
    echo Cleaning existing build directory...
    rmdir /s /q build
)

REM Create build directory
echo Creating build directory...
mkdir build
cd build

REM Configure with CMake using vcpkg
echo Configuring with CMake...
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug

if %ERRORLEVEL% neq 0 (
    echo ❌ CMake configuration failed!
    pause
    exit /b 1
)

REM Build the project
echo Building the project...
cmake --build . --config Debug

if %ERRORLEVEL% neq 0 (
    echo ❌ Build failed!
    pause
    exit /b 1
)

echo =============================================================================
echo ✅ Build completed successfully!
echo =============================================================================
echo.
echo You can now run the game:
echo   cd build
echo   Debug\BrotatoGame.exe
echo.
echo Or build individual modules:
echo   cmake --build . --target BrotatoCore
echo   cmake --build . --target BrotatoRendering
echo   cmake --build . --target BrotatoGameplay
echo   cmake --build . --target BrotatoInput
echo   cmake --build . --target BrotatoContent
echo.
pause
