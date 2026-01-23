# OpenGalaxy Compatibility Guide

## Running Games on Different Architectures

OpenGalaxy supports running games across different CPU architectures using ISA (Instruction Set Architecture) translators.

---

## 🖥️ Supported Configurations

### x86_64 Linux (Most Common)
- ✅ **Native Linux games** - Direct execution
- ✅ **Windows games** - Via Wine or Proton-GE
- ✅ **No ISA translation needed**

### ARM64 Linux (Raspberry Pi, ARM servers)
- ✅ **Native ARM64 Linux games** - Direct execution
- ✅ **x86_64 Linux games** - Via Box64, FEX-Emu, or QEMU
- ✅ **Windows games** - Via Wine + ISA translator
- ⚠️ **Performance varies** - Emulation overhead

### ARM64 macOS (Apple Silicon M1/M2/M3)
- ✅ **Native ARM64 macOS games** - Direct execution
- ✅ **x86_64 macOS games** - Via Rosetta 2 (built-in)
- ⚠️ **Windows games** - Requires Wine + Rosetta 2 (experimental)

---

## 🔧 ISA Translators

### Box64 (Recommended for ARM64 Linux)
**Best for**: Gaming on ARM64 Linux

**Installation**:
```bash
# Ubuntu/Debian
sudo apt install box64

# Or build from source
git clone https://github.com/ptitSeb/box64
cd box64
mkdir build && cd build
cmake .. -DARM_DYNAREC=ON
make -j$(nproc)
sudo make install
```

**Performance**: ⭐⭐⭐⭐ (Good)
- Dynamic recompilation for better speed
- Optimized for gaming
- Good compatibility with Wine

**Usage in OpenGalaxy**:
- Automatically detected if installed
- Select "Box64" as runner in game settings
- Works with Wine for Windows games

---

### FEX-Emu (Alternative for ARM64 Linux)
**Best for**: Modern ARM64 systems with good CPU

**Installation**:
```bash
# Ubuntu 22.04+
sudo add-apt-repository ppa:fex-emu/fex
sudo apt update
sudo apt install fex-emu

# Or from GitHub releases
# https://github.com/FEX-Emu/FEX/releases
```

**Performance**: ⭐⭐⭐⭐⭐ (Excellent)
- JIT compilation for near-native speed
- Modern architecture
- Active development

**Usage in OpenGalaxy**:
- Automatically detected if installed
- Select "FEX" as runner in game settings
- Best for newer ARM64 CPUs

---

### QEMU User Mode (Fallback)
**Best for**: Maximum compatibility, slower performance

**Installation**:
```bash
# Ubuntu/Debian
sudo apt install qemu-user-static

# Arch Linux
sudo pacman -S qemu-user-static
```

**Performance**: ⭐⭐ (Slow)
- Full CPU emulation
- Works everywhere
- Significant overhead

**Usage in OpenGalaxy**:
- Automatically detected if installed
- Select "QEMU" as runner in game settings
- Use only if Box64/FEX don't work

---

### Rosetta 2 (macOS Only)
**Best for**: Apple Silicon Macs

**Installation**:
```bash
# Usually pre-installed, but if needed:
softwareupdate --install-rosetta
```

**Performance**: ⭐⭐⭐⭐⭐ (Excellent)
- Apple's official x86_64 translator
- Near-native performance
- Transparent to applications

**Usage in OpenGalaxy**:
- Automatically available on ARM64 macOS
- Select "Rosetta2" as runner in game settings
- Works seamlessly with most games

---

## 🎮 Game Compatibility

### Native Linux Games
| Architecture | Runner | Performance | Notes |
|--------------|--------|-------------|-------|
| x86_64 | Native | ⭐⭐⭐⭐⭐ | Direct execution |
| ARM64 | Box64/FEX | ⭐⭐⭐⭐ | Good with modern CPU |

### Windows Games (via Wine)
| Architecture | Runner | Performance | Notes |
|--------------|--------|-------------|-------|
| x86_64 | Wine/Proton | ⭐⭐⭐⭐ | Standard compatibility |
| ARM64 | Wine + Box64 | ⭐⭐⭐ | Playable, some overhead |
| ARM64 | Wine + FEX | ⭐⭐⭐⭐ | Better performance |

### macOS Games
| Architecture | Runner | Performance | Notes |
|--------------|--------|-------------|-------|
| x86_64 | Native | ⭐⭐⭐⭐⭐ | Direct execution |
| ARM64 (native) | Native | ⭐⭐⭐⭐⭐ | Direct execution |
| ARM64 (x86_64) | Rosetta2 | ⭐⭐⭐⭐⭐ | Excellent translation |

---

## 🚀 Quick Start

### 1. Check Your System
```bash
# Check architecture
uname -m
# x86_64 = Intel/AMD 64-bit
# aarch64 = ARM 64-bit

# Check available translators
which box64 fex-emu qemu-x86_64
```

