# CI/CD Pipeline Documentation

## Overview

OpenGalaxy uses a comprehensive CI/CD pipeline to ensure code quality, compatibility across multiple platforms and architectures, and reliable builds. This document explains the pipeline structure, checks performed, and how to work with it locally.

## Pipeline Structure

### 1. Code Quality Checks (`code-quality.yml`)

Runs on every push to `main` or `develop` branches and on pull requests.

#### Clang-Format Check
- **Purpose**: Ensures consistent code formatting across the project
- **Configuration**: `.clang-format` file in project root
- **What it checks**: All `.cpp` and `.h` files (excluding build directories)
- **Failure**: If code doesn't match the formatting style
- **Fix**: Run `clang-format -i <file>` locally before committing

#### Clang-Tidy Check
- **Purpose**: Static code analysis to catch potential bugs and code quality issues
- **Configuration**: `.clang-tidy` file in project root
- **What it checks**: 
  - Memory management issues
  - Performance problems
  - Code style violations
  - Potential bugs
- **Failure**: If warnings are found
- **Fix**: Address warnings reported by clang-tidy

#### CMake Configuration Check
- **Purpose**: Validates CMakeLists.txt syntax
- **What it checks**: All CMakeLists.txt files can be parsed correctly
- **Failure**: If CMake configuration is invalid

#### Dependency Check
- **Purpose**: Verifies all required dependencies are properly declared
- **What it checks**: find_package and add_subdirectory calls

#### Documentation Check
- **Purpose**: Ensures documentation files exist and are accessible
- **What it checks**: docs/ directory, README.md, BUILD.md

#### License Check
- **Purpose**: Verifies license headers and LICENSE file
- **What it checks**: SPDX license headers in source files

### 2. Multi-Platform Build (`multi-platform-build.yml`)

Builds and tests the project on multiple platforms and architectures.

#### Linux Builds
- **Architectures**: x86_64, arm64, armv7, riscv64
- **Runner**: ubuntu-latest
- **QEMU**: Used for cross-compilation on non-x86_64 architectures
- **Dependencies**: build-essential, cmake, ninja-build, Qt6 libraries
- **Tests**: Run on x86_64 only (ctest)

#### macOS Builds
- **Architectures**: x86_64 (macOS 13), arm64 (macOS 14)
- **Runners**: macos-13, macos-14
- **Dependencies**: cmake, ninja, Qt6 via Homebrew
- **Tests**: Run on both architectures

#### Windows Builds
- **Architectures**: x64, x86, arm64
- **Runner**: windows-latest
- **Generator**: Visual Studio 17 2022
- **Qt Installation**: Via jurplel/install-qt-action
- **Tests**: Run on all architectures

#### Build Summary
- Aggregates results from all platform builds
- Fails if any platform build fails

## Code Quality Standards

### Formatting Style

The project uses LLVM-based formatting with the following key rules:

```yaml
BasedOnStyle: LLVM
IndentWidth: 4
UseTab: Never
ColumnLimit: 100
PointerAlignment: Right
```

**Key Points:**
- 4 spaces for indentation (no tabs)
- Maximum 100 characters per line
- Pointer `*` aligned to the right: `int* ptr`
- Consistent spacing around operators

### Naming Conventions

Enforced by clang-tidy:

| Element | Convention | Example |
|---------|-----------|---------|
| Classes | CamelCase | `GameCard`, `LibraryPage` |
| Functions | camelBack | `loadCoverImage()`, `refreshLibrary()` |
| Variables | camelBack | `gameTitle`, `installProgress` |
| Constants | UPPER_CASE | `MAX_RETRIES`, `DEFAULT_TIMEOUT` |
| Global Constants | UPPER_CASE with `k` prefix | `kMaxConnections` |
| Member Variables | camelBack with `m_` prefix | `m_gameId`, `m_isInstalled` |
| Static Variables | camelBack with `s_` prefix | `s_instanceCount` |
| Macros | UPPER_CASE | `OPENGALAXY_VERSION` |

### Code Quality Rules

Clang-tidy checks for:
- **Memory Safety**: Proper use of pointers, no memory leaks
- **Performance**: Unnecessary copies, inefficient algorithms
- **Modernization**: Use of C++20 features where appropriate
- **Readability**: Clear variable names, proper comments
- **Concurrency**: Thread-safe code patterns

**Excluded Checks** (too strict for this project):
- fuchsia-* (Fuchsia OS specific)
- google-* (Google style guide)
- llvm-* (LLVM project specific)
- hicpp-* (High Integrity C++ specific)
- cppcoreguidelines-pro-* (Some core guidelines)
- readability-magic-numbers (Allows reasonable magic numbers)
- readability-identifier-length (Allows short variable names)

## Working with the Pipeline Locally

### Prerequisites

Install code quality tools:

```bash
# Ubuntu/Debian
sudo apt-get install clang-format clang-tidy

# macOS
brew install clang-format llvm

# Windows
# Download from LLVM releases or use Visual Studio's built-in tools
```

