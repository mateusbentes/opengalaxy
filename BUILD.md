## Building OpenGalaxy

Complete build instructions for all platforms.

## 📋 Prerequisites

### All Platforms
- CMake 3.21 or later
- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- Qt 6.5 or later

### Linux

#### Ubuntu/Debian 22.04+
```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    ninja-build \
    qt6-base-dev \
    qt6-base-private-dev \
    qt6-webengine-dev \
    libqt6sql6-sqlite \
    libssl-dev \
    git
```

#### Fedora 36+
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    ninja-build \
    qt6-qtbase-devel \
    qt6-qtwebengine-devel \
    qt6-qtsql \
    openssl-devel \
    git
```

#### Arch Linux
```bash
sudo pacman -S --needed \
    base-devel \
    cmake \
    ninja \
    qt6-base \
    qt6-webengine \
    qt6-sql \
    openssl \
    git
```

### Windows

1. **Install Visual Studio 2022**
   - Download from https://visualstudio.microsoft.com/
   - Select "Desktop development with C++"
   - Include CMake tools

2. **Install Qt 6**
   - Download Qt Online Installer from https://www.qt.io/download
   - Install Qt 6.5+ with MSVC 2019 64-bit
   - Select components:
     - Qt WebEngine
     - Qt SQL SQLite driver

3. **Install CMake**
   - Download from https://cmake.org/download/
   - Add to PATH during installation

### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake ninja qt@6 openssl@3
```

## 🔨 Building

### Linux/macOS

```bash
# Clone repository
git clone https://github.com/yourusername/opengalaxy.git
cd opengalaxy

# Create build directory
mkdir build && cd build

# Configure (Release build)
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DBUILD_CLI=ON \
    -DBUILD_UI=ON \
    -GNinja

# Build
ninja -j$(nproc)

# Run tests
ctest --output-on-failure

# Install (optional)
sudo ninja install
```

### Windows (Visual Studio)

```powershell
# Clone repository
git clone https://github.com/yourusername/opengalaxy.git
cd opengalaxy

# Create build directory
mkdir build
cd build

# Configure (adjust Qt path)
cmake .. `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64" `
    -DBUILD_TESTS=ON `
    -G "Visual Studio 17 2022" `
    -A x64

# Build
cmake --build . --config Release -j

# Run tests
ctest -C Release --output-on-failure

# Install
cmake --install . --config Release
```

### Windows (MinGW)

```bash
# Configure
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/mingw_64" \
    -G "MinGW Makefiles"

# Build
mingw32-make -j8

# Run tests
ctest --output-on-failure
```

## 🐛 Debug Build

```bash
# Configure with debug symbols
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON

# Build
cmake --build . -j

# Run with debugger
gdb ./ui/opengalaxy
```

## 📊 Code Coverage (Linux only)

```bash
# Install coverage tools
sudo apt install lcov

# Configure with coverage
cmake .. \
    -DCMAKE_BUILD_TYPE=Coverage \
    -DBUILD_TESTS=ON

# Build and run tests
cmake --build . -j
ctest

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage

# View report
xdg-open coverage/index.html
```

## 📦 Packaging

### AppImage (Linux)

```bash
# Install linuxdeploy
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
chmod +x linuxdeploy*.AppImage

# Build release
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build . -j
DESTDIR=AppDir cmake --install .

# Create AppImage
./linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --plugin qt \
    --output appimage

# Result: OpenGalaxy-x86_64.AppImage
```

### Flatpak (Linux)

```bash
# Install flatpak-builder
sudo apt install flatpak-builder

# Build flatpak
flatpak-builder --force-clean build-dir org.opengalaxy.OpenGalaxy.yml

# Install locally
flatpak-builder --user --install --force-clean build-dir org.opengalaxy.OpenGalaxy.yml

# Run
flatpak run org.opengalaxy.OpenGalaxy
```

### Windows Installer (NSIS)

```powershell
# Install NSIS
# Download from https://nsis.sourceforge.io/

# Build release
cmake --build . --config Release

# Create installer
makensis installer.nsi

# Result: OpenGalaxy-Setup.exe
```

### macOS Bundle

```bash
# Build release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j

# Create bundle
macdeployqt ui/opengalaxy.app -dmg

# Result: opengalaxy.dmg
```

## 🔧 Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | ON | Build unit tests |
| `BUILD_CLI` | ON | Build CLI tool |
| `BUILD_UI` | ON | Build Qt GUI |
| `CMAKE_BUILD_TYPE` | Release | Build type (Debug/Release/Coverage) |
| `CMAKE_INSTALL_PREFIX` | /usr/local | Installation prefix |

### Example: Minimal Build (No UI)

```bash
cmake .. \
    -DBUILD_UI=OFF \
    -DBUILD_TESTS=OFF \
    -DCMAKE_BUILD_TYPE=Release

cmake --build . -j
```

## 🚨 Troubleshooting

### Qt not found

```bash
# Specify Qt path manually
cmake .. -DCMAKE_PREFIX_PATH="/path/to/Qt/6.5.0/gcc_64"
```

### Missing WebEngine

```bash
# Ubuntu/Debian
sudo apt install qt6-webengine-dev

# Fedora
sudo dnf install qt6-qtwebengine-devel

# Arch
sudo pacman -S qt6-webengine
```

### OpenSSL errors (Windows)

Download OpenSSL from https://slproweb.com/products/Win32OpenSSL.html and add to PATH.

### Linker errors

```bash
# Clear CMake cache
rm -rf build
mkdir build && cd build
cmake ..
```

## 📝 Development Tips

### Fast Incremental Builds

```bash
# Use Ninja (faster than Make)
cmake .. -GNinja
ninja

# Parallel builds
ninja -j$(nproc)
```

### IDE Integration

#### Visual Studio Code
```bash
# Install CMake Tools extension
# Open folder in VS Code
# Select kit and configure
```

#### CLion
```bash
# Open CMakeLists.txt as project
# CLion will auto-configure
```

#### Qt Creator
```bash
# Open CMakeLists.txt
# Select Qt kit
# Build
```

### Static Analysis

```bash
# clang-tidy
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
clang-tidy -p build src/**/*.cpp

# cppcheck
cppcheck --enable=all --project=build/compile_commands.json
```

## 🎯 Next Steps

After building:
1. Run tests: `ctest --output-on-failure`
2. Try CLI: `./cli/opengalaxy-cli --help`
3. Launch GUI: `./ui/opengalaxy`
4. Read [USAGE.md](USAGE.md) for usage instructions

## 💬 Need Help?

- Check [FAQ](FAQ.md)
- Open an issue: https://github.com/yourusername/opengalaxy/issues
- Join discussions: https://github.com/yourusername/opengalaxy/discussions
