# OpenGalaxy Testing Guide

**Date**: January 26, 2026  
**Status**: Tests are local-only (disabled in CI/CD)

## Overview

OpenGalaxy uses **QtTest** for unit and integration testing. Tests run **locally during development** but are **disabled in the CI/CD pipeline** to keep builds fast and reliable.

## Test Structure

```
tests/
├── core_tests.cpp          # Core utilities (Result, Logger)
├── api_tests.cpp           # API functionality (Session, HTTP)
├── runner_tests.cpp        # Runner discovery and selection
├── network_tests.cpp       # Network operations (login, downloads)
├── download_tests.cpp      # Download progress and error handling
├── update_tests.cpp        # Update checking and installation
├── mock_http_client.h      # Mock HTTP client for testing
└── CMakeLists.txt          # Test configuration
```

## Running Tests Locally

### Build with Tests Enabled

```bash
cd build
cmake .. -DBUILD_TESTS=ON
ninja
```

### Run All Tests

```bash
ctest --output-on-failure
```

### Run Specific Test

```bash
ctest -R CoreTests --output-on-failure
ctest -R ApiTests --output-on-failure
ctest -R RunnerTests --output-on-failure
ctest -R NetworkTests --output-on-failure
ctest -R DownloadTests --output-on-failure
ctest -R UpdateTests --output-on-failure
```

### Run Test with Verbose Output

```bash
ctest -R CoreTests -V
```

### Run Individual Test Executable

```bash
./build/core_tests
./build/api_tests
./build/runner_tests
./build/network_tests
./build/download_tests
./build/update_tests
```

## Test Categories

### 1. Core Tests (`core_tests.cpp`)
Tests fundamental utilities:
- ✅ Result type (success/error handling)
- ✅ Logger (logging levels, output)

**Status**: Should pass

### 2. API Tests (`api_tests.cpp`)
Tests API client functionality:
- ✅ Session creation
- ✅ Token expiry detection
- ✅ HTTP client GET requests
- ✅ HTTP client timeouts

**Status**: May fail (network-dependent)

### 3. Runner Tests (`runner_tests.cpp`)
Tests game runner functionality:
- ✅ Runner discovery
- ✅ Native runner availability
- ✅ Architecture detection
- ✅ Platform detection
- ✅ Best runner selection

**Status**: Should pass

### 4. Network Tests (`network_tests.cpp`)
Tests network operations:
- ✅ Login success/failure
- ✅ Token refresh
- ✅ Library fetching
- ✅ Download operations
- ✅ Update checking

**Status**: May fail (network-dependent, incomplete mocking)

### 5. Download Tests (`download_tests.cpp`)
Tests download functionality:
- ✅ Progress reporting
- ✅ Download cancellation
- ✅ Multiple simultaneous downloads
- ✅ Error handling (invalid URL, permissions)

**Status**: May fail (incomplete implementation)

### 6. Update Tests (`update_tests.cpp`)
Tests update functionality:
- ✅ Game update checking
- ✅ App update checking
- ✅ Update download/installation
- ✅ Update notifications
- ✅ Rollback functionality

**Status**: May fail (mostly mocked, incomplete)

## Known Issues

### Current Test Problems

1. **Network-dependent tests fail** - Tests that make real HTTP requests may timeout
2. **Incomplete mocking** - Some tests use `QVERIFY(true)` placeholders
3. **Timing issues** - Async operations may not complete in time
4. **Platform-specific failures** - Tests may behave differently on Linux/macOS/Windows

### Why Tests Are Disabled in CI/CD

- ❌ Tests are buggy and unreliable
- ❌ Network tests fail in CI/CD environment
- ❌ Slow down the pipeline significantly
- ✅ Better to keep CI/CD focused on compilation

## Fixing Tests

### Before Committing

1. **Run tests locally**:
   ```bash
   cd build
   ctest --output-on-failure
   ```

2. **Fix any failures** before pushing

3. **Document failures** if they're environment-specific

### Common Fixes

**Network timeouts**:
- Increase `QTest::qWait()` duration
- Use mock HTTP client instead of real requests
- Check internet connection

**Async operation failures**:
- Increase wait time: `QTest::qWait(5000)` instead of `QTest::qWait(1000)`
- Use `QSignalSpy` to wait for signals

**Platform-specific issues**:
- Use `#ifdef Q_OS_LINUX`, `#ifdef Q_OS_WIN`, `#ifdef Q_OS_MACOS`
- Test on all platforms before committing

## Future: UI Tests

When UI tests are added, they will:
- ✅ Test user interactions (clicks, form inputs)
- ✅ Test dialogs and notifications
- ✅ Test complete user workflows
- ✅ Run **locally only** (too slow for CI/CD)

Example:
```cpp
void testLoginFlow() {
    AppWindow window;
    window.show();
    
    auto loginBtn = window.findChild<QPushButton*>("loginButton");
    QTest::mouseClick(loginBtn, Qt::LeftButton);
    
    QTest::qWait(500);
    auto loginDialog = window.findChild<LoginDialog*>();
    QVERIFY(loginDialog->isVisible());
}
```

## Best Practices

### Writing Tests

1. **Use descriptive names**: `testLoginWithValidCredentials()` not `test1()`
2. **Test one thing**: Each test should verify one behavior
3. **Use mocks**: Don't make real network requests
4. **Clean up**: Use `cleanupTestCase()` to free resources
5. **Document**: Add comments explaining complex test logic

### Running Tests

1. **Before committing**: Always run `ctest --output-on-failure`
2. **After major changes**: Run full test suite
3. **On different platforms**: Test on Linux, macOS, Windows if possible
4. **With different configurations**: Test with different CMake options

### Debugging Tests

```bash
# Run with verbose output
ctest -R TestName -V

# Run specific test executable with GDB
gdb ./build/core_tests

# Run with environment variables
QT_DEBUG_PLUGINS=1 ctest -R TestName -V
```

## CI/CD Pipeline

**Tests are disabled in CI/CD**:
```yaml
# .github/workflows/multi-platform-build.yml
-DBUILD_TESTS=OFF
```

**Why**:
- Tests are buggy and unreliable
- CI/CD focuses on compilation verification
- Developers run tests locally before committing
- Keeps pipeline fast (~15 minutes)

**If you want to re-enable tests**:
1. Fix all test failures locally
2. Ensure tests pass consistently
3. Update workflow: `-DBUILD_TESTS=ON`
4. Add `ctest --output-on-failure` step

## Resources

- [Qt Test Documentation](https://doc.qt.io/qt-6/qttest-index.html)
- [QtTest Best Practices](https://doc.qt.io/qt-6/qtest.html)
- [Mock Objects in Qt](https://doc.qt.io/qt-6/qtest.html#creating-a-benchmark)

## Summary

| Aspect | Status |
|--------|--------|
| **Local Testing** | ✅ Enabled (run before committing) |
| **CI/CD Testing** | ❌ Disabled (keep pipeline fast) |
| **Test Framework** | QtTest |
| **Test Count** | 6 test suites, 50+ test cases |
| **Current Status** | Buggy, needs fixes |
| **UI Tests** | Planned for future (local-only) |

---

**Remember**: Run tests locally before committing! 🧪
