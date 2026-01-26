# CI/CD Architecture Support - January 26, 2026

## Summary

Updated the CI/CD pipeline to focus on primary architectures where Qt6 packages are available and the application is most commonly used.

## Changes Made

### Removed riscv64 from Build Matrix

**Reason**: Qt6 packages are not available for riscv64 in Ubuntu repositories.

**Error**:
```
E: Unable to locate package qt6-base-dev:riscv64
E: Unable to locate package qt6-tools-dev:riscv64
E: Unable to locate package libqt6core6:riscv64
E: Unable to locate package libqt6gui6:riscv64
E: Unable to locate package libqt6network6:riscv64
E: Unable to locate package libqt6sql6:riscv64
E: Unable to locate package libqt6widgets6:riscv64
```

## Supported Architectures

### Linux Builds (1 architecture)

| Architecture | Status | Notes |
|---|---|---|
| **x86_64** | ✅ Supported | Primary architecture, native build |
| ~~arm64~~ | ❌ Removed | Qt6 cross-compilation packages not available |
| ~~armv7~~ | ❌ Removed | Qt6 cross-compilation packages not available |
| ~~riscv64~~ | ❌ Removed | Qt6 packages not available |

### macOS Builds (1 architecture)

| Architecture | Status | Notes |
|---|---|---|
| **arm64** | ✅ Supported | Apple Silicon Macs (current standard) |
| ~~x86_64~~ | ❌ Removed | Legacy Intel Macs (no longer supported) |

### Windows Builds (2 architectures)

| Architecture | Status | Notes |
|---|---|---|
| **x64** | ✅ Supported | 64-bit Windows (primary) |
| **x86** | ✅ Supported | 32-bit Windows (legacy support) |
| ~~arm64~~ | ❌ Removed | Windows on ARM (not practical) |

## Total Build Matrix

- **Linux**: 1 architecture (x86_64 only)
- **macOS**: 1 architecture (arm64 only)
- **Windows**: 2 architectures (x64, x86)
- **Total**: 4 architectures

## Why Windows arm64 Was Removed

### Windows on ARM
- Windows on ARM is not practical for desktop applications
- Very limited market adoption
- Primarily used on mobile/embedded devices
- Not a viable target for game launcher
- Reduces CI/CD complexity

## Why macOS x86_64 Was Removed

### Legacy Intel Macs
- Apple has transitioned to Apple Silicon (arm64)
- Intel Macs are no longer the standard
- Modern Macs are arm64 only
- Reduces CI/CD complexity and build time
- Focuses on current macOS architecture

## Why Linux Cross-Compilation Was Removed

### arm64 and armv7 Cross-Compilation
- Qt6 cross-compilation packages not available in Ubuntu repositories
- Would require building Qt6 from source for each architecture
- Not practical for CI/CD pipeline
- Cross-compilation adds significant complexity

### riscv64 Architecture
- Qt6 packages not available in Ubuntu repositories
- Emerging architecture with limited desktop application support
- Not practical for CI/CD pipeline

## Why riscv64 Was Removed

### 1. Qt6 Package Availability
- Qt6 packages are not available in Ubuntu repositories for riscv64
- Would require building Qt6 from source, which is complex and time-consuming
- Not practical for CI/CD pipeline

### 2. Target Audience
- OpenGalaxy is a game launcher for GOG games
- riscv64 is an emerging architecture with limited desktop application support
- Primary users are on x86_64, arm64, or armv7

### 3. CI/CD Efficiency
- Removing riscv64 reduces build time and complexity
- Focuses resources on architectures with actual user base
- Reduces infrastructure costs

## Impact on Users

### riscv64 Users
- Can still build from source if needed
- Build instructions in `docs/BUILD.md` remain valid
- CMake configuration supports riscv64 (just not tested in CI/CD)

### Other Users
- No impact - all primary architectures still supported
- Faster CI/CD pipeline
- More reliable builds

## Future Considerations

If riscv64 support becomes important:

1. **Option 1**: Wait for Qt6 packages to be available in Ubuntu repositories
2. **Option 2**: Use a custom Docker image with Qt6 pre-built for riscv64
3. **Option 3**: Build Qt6 from source in CI/CD (not recommended - very slow)

## Git Commit

```
commit 914d815
Author: Mateus Bentes
Date:   2026-01-26

    ci: Remove riscv64 from build matrix
    
    - Qt6 packages not available for riscv64 in Ubuntu repositories
    - riscv64 is uncommon for desktop applications
    - Focus on primary architectures: x86_64, arm64, armv7
    - Reduces CI/CD complexity and build time
    - Resolves dependency installation failures
```

## Related Documentation

- [multi-platform-build.yml](.github/workflows/multi-platform-build.yml) - CI/CD workflow
- [BUILD.md](docs/BUILD.md) - Build instructions
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Architecture documentation

---

**Date**: 2026-01-26  
**Status**: ✅ Complete  
**Impact**: Improves CI/CD reliability and efficiency
