# Runner Auto-Detection and Cross-Platform Support

## Overview

OpenGalaxy automatically detects and selects the optimal game runner for each game across all platforms (Windows, Linux, macOS). This document explains how runner detection works and what platforms are supported.

## Table of Contents

1. [Runner Auto-Detection](#runner-auto-detection)
2. [Cross-Platform Support](#cross-platform-support)
3. [Installation Process](#installation-process)
4. [Supported Game Types](#supported-game-types)
5. [Platform-Specific Details](#platform-specific-details)
6. [Troubleshooting](#troubleshooting)

## Runner Auto-Detection

### What is Runner Auto-Detection?

When you install a game, OpenGalaxy automatically detects the best runner to use for that game and saves it to the database. This means:

- ✅ No manual runner selection needed
- ✅ Optimal runner is chosen automatically
- ✅ Game properties show the correct runner
- ✅ Game launches with the best runner every time

### How It Works

#### For Windows Games

The application searches for runners in this priority order:

1. **Proton-GE** (best compatibility for games)
   - Location: `~/.steam/steam/compatibilitytools.d/GE-Proton*/`
   - Location: `~/.local/share/Steam/compatibilitytools.d/GE-Proton*/`

2. **Proton** (Steam's official compatibility layer)
   - Location: `~/.steam/steam/steamapps/common/Proton*/`
   - Location: `~/.local/share/Steam/steamapps/common/Proton*/`

3. **Wine-Staging** (community-maintained Wine fork)
   - Found via: `which wine-staging`

4. **Wine-TKG** (optimized Wine build)
   - Found via: `which wine-tkg`

5. **Wine** (standard Wine)
   - Found via: `which wine`

**Result:** The first available runner is selected and saved.

#### For DOS Games

- **Pure DOS games:** Automatically set to **DOSBox**
- **DOS games with Windows installer:** 
  - Uses Wine/Proton to run the installer
  - Then sets preferred runner to **DOSBox** for playing

### Database Persistence

The detected runner is saved to the SQLite database in the `games` table:

```sql
UPDATE games SET preferredRunner = 'Proton-GE' WHERE id = 'game_id';
```

This means:
- ✅ Runner preference persists across sessions
- ✅ Game properties dialog shows the correct runner
- ✅ Game launches with the same runner every time

## Cross-Platform Support

### Platform Detection

OpenGalaxy detects the current platform at compile time:

```cpp
#ifdef Q_OS_WIN
    currentOS = "windows";
#elif defined(Q_OS_LINUX)
    currentOS = "linux";
#elif defined(Q_OS_MACOS)
    currentOS = "mac";
#endif
```

### Download Selection

When installing a game:

1. **First:** Try to find a download matching the current OS
2. **Fallback:** Use the first available download if no exact match

This allows installing games from any platform on any platform.

## Installation Process

### Step-by-Step Flow

```
1. User clicks "Install"
   ↓
2. Fetch game downloads from GOG
   ↓
3. Select download for current platform
   ↓
4. Download installer file
   ↓
5. Detect game type:
   ├─ Windows .exe?
   ├─ DOS .exe?
   ├─ DOS + Windows installer?
   ├─ Linux .sh?
   ├─ macOS .pkg/.dmg?
   └─ Archive (zip/tar/7z)?
   ↓
6. Run appropriate installer/extractor
   ↓
7. Auto-detect best runner
   ↓
8. Save runner to database
   ↓
9. Installation complete!
```

### Exit Code Handling

Windows installers often exit with non-zero codes even on success:

- **Exit code 0:** Success
- **Exit code 1:** Reboot needed
- **Exit code 2:** User cancelled
- **Exit code 3:** Other (but still successful)

OpenGalaxy accepts exit codes **0-3 as success** for Windows installers, preventing false failures.

## Supported Game Types

### 1. Windows Games (.exe)

| Platform | Support | Method | Runner |
|----------|---------|--------|--------|
| Windows | ✅ Full | Native | Native Windows |
| Linux | ✅ Full | Wine/Proton | Proton-GE > Proton > Wine |
| macOS | ⚠️ Experimental | Wine + Rosetta2 | Wine |

**Example:** Baldur's Gate 3, Cyberpunk 2077

### 2. DOS Games (pure DOS .exe)

| Platform | Support | Method | Runner |
|----------|---------|--------|--------|
| Windows | ✅ Full | DOSBox | DOSBox |
| Linux | ✅ Full | DOSBox | DOSBox |
| macOS | ✅ Full | DOSBox | DOSBox |

**Example:** The Secret of Monkey Island, Doom

### 3. DOS Games with Windows Installer

| Platform | Support | Install Method | Play Runner |
|----------|---------|-----------------|-------------|
| Windows | ✅ Full | Wine/Proton | DOSBox |
| Linux | ✅ Full | Wine/Proton | DOSBox |
| macOS | ✅ Full | Wine/Proton | DOSBox |

**How it works:**
1. Detects DOS game packaged as Windows installer
2. Uses Wine/Proton to run the Windows setup.exe
3. Sets preferred runner to DOSBox for playing
4. Works identically on all platforms

**Example:** The Elder Scrolls: Arena, Ultima

### 4. Linux Games (.sh shell scripts)

| Platform | Support | Method |
|----------|---------|--------|
| Windows | ❌ No | Not applicable |
| Linux | ✅ Full | Run directly with bash |
| macOS | ⚠️ Limited | May work with bash |

### 5. macOS Games (.pkg, .dmg)

| Platform | Support | Method |
|----------|---------|--------|
| Windows | ❌ No | Not supported |
| Linux | ❌ No | Not supported |
| macOS | ✅ Full | Native installer/mount |

### 6. Archives (zip, tar, 7z, gz)

| Platform | Support | Method |
|----------|---------|--------|
| Windows | ✅ Full | Extract with unzip/7z |
| Linux | ✅ Full | Extract with unzip/tar/7z |
| macOS | ✅ Full | Extract with unzip/tar/7z |

## Platform-Specific Details

### Windows

**Supported Game Types:**
- ✅ Windows games (native)
- ✅ DOS games (via DOSBox)
- ✅ DOS with Windows installer (via Wine/Proton)
- ✅ Archives

**Required Tools:**
- For Windows games: None (native)
- For DOS games: DOSBox
- For DOS with Windows installer: Wine or Proton

**Installation:**
```bash
# DOSBox
choco install dosbox

# Wine
choco install wine

# Proton-GE (manual)
# Download from: https://github.com/GloriousEggroll/proton-ge-custom/releases
# Extract to: ~/.steam/steam/compatibilitytools.d/
```

### Linux

**Supported Game Types:**
- ✅ Windows games (via Wine/Proton)
- ✅ DOS games (via DOSBox)
- ✅ DOS with Windows installer (via Wine/Proton)
- ✅ Linux games (native)
- ✅ Archives

**Required Tools:**
- For Windows games: Wine or Proton
- For DOS games: DOSBox
- For Linux games: None (native)

**Installation:**
```bash
# Ubuntu/Debian
sudo apt install wine dosbox

# Fedora
sudo dnf install wine dosbox

# Arch
sudo pacman -S wine dosbox

# Proton-GE (manual)
# Download from: https://github.com/GloriousEggroll/proton-ge-custom/releases
# Extract to: ~/.steam/steam/compatibilitytools.d/
```

### macOS

**Supported Game Types:**
- ⚠️ Windows games (experimental via Wine)
- ✅ DOS games (via DOSBox)
- ✅ DOS with Windows installer (via Wine/Proton)
- ✅ macOS games (native)
- ✅ Archives

**Required Tools:**
- For Windows games: Wine (experimental)
- For DOS games: DOSBox
- For macOS games: None (native)

**Installation:**
```bash
# Homebrew
brew install wine dosbox

# Proton-GE (manual)
# Download from: https://github.com/GloriousEggroll/proton-ge-custom/releases
# Extract to: ~/.steam/steam/compatibilitytools.d/
```

## Troubleshooting

### Installation Fails with "Wine/Proton not found"

**Problem:** You're trying to install a Windows game but Wine/Proton is not installed.

**Solution:**
```bash
# Linux
sudo apt install wine

# macOS
brew install wine

# Windows
choco install wine
```

### Installation Fails with "DOSBox not found"

**Problem:** You're trying to install a DOS game but DOSBox is not installed.

**Solution:**
```bash
# Linux
sudo apt install dosbox

# macOS
brew install dosbox

# Windows
choco install dosbox
```

### Game Properties Shows "Auto" Instead of Detected Runner

**Problem:** The runner wasn't saved to the database.

**Solution:**
1. Reinstall the game
2. Or manually select the runner in Game Properties
3. The runner will be saved for future launches

### Game Launches with Wrong Runner

**Problem:** The auto-detected runner is not optimal for your system.

**Solution:**
1. Open Game Properties
2. Change "Translator / Runner" to your preferred runner
3. Click Save
4. The new runner will be used for all future launches

## Technical Details

### Code Locations

- **Runner detection:** `core/src/install/install_service.cpp:529-880`
- **Database persistence:** `core/src/library/library_service.cpp:137-167`
- **Signal handling:** `ui/qt/pages/library_page.cpp:229-249`
- **Runner management:** `core/src/runners/runner_manager.cpp`

### Signal Flow

```
InstallService::installGame()
    ↓
[Detect game type and runner]
    ↓
InstallService::installCompleted(gameId, installPath, detectedRunner)
    ↓
LibraryPage::onInstallCompleted()
    ↓
LibraryService::updateGameProperties(game)
    ↓
[Save to database]
    ↓
Game properties now shows correct runner
```

### Database Schema

```sql
CREATE TABLE games (
    id TEXT PRIMARY KEY,
    title TEXT,
    preferredRunner TEXT,  -- Stores auto-detected runner
    ...
);
```

## See Also

- [WINE_PROTON_TOOLS.md](WINE_PROTON_TOOLS.md) - Wine and Proton configuration
- [DOS_GAMES_SUPPORT.md](DOS_GAMES_SUPPORT.md) - DOS game support details
- [COMPATIBILITY_GUIDE.md](COMPATIBILITY_GUIDE.md) - Game compatibility information
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - General troubleshooting guide
