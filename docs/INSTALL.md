# Installing OpenGalaxy

Quick installation guide for end users.

## 📦 Pre-built Binaries

### Linux

#### AppImage (Recommended)
```bash
# Download latest release
wget https://github.com/mateusbentes/opengalaxy/releases/latest/download/OpenGalaxy-x86_64.AppImage

# Make executable
chmod +x OpenGalaxy-x86_64.AppImage

# Run
./OpenGalaxy-x86_64.AppImage
```

#### Flatpak
```bash
# Install from Flathub (coming soon)
flatpak install flathub org.opengalaxy.OpenGalaxy

# Run
flatpak run org.opengalaxy.OpenGalaxy
```

#### Debian/Ubuntu (.deb)
```bash
# Download .deb package
wget https://github.com/mateusbentes/opengalaxy/releases/latest/download/opengalaxy_1.0.0_amd64.deb

# Install
sudo dpkg -i opengalaxy_1.0.0_amd64.deb

# Install dependencies if needed
sudo apt install -f

# Run
opengalaxy
```

#### Arch Linux (AUR)
```bash
# Using yay
yay -S opengalaxy

# Using paru
paru -S opengalaxy

# Run
opengalaxy
```

### Windows

#### Installer (.exe)
1. Download `OpenGalaxy-Setup.exe` from [releases page](https://github.com/mateusbentes/opengalaxy/releases)
2. Run the installer
3. Follow installation wizard
4. Launch from Start Menu or Desktop shortcut

#### Portable (.zip)
1. Download `OpenGalaxy-Portable.zip`
2. Extract to desired location
3. Run `opengalaxy.exe`

### macOS

#### DMG Package
1. Download `OpenGalaxy.dmg` from [releases page](https://github.com/mateusbentes/opengalaxy/releases)
2. Open the DMG file
3. Drag OpenGalaxy to Applications folder
4. Launch from Applications

#### Homebrew
```bash
# Install via Homebrew (coming soon)
brew install --cask opengalaxy

# Run
opengalaxy
```

## 🔨 Building from Source

If pre-built binaries are not available for your platform, see [BUILD.md](BUILD.md) for detailed build instructions.

### Quick Build (Linux)

```bash
# Install dependencies
sudo apt install build-essential cmake qt6-base-dev qt6-webengine-dev libssl-dev

# Clone and build
git clone https://github.com/mateusbentes/opengalaxy.git
cd opengalaxy
./build.sh

# Run
./build/ui/opengalaxy
```

## ⚙️ Post-Installation

### First Run

1. **Launch OpenGalaxy**
2. **Login** with your GOG credentials
3. **Wait for library sync** (first time may take a minute)
4. **Start playing!**

### Optional: Install Compatibility Layers

For running Windows games on Linux:

```bash
# Wine (recommended)
sudo apt install wine-stable

# Proton-GE (for better compatibility)
# Download from https://github.com/GloriousEggroll/proton-ge-custom
```

For ARM64 systems (Raspberry Pi, Apple Silicon):

```bash
# Box64 (for x86_64 games on ARM64)
sudo apt install box64

# Or FEX-Emu (better performance)
sudo add-apt-repository ppa:fex-emu/fex
sudo apt install fex-emu
```

See [COMPATIBILITY_GUIDE.md](COMPATIBILITY_GUIDE.md) for detailed runner setup.

## 🗑️ Uninstalling

### Linux

**AppImage**: Just delete the file
```bash
rm OpenGalaxy-x86_64.AppImage
```

**Flatpak**:
```bash
flatpak uninstall org.opengalaxy.OpenGalaxy
```

**Debian/Ubuntu**:
```bash
sudo apt remove opengalaxy
```

**Arch Linux**:
```bash
yay -R opengalaxy
```

**Remove user data** (optional):
```bash
rm -rf ~/.local/share/opengalaxy/
```

### Windows

1. Open **Settings** → **Apps** → **Installed apps**
2. Find **OpenGalaxy**
3. Click **Uninstall**

Or use the uninstaller from Start Menu.

**Remove user data** (optional):
```powershell
Remove-Item -Recurse -Force "$env:APPDATA\opengalaxy"
```

### macOS

1. Drag **OpenGalaxy** from Applications to Trash
2. Empty Trash

**Remove user data** (optional):
```bash
rm -rf ~/Library/Application\ Support/opengalaxy/
```

## 🔧 Troubleshooting

### Linux: Missing Qt6 Libraries

```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-webengine-dev

# Fedora
sudo dnf install qt6-qtbase qt6-qtwebengine

# Arch
sudo pacman -S qt6-base qt6-webengine
```

### Windows: VCRUNTIME140.dll Missing

Install [Visual C++ Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe)

### macOS: "App is damaged and can't be opened"

```bash
# Remove quarantine attribute
xattr -cr /Applications/OpenGalaxy.app
```

### Can't Login

1. Check internet connection
2. Verify GOG credentials at https://www.gog.com
3. Try OAuth login instead of password
4. Check logs: see [DATA_LOCATIONS.md](DATA_LOCATIONS.md)

## 📚 Next Steps

- [QUICKSTART.md](QUICKSTART.md) - Quick start guide
- [README.md](README.md) - Full feature list
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Common issues
- [COMPATIBILITY_GUIDE.md](COMPATIBILITY_GUIDE.md) - Runner setup

## 💬 Need Help?

- **Issues**: https://github.com/mateusbentes/opengalaxy/issues
- **Discussions**: https://github.com/mateusbentes/opengalaxy/discussions
- **Email**: mateusbentes@tuta.io

---

**Last Updated**: 2026-01-23
