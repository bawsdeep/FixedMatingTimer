#!/bin/bash

# Exit on error
set -e

echo "========================================="
echo "FixedMatingTimer Plugin Build Script"
echo "========================================="

# Configuration
BUILD_DIR="build"
ARK_API_PATH="${ARK_API_PATH:-$(pwd)/../AsaApi/AsaApi}"

# Check if AsaApi exists
if [ ! -d "$ARK_API_PATH" ]; then
    echo "Error: AsaApi not found at $ARK_API_PATH"
    echo "Your current structure should be:"
    echo "  projects/"
    echo "  ├── AsaApi/AsaApi/"
    echo "  └── FixedMatingTimer/"
    echo ""
    echo "If AsaApi is elsewhere, set ARK_API_PATH:"
    echo "  export ARK_API_PATH=/path/to/AsaApi/AsaApi"
    exit 1
fi

echo "Using AsaApi from: $ARK_API_PATH"

# Check MinGW installation
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "Error: MinGW-w64 not found"
    echo "Install it with: sudo pacman -S mingw-w64-gcc"
    exit 1
fi

echo "MinGW found: $(x86_64-w64-mingw32-g++ --version | head -n1)"

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake for cross-compilation
echo ""
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DARK_API_DIR="$ARK_API_PATH"

# Build
echo ""
echo "Building plugin..."
cmake --build . --config Release -j$(nproc)

# Check if build succeeded
echo ""
if [ -f "FixedMatingTimer.dll" ]; then
    echo "========================================="
    echo "✓ Build successful!"
    echo "========================================="
    echo "Output files:"
    ls -lh FixedMatingTimer.dll PluginInfo.json config.json
    echo ""
    echo "File size: $(du -h FixedMatingTimer.dll | cut -f1)"
    echo ""
    echo "To install on your server, copy these files to:"
    echo "ShooterGame/Binaries/Win64/ArkApi/Plugins/FixedMatingTimer/"
    echo ""
    echo "Files to copy:"
    echo "  - FixedMatingTimer.dll"
    echo "  - PluginInfo.json"
    echo "  - config.json"
else
    echo "========================================="
    echo "✗ Build failed - DLL not found"
    echo "========================================="
    exit 1
fi
