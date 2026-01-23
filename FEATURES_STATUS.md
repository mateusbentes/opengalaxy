# OpenGalaxy Features Status

## ✅ Fully Implemented Features

### 1. Compatibility Layers / ISA Translation
**Status**: Production-ready

OpenGalaxy includes comprehensive compatibility layer support for running games across different platforms and architectures:

#### Supported Runners:
- **Native Linux**: Direct execution of Linux games
- **Wine**: Windows games on Linux (x86_64)
- **Proton-GE**: Auto-discovers Steam compatibility tools
- **Cross-Architecture Wrappers**:
  - **Box64**: ARM64 → x86_64 translation on Linux
  - **FEX-Emu**: ARM64 → x86_64 translation on Linux
  - **QEMU**: Generic CPU emulation
  - **Rosetta2**: ARM64 → x86_64 on macOS

#### How It Works:
- `RunnerManager` automatically discovers available runners on system startup
- `findBestRunner()` selects the optimal runner based on:
  - Game platform (Windows/Linux/Mac)
  - Host architecture (x86_64/ARM64)
  - Runner availability
- Users can override runner selection per-game via settings
- All runners use `QProcess` for execution with proper environment variables and arguments

#### Implementation:
- Core: `core/src/runners/`
- Files: `runner_manager.cpp`, `wine_runner.cpp`, `proton_runner.cpp`, `wrapper_runner.cpp`

---

### 2. Game Installation
**Status**: Production-ready

Full game download and installation functionality with GOG API integration.

#### Features:
- ✅ Async HTTP downloads with progress tracking
- ✅ GOG API integration for download URL resolution
- ✅ Wine installer execution for Windows games
- ✅ Thread-safe installation management
- ✅ Install/Cancel/Uninstall operations
- ✅ Real-time progress updates via signals
- ✅ CLI support: `opengalaxy-cli install --game ID --dir PATH`

#### UI Integration:
- Install/Cancel/Play button states in game cards
- Progress bars with percentage and download speed
- Toast notifications for success/failure
- Library page integration

#### Recent Fixes (2026-01-23):
- ✅ Fixed "Protocol unknown" error by adding URL validation
- ✅ Added support for protocol-relative URLs (`//cdn.gog.com/...`)
- ✅ Validates both initial download URLs and resolved downlinks
- ✅ Proper error handling for empty/malformed URLs

#### Implementation:
- Core: `core/src/install/install_service.cpp`
- UI: `ui/qt/pages/library_page.cpp`, `ui/qt/widgets/game_card.cpp`

---

## ⚠️ Partially Implemented / Limited Features

### 3. Cloud Game Saves
**Status**: Not functional (stub implementation only)

#### Current State:
- ✅ Data models defined (`CloudSave` struct)
- ✅ API methods declared in `GOGClient`
- ✅ UI dialog skeleton exists
- ✅ Settings toggle present
- ❌ **No real implementation** - returns empty lists/errors

#### Why Not Implemented:
GOG does not provide a public REST API for cloud saves. The official implementation requires:

1. **GOG Galaxy SDK Integration**: Cloud saves use the proprietary `IStorage` interface from the GOG Galaxy SDK (C++ library)
2. **Client-Side Sync**: The official Galaxy client handles automatic synchronization between local storage and GOG's cloud
3. **No HTTP Endpoints**: Unlike library/store APIs, cloud saves don't have documented HTTP endpoints

#### Alternatives:
- **Use Official Galaxy Client**: For cloud save functionality, users should use the official GOG Galaxy client alongside OpenGalaxy
- **Manual Sync**: Users can manually backup/restore save files using external tools (rsync, Syncthing, etc.)
- **Future Implementation**: Possible approaches:
  - Integrate GOG Galaxy SDK (requires proprietary library)
  - Reverse-engineer Galaxy client protocol (complex, may violate ToS)
  - Wait for GOG to provide public API

#### Implementation:
- Core: `core/src/api/gog_client.cpp` (lines 156-169)
- UI: `ui/qt/dialogs/cloud_saves_dialog.cpp`
- Models: `core/include/opengalaxy/api/models.h`

---

## 📊 Feature Comparison

| Feature | Status | Notes |
|---------|--------|-------|
| Game Library | ✅ Complete | Fetches owned games from GOG API |
| Store Browse/Search | ✅ Complete | Public GOG store API |
| Game Installation | ✅ Complete | Fixed HTTP protocol errors |
| Wine/Proton Support | ✅ Complete | Auto-detection and selection |
| ISA Translation | ✅ Complete | Box64, FEX, QEMU, Rosetta2 |
| Game Launching | ✅ Complete | Via runner system |
| Cloud Saves | ❌ Not Available | Requires GOG Galaxy SDK |
| Achievements | ⚠️ Partial | API exists, UI incomplete |
| Friends/Social | ⚠️ Partial | API exists, UI incomplete |
| DLC Management | ⚠️ Partial | Detection works, install needs work |
| Overlay | ❌ Not Planned | Requires deep system integration |

---

## 🔧 Technical Details

### Installation Error Fix (2026-01-23)

**Problem**: HTTP requests failing with "Protocol '' is unknown"

**Root Cause**: GOG API sometimes returns:
- Empty download URLs
- Protocol-relative URLs (`//cdn.gog.com/...`)

**Solution**: Added validation in `install_service.cpp`:
```cpp
// Validate URL
if (selected.url.isEmpty()) {
    return error("No valid download URL");
}

// Fix protocol-relative URLs
QString downloadUrl = selected.url;
if (downloadUrl.startsWith("//")) {
    downloadUrl = "https:" + downloadUrl;
}
```

Applied to both:
1. Initial download URL (line 80-94)
2. Resolved downlink URL (line 113-120)

---

## 🚀 Next Steps

### High Priority:
1. ~~Fix installation HTTP errors~~ ✅ **DONE**
2. Improve DLC installation workflow
3. Complete achievements UI
4. Add update checking for installed games

### Medium Priority:
1. Friends list and social features UI
2. Game time tracking
3. Screenshot management
4. Controller configuration

### Low Priority / Future:
1. Cloud saves (waiting on GOG API or SDK integration)
2. Mod management
3. Community features
4. Streaming/remote play

---

## 📝 Notes for Contributors

### Testing Installation:
```bash
# Build the project
./build.sh

# Test CLI installation
./build/cli/opengalaxy-cli install --game <GAME_ID> --dir ~/Games

# Test GUI
./build/ui/qt/opengalaxy
```

### Adding New Runners:
1. Create runner class inheriting from `Runner`
2. Implement `canRun()`, `launch()`, `isAvailable()`
3. Register in `RunnerManager::discoverRunners()`
4. Add platform/architecture detection logic

### Cloud Saves Research:
If you want to help implement cloud saves:
- Research: GOG Galaxy SDK documentation
- Check: Heroic Games Launcher's implementation
- Consider: Legal implications of reverse engineering
- Alternative: Petition GOG for public API

---

## 📚 References

- [GOG API Documentation (Unofficial)](https://gogapidocs.readthedocs.io/)
- [GOG Galaxy SDK Docs](https://docs.gog.com/sdk-storage/)
- [MiniGalaxy Project](https://github.com/sharkwouter/minigalaxy)
- [Heroic Games Launcher](https://github.com/Heroic-Games-Launcher/HeroicGamesLauncher)
