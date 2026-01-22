# Contributing to OpenGalaxy

Thank you for your interest in contributing to OpenGalaxy! This document provides guidelines and instructions for contributing.

## 🤝 Code of Conduct

- Be respectful and inclusive
- Welcome newcomers
- Focus on constructive feedback
- Respect different viewpoints

## 🐛 Reporting Bugs

Before creating a bug report:
1. Check existing issues
2. Update to the latest version
3. Verify it's reproducible

### Bug Report Template

```markdown
**Describe the bug**
A clear description of what the bug is.

**To Reproduce**
Steps to reproduce:
1. Go to '...'
2. Click on '...'
3. See error

**Expected behavior**
What you expected to happen.

**Screenshots**
If applicable, add screenshots.

**Environment:**
 - OS: [e.g. Ubuntu 22.04]
 - Qt Version: [e.g. 6.5.0]
 - OpenGalaxy Version: [e.g. 1.0.0]

**Additional context**
Any other relevant information.
```

## 💡 Suggesting Features

Feature requests are welcome! Please:
1. Check if it's already requested
2. Explain the use case
3. Describe the proposed solution
4. Consider alternatives

## 🔧 Development Setup

### 1. Fork and Clone

```bash
# Fork on GitHub, then:
git clone https://github.com/YOUR_USERNAME/opengalaxy.git
cd opengalaxy
git remote add upstream https://github.com/opengalaxy/opengalaxy.git
```

### 2. Install Dependencies

See [BUILD.md](BUILD.md) for platform-specific instructions.

### 3. Create a Branch

```bash
git checkout -b feature/your-feature-name
# or
git checkout -b fix/your-bug-fix
```

### 4. Make Changes

Follow our coding standards (see below).

### 5. Test

```bash
# Build with tests
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build . -j

# Run tests
ctest --output-on-failure

# Run specific test
./tests/opengalaxy_tests
```

### 6. Commit

```bash
git add .
git commit -m "feat: add awesome feature"
```

Follow [Conventional Commits](https://www.conventionalcommits.org/):
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation
- `style:` Formatting
- `refactor:` Code restructuring
- `test:` Adding tests
- `chore:` Maintenance

### 7. Push and Create PR

```bash
git push origin feature/your-feature-name
```

Then create a Pull Request on GitHub.

## 📝 Coding Standards

### C++ Style

```cpp
// Use C++20 features
// Use snake_case for variables and functions
// Use PascalCase for classes
// Use UPPER_CASE for constants

namespace opengalaxy {

class MyClass {
public:
    MyClass();
    ~MyClass();

    void doSomething();
    int getValue() const { return value_; }

private:
    int value_;
    QString name_;
};

} // namespace opengalaxy
```

### Qt Conventions

```cpp
// Use Qt types
QString text;
QList<int> numbers;

// Use signals and slots
connect(button, &QPushButton::clicked, this, &MyClass::onButtonClicked);

// Use Qt parent-child ownership
auto* widget = new QWidget(parent);  // parent will delete
```

### Error Handling

```cpp
// Use Result<T> for operations that can fail
util::Result<GameInfo> getGame(const QString& id) {
    if (id.isEmpty()) {
        return util::Result<GameInfo>::error("Invalid game ID");
    }
    
    GameInfo game = fetchGame(id);
    return util::Result<GameInfo>::success(game);
}

// Use callbacks for async operations
void fetchLibrary(std::function<void(util::Result<std::vector<GameInfo>>)> callback) {
    // Async operation
}
```

### Logging

```cpp
#include "opengalaxy/util/log.h"

LOG_DEBUG("Debug message");
LOG_INFO("Info message");
LOG_WARNING("Warning message");
LOG_ERROR("Error message");
LOG_CRITICAL("Critical message");
```

### Documentation

```cpp
/**
 * @brief Brief description
 * 
 * Detailed description of what this does.
 * 
 * @param param1 Description of param1
 * @param param2 Description of param2
 * @return Description of return value
 */
int myFunction(int param1, const QString& param2);
```

## 🧪 Testing

### Unit Tests

```cpp
#include <QtTest/QtTest>

class MyTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Setup before all tests
    }

    void testSomething() {
        QCOMPARE(1 + 1, 2);
        QVERIFY(true);
    }

    void cleanupTestCase() {
        // Cleanup after all tests
    }
};

QTEST_MAIN(MyTests)
#include "my_tests.moc"
```

### Test Coverage

Aim for >80% code coverage:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Coverage
cmake --build . -j
ctest
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage
```

## 📚 Documentation

- Update README.md for user-facing changes
- Update BUILD.md for build process changes
- Add inline comments for complex logic
- Update API documentation

## 🔍 Code Review

All submissions require review. We use GitHub pull requests:

1. **Automated Checks**: CI must pass
2. **Code Review**: At least one approval required
3. **Testing**: All tests must pass
4. **Documentation**: Must be updated if needed

### Review Checklist

- [ ] Code follows style guidelines
- [ ] Tests added/updated
- [ ] Documentation updated
- [ ] No compiler warnings
- [ ] No memory leaks (valgrind)
- [ ] Commit messages follow conventions

## 🚀 Release Process

1. Update version in CMakeLists.txt
2. Update CHANGELOG.md
3. Create release branch
4. Tag release
5. Build packages
6. Publish release notes

## 📋 Project Structure

```
opengalaxy/
├── core/              # Core library
│   ├── include/       # Public API headers
│   └── src/           # Implementation
├── ui/                # Qt GUI
│   └── qt/            # Qt widgets
├── cli/               # CLI tool
├── tests/             # Unit tests
├── docs/              # Documentation
└── scripts/           # Build scripts
```

## 🎯 Areas Needing Help

- [ ] macOS testing and support
- [ ] Windows installer improvements
- [ ] Additional runner support
- [ ] Translations
- [ ] Documentation
- [ ] UI/UX improvements
- [ ] Performance optimization

## 💬 Communication

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: General questions and ideas
- **Pull Requests**: Code contributions

## 📜 License

By contributing, you agree that your contributions will be licensed under the Apache License 2.0.

## 🙏 Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md
- Mentioned in release notes
- Credited in the about dialog

Thank you for contributing to OpenGalaxy! 🚀
