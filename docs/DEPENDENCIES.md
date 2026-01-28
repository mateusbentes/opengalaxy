# OpenGalaxy Dependencies Guide

This document lists all external tools and libraries required to run OpenGalaxy and play different types of games.

## Table of Contents

1. [Build Dependencies](#build-dependencies)
2. [Runtime Dependencies](#runtime-dependencies)
3. [Game-Specific Tools](#game-specific-tools)
4. [Installation Instructions](#installation-instructions)
5. [Troubleshooting](#troubleshooting)

## Build Dependencies

Required to compile OpenGalaxy from source.

### Core Build Tools

- **CMake** 3.21+
- **C++ Compiler** (GCC 11+, Clang 13+, or MSVC 2019+)
- **Qt6** 6.5+ (Core, Network, Widgets, Gui, Sql, WebEngine)
- **OpenSSL** 1.1+
- **SQLite** 3

### Linux

```bash
# Ubuntu/Debian
sudo apt install cmake g++ qt6-base-dev qt6-webengine-dev libssl-dev sqlite3

# Fedora
sudo dnf install cmake gcc-c++ qt6-qtbase-devel qt6-qtwebengine-devel openssl-devel sqlite-devel

# Arch
sudo pacman -S cmake gcc qt6-base qt6-webengine openssl sqlite
```

### macOS

```bash
brew install cmake qt@6 openssl sqlite
```

### Windows

- Download Qt6 from https://www.qt.io/download
- Install Visual Studio Build Tools or MSVC
- Install OpenSSL from https://slproweb.com/products/Win32OpenSSL.html

## Runtime Dependencies

Required to run OpenGalaxy.

### Core Libraries

- **Qt6 Runtime** (automatically included with Qt installation)
- **OpenSSL** (for HTTPS connections)
- **SQLite** (for game library database)

### Linux

```bash
# Ubuntu/Debian
sudo apt install libqt6core6 libqt6gui6 libqt6network6 libqt6sql6 libqt6webenginecore6 libssl3 sqlite3

# Fedora
sudo dnf install qt6-qtbase qt6-qtwebengine openssl sqlite

# Arch
sudo pacman -S qt6-base qt6-webengine openssl sqlite
```

## Game-Specific Tools

Tools needed to play different types of games.

### Windows Games (Required)

**Wine or Proton** - Compatibility layer to run Windows games on Linux/macOS

#### Wine

```bash
# Ubuntu/Debian
sudo apt install wine wine32 wine64

# Fedora
sudo dnf install wine

# Arch
sudo pacman -S wine

# macOS
brew install wine
```

#### Wine Variants (Optional but Recommended)

**Wine-Staging** - Community-maintained Wine fork with experimental features

```bash
# Ubuntu/Debian
sudo apt install wine-staging

# Fedora
sudo dnf install wine-staging

# Arch
sudo pacman -S wine-staging

# macOS
brew install wine-staging
```

**Wine-TKG** - Optimized Wine build for gaming

```bash
# Download from: https://github.com/Frogging-Family/wine-tkg-git
# Follow installation instructions on GitHub
```

#### Proton (Recommended for Windows Games)

**Proton** - Valve's compatibility layer (comes with Steam)

```bash
# Install Steam first, then Proton is available automatically
# Or download from: https://github.com/ValveSoftware/Proton/releases
```

**Proton-GE** - Community-enhanced Proton (Best for games)

```bash
# Download from: https://github.com/GloriousEggroll/proton-ge-custom/releases
# Extract to: ~/.steam/steam/compatibilitytools.d/
# Or: ~/.local/share/Steam/compatibilitytools.d/
```

### DOS Games (Required)

**DOSBox** - DOS emulator for running classic DOS games

```bash
# Ubuntu/Debian
sudo apt install dosbox

# Fedora
sudo dnf install dosbox

# Arch
sudo pacman -S dosbox

# macOS
brew install dosbox

# Windows
choco install dosbox
```

**DOSBox-X** - Enhanced DOSBox with more features

```bash
# Download from: https://dosbox-x.com/
# Or via package manager:

# Ubuntu/Debian
sudo apt install dosbox-x

# Fedora
sudo dnf install dosbox-x

# Arch
sudo pacman -S dosbox-x

# macOS
brew install dosbox-x
```

### DOS Game Installer Extraction (Required for GOG DOS Games)

**innoextract** - Extracts Inno Setup installers (used by GOG)

```bash
# Ubuntu/Debian
sudo apt install innoextract

# Fedora
sudo dnf install innoextract

# Arch
sudo pacman -S innoextract

# macOS
brew install innoextract

# Windows
choco install innoextract
```

### ISA Translation (Optional, for ARM64 Macs)

**FEX-Emu** - x86/x64 emulator for ARM64 systems

```bash
# Download from: https://github.com/FEX-Emu/FEX/releases
# Follow installation instructions
```

**Box64** - x86_64 emulator for ARM64 systems

```bash
# Download from: https://github.com/ptitSeb/box64
# Follow compilation instructions
```

**QEMU** - Generic emulator (fallback option)

```bash
# Ubuntu/Debian
sudo apt install qemu-user-static

# Fedora
sudo dnf install qemu-user-static

# Arch
sudo pacman -S qemu-user-static

# macOS
brew install qemu
```

## Installation Instructions

### Quick Setup (Ubuntu/Debian)

```bash
# Install all dependencies at once
sudo apt install cmake g++ qt6-base-dev qt6-webengine-dev libssl-dev sqlite3 \
  wine wine-staging dosbox innoextract

# Optional: Proton-GE for better Windows game compatibility
# Download from: https://github.com/GloriousEggroll/proton-ge-custom/releases
```

### Quick Setup (Fedora)

```bash
# Install all dependencies at once
sudo dnf install cmake gcc-c++ qt6-qtbase-devel qt6-qtwebengine-devel \
  openssl-devel sqlite-devel wine wine-staging dosbox innoextract

# Optional: Proton-GE for better Windows game compatibility
# Download from: https://github.com/GloriousEggroll/proton-ge-custom/releases
```

### Quick Setup (Arch)

```bash
# Install all dependencies at once
sudo pacman -S cmake gcc qt6-base qt6-webengine openssl sqlite \
  wine wine-staging dosbox innoextract

# Optional: Proton-GE for better Windows game compatibility
# Download from: https://github.com/GloriousEggroll/proton-ge-custom/releases
```

### Quick Setup (macOS)

```bash
# Install all dependencies at once
brew install cmake qt@6 openssl sqlite wine wine-staging dosbox innoextract

# Optional: Proton-GE for better Windows game compatibility
# Download from: https://github.com/GloriousEggroll/proton-ge-custom/releases
```

### Quick Setup (Windows)

1. Install Visual Studio Build Tools or MSVC
2. Install Qt6 from https://www.qt.io/download
3. Install OpenSSL from https://slproweb.com/products/Win32OpenSSL.html
4. Install dependencies via Chocolatey:

```powershell
choco install cmake wine dosbox innoextract
```

## Dependency Matrix

| Game Type | Windows | Linux | macOS | Required Tools |
|-----------|---------|-------|-------|-----------------|
| Windows Games | ✅ Native | ✅ Wine/Proton | ⚠️ Wine | Wine or Proton |
| DOS Games | ✅ DOSBox | ✅ DOSBox | ✅ DOSBox | DOSBox |
| DOS + Windows Installer | ✅ Wine→DOSBox | ✅ Wine→DOSBox | ✅ Wine→DOSBox | Wine + DOSBox + innoextract |
| Linux Games | ❌ No | ✅ Native | ⚠️ Maybe | None |
| macOS Games | ❌ No | ❌ No | ✅ Native | None |

## Troubleshooting

### Wine/Proton Not Found

**Error:** "Wine/Proton not found. Please install Wine or Proton..."

**Solution:**
```bash
# Install Wine
sudo apt install wine

# Or install Proton-GE
# Download from: https://github.com/GloriousEggroll/proton-ge-custom/releases
# Extract to: ~/.steam/steam/compatibilitytools.d/
```

### DOSBox Not Found

**Error:** "DOSBox not found. Please install DOSBox..."

**Solution:**
```bash
# Install DOSBox
sudo apt install dosbox

# Or DOSBox-X for more features
sudo apt install dosbox-x
```

### innoextract Not Found

**Error:** "innoextract not found, will use Wine/Proton to install"

**Solution:**
```bash
# Install innoextract for automatic DOS game extraction
sudo apt install innoextract

# Without innoextract, installation will fall back to Wine
# (slower and may require display server)
```

### Wine Prefix Issues

**Error:** Game won't run or crashes with Wine

**Solution:**
```bash
# Verify Wine is properly installed
wine --version

# Check Wine prefix
ls ~/.wine/drive_c/

# Reinstall game to create fresh Wine prefix
```

### Proton Not Detected

**Error:** "Proton not found"

**Solution:**
```bash
# Verify Steam is installed
which steam

# Check Proton location
ls ~/.steam/steam/compatibilitytools.d/
ls ~/.local/share/Steam/compatibilitytools.d/

# Download Proton-GE manually if needed
# https://github.com/GloriousEggroll/proton-ge-custom/releases
```

## Optional Enhancements

### DXVK (Better DirectX Support)

```bash
# Ubuntu/Debian
sudo apt install dxvk

# Fedora
sudo dnf install dxvk

# Arch
sudo pacman -S dxvk
```

### VKD3D (Direct3D 12 Support)

```bash
# Ubuntu/Debian
sudo apt install vkd3d

# Fedora
sudo dnf install vkd3d

# Arch
sudo pacman -S vkd3d
```

### MangoHUD (Performance Monitoring)

```bash
# Ubuntu/Debian
sudo apt install mangohud

# Fedora
sudo dnf install mangohud

# Arch
sudo pacman -S mangohud
```

## See Also

- [BUILD.md](BUILD.md) - Build instructions
- [INSTALLATION_PROCESS.md](INSTALLATION_PROCESS.md) - Game installation guide
- [WINE_PROTON_TOOLS.md](WINE_PROTON_TOOLS.md) - Wine/Proton configuration
- [DOS_GAMES_SUPPORT.md](DOS_GAMES_SUPPORT.md) - DOS game support details
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - General troubleshooting
