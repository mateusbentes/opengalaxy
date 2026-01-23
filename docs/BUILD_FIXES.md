# Build Fixes and Common Issues

## Translation Files Build Error

### Problem

Build fails with error:
```
gmake[2]: *** No rule to make target '.../opengalaxy_en_US.qm', needed by 'ui/opengalaxy_autogen/.../qrc_resources.cpp'. Stop.
```

### Root Cause

The `resources.qrc` file references compiled translation files (`.qm`) that don't exist when Qt Linguist tools are not installed or not found.

### Solution (Implemented)

The build system now automatically handles missing Qt Linguist tools:

1. **With Qt Linguist tools**: Uses full resources including translations
2. **Without Qt Linguist tools**: Creates a minimal resources file without translations

**Changes in `ui/CMakeLists.txt`**:
```cmake
if(Qt6LinguistTools_FOUND)
    # Full build with translations
    qt_add_lupdate(opengalaxy SOURCES ${UI_SOURCES} ${UI_HEADERS} TS_FILES ${TS_FILES})
    qt_add_lrelease(opengalaxy TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES)
    set(UI_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/qt/resources/resources.qrc)
else()
    # Build without translations - create minimal resources file
    set(RESOURCES_NO_I18N ${CMAKE_CURRENT_BINARY_DIR}/resources_no_i18n.qrc)
    file(WRITE ${RESOURCES_NO_I18N} "...")
    set(UI_RESOURCES ${RESOURCES_NO_I18N})
endif()
```

### Installing Qt Linguist Tools (Optional)

If you want translation support:

**Ubuntu/Debian**:
```bash
sudo apt install qt6-l10n-tools
```

**Fedora**:
```bash
sudo dnf install qt6-linguist
```

**Arch Linux**:
```bash
sudo pacman -S qt6-tools
```

**macOS**:
```bash
brew install qt6
# Linguist tools included
```

**Windows**:
- Install Qt6 with "Qt Linguist" component via Qt Online Installer

### Verification

After the fix, the build should work with or without Qt Linguist tools:

```bash
cd build
cmake ..
cmake --build . -j$(nproc)
```

**Expected output**:
- With tools: `Qt Linguist tools found - translations enabled`
- Without tools: `Qt Linguist tools not found - translations disabled`

Both cases should build successfully.

## Other Common Build Issues

### Qt6 Not Found

**Error**:
```
CMake Error: Could not find a package configuration file provided by "Qt6"
```

**Solution**:
```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-webengine-dev

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel

# Arch
sudo pacman -S qt6-base qt6-webengine

# Or specify Qt path manually
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64
```

### C++20 Compiler Required

**Error**:
```
error: #error This file requires compiler and library support for the ISO C++ 2020 standard
```

**Solution**:
```bash
# Update compiler
# Ubuntu/Debian
sudo apt install g++-10

# Fedora
sudo dnf install gcc-c++

# Check version
g++ --version  # Should be 10+ for GCC, 12+ for Clang
```

### OpenSSL Not Found

**Error**:
```
Could not find OpenSSL
```

**Solution**:
```bash
# Ubuntu/Debian
sudo apt install libssl-dev

# Fedora
sudo dnf install openssl-devel

# Arch
sudo pacman -S openssl
```

### WebEngine Not Found (Optional)

**Warning**:
```
Qt WebEngine not found - OAuth web login disabled
```

**Solution** (optional, only needed for OAuth login):
```bash
# Ubuntu/Debian
sudo apt install qt6-webengine-dev

# Fedora
sudo dnf install qt6-qtwebengine-devel

# Arch
sudo pacman -S qt6-webengine
```

**Note**: OAuth login will be disabled but password login still works.

### CMake Version Too Old

**Error**:
```
CMake 3.21 or higher is required
```

**Solution**:
```bash
# Ubuntu/Debian (add Kitware repository)
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt update
sudo apt install cmake

# Or download from cmake.org
wget https://github.com/Kitware/CMake/releases/download/v3.28.0/cmake-3.28.0-linux-x86_64.sh
sudo sh cmake-3.28.0-linux-x86_64.sh --prefix=/usr/local --skip-license
```

### Ninja Build System Not Found

**Error**:
```
CMake Error: CMake was unable to find a build program corresponding to "Ninja"
```

**Solution**:
```bash
# Ubuntu/Debian
sudo apt install ninja-build

# Fedora
sudo dnf install ninja-build

# Arch
sudo pacman -S ninja

# Or use Make instead
cmake .. -G "Unix Makefiles"
```

## Platform-Specific Issues

### Linux: Missing X11 Development Files

**Error**:
```
Could not find X11
```

**Solution**:
```bash
sudo apt install libx11-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev
```

### macOS: Code Signing Issues

**Error**:
```
Code signing failed
```

**Solution**:
```bash
# For development, disable code signing
cmake .. -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY=""

# Or sign with your certificate
codesign -s "Your Developer ID" opengalaxy.app
```

### Windows: MSVC Not Found

**Error**:
```
No CMAKE_CXX_COMPILER could be found
```

**Solution**:
- Install Visual Studio 2019 or later with "Desktop development with C++"
- Or use MinGW: `cmake .. -G "MinGW Makefiles"`

## Build Configuration Options

### Minimal Build (No UI)

```bash
cmake .. -DBUILD_UI=OFF -DBUILD_TESTS=OFF
cmake --build . -j$(nproc)
```

### Debug Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j$(nproc)
```

### Release Build with Optimizations

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"
cmake --build . -j$(nproc)
```

### Build with Coverage

```bash
cmake .. -DCMAKE_BUILD_TYPE=Coverage -DBUILD_TESTS=ON
cmake --build . -j$(nproc)
ctest
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage
```

## Clean Build

If you encounter persistent build issues:

```bash
# Remove build directory
rm -rf build

# Clean CMake cache
rm -rf CMakeCache.txt CMakeFiles/

# Rebuild from scratch
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```

## Verbose Build Output

To see detailed build commands:

```bash
cmake --build . -j$(nproc) --verbose
```

Or with Make:
```bash
make VERBOSE=1
```

## Parallel Build

Speed up compilation:

```bash
# Use all CPU cores
cmake --build . -j$(nproc)

# Or specify number of jobs
cmake --build . -j8
```

## Getting Help

If you encounter build issues not covered here:

1. **Check logs**: Look for specific error messages
2. **Search issues**: https://github.com/mateusbentes/opengalaxy/issues
3. **Ask for help**: Open a new issue with:
   - Operating system and version
   - Compiler version (`g++ --version`)
   - CMake version (`cmake --version`)
   - Qt version (`qmake --version`)
   - Full error message
   - Build command used

## See Also

- [BUILD.md](BUILD.md) - Complete build instructions
- [MULTI_ARCH_SUPPORT.md](MULTI_ARCH_SUPPORT.md) - Building for different architectures
- [CONTRIBUTING.md](../CONTRIBUTING.md) - Development setup

---

**Last Updated**: 2026-01-23
