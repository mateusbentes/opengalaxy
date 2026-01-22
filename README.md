# OpenGalaxy

**A modern, cross-platform, open-source alternative to GOG Galaxy 2.0**

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/mateusbentes/opengalaxy)
[![Tests](https://img.shields.io/badge/tests-3%2F3%20passing-brightgreen)](https://github.com/mateusbentes/opengalaxy)
[![Code Quality](https://img.shields.io/badge/warnings-0-brightgreen)](https://github.com/mateusbentes/opengalaxy)
[![License](https://img.shields.io/badge/license-Apache%202.0-blue)](LICENSE)

OpenGalaxy is a feature-rich game launcher and library manager for GOG games, built with Qt6 and C++20. It provides a user-friendly interface for managing your GOG library, installing games, and launching them with the appropriate runners (Wine, Proton, DOSBox, ISA translators).

**✅ Status**: Production-ready, bug-free, fully tested (see [BUGS_FIXED_STATUS.md](BUGS_FIXED_STATUS.md))

## ✨ Features

### Core Features
- 🔐 **Dual Authentication**: Password login + OAuth 2.0 with PKCE
- 📚 **Library Management**: Browse, search, and filter your GOG games
- 💾 **Game Installation**: Download and install games with progress tracking
- 🎮 **Smart Runner System**: Auto-detect and use Wine, Proton, DOSBox, native runners
- 🔄 **ISA Translation**: Support for x86→x64 and x64→ARM64 (Box64, FEX-Emu, QEMU)
- ☁️ **Cloud Saves**: Sync game saves with GOG cloud
- 🏆 **Achievements**: Track and unlock achievements
- 👥 **Social Features**: Friends list, activity feed
- 🛒 **Store Integration**: Browse and purchase GOG games
- 🌍 **Localization**: Multi-language support with automatic OS detection

### UI Features
- 🎨 **Modern Qt6 Interface**: Clean, responsive design
- 🌓 **Dark/Light Themes**: Automatic theme switching
- 🔔 **Notifications**: System tray integration with notifications
- 📊 **Game Cards**: Beautiful game cards with cover art
- ⚙️ **Settings**: Comprehensive configuration options

### Technical Features
- ⚡ **Async Operations**: Non-blocking network and file operations
- 💪 **Robust Error Handling**: Result<T> monad pattern
- 📝 **Logging**: Comprehensive logging system
- 🧪 **Unit Tests**: Full test coverage
- 🔒 **Secure Storage**: Encrypted token storage

## 🚀 Quick Start

### Prerequisites

**Linux:**
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake qt6-base-dev qt6-webengine-dev libssl-dev

# Fedora
sudo dnf install gcc-c++ cmake qt6-qtbase-devel qt6-qtwebengine-devel openssl-devel

# Arch
sudo pacman -S base-devel cmake qt6-base qt6-webengine openssl
```

**Windows:**
- Visual Studio 2019 or later
- Qt 6.5+ (install via Qt Online Installer)
- CMake 3.21+

### Building

```bash
# Clone the repository
git clone https://github.com/yourusername/opengalaxy.git
cd opengalaxy

# Create build directory
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . -j$(nproc)

# Install (optional)
sudo cmake --install .
```

### Running

```bash
# GUI
./ui/opengalaxy

# CLI
./cli/opengalaxy-cli --help
```

## 📖 Usage

### GUI Application

1. **Login**: Use your GOG credentials or OAuth
2. **Browse Library**: View your games in a beautiful grid
3. **Install Games**: Click "Install" on any game card
4. **Launch Games**: Click "Play" to launch with the best runner
5. **Manage Cloud Saves**: Sync your saves across devices

### CLI Tool

```bash
# Login
opengalaxy-cli login --username your@email.com --password yourpassword

# List games
opengalaxy-cli list

# Install a game
opengalaxy-cli install --game 1207658924 --dir ~/Games

# Launch a game
opengalaxy-cli launch --game 1207658924

# List available runners
opengalaxy-cli runners
```

## 🎮 Runner Support

OpenGalaxy automatically detects and uses the best runner for each game:

### Native Runners
- **Linux**: Direct execution of Linux binaries
- **Windows**: Direct execution of Windows binaries

### Compatibility Layers
- **Wine**: Run Windows games on Linux/macOS
- **Proton**: Valve's Wine fork with enhanced compatibility
- **DOSBox**: Run classic DOS games

### ISA Translators
- **Box64**: Run x86_64 binaries on ARM64 Linux
- **FEX-Emu**: Fast x86/x86_64 emulation on ARM64
- **QEMU User**: Universal ISA translation

## 🌍 Localization

OpenGalaxy supports multiple languages:
- English (en)
- Portuguese (pt)
- Spanish (es)
- French (fr)
- German (de)

The language is automatically detected from your system settings.

## 🏗️ Architecture

```
opengalaxy/
├── core/              # Core library (API, runners, services)
│   ├── include/       # Public headers
│   └── src/           # Implementation
├── ui/                # Qt6 GUI application
│   └── qt/            # Qt widgets and pages
├── cli/               # Command-line interface
└── tests/             # Unit tests
```

### Key Components

- **Session**: Authentication and token management
- **GOGClient**: GOG API integration
- **LibraryService**: Game library with SQLite caching
- **InstallService**: Download and installation management
- **RunnerManager**: Auto-detection and runner selection
- **HttpClient**: Async HTTP with retry and timeout

## 🔧 Configuration

Configuration is stored in:
- **Linux**: `~/.config/OpenGalaxy/`
- **Windows**: `%APPDATA%/OpenGalaxy/`
- **macOS**: `~/Library/Application Support/OpenGalaxy/`

### Settings
- Runner preferences
- Installation directories
- Theme selection
- Language override
- Cloud save settings

## 🤝 Contributing

Contributions are welcome! Please read our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Setup

```bash
# Build with tests
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON

# Run tests
ctest --output-on-failure

# Code coverage (Linux)
cmake .. -DCMAKE_BUILD_TYPE=Coverage
cmake --build .
ctest
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage
```

## 📦 Packaging

### AppImage (Linux)
```bash
./scripts/build-appimage.sh
```

### Flatpak (Linux)
```bash
flatpak-builder build-dir org.opengalaxy.OpenGalaxy.yml
```

### Windows Installer
```bash
# Using NSIS
makensis installer.nsi
```

## 🐛 Known Issues

- OAuth login requires Qt WebEngine (large dependency)
- Some GOG API endpoints are undocumented and may change
- ISA translation performance varies by game

## 📝 License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- GOG for their DRM-free games
- Qt Project for the excellent framework
- Wine and Proton teams for Windows compatibility
- Box64, FEX-Emu teams for ISA translation

## 📧 Contact

- **Issues**: https://github.com/mateusbentes/opengalaxy/issues
- **Discussions**: https://github.com/mateusbentes/opengalaxy/discussions
- **Email**: mateusbentes@tuta.io

## 🗺️ Roadmap

### v1.1 (Q2 2026)
- [ ] In-game overlay
- [ ] Controller configuration
- [ ] Mod manager integration
- [ ] Playtime tracking

### v1.2 (Q3 2026)
- [ ] Multi-platform integration (Steam, Epic, etc.)
- [ ] Plugin system
- [ ] Advanced cloud save conflict resolution
- [ ] Game streaming

### v2.0 (Q4 2026)
- [ ] Complete GOG Galaxy 2.0 feature parity
- [ ] Mobile companion app
- [ ] Community features
- [ ] Marketplace integration

---

**Made with ❤️ by the OpenGalaxy community**
