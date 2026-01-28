# ISA Translation Tools Guide

This document explains ISA (Instruction Set Architecture) translation tools used to run x86/x64 games on ARM64 systems (like Apple Silicon Macs).

## Overview

ISA translation allows running games compiled for one CPU architecture on a different architecture. This is essential for:

- **Apple Silicon Macs** (ARM64) running x86/x64 games
- **ARM64 Linux systems** running x86/x64 games
- **Future ARM-based Windows** systems

## Supported Tools

### 1. FEX-Emu (Recommended for macOS)

**FEX-Emu** is a high-performance x86/x64 emulator for ARM64 systems.

#### Features

- ✅ Full x86/x64 instruction set support
- ✅ Good performance for games
- ✅ Active development
- ✅ Works with Wine/Proton
- ✅ macOS and Linux support

#### Installation

**macOS:**
```bash
# Download from: https://github.com/FEX-Emu/FEX/releases
# Extract and follow installation instructions
# Or via Homebrew (if available):
brew install fex-emu
```

**Linux:**
```bash
# Ubuntu/Debian
sudo apt install fex-emu

# Fedora
sudo dnf install fex-emu

# Arch
sudo pacman -S fex-emu
```

#### Configuration

```bash
# Set FEX as default translator
export FEX_ROOTFS=/path/to/rootfs

# Run Wine with FEX
FEX_ROOTFS=/path/to/rootfs wine game.exe
```

### 2. Box64 (Lightweight Alternative)

**Box64** is a lightweight x86_64 emulator for ARM64 systems.

#### Features

- ✅ Lightweight and fast
- ✅ Good for older games
- ✅ Lower memory usage
- ✅ Linux-focused
- ⚠️ Less complete than FEX-Emu

#### Installation

**Linux:**
```bash
# Clone and compile
git clone https://github.com/ptitSeb/box64.git
cd box64
mkdir build
cd build
cmake .. -DARM64=1
make -j$(nproc)
sudo make install
```

**Arch:**
```bash
sudo pacman -S box64
```

#### Configuration

```bash
# Run Wine with Box64
BOX64_PATH=/usr/local/bin/box64 wine game.exe
```

### 3. QEMU (Generic Fallback)

**QEMU** is a generic emulator that can translate between architectures.

#### Features

- ✅ Universal support
- ✅ Works on any platform
- ⚠️ Slower than specialized tools
- ⚠️ Higher CPU usage
- ⚠️ Not recommended for games

#### Installation

**Ubuntu/Debian:**
```bash
sudo apt install qemu-user-static
```

**Fedora:**
```bash
sudo dnf install qemu-user-static
```

**Arch:**
```bash
sudo pacman -S qemu-user-static
```

**macOS:**
```bash
brew install qemu
```

#### Configuration

```bash
# Run Wine with QEMU
qemu-x86_64-static wine game.exe
```

## Comparison Table

| Tool | Platform | Performance | Compatibility | Recommended |
|------|----------|-------------|---------------|-------------|
| FEX-Emu | macOS, Linux | Excellent | Excellent | ✅ Yes |
| Box64 | Linux | Good | Good | ✅ Yes (Linux) |
| QEMU | All | Poor | Good | ⚠️ Fallback |

## Integration with OpenGalaxy

OpenGalaxy automatically detects and uses available ISA translation tools:

1. **Detection Order:**
   - FEX-Emu (preferred)
   - Box64 (fallback)
   - QEMU (last resort)

2. **Automatic Configuration:**
   - Sets environment variables
   - Configures Wine/Proton
   - Handles architecture detection

3. **User Configuration:**
   - Can override in Game Properties
   - Can set preferred translator
   - Can disable translation if needed

## Performance Tips

### For FEX-Emu

```bash
# Enable JIT compilation (faster)
export FEX_ENABLE_JIT=1

# Set CPU cores
export FEX_CORES=4

# Enable optimizations
export FEX_OPTIMIZE=1
```

### For Box64

```bash
# Enable JIT
export BOX64_JIT=1

# Set CPU cores
export BOX64_CPUCORE=4

# Enable AVX
export BOX64_AVX=1
```

### For QEMU

```bash
# Use KVM acceleration (if available)
qemu-x86_64-static -enable-kvm wine game.exe

# Set CPU cores
qemu-x86_64-static -smp 4 wine game.exe
```

## Troubleshooting

### FEX-Emu Not Found

**Error:** "FEX-Emu not found"

**Solution:**
```bash
# Install FEX-Emu
brew install fex-emu

# Or download from: https://github.com/FEX-Emu/FEX/releases
```

### Box64 Not Found

**Error:** "Box64 not found"

**Solution:**
```bash
# Install Box64
sudo pacman -S box64

# Or compile from source:
git clone https://github.com/ptitSeb/box64.git
cd box64 && mkdir build && cd build
cmake .. -DARM64=1 && make -j$(nproc) && sudo make install
```

### Poor Performance

**Issue:** Games run slowly with ISA translation

**Solutions:**
1. Use FEX-Emu instead of QEMU
2. Enable JIT compilation
3. Increase CPU cores allocation
4. Close other applications
5. Check system resources

### Wine Prefix Issues

**Issue:** Wine prefix not compatible with translator

**Solution:**
```bash
# Recreate Wine prefix
rm -rf ~/.wine
# Reinstall game to create new prefix
```

## Advanced Configuration

### Custom Rootfs

```bash
# Create custom rootfs for FEX-Emu
mkdir -p ~/fex-rootfs
# Copy necessary files
# Configure in OpenGalaxy settings
```

### Environment Variables

```bash
# Set in ~/.bashrc or ~/.zshrc
export FEX_ENABLE_JIT=1
export FEX_CORES=4
export BOX64_JIT=1
export BOX64_CPUCORE=4
```

### Per-Game Configuration

In OpenGalaxy Game Properties:
1. Set "ISA Translator" to preferred tool
2. Configure environment variables
3. Set CPU core allocation
4. Enable/disable JIT compilation

## See Also

- [DEPENDENCIES.md](DEPENDENCIES.md) - All dependencies
- [BUILD.md](BUILD.md) - Build instructions
- [MULTI_ARCH_SUPPORT.md](MULTI_ARCH_SUPPORT.md) - Architecture support
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - General troubleshooting
