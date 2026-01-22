#!/bin/bash
# SPDX-License-Identifier: Apache-2.0
# OpenGalaxy build script

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}OpenGalaxy Build Script${NC}"
echo "========================"
echo ""

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    OS="Windows"
else
    OS="Unknown"
fi

echo "Detected OS: $OS"
echo ""

# Check dependencies
echo "Checking dependencies..."

if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake not found${NC}"
    echo "Please install CMake 3.21 or later"
    exit 1
fi

CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
echo "  ✓ CMake $CMAKE_VERSION"

if ! command -v qmake6 &> /dev/null && ! command -v qmake &> /dev/null; then
    echo -e "${YELLOW}Warning: Qt6 qmake not found in PATH${NC}"
    echo "  You may need to specify CMAKE_PREFIX_PATH"
fi

# Build type
BUILD_TYPE=${1:-Release}
echo ""
echo "Build type: $BUILD_TYPE"

# Create build directory
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Build directory exists. Cleaning...${NC}"
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo ""
echo "Configuring..."
cmake .. \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DBUILD_TESTS=ON \
    -DBUILD_CLI=ON \
    -DBUILD_UI=ON \
    -GNinja 2>/dev/null || cmake .. \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DBUILD_TESTS=ON \
    -DBUILD_CLI=ON \
    -DBUILD_UI=ON

# Build
echo ""
echo "Building..."
CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build . -j$CORES

# Test
if [ "$BUILD_TYPE" != "Release" ]; then
    echo ""
    echo "Running tests..."
    ctest --output-on-failure || echo -e "${YELLOW}Some tests failed${NC}"
fi

# Summary
echo ""
echo -e "${GREEN}Build complete!${NC}"
echo ""
echo "Binaries:"
echo "  GUI: $BUILD_DIR/ui/opengalaxy"
echo "  CLI: $BUILD_DIR/cli/opengalaxy-cli"
echo ""
echo "To install:"
echo "  sudo cmake --install $BUILD_DIR"
echo ""
echo "To run:"
echo "  $BUILD_DIR/ui/opengalaxy"
echo ""
