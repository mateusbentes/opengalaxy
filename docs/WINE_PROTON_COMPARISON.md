# Wine vs Proton Comparison

## Overview

OpenGalaxy supports multiple Windows compatibility layers for running Windows games on Linux. This guide compares Wine, Wine-Staging, Proton, and Proton-GE.

## Quick Comparison

| Feature | Wine | Wine-Staging | Proton | Proton-GE |
|---------|------|--------------|--------|-----------|
| **Best For** | General apps | Testing | Steam games | GOG games |
| **DirectX Support** | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Game Compatibility** | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Ease of Use** | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Performance** | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Setup Complexity** | Easy | Easy | Medium | Medium |
| **Codec Support** | Limited | Good | Good | Excellent |
| **Auto-Updates** | Yes | Yes | Yes (Steam) | Manual |

## Detailed Comparison

### Wine (Standard)

**What is it**: Open-source Windows compatibility layer

**Pros**:
- ✅ Easy to install from package manager
- ✅ Stable and well-tested
- ✅ Works for most applications
- ✅ Good documentation
- ✅ Active community

**Cons**:
- ❌ Limited DirectX 11/12 support
- ❌ Manual dependency installation
- ❌ Slower updates
- ❌ Some games don't work

**Installation**:
```bash
# Ubuntu/Debian
sudo apt install wine

# Fedora
sudo dnf install wine

# Arch
sudo pacman -S wine
```

**Best For**:
- General Windows applications
- Older games (DirectX 9 and below)
- Users who want stable, tested software

### Wine-Staging

**What is it**: Wine with experimental patches

**Pros**:
- ✅ Better game compatibility
- ✅ Experimental features
- ✅ Faster DirectX performance
- ✅ More bug fixes
- ✅ Easy to install

**Cons**:
- ❌ Less stable than Wine
- ❌ Experimental features may break
- ❌ Still manual dependency management

**Installation**:
```bash
# Ubuntu/Debian
sudo apt install wine-staging

# Arch
sudo pacman -S wine-staging
```

**Best For**:
- Testing new features
- Games that don't work with standard Wine
- Users who want cutting-edge compatibility

### Proton

**What is it**: Valve's Wine fork for Steam

**Pros**:
- ✅ Excellent game compatibility
- ✅ DXVK built-in (DirectX to Vulkan)
- ✅ Automatic dependency installation
- ✅ Optimized for gaming
- ✅ Regular updates via Steam

**Cons**:
- ❌ Requires Steam installation
- ❌ Designed for Steam games
- ❌ Less flexible than Wine
- ❌ Larger download size

**Installation**:
```bash
# Install Steam
sudo apt install steam

# Proton is installed automatically with Steam
# Located in: ~/.steam/steam/steamapps/common/Proton*/
```

**Best For**:
- Steam games
- Users who already have Steam
- Games requiring DirectX 11/12
- Maximum compatibility

### Proton-GE (GloriousEggroll)

**What is it**: Community-enhanced Proton with extra patches

**Pros**:
- ✅ Best game compatibility
- ✅ Additional codec support (H.264, etc.)
- ✅ Extra game-specific fixes
- ✅ Works with non-Steam games
- ✅ Frequent updates
- ✅ Community-driven

**Cons**:
- ❌ Manual installation
- ❌ Manual updates
- ❌ Requires Steam directory structure
- ❌ Larger download (~1GB)

**Installation**:
```bash
# Download latest release
wget https://github.com/GloriousEggroll/proton-ge-custom/releases/download/GE-Proton8-25/GE-Proton8-25.tar.gz

# Extract to compatibility tools directory
mkdir -p ~/.steam/steam/compatibilitytools.d
tar -xf GE-Proton8-25.tar.gz -C ~/.steam/steam/compatibilitytools.d/

# OpenGalaxy will auto-detect it
```

**Best For**:
- **GOG games** (recommended!)
- Games with video cutscenes
- Maximum compatibility
- Latest game releases
- Users who want the best experience

## OpenGalaxy Detection Order

OpenGalaxy automatically detects and prefers runners in this order:

1. **Proton-GE** ⭐⭐⭐⭐⭐ (best for GOG games)
2. **Proton** ⭐⭐⭐⭐
3. **Wine-Staging** ⭐⭐⭐⭐
4. **Wine-TKG** ⭐⭐⭐⭐
5. **Wine** ⭐⭐⭐

This ensures the best compatibility automatically.

## Feature Comparison

### DirectX Support

