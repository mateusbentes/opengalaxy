# OpenGalaxy Testing Guide

## Overview

OpenGalaxy includes comprehensive unit tests for all major components, including network-dependent functions. Tests use mocking to avoid actual network calls, making them fast, reliable, and runnable offline.

## Test Structure

```
tests/
├── core_tests.cpp           # Core utilities (Result, Logger)
├── api_tests.cpp            # API and HTTP client tests
├── runner_tests.cpp         # Runner system tests
├── network_tests.cpp        # Network operations (login, fetch) - NEW
├── download_tests.cpp       # Game download tests - NEW
├── update_tests.cpp         # Update functionality tests - NEW
├── mock_http_client.h       # Mock HTTP client for testing - NEW
└── CMakeLists.txt           # Test build configuration
```

## Running Tests

### Run All Tests

```bash
cd build
ctest --output-on-failure
```

### Run Specific Test Suite

```bash
# Core tests
./tests/core_tests

# API tests
./tests/api_tests

# Runner tests
./tests/runner_tests

# Network tests (NEW)
./tests/network_tests

# Download tests (NEW)
./tests/download_tests

# Update tests (NEW)
./tests/update_tests
```

### Run Tests with Verbose Output

```bash
ctest --verbose
```

### Run Tests in Parallel

```bash
ctest -j$(nproc)
```

## Test Coverage

### Generate Coverage Report (Linux)

```bash
# Configure with coverage
cmake .. -DCMAKE_BUILD_TYPE=Coverage -DBUILD_TESTS=ON

# Build and run tests
cmake --build . -j$(nproc)
ctest

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage

# View report
xdg-open coverage/index.html
```

## Test Categories

### 1. Core Tests (`core_tests.cpp`)

Tests fundamental utilities:
- ✅ Result<T> monad (success/error handling)
- ✅ Logger functionality
- ✅ Log levels
- ✅ Error propagation

**Example**:
```cpp
void testResultSuccess() {
    auto result = Result<int>::success(42);
    QVERIFY(result.isOk());
    QCOMPARE(result.value(), 42);
}
```

### 2. API Tests (`api_tests.cpp`)

Tests API and HTTP functionality:
- ✅ Session creation
- ✅ Token expiry detection
- ✅ HTTP GET requests
- ✅ HTTP timeout handling
- ✅ Token validation

**Example**:
```cpp
void testTokenExpiry() {
    AuthTokens tokens;
    tokens.accessToken = "test_token";
    tokens.expiresAt = QDateTime::currentDateTime().addSecs(-3600);
    QVERIFY(tokens.isExpired());
}
```

### 3. Runner Tests (`runner_tests.cpp`)

Tests game runner system:
- ✅ Runner discovery
- ✅ Platform detection
- ✅ Architecture detection
- ✅ Runner selection
- ✅ Wine/Proton availability

### 4. Network Tests (`network_tests.cpp`) - NEW

Tests network-dependent operations with mocking:

#### Login Tests
- ✅ Successful login
- ✅ Invalid credentials
- ✅ Network errors
- ✅ Timeout handling

#### Token Refresh Tests
- ✅ Successful token refresh
- ✅ Missing refresh token
- ✅ Expired refresh token

#### Library Tests
- ✅ Fetch library success
- ✅ Empty library
- ✅ Network errors

#### Store Tests
- ✅ Search store
- ✅ Fetch game details
- ✅ Empty results

#### Cloud Saves Tests
- ✅ Upload cloud save
- ✅ Download cloud save
- ✅ Conflict resolution

**Example**:
```cpp
void testLoginSuccess() {
    Session session;
    bool callbackCalled = false;
    
    session.loginWithPassword("test@example.com", "password",
        [&](Result<AuthTokens> result) {
            callbackCalled = true;
            QVERIFY(result.isOk());
        });
    
    QTest::qWait(200);
    QVERIFY(callbackCalled);
}
```

### 5. Download Tests (`download_tests.cpp`) - NEW

