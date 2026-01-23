# OpenGalaxy Data Storage Locations

## Overview

OpenGalaxy stores different types of data in different locations following platform conventions.

## 📁 Data Directory Structure

### Linux (Your System)
```
~/.local/share/opengalaxy/
├── session.json          # Login session (access tokens)
├── library.db            # Game library database (SQLite)
├── opengalaxy.log        # Application logs
└── settings.conf         # Application settings (Qt format)

~/Games/                  # Default game installation directory
├── Game Title 1/
├── Game Title 2/
└── ...
```

### macOS
```
~/Library/Application Support/opengalaxy/
├── session.json
├── library.db
├── opengalaxy.log
└── settings.conf

~/Games/                  # Default game installation directory
```

### Windows
```
%APPDATA%\opengalaxy\
├── session.json
├── library.db
├── opengalaxy.log
└── settings.conf

C:\Users\YourName\Games\  # Default game installation directory
```

## 📄 File Details

### 1. Session Data (`session.json`)

**Location**: `~/.local/share/opengalaxy/session.json`

**Contains**:
- Access token (for GOG API authentication)
- Refresh token (to renew expired access tokens)
- Token expiration timestamp

**Format**: JSON
```json
{
  "accessToken": "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9...",
  "refreshToken": "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9...",
  "expiresAt": "2026-01-23T22:30:00Z"
}
```

**Security**:
- File permissions: 0600 (read/write for user only)
- Contains sensitive authentication data
- Automatically refreshed when expired
- Deleted on logout

**When Created**: First successful login

**When Updated**: 
- On login
- On token refresh
- On logout (deleted)

---

### 2. Library Database (`library.db`)

**Location**: `~/.local/share/opengalaxy/library.db`

**Contains**:
- List of owned games
- Game metadata (title, ID, platform)
- Installation status
- Installation paths
- Last sync timestamp

**Format**: SQLite database

**Tables**:
- `games` - Game information
- `installations` - Installed games tracking

**When Created**: First time library is fetched from GOG

**When Updated**:
- When library is refreshed
- When games are installed/uninstalled
- When game metadata changes

---

### 3. Application Logs (`opengalaxy.log`)

**Location**: `~/.local/share/opengalaxy/opengalaxy.log`

**Contains**:
- Application events
- HTTP requests/responses
- Errors and warnings
- Debug information

**Format**: Plain text with timestamps
```
2026-01-22T22:27:07 [INFO] Application started
2026-01-22T22:27:08 [DEBUG] HTTP GET https://embed.gog.com/userData.json
2026-01-22T22:27:09 [ERROR] HTTP request failed: ...
```

**Log Levels**:
- `[DEBUG]` - Detailed debugging information
- `[INFO]` - General information
- `[WARNING]` - Warning messages
- `[ERROR]` - Error messages

**Rotation**: Appends to file (no automatic rotation yet)

**When Created**: First application run

---

### 4. Application Settings (`settings.conf`)

**Location**: `~/.local/share/opengalaxy/settings.conf`

**Contains**:
- UI language preference
- Window size/position
- Install directory preference
- Runner settings (Wine, Proton)
- Other user preferences

**Format**: Qt INI-style format
```ini
[ui]
locale=en_US

[install]
defaultPath=/home/mateus/Games

[runners]
protonEnabled=true
protonVersion=GE-Proton8-25
```

**When Created**: First time settings are saved

**When Updated**: When user changes settings

---

### 5. Game Installations

**Default Location**: `~/Games/` (Linux/macOS) or `C:\Users\YourName\Games\` (Windows)

**Structure**:
```
~/Games/
├── Cyberpunk 2077/
│   ├── bin/
│   ├── data/
│   └── Cyberpunk2077.exe
├── The Witcher 3/
│   ├── bin/
│   ├── content/
│   └── witcher3.exe
└── ...
```

**Customizable**: Users can choose different install directories per game

**When Created**: When installing a game

**Size**: Varies by game (can be several GB)

---

## 🔍 How to Find Your Data

### On Linux (Your System)

```bash
# View data directory
ls -lah ~/.local/share/opengalaxy/

# View session file
cat ~/.local/share/opengalaxy/session.json

# View logs
tail -f ~/.local/share/opengalaxy/opengalaxy.log

# View settings
cat ~/.local/share/opengalaxy/settings.conf

# View installed games
ls -lah ~/Games/
```

### Check Current Locations

Run this command to see actual paths:
```bash
# Data directory
echo "Data: ~/.local/share/opengalaxy/"

# Games directory
echo "Games: ~/Games/"

