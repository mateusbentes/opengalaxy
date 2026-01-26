# Code Quality Quick Reference

A quick guide for developers to maintain code quality in OpenGalaxy.

## Pre-Commit Checklist

- [ ] Code is formatted with clang-format
- [ ] No clang-tidy warnings
- [ ] All tests pass locally
- [ ] CMake configuration is valid
- [ ] Commit message is clear and descriptive

## Quick Commands

### Format Code

```bash
# Format all C++ files
find . -name "*.cpp" -o -name "*.h" | grep -v build | xargs clang-format -i

# Format specific file
clang-format -i src/file.cpp
```

### Check Formatting (without modifying)

```bash
# Check all files
find . -name "*.cpp" -o -name "*.h" | grep -v build | xargs clang-format --dry-run -Werror

# Check specific file
clang-format --dry-run -Werror src/file.cpp
```

### Run Static Analysis

```bash
# Build with clang-tidy enabled
mkdir build && cd build
cmake .. -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks=*,-fuchsia-*,-google-*"
ninja
```

### Build and Test

```bash
# Configure
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON

# Build
ninja

# Run tests
ctest --output-on-failure
```

### Clean Build

```bash
rm -rf build
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

## Naming Conventions

| Type | Convention | Example |
|------|-----------|---------|
| Class | `CamelCase` | `GameCard` |
| Function | `camelBack` | `loadCoverImage()` |
| Variable | `camelBack` | `gameTitle` |
| Constant | `UPPER_CASE` | `MAX_RETRIES` |
| Member | `m_camelBack` | `m_gameId` |
| Static | `s_camelBack` | `s_instanceCount` |
| Macro | `UPPER_CASE` | `OPENGALAXY_VERSION` |

## Code Style Rules

### Indentation
- Use **4 spaces** (no tabs)
- Consistent indentation in all files

### Line Length
- Maximum **100 characters** per line
- Break long lines at logical points

### Pointers and References
- Pointer `*` aligned right: `int* ptr`
- Reference `&` aligned right: `int& ref`

### Spacing
- Space after keywords: `if (condition)` not `if(condition)`
- Space around operators: `a + b` not `a+b`
- No space before function parentheses: `function()` not `function ()`

### Braces
- Opening brace on same line: `if (x) {`
- Closing brace on new line
- Always use braces, even for single statements

```cpp
// Good
if (condition) {
    doSomething();
}

// Bad
if (condition)
    doSomething();
```

### Comments
- Use `//` for single-line comments
- Use `/* */` for multi-line comments
- Comment non-obvious code
- Keep comments up-to-date with code

```cpp
// Good
int calculateHash(const std::string& data) {
    // Use FNV-1a algorithm for better distribution
    int hash = 2166136261;
    for (char c : data) {
        hash ^= c;
        hash *= 16777619;
    }
    return hash;
}

// Bad
int calculateHash(const std::string& data) {
    int hash = 2166136261;
    for (char c : data) {
        hash ^= c;
        hash *= 16777619;
    }
    return hash;
}
```

## Common Issues and Fixes

### Formatting Issues

**Problem**: `clang-format --dry-run -Werror` fails

**Solution**:
```bash
# Auto-fix the file
clang-format -i src/file.cpp
```

### Naming Convention Issues

**Problem**: `readability-identifier-naming` warning

**Solution**: Rename to follow conventions
```cpp
// Bad
int gameID;
void LoadGame();

// Good
int gameId;
void loadGame();
```

### Performance Issues

**Problem**: `performance-unnecessary-value-param` warning

**Solution**: Pass by const reference
```cpp
// Bad
void processGame(Game game) { }

// Good
void processGame(const Game& game) { }
```

### Memory Issues

**Problem**: `cppcoreguidelines-pro-type-*` warnings

**Solution**: Use modern C++ features
```cpp
// Bad
int* ptr = new int(42);
delete ptr;

// Good
auto ptr = std::make_unique<int>(42);
```

## Before Pushing

1. **Format your code**
   ```bash
   clang-format -i src/**/*.cpp src/**/*.h
   ```

2. **Check for warnings**
   ```bash
   cd build && ninja 2>&1 | grep warning
   ```

3. **Run tests**
   ```bash
   cd build && ctest --output-on-failure
   ```

4. **Verify CMake**
   ```bash
   cd build && cmake .. -G Ninja
   ```

5. **Commit with clear message**
   ```bash
   git add .
   git commit -m "Feature: Add game repair functionality"
   ```

## IDE Integration

### Visual Studio Code

Install extensions:
- C/C++ (Microsoft)
- Clang-Format (xaver)
- Clang-Tidy (notskm)

Add to `.vscode/settings.json`:
```json
{
    "C_Cpp.clangFormatOnSave": true,
    "C_Cpp.clangFormatStyle": "file",
    "[cpp]": {
        "editor.defaultFormatter": "xaver.clang-format",
        "editor.formatOnSave": true
    }
}
```

### CLion

Built-in support:
- Settings → Editor → Code Style → C/C++
- Enable "Reformat Code" on save
- Use `.clang-format` file automatically

### Visual Studio

Built-in support:
- Tools → Options → Text Editor → C/C++ → Code Style
- Use `.clang-format` file automatically

## Resources

- [LLVM Clang-Format](https://clang.llvm.org/docs/ClangFormat/)
- [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/)
- [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

## Getting Help

- Check [CI_CD_PIPELINE.md](CI_CD_PIPELINE.md) for detailed information
- Review [CONTRIBUTING.md](../CONTRIBUTING.md) for contribution guidelines
- Open an issue on GitHub for questions
