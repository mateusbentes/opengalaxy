# First Commit Message

## Title
```
feat: Complete rewrite - Production-ready GOG Galaxy 2.0 alternative
```

## Body
```
Complete architectural rewrite of OpenGalaxy with modern C++20 and Qt6.

BREAKING CHANGE: Complete rewrite from scratch. Previous code moved to backup.

### New Architecture
- Clean separation: core library, UI, CLI, tests
- Proper header/implementation structure (include/ and src/)
- Modern C++20 with Qt6 framework
- Cross-platform support (Linux, Windows, macOS)

### Core Features
- Session management with OAuth 2.0 PKCE + password authentication
- Async HTTP client with timeout, retry, and progress tracking
- GOG API client integration (library, store, achievements, cloud saves)
- Runner system with auto-detection (Wine, Proton, DOSBox, ISA translators)
- ISA translation support (Box64, FEX-Emu, QEMU) for x86→x64 and x64→ARM64
- Library service with SQLite caching
- Install service with resumable downloads
- Comprehensive logging system
- Result<T> monad for error handling

### UI Features
- Modern Qt6 interface with dark/light themes
- Login page with dual authentication methods
- Library page with game grid, search, and filters
- Store page for browsing GOG games
- Friends page for social features
- Settings page with runner configuration
- Game cards with cover art and actions
- Notification system with system tray integration
- Localization support (EN, PT, ES, FR, DE)

### CLI Tool
- Command-line interface for automation
- Commands: login, list, install, launch, runners
- Progress tracking for downloads
- Scriptable operations

### Testing & Documentation
- Unit tests for core functionality
- Comprehensive README with features and usage
- Detailed BUILD guide for all platforms
- CONTRIBUTING guide for developers
- Quick start guide

### Build System
- Modern CMake 3.21+ with proper targets
- Qt6 integration with AUTOMOC
- Packaging support (AppImage, Flatpak, NSIS, DMG)
- Build script for easy compilation

### Technical Stack
- C++20
- Qt 6.5+
- CMake 3.21+
- Platforms: Linux, Windows, macOS
- Architectures: x86_64, ARM64

### What Changed
- Removed monolithic architecture
- Removed .cpp includes (replaced with proper headers)
- Removed synchronous blocking calls
- Removed hardcoded credentials and paths
- Fixed memory leaks
- Fixed compilation errors
- Added proper error handling
- Added async-first design

### Migration
Old code preserved in git history. This is a fresh start with
production-ready architecture.

Closes #1 (if you have an issue for the rewrite)
```

## Files Changed Summary
```
New files:
- CMakeLists.txt (root build configuration)
- build.sh (build automation script)
- core/ (complete core library)
  - include/opengalaxy/ (public API headers)
  - src/ (implementations)
- ui/ (Qt6 GUI application)
- cli/ (command-line tool)
- tests/ (unit tests)
- BUILD.md (build instructions)
- CONTRIBUTING.md (developer guide)
- CHANGELOG.md (version history)
- QUICKSTART.md (quick start guide)

Modified files:
- README.md (complete rewrite)
- LICENSE (preserved)
- .gitignore (updated for new structure)

Removed:
- Old monolithic UI code (preserved in git history)
```
