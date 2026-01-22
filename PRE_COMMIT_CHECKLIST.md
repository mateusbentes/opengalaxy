# Pre-Commit Checklist

Before committing and pushing, verify:

## ✅ Code Quality

- [ ] All files use proper includes (`opengalaxy/` namespace)
- [ ] No compilation errors
- [ ] No memory leaks (checked with valgrind if possible)
- [ ] Proper error handling with Result<T>
- [ ] Logging added where appropriate
- [ ] No hardcoded credentials or paths

## ✅ Documentation

- [ ] README.md is up to date
- [ ] BUILD.md has correct build instructions
- [ ] CHANGELOG.md reflects changes
- [ ] Code comments added for complex logic
- [ ] API documentation updated

## ✅ Build System

- [ ] CMakeLists.txt includes all source files
- [ ] Build script works: `./build.sh`
- [ ] Cross-platform compatibility maintained
- [ ] Dependencies documented

## ✅ Testing

- [ ] Unit tests pass: `cd build && ctest`
- [ ] Manual testing done for UI changes
- [ ] CLI commands tested
- [ ] No regressions introduced

## ✅ Git

- [ ] .gitignore is comprehensive
- [ ] No sensitive data in commits
- [ ] Commit message follows conventions
- [ ] Branch is up to date with main

## ✅ Files to Review

### New Files
```
CMakeLists.txt
build.sh
core/include/opengalaxy/**/*.h
core/src/**/*.cpp
ui/qt/**/*
cli/main.cpp
tests/**/*.cpp
BUILD.md
CONTRIBUTING.md
CHANGELOG.md
QUICKSTART.md
```

### Modified Files
```
README.md
LICENSE
.gitignore
```

## 🚀 Ready to Commit?

If all checkboxes are checked, run:

```bash
./commit-and-push.sh
```

Or manually:

```bash
# Stage all files
git add .

# Commit
git commit -m "feat: Complete rewrite - Production-ready GOG Galaxy 2.0 alternative"

# Push
git push origin main
```

## 📝 Commit Message Template

```
feat: Complete rewrite - Production-ready GOG Galaxy 2.0 alternative

Complete architectural rewrite of OpenGalaxy with modern C++20 and Qt6.

BREAKING CHANGE: Complete rewrite from scratch.

- New architecture with clean separation
- Core library with proper headers
- Qt6 UI with modern design
- CLI tool for automation
- Comprehensive documentation
- Cross-platform support
- ISA translation support
```

## 🎯 Post-Commit

After pushing:

1. Verify on GitHub that all files are there
2. Check CI/CD pipeline (if configured)
3. Update project board/issues
4. Announce the release
5. Tag the release: `git tag v1.0.0 && git push --tags`
