# OpenGalaxy Configuration System

## Overview

OpenGalaxy uses a centralized configuration system that works seamlessly across Windows, macOS, and Linux. The system automatically uses platform-specific standard locations for data and settings.

## Cross-Platform Paths

### Automatic Path Selection

OpenGalaxy automatically selects the correct paths based on your operating system:

| Type | Linux | macOS | Windows |
|------|-------|-------|---------|
| **Data** | `~/.local/share/opengalaxy/` | `~/Library/Application Support/opengalaxy/` | `%APPDATA%\opengalaxy\` |
| **Config** | `~/.config/OpenGalaxy/` | `~/Library/Preferences/OpenGalaxy/` | `%APPDATA%\OpenGalaxy\` |
| **Games** | `~/Games/` | `~/Games/` | `%USERPROFILE%\Games\` |

### What's Stored Where

**Data Directory** (session, library, logs):
- `session.json` - Authentication tokens
- `library.db` - Game library cache (SQLite)
- `opengalaxy.log` - Application logs

**Config Directory** (settings):
- `OpenGalaxy.ini` - All application settings

**Games Directory** (game installations):
- Individual game folders
- Customizable per user

## Configuration API

### Using the Config Class

The `Config` class provides a centralized way to access all settings:

```cpp
#include "opengalaxy/util/config.h"

// Initialize at application startup (required)
opengalaxy::util::Config::initialize();

// Access the singleton instance
auto& config = opengalaxy::util::Config::instance();

// Get paths
QString dataDir = config.dataDir();
QString configDir = config.configDir();
QString sessionPath = config.sessionFilePath();
QString libraryPath = config.libraryDbPath();
QString logPath = config.logFilePath();

// Get/set games directory
QString gamesDir = config.gamesDirectory();
config.setGamesDirectory("/custom/path/to/games");

// Get/set language
QString lang = config.language();
config.setLanguage("pt_BR");

// Get/set preferred runner
QString runner = config.preferredRunner();
config.setPreferredRunner("Proton");

// Other settings
bool autoRefresh = config.autoRefreshLibrary();
config.setAutoRefreshLibrary(true);

bool cloudSaves = config.enableCloudSaves();
config.setEnableCloudSaves(false);

// Window state (for UI)
QByteArray geometry = config.windowGeometry();
config.setWindowGeometry(mainWindow->saveGeometry());

QByteArray state = config.windowState();
config.setWindowState(mainWindow->saveState());
```

## Settings File Format

The settings are stored in INI format (`OpenGalaxy.ini`):

```ini
[install]
gamesDirectory=/home/user/Games

[ui]
locale=en_US

[runners]
preferred=Proton

[library]
autoRefresh=true

[features]
cloudSaves=false

[window]
geometry=@ByteArray(...)
state=@ByteArray(...)
```

## Session Persistence

### How It Works

1. **Login**: User logs in with GOG credentials
2. **Token Storage**: Access and refresh tokens are saved to `session.json`
3. **Auto-Login**: On next startup, tokens are loaded automatically
4. **Token Refresh**: Expired tokens are refreshed automatically
5. **Cross-Platform**: Works identically on all platforms

### Session File Location

The session file is stored in the platform-specific data directory:

- **Linux**: `~/.local/share/opengalaxy/session.json`
- **macOS**: `~/Library/Application Support/opengalaxy/session.json`
- **Windows**: `%APPDATA%\opengalaxy\session.json`

### Session File Format

```json
{
  "accessToken": "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9...",
  "refreshToken": "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9...",
  "expiresAt": "2026-01-24T12:00:00Z"
}
```

### Security

- File permissions: `0600` (read/write for user only)
- Tokens are encrypted in transit (HTTPS)
- Tokens expire after ~1 hour
- Refresh tokens valid for ~30 days
- Automatic cleanup on logout

## Games Directory Configuration

### Default Locations

OpenGalaxy uses sensible defaults for game installations:

- **Linux**: `~/Games/`
- **macOS**: `~/Games/`
- **Windows**: `%USERPROFILE%\Games\`

### Customizing Games Directory

#### Via UI

1. Open **Settings** page
2. Go to **Installation** section
3. Click **Browse** next to "Games Directory"
4. Select your preferred location
5. Click **Save**

#### Via CLI

```bash
# The games directory is used automatically for installations
opengalaxy-cli install --game GAME_ID --dir /custom/path
```

#### Programmatically

```cpp
#include "opengalaxy/util/config.h"

// Set custom games directory
opengalaxy::util::Config::instance().setGamesDirectory("/mnt/games");

// Get current games directory
QString gamesDir = opengalaxy::util::Config::instance().gamesDirectory();
```

### Per-Game Installation Paths

Each game can be installed to a different location:

```
~/Games/
├── Cyberpunk 2077/
├── The Witcher 3/
└── ...

/mnt/ssd/Games/
├── Fast Game 1/
└── Fast Game 2/