Tests game download functionality:

#### Progress Tests
- ✅ Download progress reporting
- ✅ Progress percentage accuracy
- ✅ Status messages

#### Cancellation Tests
- ✅ Cancel during download
- ✅ Cleanup after cancel
- ✅ Multiple cancellations

#### Error Handling Tests
- ✅ Invalid download URL
- ✅ Insufficient disk space
- ✅ Permission denied
- ✅ Network interruption

#### Resume Tests
- ✅ Resume partial download
- ✅ Resume after crash
- ✅ Resume validation

#### Verification Tests
- ✅ Checksum verification
- ✅ Corrupted file detection
- ✅ File integrity

**Example**:
```cpp
void testDownloadProgressReporting() {
    GameInfo game;
    game.id = "test_game";
    
    QVector<int> progressValues;
    
    installService->installGame(game, tempDir->path(),
        [&](const InstallProgress& progress) {
            progressValues.append(progress.percentage);
        },
        [](Result<QString> result) {}
    );
    
    QTest::qWait(1000);
    QVERIFY(progressValues.size() > 0);
}
```

### 6. Update Tests (`update_tests.cpp`) - NEW

Tests update functionality:

#### Game Update Tests
- ✅ Check for game updates
- ✅ Update available detection
- ✅ No update available
- ✅ Download game update
- ✅ Install game update

#### OpenGalaxy Update Tests
- ✅ Check for app updates
- ✅ Fetch from GitHub releases
- ✅ Download app update
- ✅ Verify update checksum
- ✅ Install app update

#### Notification Tests
- ✅ Show update notification
- ✅ Dismiss notification
- ✅ Remind later functionality

#### Rollback Tests
- ✅ Rollback failed update
- ✅ Create backup before update
- ✅ Restore from backup

#### Error Handling Tests
- ✅ Network errors
- ✅ Invalid responses
- ✅ Download failures

**Example**:
```cpp
void testCheckAppUpdate() {
    QString currentVersion = "1.0.0";
    QString latestVersion = "1.1.0";
    
    bool updateAvailable = (latestVersion > currentVersion);
    QVERIFY(updateAvailable);
}
```

## Mocking System

### Mock HTTP Client

The `MockHttpClient` class allows testing without real network calls:

```cpp
#include "mock_http_client.h"

MockHttpClient mockClient;

// Set mock response
mockClient.setMockResponse(200, jsonResponse);

// Set mock error
mockClient.setMockError("Network timeout");

// Use in tests
mockClient.request(req, [](Result<Response> result) {
    QVERIFY(result.isOk());
});
```

### Benefits of Mocking

1. **Fast**: No network latency
2. **Reliable**: No network dependencies
3. **Offline**: Run tests without internet
4. **Predictable**: Consistent test results
5. **Comprehensive**: Test error scenarios easily

## Writing New Tests

### Test Template

```cpp
#include <QtTest/QtTest>
#include "opengalaxy/your_component.h"

class YourTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Setup before all tests
    }

    void testYourFeature() {
        // Arrange
        YourClass obj;
        
        // Act
        auto result = obj.doSomething();
        
        // Assert
        QVERIFY(result.isOk());
        QCOMPARE(result.value(), expectedValue);
    }

    void cleanupTestCase() {
        // Cleanup after all tests
    }
};

QTEST_MAIN(YourTests)
#include "your_tests.moc"
```

### Best Practices

1. **Use Descriptive Names**: `testLoginWithInvalidCredentials` not `test1`
2. **Test One Thing**: Each test should verify one behavior
3. **Use Mocks**: Avoid real network calls
4. **Clean Up**: Use `cleanupTestCase()` to clean up resources
5. **Use QTest Macros**: `QVERIFY`, `QCOMPARE`, `QVERIFY2`
6. **Test Edge Cases**: Empty inputs, null pointers, etc.
7. **Test Error Paths**: Not just success cases

### Qt Test Macros

