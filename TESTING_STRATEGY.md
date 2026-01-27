# OpenGalaxy Testing Strategy

**Date**: January 26, 2026  
**Decision**: Tests are **local-only** (disabled in CI/CD)

## Executive Summary

OpenGalaxy uses **QtTest** for unit and integration testing with a **local-first approach**:

- ✅ **Local Development**: Developers run tests before committing
- ❌ **CI/CD Pipeline**: Tests disabled to keep builds fast and reliable
- 🎯 **Future UI Tests**: Will be local-only when implemented

## Why Tests Are Disabled in CI/CD

### Current Issues
1. **Tests are buggy** - Unreliable, fail intermittently
2. **Network-dependent** - Tests fail in CI/CD environment
3. **Slow** - Add 10+ minutes to pipeline
4. **Incomplete mocking** - Some tests use placeholder assertions

### Benefits of Disabling
- ✅ **Fast CI/CD** - Builds complete in ~15 minutes
- ✅ **Reliable** - No flaky test failures
- ✅ **Developer-focused** - Tests run locally before committing
- ✅ **Maintainable** - Simpler workflow configuration

## Test Execution Model

### Local Development (Before Committing)

```bash
# Build with tests
cd build
cmake .. -DBUILD_TESTS=ON
ninja

# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R CoreTests --output-on-failure
```

### CI/CD Pipeline (GitHub Actions)

```bash
# Build without tests
cmake .. -DBUILD_TESTS=OFF
ninja  # Compile only, no tests
```

## Test Coverage

### Current Tests (6 Suites)

| Suite | Tests | Status | Purpose |
|-------|-------|--------|---------|
| **CoreTests** | 5 | ✅ Should pass | Result type, Logger |
| **ApiTests** | 4 | ⚠️ May fail | Session, HTTP client |
| **RunnerTests** | 5 | ✅ Should pass | Runner discovery, selection |
| **NetworkTests** | 15+ | ⚠️ May fail | Login, downloads, updates |
| **DownloadTests** | 8 | ⚠️ May fail | Progress, cancellation, errors |
| **UpdateTests** | 20+ | ⚠️ May fail | Game/app updates, notifications |

**Total**: 50+ test cases

### Future UI Tests (Planned)

When implemented, UI tests will:
- Test user interactions (clicks, form inputs)
- Test dialogs and notifications
- Test complete user workflows
- Run **locally only** (too slow for CI/CD)

## Developer Workflow

### Before Committing

1. **Build with tests**:
   ```bash
   cd build && cmake .. -DBUILD_TESTS=ON && ninja
   ```

2. **Run tests**:
   ```bash
   ctest --output-on-failure
   ```

3. **Fix any failures** before pushing

4. **Commit and push**:
   ```bash
   git add .
   git commit -m "Your message"
   git push
   ```

### CI/CD Pipeline

1. Checkout code
2. Install dependencies
3. Build (tests disabled)
4. Upload artifacts
5. Done! (~15 minutes)

## Fixing Tests

### Common Issues

**Network timeouts**:
- Use mock HTTP client instead of real requests
- Increase `QTest::qWait()` duration
- Check internet connection

**Async operation failures**:
- Use `QSignalSpy` to wait for signals
- Increase wait times
- Verify callbacks are called

**Platform-specific issues**:
- Use `#ifdef Q_OS_LINUX`, `#ifdef Q_OS_WIN`, `#ifdef Q_OS_MACOS`
- Test on all platforms

### Steps to Fix

1. Run test locally: `ctest -R TestName -V`
2. Identify failure reason
3. Fix the test or implementation
4. Verify fix: `ctest -R TestName --output-on-failure`
5. Commit with explanation

## Re-enabling Tests in CI/CD (Future)

If you want to re-enable tests in CI/CD:

1. **Fix all test failures locally**
2. **Ensure tests pass consistently** (run 5+ times)
3. **Update workflow**:
   ```yaml
   -DBUILD_TESTS=ON
   ```
4. **Add test step**:
   ```yaml
   - name: Run tests
     run: cd build && ctest --output-on-failure
   ```
5. **Monitor pipeline** for flaky tests

## Best Practices

### Writing Tests

✅ **Do**:
- Use descriptive test names
- Test one behavior per test
- Use mocks for external dependencies
- Clean up resources in `cleanupTestCase()`
- Document complex test logic

❌ **Don't**:
- Make real network requests
- Test multiple behaviors in one test
- Leave resources uncleaned
- Use `QVERIFY(true)` placeholders
- Depend on test execution order

### Running Tests

✅ **Do**:
- Run tests before committing
- Run full suite after major changes
- Test on different platforms
- Use `--output-on-failure` flag
- Document test failures

❌ **Don't**:
- Commit without running tests
- Ignore test failures
- Assume tests pass on all platforms
- Run tests in CI/CD (keep pipeline fast)

## Files Involved

| File | Purpose |
|------|---------|
| `.github/workflows/multi-platform-build.yml` | CI/CD config (DBUILD_TESTS=OFF) |
| `CMakeLists.txt` | Root CMake config |
| `tests/CMakeLists.txt` | Test configuration |
| `tests/*.cpp` | Test implementations |
| `docs/TESTING_GUIDE.md` | Developer testing guide |

## Summary

| Aspect | Status |
|--------|--------|
| **Local Testing** | ✅ Enabled (run before committing) |
| **CI/CD Testing** | ❌ Disabled (keep pipeline fast) |
| **Test Framework** | QtTest |
| **Test Count** | 6 suites, 50+ cases |
| **Current Status** | Buggy, needs fixes |
| **UI Tests** | Planned (local-only) |
| **Pipeline Time** | ~15 minutes (no tests) |

## Next Steps

1. **Document test procedures** for developers ✅
2. **Debug and fix** current test failures
3. **Verify tests pass** locally before committing
4. **Add UI tests** gradually (local-only)
5. **Monitor test health** over time

---

**Remember**: Run tests locally before committing! 🧪

For detailed testing instructions, see [TESTING_GUIDE.md](docs/TESTING_GUIDE.md)