| Version | Wine | Wine-Staging | Proton | Proton-GE |
|---------|------|--------------|--------|-----------|
| DirectX 9 | ✅ Native | ✅ Native | ✅ DXVK | ✅ DXVK |
| DirectX 10 | ⚠️ Limited | ⚠️ Limited | ✅ DXVK | ✅ DXVK |
| DirectX 11 | ❌ Poor | ⚠️ Limited | ✅ DXVK | ✅ DXVK |
| DirectX 12 | ❌ No | ❌ No | ✅ VKD3D | ✅ VKD3D |

### Codec Support

| Codec | Wine | Wine-Staging | Proton | Proton-GE |
|-------|------|--------------|--------|-----------|
| H.264 | ❌ | ❌ | ⚠️ Limited | ✅ Full |
| H.265 | ❌ | ❌ | ❌ | ✅ Full |
| MP3 | ✅ | ✅ | ✅ | ✅ |
| AAC | ⚠️ | ⚠️ | ✅ | ✅ |

### Performance

**Typical Game Performance** (relative to native Windows):

| Runner | 2D Games | 3D Games (DX9) | 3D Games (DX11) |
|--------|----------|----------------|-----------------|
| Wine | 95% | 80% | 50% |
| Wine-Staging | 95% | 85% | 60% |
| Proton | 98% | 95% | 90% |
| Proton-GE | 98% | 95% | 92% |

## Recommendations

### For GOG Games

**Best Choice**: **Proton-GE**

Why:
- Best compatibility with GOG installers
- Excellent codec support for cutscenes
- Community patches for GOG-specific issues
- Works great with non-Steam games

### For Older Games (Pre-2015)

**Best Choice**: **Wine or Wine-Staging**

Why:
- Lighter weight
- DirectX 9 works well
- Simpler setup
- Less overhead

### For Modern Games (2015+)

**Best Choice**: **Proton-GE or Proton**

Why:
- DirectX 11/12 support
- Better performance
- DXVK optimization
- Vulkan translation

### For Maximum Compatibility

**Best Choice**: **Install Multiple**

Install both Wine and Proton-GE:
```bash
# Install Wine for general use
sudo apt install wine

# Install Proton-GE for games
wget https://github.com/GloriousEggroll/proton-ge-custom/releases/latest
# Extract to ~/.steam/steam/compatibilitytools.d/
```

OpenGalaxy will automatically choose the best one for each game.

## Environment Variables

### Wine
```bash
WINEPREFIX=~/.wine          # Wine prefix location
WINEDEBUG=-all              # Disable debug output
WINEARCH=win64              # 64-bit Windows
WINE_LARGE_ADDRESS_AWARE=1  # More memory for 32-bit games
```

### Proton
```bash
STEAM_COMPAT_DATA_PATH=~/.proton           # Proton prefix
STEAM_COMPAT_CLIENT_INSTALL_PATH=~/.steam  # Steam location
PROTON_USE_WINED3D=1                       # Force WineD3D (fallback)
PROTON_NO_ESYNC=1                          # Disable esync
PROTON_NO_FSYNC=1                          # Disable fsync
```

## Troubleshooting

### Game Doesn't Work with Wine

Try in this order:
1. Wine-Staging
2. Proton
3. Proton-GE

### Video Cutscenes Don't Play

**Solution**: Use Proton-GE (has full codec support)

### Poor Performance

**Solution**: Use Proton/Proton-GE (DXVK is faster)

### Installer Crashes

**Solution**: Try different runner:
```bash
# Check which runner was used
tail ~/.local/share/opengalaxy/opengalaxy.log

# Try manual installation with different runner
wine installer.exe          # Try Wine
proton run installer.exe    # Try Proton
```

## Manual Runner Selection

Currently, OpenGalaxy auto-selects the best runner. Future versions will allow manual selection per-game.

## Updating Runners

### Wine
```bash
sudo apt update && sudo apt upgrade wine
```

### Wine-Staging
```bash
sudo apt update && sudo apt upgrade wine-staging
```

### Proton
- Updates automatically via Steam

### Proton-GE
```bash
# Download new version
wget https://github.com/GloriousEggroll/proton-ge-custom/releases/latest

# Remove old version
rm -rf ~/.steam/steam/compatibilitytools.d/GE-Proton*

# Extract new version
tar -xf GE-Proton*.tar.gz -C ~/.steam/steam/compatibilitytools.d/
```

## See Also

- [INSTALLATION_PROCESS.md](INSTALLATION_PROCESS.md) - Installation guide
- [COMPATIBILITY_GUIDE.md](COMPATIBILITY_GUIDE.md) - Game compatibility
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Common issues

---

**Last Updated**: 2026-01-23
**Recommended**: Proton-GE for GOG games
**Status**: All runners supported