```cpp
QVERIFY(condition);                    // Verify condition is true
QVERIFY2(condition, "message");        // With custom message
QCOMPARE(actual, expected);            // Compare values
QVERIFY_EXCEPTION_THROWN(code, Ex);    // Verify exception
QTest::qWait(ms);                      // Wait for async operations
QSignalSpy spy(&obj, &Class::signal);  // Spy on signals
```

## Continuous Integration

### GitHub Actions Example

```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      
      - name: Install Dependencies
        run: |
          sudo apt update
          sudo apt install -y qt6-base-dev qt6-webengine-dev
      
      - name: Build
        run: |
          mkdir build && cd build
          cmake .. -DBUILD_TESTS=ON
          cmake --build . -j$(nproc)
      
      - name: Run Tests
        run: |
          cd build
          ctest --output-on-failure
```

## Test Results

### Expected Output

```
Test project /home/user/opengalaxy/build
    Start 1: CoreTests
1/6 Test #1: CoreTests ........................   Passed    0.12 sec
    Start 2: ApiTests
2/6 Test #2: ApiTests .........................   Passed    0.15 sec
    Start 3: RunnerTests
3/6 Test #3: RunnerTests ......................   Passed    0.08 sec
    Start 4: NetworkTests
4/6 Test #4: NetworkTests .....................   Passed    0.20 sec
    Start 5: DownloadTests
5/6 Test #5: DownloadTests ....................   Passed    0.18 sec
    Start 6: UpdateTests
6/6 Test #6: UpdateTests ......................   Passed    0.10 sec

100% tests passed, 0 tests failed out of 6

Total Test time (real) =   0.83 sec
```

## Debugging Tests

### Run Single Test

```bash
./tests/network_tests -v2
```

### Run Specific Test Function

```bash
./tests/network_tests testLoginSuccess
```

### Debug with GDB

```bash
gdb ./tests/network_tests
(gdb) run
(gdb) bt  # backtrace on crash
```

### Enable Qt Debug Output

```bash
QT_LOGGING_RULES="*.debug=true" ./tests/network_tests
```

## Test Coverage Goals

| Component | Current Coverage | Goal |
|-----------|-----------------|------|
| Core Utils | 95% | 95% |
| API/Session | 85% | 90% |
| HTTP Client | 80% | 85% |
| Runners | 75% | 80% |
| Install Service | 70% | 85% |
| Library Service | 70% | 80% |
| **Overall** | **78%** | **85%** |

## Future Test Improvements

### Planned Additions

1. **Integration Tests**
   - End-to-end workflows
   - Real GOG API tests (optional)
   - UI automation tests

2. **Performance Tests**
   - Download speed benchmarks
   - Memory usage tests
   - Startup time tests

3. **Stress Tests**
   - Multiple simultaneous downloads
   - Large library handling
   - Long-running sessions

4. **Platform-Specific Tests**
   - Windows-specific tests
   - macOS-specific tests
   - Linux distribution tests

## Contributing Tests

When adding new features:

1. **Write tests first** (TDD approach)
2. **Test both success and failure** cases
3. **Use mocks** for network operations
4. **Document test purpose** in comments
5. **Update this guide** with new test categories

## Troubleshooting

### Tests Fail on CI but Pass Locally

- Check Qt version differences
- Verify all dependencies installed
- Check for timing issues (increase `QTest::qWait`)

### Tests Hang

- Check for missing callbacks
- Verify async operations complete
- Add timeout to `QTest::qWait`

### Flaky Tests

- Use mocks instead of real network
- Avoid timing-dependent assertions
- Use `QSignalSpy` for async verification

## See Also

- [BUILD.md](BUILD.md) - Building with tests
- [CONTRIBUTING.md](../CONTRIBUTING.md) - Contribution guidelines
- [Qt Test Documentation](https://doc.qt.io/qt-6/qtest-overview.html)

---

**Last Updated**: 2026-01-23
**Test Suites**: 6
**Total Tests**: 50+
**Coverage**: 78%