# Check if they exist
[ -d ~/.local/share/opengalaxy ] && echo "Data dir exists" || echo "Data dir not created yet"
[ -d ~/Games ] && echo "Games dir exists" || echo "Games dir not created yet"
```

---

## 🗑️ Cleaning Up / Uninstalling

### Remove All OpenGalaxy Data

```bash
# Remove application data (session, library, logs, settings)
rm -rf ~/.local/share/opengalaxy/

# Remove installed games (WARNING: This deletes all games!)
rm -rf ~/Games/

# Remove application binary
sudo rm /usr/local/bin/opengalaxy  # or wherever it's installed
```

### Remove Only Session (Logout)

```bash
# Remove session file (forces re-login)
rm ~/.local/share/opengalaxy/session.json
```

### Remove Only Logs

```bash
# Clear logs
rm ~/.local/share/opengalaxy/opengalaxy.log
```

### Keep Games, Remove App Data

```bash
# Remove app data but keep games
rm -rf ~/.local/share/opengalaxy/
# Games in ~/Games/ are preserved
```

---

## 🔒 Security Considerations

### Sensitive Data

**session.json** contains authentication tokens:
- Can be used to access your GOG account
- Should have restricted permissions (0600)
- Automatically refreshed when expired
- Deleted on logout

**Recommendations**:
- Don't share session.json
- Don't commit to version control
- Use logout feature when done
- Tokens expire automatically

### File Permissions

```bash
# Check permissions
ls -l ~/.local/share/opengalaxy/session.json

# Should show: -rw------- (600)
# If not, fix with:
chmod 600 ~/.local/share/opengalaxy/session.json
```

---

## 📊 Disk Space Usage

### Typical Sizes

| Item | Size | Notes |
|------|------|-------|
| session.json | ~2 KB | Small JSON file |
| library.db | ~100 KB | Grows with library size |
| opengalaxy.log | Varies | Can grow large over time |
| settings.conf | ~1 KB | Small config file |
| **Total App Data** | **~100-500 KB** | Minimal |
| **Games** | **Varies** | Can be 1-100+ GB per game |

### Check Actual Usage

```bash
# App data size
du -sh ~/.local/share/opengalaxy/

# Games size
du -sh ~/Games/

# Detailed breakdown
du -h ~/.local/share/opengalaxy/*
```

---

## 🔄 Backup and Restore

### Backup Your Data

```bash
# Backup everything
tar -czf opengalaxy-backup.tar.gz ~/.local/share/opengalaxy/

# Backup only session (for quick restore)
cp ~/.local/share/opengalaxy/session.json ~/session-backup.json

# Backup library database
cp ~/.local/share/opengalaxy/library.db ~/library-backup.db
```

### Restore from Backup

```bash
# Restore everything
tar -xzf opengalaxy-backup.tar.gz -C ~/

# Restore session
cp ~/session-backup.json ~/.local/share/opengalaxy/session.json

# Restore library
cp ~/library-backup.db ~/.local/share/opengalaxy/library.db
```

---

## 🐛 Troubleshooting

### Can't Login / Session Issues

```bash
# Remove session and try again
rm ~/.local/share/opengalaxy/session.json
# Then restart app and login
```

### Library Not Loading

```bash
# Check if database exists
ls -lah ~/.local/share/opengalaxy/library.db

# If corrupted, remove and re-sync
rm ~/.local/share/opengalaxy/library.db
# Then restart app and refresh library
```

### Check Logs for Errors

```bash
# View recent logs
tail -100 ~/.local/share/opengalaxy/opengalaxy.log

# Search for errors
grep ERROR ~/.local/share/opengalaxy/opengalaxy.log

# Follow logs in real-time
tail -f ~/.local/share/opengalaxy/opengalaxy.log
```

### Disk Space Issues

```bash
# Check available space
df -h ~

# Find large game installations
du -sh ~/Games/* | sort -h

# Clean up old logs
> ~/.local/share/opengalaxy/opengalaxy.log  # Truncate log file
```

---

## 📝 Summary

| Data Type | Location | Purpose | Size |
|-----------|----------|---------|------|
| **Session** | `~/.local/share/opengalaxy/session.json` | Login tokens | ~2 KB |
| **Library** | `~/.local/share/opengalaxy/library.db` | Game database | ~100 KB |
| **Logs** | `~/.local/share/opengalaxy/opengalaxy.log` | Debug info | Varies |
| **Settings** | `~/.local/share/opengalaxy/settings.conf` | User prefs | ~1 KB |
| **Games** | `~/Games/` | Installed games | GB per game |

---

**Last Updated**: 2026-01-22  
**Platform**: Linux (Ubuntu 24.04)
