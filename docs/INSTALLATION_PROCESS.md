# Game Installation Process

## Overview

OpenGalaxy handles game installation in multiple steps, including downloading installers and running them with appropriate compatibility layers (Wine for Windows games on Linux).

## Installation Flow

### 1. Download Phase

```
User clicks "Install"
    ↓
Select appropriate installer (Windows/Linux/Mac)
    ↓
Resolve download URL from GOG API
    ↓
Download installer to games directory
    ↓
Progress updates (percentage, speed)
    ↓
Download complete
```

### 2. Installation Phase

```
Download complete
    ↓
Detect installer type (.exe, .sh, .pkg)
    ↓
Find appropriate runner (Wine, Native, etc.)
    ↓
Run installer
    ↓
Wait for installer to complete
    ↓
Mark game as installed
```

## Windows Games on Linux

### Requirements

**Wine must be installed** to run Windows game installers:

```bash
# Ubuntu/Debian
sudo apt install wine

# Fedora
sudo dnf install wine

# Arch Linux
sudo pacman -S wine
```

### Wine Detection

OpenGalaxy searches for Wine in these locations:
1. `/usr/bin/wine` (standard location)
2. `/usr/local/bin/wine` (custom install)
3. `/opt/wine/bin/wine` (alternative location)
4. System PATH (`which wine`)

### Wine Installation Process

When installing a Windows game:

1. **Download** the `.exe` installer
2. **Find Wine** executable
3. **Create** Wine prefix in game directory
4. **Run** installer: `wine installer.exe`
5. **Wait** for installer GUI to complete
6. **Mark** game as installed

### Wine Prefix

Each game gets its own Wine prefix:
```
~/Games/GameName/.wine/
```

This isolates games from each other and prevents conflicts.

## Error Handling

### Wine Not Found

**Error Message**:
```
Wine not found. Please install Wine to run Windows installers.
Ubuntu/Debian: sudo apt install wine
Fedora: sudo dnf install wine
Arch: sudo pacman -S wine
```

**Solution**: Install Wine using the command for your distribution.

### Installer Failed to Start

**Error Message**:
```
Failed to start Wine installer: <error details>
```

**Possible Causes**:
- Wine not properly installed
- Corrupted installer file
- Insufficient permissions
- Missing Wine dependencies

**Solutions**:
1. Verify Wine installation: `wine --version`
2. Check installer file exists and is readable
3. Check disk space
4. Install Wine dependencies: `sudo apt install wine winetricks`

### Installer Exited with Error

**Error Message**:
```
Installer exited with code <number>
```

**Possible Causes**:
- User cancelled installation
- Installer encountered an error
- Missing game dependencies
- Disk space issues

**Solutions**:
1. Try installing again
2. Check Wine logs: `~/.wine/drive_c/windows/temp/`
3. Run installer manually: `wine ~/Games/GameName.exe`
4. Check game requirements

## Manual Installation

If automatic installation fails, you can install manually:

### 1. Download Installer

The installer is downloaded to:
```
~/Games/GameName.exe
```

### 2. Run Manually

```bash
cd ~/Games
wine GameName.exe
```

### 3. Install to Correct Location

Install the game to:
```
~/Games/GameName/
```

### 4. Mark as Installed

OpenGalaxy will detect the installed game if it's in the correct directory.

## Installation Directories

### Default Location

```
~/Games/                    # Linux/macOS
%USERPROFILE%\Games\       # Windows
```

### Per-Game Structure

```
~/Games/
├── GameName.exe           # Downloaded installer
├── GameName/              # Installed game
│   ├── .wine/            # Wine prefix (Windows games)
│   ├── game.exe          # Game executable
│   └── data/             # Game data
└── AnotherGame/
    └── ...
```

## Progress Tracking

### Download Progress

```cpp
InstallProgress {
    gameId: "123456"
    status: "downloading"
    percentage: 45
    downloadedBytes: 450000000
    totalBytes: 1000000000
    currentFile: "/home/user/Games/GameName.exe"
}
```