### Before Committing

#### 1. Check Formatting

```bash
# Check if files need formatting
clang-format --dry-run -Werror src/**/*.cpp src/**/*.h

# Auto-format files
clang-format -i src/**/*.cpp src/**/*.h
```

#### 2. Run Static Analysis

```bash
# Build with clang-tidy
mkdir build
cd build
cmake .. -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks=*,-fuchsia-*,-google-*"
ninja
```

#### 3. Build and Test

```bash
# Configure
mkdir build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON

# Build
ninja

# Run tests
ctest --output-on-failure
```

### Fixing Common Issues

#### Formatting Issues

If clang-format fails:

```bash
# Auto-fix all files
find . -name "*.cpp" -o -name "*.h" | grep -v build | xargs clang-format -i
```

#### Clang-Tidy Warnings

Common warnings and fixes:

| Warning | Cause | Fix |
|---------|-------|-----|
| `readability-identifier-naming` | Wrong naming convention | Rename to follow conventions |
| `modernize-use-auto` | Explicit type instead of auto | Use `auto` where type is obvious |
| `performance-unnecessary-value-param` | Passing by value | Pass by const reference |
| `cppcoreguidelines-pro-type-reinterpret-cast` | Unsafe cast | Use safer alternatives |

#### Build Failures

If CMake configuration fails:

```bash
# Clean build
rm -rf build
mkdir build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

# Check for missing dependencies
cmake --debug-output 2>&1 | grep -i "not found"
```

## Configuration Files

### `.clang-format`

Located in project root. Defines code formatting rules.

**Key settings:**
- `BasedOnStyle: LLVM` - Base style
- `IndentWidth: 4` - 4-space indentation
- `ColumnLimit: 100` - Line length limit
- `PointerAlignment: Right` - Pointer alignment

### `.clang-tidy`

Located in project root. Defines static analysis rules.

**Key settings:**
- `Checks:` - Which checks to run
- `WarningsAsErrors:` - Treat warnings as errors
- `CheckOptions:` - Naming conventions and other options

## CI/CD Workflow

### On Push to main/develop

1. **Code Quality Checks** (parallel)
   - clang-format
   - clang-tidy
   - CMake check
   - Dependency check
   - Documentation check
   - License check

2. **Multi-Platform Builds** (parallel)
   - Linux builds (4 architectures)
   - macOS builds (2 architectures)
   - Windows builds (3 architectures)

3. **Build Summary**
   - Aggregates all results
   - Fails if any check fails

### On Pull Request

Same checks as push, but must pass before merge.

### Manual Trigger

Can be triggered manually via GitHub Actions UI with `workflow_dispatch`.

## Troubleshooting

### Pipeline Failures

**Check the logs:**
1. Go to GitHub Actions tab
2. Click on the failed workflow
3. Expand the failed job
4. Review the error messages

**Common issues:**

| Issue | Solution |
|-------|----------|
| Formatting errors | Run `clang-format -i` on changed files |
| Clang-tidy warnings | Fix the reported issues or update `.clang-tidy` |
| Build failures | Check CMakeLists.txt, ensure dependencies are installed |
| Test failures | Run tests locally with `ctest --output-on-failure` |
| Cross-compilation failures | Check QEMU setup, architecture-specific code |

### Local Build Issues

**Qt not found:**
```bash
# Ubuntu/Debian
sudo apt-get install qt6-base-dev qt6-tools-dev

# macOS
brew install qt6

# Windows
# Use jurplel/install-qt-action or download from qt.io
```

**CMake not found:**
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
# Download from cmake.org or use Visual Studio installer
```

## Best Practices

1. **Format before committing**
   ```bash
   clang-format -i <changed-files>
   ```

2. **Run tests locally**
   ```bash
   cd build && ctest --output-on-failure
   ```

3. **Check clang-tidy warnings**
   ```bash
   # Build with tidy enabled
   cmake .. -DCMAKE_CXX_CLANG_TIDY="clang-tidy"
   ninja 2>&1 | grep warning
   ```

4. **Keep commits focused**
   - One feature per commit
   - Easier to review and debug

5. **Write meaningful commit messages**
   - Describe what changed and why
   - Reference issues if applicable

6. **Test on multiple platforms**
   - Use CI results to verify cross-platform compatibility
   - Test locally on different architectures if possible

## Resources

- [LLVM Clang-Format Documentation](https://clang.llvm.org/docs/ClangFormat/)
- [Clang-Tidy Documentation](https://clang.llvm.org/extra/clang-tidy/)
- [CMake Documentation](https://cmake.org/documentation/)
- [Qt6 Documentation](https://doc.qt.io/qt-6/)
- [C++20 Standard](https://en.cppreference.com/w/cpp/20)

## Contributing

When contributing to OpenGalaxy:

1. Follow the code quality standards
2. Ensure all CI checks pass
3. Test on multiple platforms if possible
4. Update documentation if needed
5. Write clear commit messages

For more information, see [CONTRIBUTING.md](../CONTRIBUTING.md).
