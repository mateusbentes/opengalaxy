# CI/CD Troubleshooting Guide

This guide helps you diagnose and fix common CI/CD pipeline issues.

## Table of Contents

1. [Build Failures](#build-failures)
2. [Code Quality Check Failures](#code-quality-check-failures)
3. [Test Failures](#test-failures)
4. [Platform-Specific Issues](#platform-specific-issues)
5. [Local Development Issues](#local-development-issues)

## Build Failures

### CMake Configuration Fails

**Error**: `CMake Error at CMakeLists.txt:XX`

**Diagnosis**:
1. Check the error message for the specific issue
2. Verify all dependencies are installed
3. Check CMakeLists.txt syntax

**Solutions**:

**Missing Qt6**
```bash
# Ubuntu/Debian
sudo apt-get install qt6-base-dev qt6-tools-dev

# macOS
brew install qt6

# Windows
# Use Visual Studio installer or download from qt.io
```

**Missing CMake**
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
# Download from cmake.org
```

**Invalid CMakeLists.txt**
```bash
# Check syntax
cmake --debug-output 2>&1 | head -50

# Look for specific errors
grep -n "ERROR" CMakeLists.txt
```

### Build Compilation Fails

**Error**: `error: undefined reference to 'symbol'`

**Diagnosis**:
1. Check if all source files are listed in CMakeLists.txt
2. Verify linking order
3. Check for missing libraries

**Solutions**:

**Missing source file**
```cmake
# In CMakeLists.txt, ensure all .cpp files are listed
set(SOURCES
    src/file1.cpp
    src/file2.cpp
    # Add missing files here
)
```

**Missing library link**
```cmake
# Ensure all libraries are linked
target_link_libraries(target_name
    PRIVATE
        library1
        library2
)
```

**Circular dependency**
```bash
# Check for circular includes
grep -r "#include" src/ | sort | uniq -d
```

### Linker Errors

**Error**: `undefined reference to 'function'`

**Diagnosis**:
1. Function is declared but not defined
2. Function is in a library that's not linked
3. Function has wrong visibility (static, private)

**Solutions**:

**Check function definition**
```bash
# Search for function definition
grep -r "void function()" src/

# If not found, implement it
```

**Check library linking**
```cmake
# Verify library is linked
target_link_libraries(target PRIVATE library_name)
```

**Check visibility**
```cpp
// Bad - function is static (internal linkage)
static void function() { }

// Good - function is visible
void function() { }
```

## Code Quality Check Failures

### Clang-Format Failures

**Error**: `error: code is not formatted`

**Diagnosis**:
1. Code doesn't match `.clang-format` style
2. Line length exceeds 100 characters
3. Indentation is inconsistent

**Solutions**:

**Auto-format files**
```bash
# Format all files
find . -name "*.cpp" -o -name "*.h" | grep -v build | xargs clang-format -i

# Format specific file
clang-format -i src/file.cpp
```

**Check what needs fixing**
```bash
# See differences without modifying
clang-format --dry-run -Werror src/file.cpp

# Show diff
clang-format src/file.cpp | diff -u src/file.cpp -
```

**Common formatting issues**:

| Issue | Fix |
|-------|-----|
| Line too long (>100 chars) | Break into multiple lines |
| Wrong indentation | Use 4 spaces, no tabs |
| Missing space after keyword | Add space: `if (x)` not `if(x)` |
| Wrong pointer alignment | Use `int* ptr` not `int *ptr` |

**Example fixes**:

```cpp
// Bad - line too long
void processGameWithLongNameAndManyParameters(const std::string& gameName, int gameId, bool isInstalled) { }

// Good - broken into multiple lines
void processGameWithLongNameAndManyParameters(
    const std::string& gameName,
    int gameId,
    bool isInstalled) { }
```

### Clang-Tidy Failures

**Error**: `warning: [check-name] message`

**Diagnosis**:
1. Code violates a clang-tidy rule
2. Naming convention is wrong
3. Code pattern is unsafe or inefficient

**Solutions**:

**Naming convention issues**:

```cpp
// Bad - wrong naming
int gameID;
void LoadGame();
class game_card { };

// Good - correct naming
int gameId;
void loadGame();
class GameCard { };
```

**Performance issues**:

```cpp
// Bad - unnecessary copy
void processGame(Game game) { }

// Good - const reference
void processGame(const Game& game) { }
```

**Memory issues**:

```cpp
// Bad - manual memory management
int* ptr = new int(42);
delete ptr;

// Good - smart pointer
auto ptr = std::make_unique<int>(42);
```

**Suppress specific warnings** (if necessary):

```cpp
// Suppress for specific line
// NOLINTNEXTLINE(readability-identifier-naming)
int gameID = 0;

// Suppress for entire function
// NOLINT(*)
void legacyFunction() { }
```

**Update `.clang-tidy` if needed**:

```yaml
# In .clang-tidy, add to Checks to disable a check
Checks: >
  *,
  -readability-identifier-naming,  # Disable if too strict
```

### CMake Configuration Check Fails

**Error**: `CMake configuration failed`

**Diagnosis**:
1. CMakeLists.txt has syntax errors
2. Required packages not found
3. Invalid variable references

**Solutions**:

**Check CMakeLists.txt syntax**:
```bash
# Validate syntax
cmake --debug-output 2>&1 | grep -i error

# Check specific file
cmake -P CMakeLists.txt
```

**Verify package availability**:
```bash
# Check if package is installed
pkg-config --list-all | grep qt6

# Install missing package
sudo apt-get install qt6-base-dev
```

**Check variable references**:
```cmake
# Bad - undefined variable
message(${UNDEFINED_VAR})

# Good - check if defined
if(DEFINED MY_VAR)
    message(${MY_VAR})
endif()
```

## Test Failures

### Unit Tests Fail

**Error**: `Test failed: assertion error`

**Diagnosis**:
1. Test logic is wrong
2. Code being tested has a bug
3. Test environment is different from CI

**Solutions**:

**Run tests locally**:
```bash
cd build
ctest --output-on-failure
```

**Run specific test**:
```bash
cd build
ctest -R test_name --output-on-failure
```

**Debug test**:
```bash
cd build
./tests/test_executable --verbose
```

**Check test code**:
```bash
# Review test file
cat tests/test_file.cpp

# Look for assertions
grep -n "ASSERT\|EXPECT" tests/test_file.cpp
```

### Tests Don't Run

**Error**: `No tests found` or `ctest: No tests were found`

**Diagnosis**:
1. Tests not built (BUILD_TESTS=OFF)
2. Test executable not found
3. CMakeLists.txt doesn't register tests

**Solutions**:

**Enable tests in CMake**:
```bash
cmake .. -DBUILD_TESTS=ON
```

**Check test registration**:
```cmake
# In tests/CMakeLists.txt
add_executable(test_name test_file.cpp)
add_test(NAME test_name COMMAND test_name)
```

**Verify test executable exists**:
```bash
ls -la build/tests/
```

## Platform-Specific Issues

### Linux Build Fails

**Error**: `Package not found` or `undefined reference`

**Diagnosis**:
1. Missing development packages
2. Wrong architecture (cross-compilation issue)
3. Library version mismatch

**Solutions**:

**Install missing packages**:
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    qt6-base-dev \
    qt6-tools-dev
```

**Check architecture**:
```bash
uname -m
dpkg --print-architecture
```

**For cross-compilation**:
```bash
# Install cross-compilation tools
sudo apt-get install crossbuild-essential-arm64

# Configure with architecture
cmake .. -DCMAKE_SYSTEM_PROCESSOR=aarch64
```

### macOS Build Fails

**Error**: `xcrun: error` or `Qt not found`

**Diagnosis**:
1. Xcode command line tools not installed
2. Qt not installed via Homebrew
3. Architecture mismatch (Intel vs Apple Silicon)

**Solutions**:

**Install Xcode tools**:
```bash
xcode-select --install
```

**Install Qt**:
```bash
brew install qt6
```

**Check architecture**:
```bash
uname -m
# arm64 = Apple Silicon
# x86_64 = Intel
```

**Build for specific architecture**:
```bash
cmake .. -DCMAKE_OSX_ARCHITECTURES=arm64
```

### Windows Build Fails

**Error**: `Visual Studio not found` or `Qt not found`

**Diagnosis**:
1. Visual Studio not installed
2. Qt not installed
3. Wrong architecture specified

**Solutions**:

**Install Visual Studio**:
- Download from visualstudio.microsoft.com
- Install "Desktop development with C++"

**Install Qt**:
- Download from qt.io
- Or use: `choco install qt` (if using Chocolatey)

**Configure for architecture**:
```bash
# x64
cmake .. -A x64

# x86
cmake .. -A Win32

# ARM64
cmake .. -A ARM64
```

## Local Development Issues

### Can't Install Dependencies

**Problem**: `sudo: command not found` or permission denied

**Solutions**:

**Ubuntu/Debian**:
```bash
# Update package list
sudo apt-get update

# Install package
sudo apt-get install package-name

# If sudo not available, ask system administrator
```

**macOS**:
```bash
# Install Homebrew first
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install package
brew install package-name
```

**Windows**:
```powershell
# Use Chocolatey (if installed)
choco install package-name

# Or download installers manually
```

### CMake Can't Find Qt

**Problem**: `Could not find Qt6`

**Solutions**:

**Set Qt path manually**:
```bash
# Ubuntu/Debian
cmake .. -DQt6_DIR=/usr/lib/cmake/Qt6

# macOS
cmake .. -DQt6_DIR=$(brew --prefix qt6)/lib/cmake/Qt6

# Windows
cmake .. -DQt6_DIR="C:\Qt\6.x.x\msvc2022_64\lib\cmake\Qt6"
```

**Verify Qt installation**:
```bash
# Ubuntu/Debian
dpkg -l | grep qt6

# macOS
brew list qt6

# Windows
dir "C:\Qt"
```

### Build Takes Too Long

**Problem**: Compilation is very slow

**Solutions**:

**Use parallel build**:
```bash
# Linux/macOS
ninja -j$(nproc)

# Windows
cmake --build . --parallel %NUMBER_OF_PROCESSORS%
```

**Use precompiled headers**:
```cmake
# In CMakeLists.txt
target_precompile_headers(target PRIVATE <vector> <string>)
```

**Reduce build type**:
```bash
# Debug builds are slower
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Out of Disk Space

**Problem**: `No space left on device`

**Solutions**:

**Clean build artifacts**:
```bash
rm -rf build
```

**Remove old builds**:
```bash
rm -rf build_* 
```

**Check disk usage**:
```bash
df -h
du -sh build/
```

## Getting Help

If you can't resolve the issue:

1. **Check the logs carefully**
   - Read the full error message
   - Look for the actual error, not just the last line

2. **Search for similar issues**
   - GitHub Issues
   - Stack Overflow
   - Project documentation

3. **Create a minimal reproduction**
   - Simplify the code to isolate the issue
   - Test with a clean build

4. **Ask for help**
   - Open an issue on GitHub
   - Include error logs
   - Describe your environment (OS, compiler, Qt version)

5. **Check related documentation**
   - [CI_CD_PIPELINE.md](CI_CD_PIPELINE.md)
   - [CODE_QUALITY_QUICK_REFERENCE.md](CODE_QUALITY_QUICK_REFERENCE.md)
   - [BUILD.md](BUILD.md)
