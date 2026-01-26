# Wine/Proton Tools Integration

## Overview

OpenGalaxy includes integrated Wine/Proton tool launchers in the Game Properties dialog. These tools allow users to configure Wine/Proton settings, manage DLLs, and edit the Windows registry directly from the application.

## Available Tools

### 1. Winecfg

**Purpose**: Configure Wine settings

**What it does**:
- Configure graphics settings (resolution, color depth, etc.)
- Configure audio settings
- Configure drives and paths
- Configure application settings
- Manage DLL overrides

**How to use**:
1. Open Game Properties (right-click game → Properties)
2. Click "Winecfg" button
3. Wine configuration window opens

**Requirements**: Wine must be installed on your system

**Error message**: "Failed to launch winecfg. Make sure Wine is installed."
- This means Wine is not installed or not in your PATH
- Install Wine: `sudo apt install wine` (Ubuntu/Debian) or equivalent for your distro

---

### 2. Protontricks

**Purpose**: Manage DLLs and runtimes for Proton

**What it does**:
- Install/remove Windows DLLs
- Install/remove Windows runtimes (DirectX, .NET, etc.)
- Configure compatibility settings
- Manage Proton prefixes

**How to use**:
1. Open Game Properties (right-click game → Properties)
2. Click "Protontricks" button
3. Protontricks GUI opens with list of available apps
4. Select a game and install required components

**Requirements**: Protontricks must be installed on your system

**Error message**: "Failed to launch protontricks. Make sure Protontricks is installed."
- This means Protontricks is not installed or not in your PATH
- Install Protontricks: `pip install protontricks` or `sudo apt install protontricks`

---

### 3. Winetricks

**Purpose**: Manage DLLs and runtimes for Wine

**What it does**:
- Install/remove Windows DLLs
- Install/remove Windows runtimes (DirectX, .NET, etc.)
- Configure compatibility settings
- Manage Wine prefixes

**How to use**:
1. Open Game Properties (right-click game → Properties)
2. Click "Winetricks" button
3. Winetricks GUI opens
4. Select components to install/remove

**Requirements**: Winetricks must be installed on your system

**Error message**: "Failed to launch winetricks. Make sure Winetricks is installed."
- This means Winetricks is not installed or not in your PATH
- Install Winetricks: `sudo apt install winetricks` (Ubuntu/Debian) or equivalent

---

### 4. Regedit

**Purpose**: Edit Windows Registry

**What it does**:
- View/edit Windows registry entries
- Configure advanced Wine settings
- Modify game-specific registry entries
- Debug compatibility issues

**How to use**:
1. Open Game Properties (right-click game → Properties)
2. Click "Regedit" button
3. Wine Registry Editor opens
4. Navigate and edit registry entries as needed

**Requirements**: Wine must be installed on your system

**Error message**: "Failed to launch regedit. Make sure Wine is installed."
- This means Wine is not installed or not in your PATH
- Install Wine: `sudo apt install wine` (Ubuntu/Debian) or equivalent

---

## Installation Guide

### Ubuntu/Debian

```bash
# Install Wine
sudo apt install wine wine32 wine64

# Install Winetricks
sudo apt install winetricks

# Install Protontricks
pip install protontricks
# or
sudo apt install protontricks
```

### Fedora/RHEL

```bash
# Install Wine
sudo dnf install wine

# Install Winetricks
sudo dnf install winetricks

# Install Protontricks
pip install protontricks
# or
sudo dnf install protontricks
```

### Arch Linux

```bash
# Install Wine
sudo pacman -S wine

# Install Winetricks
sudo pacman -S winetricks

# Install Protontricks
sudo pacman -S protontricks
```

### macOS

```bash
# Install Wine (via Homebrew)
brew install wine

# Install Winetricks
brew install winetricks

# Install Protontricks
pip install protontricks
```

---

## Common Environment Variables

When using these tools, you may want to set environment variables for advanced configuration:

### DXVK (Direct3D 11 → Vulkan)

```
DXVK_HUD=fps              # Show FPS counter
DXVK_LOG_LEVEL=info       # Enable logging
DXVK_ASYNC=1              # Async shader compilation
```

### VKD3D (Direct3D 12 → Vulkan)

```
VKD3D_CONFIG=...          # Configuration options
VKD3D_DEBUG=...           # Debug options
```

### Proton

```
PROTON_LOG=1              # Enable logging
PROTON_USE_WINED3D=1      # Use WineD3D instead of DXVK
PROTON_NO_ESYNC=1         # Disable esync
```

### Wine

```
WINEPREFIX=/path/to/prefix    # Wine prefix location
WINEDLLOVERRIDES=...          # DLL overrides
WINE_CPU_TOPOLOGY=...         # CPU topology
```

---

## Troubleshooting

### Tools not launching

**Problem**: Getting "Failed to launch [tool]" error

**Solutions**:
1. Check if the tool is installed: `which winecfg` (or winetricks, protontricks, etc.)
2. Install the missing tool (see Installation Guide above)
3. Make sure the tool is in your PATH
4. Try running the tool manually from terminal to verify it works

### Tools launching but not responding

**Problem**: Tool window opens but doesn't respond

**Solutions**:
1. Wait a few seconds - tools can be slow to start
2. Check if there's a terminal window that opened (some tools show output there)
3. Try running the tool manually from terminal to see error messages
4. Check system logs for errors

### Wine/Proton prefix issues

**Problem**: Tools can't find the Wine/Proton prefix

**Solutions**:
1. Set WINEPREFIX environment variable in the Properties dialog
2. Make sure the prefix exists: `ls ~/.wine` or `ls ~/.steam/root/compatibilitytools.d/`
3. Create a new prefix if needed: `WINEPREFIX=~/.wine wineboot`

---

## Implementation Details

### How Tool Launchers Work

1. **Process Creation**: Uses Qt's QProcess to launch external tools
2. **Non-blocking**: Tools run in separate processes, dialog stays open
3. **Environment Setup**: Inherits system environment and sets WINEPREFIX if available
4. **Error Handling**: Shows user-friendly error messages if tools aren't installed

### Code Location

- **Header**: `ui/qt/dialogs/game_details_dialog.h`
- **Implementation**: `ui/qt/dialogs/game_details_dialog.cpp`
- **Methods**:
  - `launchWinecfg()`
  - `launchProtontricks()`
  - `launchWinetricks()`
  - `launchRegedit()`

---

## Related Documentation

- [LINK_SYSTEMS.md](LINK_SYSTEMS.md) - GOG store/support/forum links
- [CONFIGURATION.md](CONFIGURATION.md) - Game configuration options
- [ARCHITECTURE.md](ARCHITECTURE.md) - Overall system architecture

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-25 | Initial implementation with 4 tool launchers |

---

## Support

For issues or questions:
1. Check the Troubleshooting section above
2. Verify tools are installed and in PATH
3. Check console output for debug messages
4. Report issues on GitHub with debug output