/mnt/hdd/Games/
├── Large Game 1/
└── Large Game 2/
```

## Platform-Specific Considerations

### Linux

- Uses XDG Base Directory specification
- Config: `$XDG_CONFIG_HOME/OpenGalaxy/` (defaults to `~/.config/OpenGalaxy/`)
- Data: `$XDG_DATA_HOME/opengalaxy/` (defaults to `~/.local/share/opengalaxy/`)
- Respects environment variables

### macOS

- Follows Apple's File System Programming Guide
- Config: `~/Library/Preferences/OpenGalaxy/`
- Data: `~/Library/Application Support/opengalaxy/`
- Integrates with macOS Keychain (future enhancement)

### Windows

- Uses Windows Known Folders
- Config: `%APPDATA%\OpenGalaxy\`
- Data: `%APPDATA%\opengalaxy\`
- Integrates with Windows Credential Manager (future enhancement)

## Migration from Old Versions

If you're upgrading from an older version that used different paths:

### Automatic Migration

OpenGalaxy will automatically:
1. Check for old session files
2. Migrate to new locations
3. Preserve all settings
4. Update game library paths

### Manual Migration

If automatic migration fails:

```bash
# Linux
mv ~/.opengalaxy/* ~/.local/share/opengalaxy/
mv ~/.config/opengalaxy.conf ~/.config/OpenGalaxy/OpenGalaxy.ini

# macOS
mv ~/Library/Application\ Support/opengalaxy.old/* ~/Library/Application\ Support/opengalaxy/

# Windows (PowerShell)
Move-Item "$env:APPDATA\opengalaxy.old\*" "$env:APPDATA\opengalaxy\"
```

## Troubleshooting

### Can't Find Configuration File

```bash
# Linux
ls -la ~/.config/OpenGalaxy/OpenGalaxy.ini

# macOS
ls -la ~/Library/Preferences/OpenGalaxy/OpenGalaxy.ini

# Windows (PowerShell)
Get-ChildItem "$env:APPDATA\OpenGalaxy\OpenGalaxy.ini"
```

### Session Not Persisting

1. Check file permissions:
   ```bash
   # Linux/macOS
   ls -l ~/.local/share/opengalaxy/session.json
   # Should show: -rw------- (600)
   ```

2. Check disk space:
   ```bash
   df -h ~
   ```

3. Check logs:
   ```bash
   tail -50 ~/.local/share/opengalaxy/opengalaxy.log | grep session
   ```

### Games Directory Not Created

The games directory is created automatically when:
- First game is installed
- Settings are saved with a custom path

To create manually:
```bash
# Linux/macOS
mkdir -p ~/Games

# Windows (PowerShell)
New-Item -ItemType Directory -Path "$env:USERPROFILE\Games"
```

### Reset Configuration

To reset all settings to defaults:

```bash
# Linux
rm ~/.config/OpenGalaxy/OpenGalaxy.ini

# macOS
rm ~/Library/Preferences/OpenGalaxy/OpenGalaxy.ini

# Windows (PowerShell)
Remove-Item "$env:APPDATA\OpenGalaxy\OpenGalaxy.ini"
```

**Note**: This will NOT delete your session or game library.

## Environment Variables

OpenGalaxy respects these environment variables:

| Variable | Purpose | Example |
|----------|---------|---------|
| `XDG_CONFIG_HOME` | Config directory (Linux) | `~/.config` |
| `XDG_DATA_HOME` | Data directory (Linux) | `~/.local/share` |
| `APPDATA` | App data directory (Windows) | `C:\Users\...\AppData\Roaming` |
| `HOME` / `USERPROFILE` | User home directory | `/home/user` |

## Best Practices

### For Users

1. **Don't manually edit** `session.json` - use the UI to login/logout
2. **Backup settings** before major updates
3. **Use default paths** unless you have a specific reason to change them
4. **Check disk space** in games directory regularly

### For Developers

1. **Always use Config class** - never hardcode paths
2. **Initialize Config early** - call `Config::initialize()` in `main()`
3. **Use appropriate getters** - `dataDir()`, `configDir()`, `gamesDirectory()`
4. **Sync settings** - Config class handles this automatically
5. **Test on all platforms** - paths differ significantly

## API Reference

### Config Class Methods

```cpp
// Initialization
static void initialize();
static Config& instance();

// Path getters
QString dataDir() const;
QString configDir() const;
QString sessionFilePath() const;
QString libraryDbPath() const;
QString logFilePath() const;
QString defaultGamesDir() const;

// Settings accessors
QString gamesDirectory() const;
void setGamesDirectory(const QString& path);

QString language() const;
void setLanguage(const QString& locale);

QString preferredRunner() const;
void setPreferredRunner(const QString& runner);

bool autoRefreshLibrary() const;
void setAutoRefreshLibrary(bool enabled);

bool enableCloudSaves() const;
void setEnableCloudSaves(bool enabled);

// Window state
QByteArray windowGeometry() const;
void setWindowGeometry(const QByteArray& geometry);

QByteArray windowState() const;
void setWindowState(const QByteArray& state);

// Utilities
void ensureDirectoriesExist();
void clear();
```

## See Also

- [DATA_LOCATIONS.md](DATA_LOCATIONS.md) - Detailed data storage information
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Common configuration issues
- [BUILD.md](BUILD.md) - Building with configuration support

---

**Last Updated**: 2026-01-23