### 2. Install Recommended Translator

**For ARM64 Linux**:
```bash
# Install Box64 (recommended)
sudo apt install box64

# Or FEX-Emu for better performance
sudo add-apt-repository ppa:fex-emu/fex
sudo apt install fex-emu
```

**For ARM64 macOS**:
```bash
# Rosetta 2 (if not installed)
softwareupdate --install-rosetta
```

### 3. Configure OpenGalaxy

1. Launch OpenGalaxy
2. Go to **Settings** → **Runners**
3. Verify detected runners
4. For each game:
   - Right-click → **Properties**
   - Select preferred runner
   - Save

---

## 🔍 Troubleshooting

### Game Won't Launch

**Check runner availability**:
```bash
# Verify executable exists
ls -l /usr/bin/box64
ls -l /usr/bin/FEXInterpreter
ls -l /usr/bin/qemu-x86_64
```

**Check OpenGalaxy logs**:
```bash
# Run from terminal to see logs
./build/ui/opengalaxy

# Look for lines like:
# [INFO] Discovering runners...
# [INFO] Launched game via Box64 on Linux
```

### Poor Performance

1. **Try different translator**:
   - Box64 → FEX-Emu (usually faster)
   - QEMU → Box64 (much faster)

2. **Check CPU usage**:
   ```bash
   htop
   # Look for high CPU usage during gameplay
   ```

3. **Reduce game settings**:
   - Lower resolution
   - Disable anti-aliasing
   - Reduce texture quality

### Compatibility Issues

**Windows games on ARM64**:
1. Install Wine dependencies:
   ```bash
   sudo dpkg --add-architecture armhf
   sudo apt update
   sudo apt install wine-stable:armhf
   ```

2. Use 32-bit Box64:
   ```bash
   # Box64 handles 64-bit, Box86 handles 32-bit
   sudo apt install box86
   ```

3. Check game architecture:
   ```bash
   file game.exe
   # PE32+ = 64-bit (use Box64)
   # PE32 = 32-bit (use Box86)
   ```

---

## 📊 Performance Comparison

### Benchmark: Typical Indie Game (2D)
| Configuration | FPS | Notes |
|---------------|-----|-------|
| x86_64 Native | 60 | Baseline |
| ARM64 + Box64 | 45-55 | ~80% performance |
| ARM64 + FEX | 55-60 | ~95% performance |
| ARM64 + QEMU | 20-30 | ~40% performance |
| macOS Rosetta2 | 58-60 | ~97% performance |

### Benchmark: 3D Game (Medium Settings)
| Configuration | FPS | Notes |
|---------------|-----|-------|
| x86_64 Native | 60 | Baseline |
| ARM64 + Box64 | 30-40 | ~60% performance |
| ARM64 + FEX | 45-55 | ~80% performance |
| ARM64 + QEMU | 10-15 | Unplayable |
| macOS Rosetta2 | 55-60 | ~92% performance |

*Note: Actual performance varies by game, CPU, and GPU*

---

## 🛠️ Advanced Configuration

### Custom Runner Arguments

Edit game properties to add custom arguments:

**Box64**:
```bash
# Enable logging
BOX64_LOG=1

# Force specific dynarec
BOX64_DYNAREC=1

# Set CPU emulation
BOX64_EMULATED_LIBS=libGL.so.1
```

**FEX-Emu**:
```bash
# Enable JIT
FEX_ENABLEJIT=1

# Set thread count
FEX_THREADS=4
```

### Wine + ISA Translator

For Windows games on ARM64:

1. Install Wine (ARM64 version)
2. Configure Wine prefix:
   ```bash
   WINEPREFIX=~/.wine-arm64 winecfg
   ```
3. In OpenGalaxy:
   - Select "Wine" as primary runner
   - ISA translator auto-applied if needed

---

## 📚 Additional Resources

- [Box64 Wiki](https://github.com/ptitSeb/box64/wiki)
- [FEX-Emu Documentation](https://fex-emu.com/Docs/)
- [Wine on ARM](https://wiki.winehq.org/ARM)
- [Apple Rosetta 2](https://developer.apple.com/documentation/apple-silicon/about-the-rosetta-translation-environment)

---

## 🤝 Contributing

Found a compatibility issue? Help improve this guide:

1. Test the game with different runners
2. Document your findings
3. Submit an issue or PR to the repository
4. Include system specs and logs

---

## ⚠️ Known Limitations

- **Anti-cheat**: Games with kernel-level anti-cheat won't work
- **DRM**: Some DRM systems may fail on translated architectures
- **Multiplayer**: Online features may have issues
- **VR**: Virtual reality not supported via translation
- **Performance**: Expect 60-95% of native performance depending on translator

---

## 📝 License

This guide is part of OpenGalaxy, licensed under Apache-2.0.