### Installation Progress

```cpp
InstallProgress {
    gameId: "123456"
    status: "installing"
    percentage: 100
    currentFile: "/home/user/Games/GameName.exe"
}
```

## Cancellation

Users can cancel installation at any time:

```cpp
installService->cancelInstallation(gameId);
```

This will:
1. Abort ongoing download
2. Stop installer process
3. Clean up partial files (optional)
4. Emit `installCancelled` signal

## Logging

Installation process is logged:

```
[INFO] Installing game: Donut County
[INFO] Download completed: /home/user/Games/Donut County.exe
[INFO] Running installer with Wine: /usr/bin/wine /home/user/Games/Donut County.exe
[INFO] Wine installer started for: Donut County
[INFO] Installation complete: /home/user/Games/Donut County
```

Check logs at:
```
~/.local/share/opengalaxy/opengalaxy.log
```

## Troubleshooting

### Download Completes But Nothing Happens

**Symptoms**: 
- Download finishes
- Log shows: "Download completed: /path/to/installer.exe"
- No installer window appears
- Installation doesn't proceed

**Cause**: Wine not found or not working

**Solution**:
1. Check if Wine is installed: `wine --version`
2. Install Wine if missing
3. Test Wine: `wine notepad`
4. Check OpenGalaxy logs for Wine errors

### Installer Window Doesn't Appear

**Possible Causes**:
- Wine not configured for GUI
- Missing X11 dependencies
- Wayland compatibility issues

**Solutions**:
```bash
# Install Wine GUI dependencies
sudo apt install wine winetricks

# Configure Wine
winecfg

# Test Wine GUI
wine notepad
```

### Installation Hangs

**Symptoms**:
- Installer window appears
- Progress bar doesn't move
- Process seems stuck

**Solutions**:
1. Wait (some installers are slow)
2. Check Wine process: `ps aux | grep wine`
3. Kill and retry: `killall wine`
4. Try manual installation

### Permission Denied

**Error**: Cannot write to installation directory

**Solution**:
```bash
# Fix permissions
chmod 755 ~/Games
chown -R $USER:$USER ~/Games

# Or choose different directory in Settings
```

## Advanced Configuration

### Custom Wine Version

To use a specific Wine version:

1. Install custom Wine (e.g., Wine-Staging, Proton)
2. Create symlink:
```bash
sudo ln -s /path/to/custom/wine /usr/local/bin/wine
```

### Wine Prefix Location

By default, each game gets its own prefix:
```
~/Games/GameName/.wine/
```

To use a shared prefix, set `WINEPREFIX` environment variable.

### Wine Configuration

Configure Wine for better compatibility:

```bash
# Run Wine configuration
winecfg

# Install common libraries
winetricks vcrun2019 d3dx9 dotnet48

# Enable DXVK (DirectX to Vulkan)
winetricks dxvk
```

## Future Improvements

### Planned Features

1. **Proton Support**: Use Proton instead of Wine for better compatibility
2. **Runner Selection**: Let users choose Wine/Proton/Native
3. **Silent Installation**: Non-interactive installation where possible
4. **Extraction Only**: Extract installers without running them
5. **Resume Downloads**: Resume interrupted downloads
6. **Parallel Downloads**: Download multiple games simultaneously
7. **Bandwidth Limiting**: Limit download speed
8. **Verification**: Verify downloaded files with checksums

### Known Limitations

1. **Wine Required**: Windows games need Wine on Linux
2. **GUI Installers**: Some installers require user interaction
3. **No Silent Mode**: Can't skip installer GUI (yet)
4. **Single Download**: One game at a time
5. **No Resume**: Downloads can't be resumed

## See Also

- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Common issues
- [COMPATIBILITY_GUIDE.md](COMPATIBILITY_GUIDE.md) - Wine/Proton setup
- [DATA_LOCATIONS.md](DATA_LOCATIONS.md) - File locations

---

**Last Updated**: 2026-01-23
**Status**: Wine detection and error handling improved
