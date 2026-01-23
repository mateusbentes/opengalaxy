# Changelog

All notable changes to OpenGalaxy will be documented in this file.

## [1.0.0] - 2026-01-22

### Complete Rewrite - Production Ready

This is a complete rewrite of OpenGalaxy with modern architecture and full feature set.

#### Added

**Core Library**
- Session management with OAuth 2.0 + PKCE and password authentication
- Async HTTP client with timeout, retry, and progress tracking
- GOG API client for library, store, achievements, cloud saves
- Runner system with auto-detection (Wine, Proton, DOSBox, ISA translators)
- ISA translation support (Box64, FEX-Emu, QEMU) for x86→x64 and x64→ARM64
- Library service with SQLite caching
- Install service with resumable downloads and extraction
- Comprehensive logging system
- Result&lt;T&gt; monad for error handling

**UI Application**
- Modern Qt6 interface with dark/light themes
- Login page with dual authentication methods
- Library page with game grid, search, and filters
- Store page for browsing GOG games
- Friends page for social features
- Settings page with runner configuration
- Game cards with cover art and actions
- Game details dialog with runner selection
- Cloud saves dialog with conflict resolution
- Notification system with system tray integration
- Localization support (EN, PT, ES, FR, DE)

**CLI Tool**
- Command-line interface for automation
- Login, list, install, launch, runners commands
- Progress tracking for downloads
- Scriptable operations

**Testing**
- Unit tests for core functionality
- API tests with mock responses
- Runner tests with platform detection
- Code coverage support

**Documentation**
- Comprehensive README with features and usage
- Detailed BUILD guide for all platforms
- CONTRIBUTING guide for developers
- Quick start guide
- Code examples and API documentation

**Build System**
- Modern CMake 3.21+ with proper targets
- Cross-platform support (Linux, Windows, macOS)
- Qt6 integration with AUTOMOC
- Packaging support (AppImage, Flatpak, NSIS, DMG)
- Build script for easy compilation

#### Changed
- Complete architecture redesign
- Moved from monolithic to modular structure
- Separated core library from UI
- Async-first design (no UI blocking)
- Proper header/implementation separation

#### Removed
- Old monolithic mainwindow.cpp (moved to old_code_backup/)
- .cpp includes (replaced with proper headers)
- Synchronous blocking calls
- Hardcoded credentials and paths

#### Fixed
- Memory leaks in network and UI code
- Compilation errors and missing headers
- ODR violations from .cpp includes
- Qt MOC generation issues
- Thread safety in async operations

#### Security
- Secure token storage with OS keychain integration
- OAuth 2.0 with PKCE for authentication
- HTTPS for all API calls
- Input validation and sanitization

### Technical Details

**Languages & Frameworks**
- C++20
- Qt 6.5+
- CMake 3.21+

**Platforms**
- Linux (Ubuntu 22.04+, Fedora 36+, Arch)
- Windows (10/11 with MSVC 2019+)
- macOS (12+)

**Architecture Support**
- x86_64 (native)
- ARM64 (with ISA translation)
- x86 (32-bit with compatibility layers)

**Dependencies**
- Qt6 Core, Network, Widgets, Gui, Sql, WebEngine
- OpenSSL 1.1+
- SQLite 3
- Standard C++ library

### Migration Guide

If upgrading from old version:

1. Backup your data:
   ```bash
   cp -r ~/.config/OpenGalaxy ~/.config/OpenGalaxy.backup
   ```

2. Rebuild from source:
   ```bash
   ./build.sh
   ```

3. Session will be preserved (OAuth tokens migrated automatically)

### Known Issues

- OAuth login requires Qt WebEngine (large dependency)
- Some GOG API endpoints may change without notice
- ISA translation performance varies by game and translator
- macOS code signing not yet implemented

### Contributors

- Initial rewrite and architecture design
- Core library implementation
- UI/UX design and implementation
- Documentation and testing

---

## [0.1.0] - Previous Version

Initial prototype (moved to old_code_backup/)

### Issues in Previous Version
- Monolithic architecture
- Memory leaks
- Compilation errors
- Missing features
- No proper error handling
- Blocking UI operations

---

**Legend:**
- Added: New features
- Changed: Changes in existing functionality
- Deprecated: Soon-to-be removed features
- Removed: Removed features
- Fixed: Bug fixes
- Security: Security improvements
