# Missing Features in OpenGalaxy

## Overview

This document lists features that are commonly found in similar game launchers (minigalaxy, Heroic, etc.) but are currently missing from OpenGalaxy's Properties dialog.

---

## Missing Toggle Switches

### 1. Show FPS in Game

**What it does**:
- Displays FPS counter overlay in-game
- Shows frame rate in real-time
- Helps monitor performance

**How it works**:
- Sets `DXVK_HUD=fps` environment variable
- Works with DXVK/VKD3D
- Visible in top-left corner of game

**UI**:
```
☐ Show FPS in game
```

**Implementation**:
- Add checkbox to Properties dialog
- Add `bool showFpsOverlay` to GameInfo struct
- Set `DXVK_HUD=fps` when enabled
- Persist setting in database

**Status**: ❌ Not Implemented

---

### 2. Hide Game

**What it does**:
- Hides game from library view
- Game still installed but not visible
- Useful for hiding unfinished/test games

**How it works**:
- Sets `isHidden` flag in database
- Filters library display
- Can be unhidden in settings

**UI**:
```
☐ Hide game
```

**Implementation**:
- Add checkbox to Properties dialog
- Add `bool isHidden` to GameInfo struct
- Filter library view based on flag
- Add "Show hidden games" option in settings
- Persist setting in database

**Status**: ❌ Not Implemented

---

### 3. Use GameMode

**What it does**:
- Enables GameMode for better performance
- Reduces CPU/GPU latency
- Improves frame consistency

**How it works**:
- Launches game with `gamemoderun` wrapper
- Requires GameMode to be installed
- Automatically optimizes system resources

**UI**:
```
☐ Use GameMode
```

**Installation**:
```bash
# Ubuntu/Debian
sudo apt install gamemode

# Fedora
sudo dnf install gamemode

# Arch
sudo pacman -S gamemode
```

**Implementation**:
- Add checkbox to Properties dialog
- Add `bool useGameMode` to GameInfo struct
- Prepend `gamemoderun` to runner command
- Check if GameMode is installed
- Persist setting in database

**Status**: ❌ Not Implemented

---

### 4. Use MangoHud

**What it does**:
- Displays performance overlay (FPS, CPU, GPU, temps)
- Shows detailed system information
- Helps with performance monitoring

**How it works**:
- Launches game with `mangohud` wrapper
- Requires MangoHud to be installed
- Configurable overlay display

**UI**:
```
☐ Use MangoHud
```

**Installation**:
```bash
# Ubuntu/Debian
sudo apt install mangohud

# Fedora
sudo dnf install mangohud

# Arch
sudo pacman -S mangohud
```

**Configuration**:
```
MangoHud Settings
├─ Enable: [✓]
├─ Position: [Top-Left ▼]
├─ Show FPS: [✓]
├─ Show CPU: [✓]
├─ Show GPU: [✓]
├─ Show Temps: [✓]
└─ Show RAM: [✓]
```

**Implementation**:
- Add checkbox to Properties dialog
- Add `bool useMangoHud` to GameInfo struct
- Prepend `mangohud` to runner command
- Check if MangoHud is installed
- Add MangoHud configuration options
- Persist setting in database

**Status**: ❌ Not Implemented

---

### 5. Enable Cloud Saves (Per-Game)

**What it does**:
- Enables cloud save synchronization for this game
- Can be disabled per-game if desired
- Overrides global cloud save setting

**How it works**:
- Syncs saves to GOG Cloud
- Requires GOG SDK (not yet available)
- Can be toggled per-game

**UI**:
```
☐ Enable Cloud Saves
```

**Implementation** (Pending GOG SDK):
- Add checkbox to Properties dialog
- Add `bool cloudSavesEnabled` to GameInfo struct
- Implement sync logic when SDK available
- Persist setting in database

**Status**: ⏳ Pending GOG SDK

---

### 6. Verify Game Files

**What it does**:
- Checks game file integrity
- Detects corrupted or missing files
- Can repair/redownload if needed

**How it works**:
- Compares local files with checksums
- Downloads missing files
- Repairs corrupted files

**UI**:
```
[🔍 Verify Files] [⚙️ Repair]
```

**Implementation**:
- Add button to Properties dialog
- Implement file verification logic
- Compare with GOG checksums
- Download missing/corrupted files
- Show progress dialog

**Status**: ❌ Not Implemented

---

## Missing Buttons/Options

### 1. Verify Game Files Button

**Purpose**: Check and repair game installation

**Implementation**:
```cpp
auto* verifyBtn = new QPushButton(tr("Verify Files"), this);
connect(verifyBtn, &QPushButton::clicked, this, &GameDetailsDialog::verifyGameFiles);
```

**Status**: ❌ Not Implemented

---

### 2. Repair Game Button

**Purpose**: Repair corrupted game files

**Implementation**:
```cpp
auto* repairBtn = new QPushButton(tr("Repair"), this);
connect(repairBtn, &QPushButton::clicked, this, &GameDetailsDialog::repairGame);
```

**Status**: ❌ Not Implemented

---

### 3. Open Game Folder Button

**Purpose**: Open game installation directory in file manager

**Implementation**:
```cpp
auto* openFolderBtn = new QPushButton(tr("Open Folder"), this);
connect(openFolderBtn, &QPushButton::clicked, this, [this]() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(game_.installPath));
});
```

**Status**: ❌ Not Implemented

---

## Implementation Priority

### High Priority (Should Implement Soon)

1. **Show FPS in Game** - Easy to implement, very useful
2. **Use GameMode** - Easy to implement, good performance boost
3. **Use MangoHud** - Easy to implement, useful for monitoring
4. **Open Game Folder** - Very easy, useful feature

### Medium Priority (Nice to Have)

1. **Hide Game** - Useful for organization
2. **Verify Game Files** - Useful for troubleshooting
3. **Repair Game** - Useful for corrupted installs

### Low Priority (Pending SDK)

1. **Enable Cloud Saves** - Requires GOG SDK
2. **Controller Profiles** - Requires input mapping system

---

## Implementation Checklist

### Show FPS in Game
- [ ] Add `bool showFpsOverlay` to GameInfo struct
- [ ] Add checkbox to Properties dialog
- [ ] Implement DXVK_HUD=fps logic
- [ ] Persist in database
- [ ] Test with DXVK games

### Use GameMode
- [ ] Add `bool useGameMode` to GameInfo struct
- [ ] Add checkbox to Properties dialog
- [ ] Check if GameMode is installed
- [ ] Prepend `gamemoderun` to command
- [ ] Persist in database
- [ ] Test with GameMode

### Use MangoHud
- [ ] Add `bool useMangoHud` to GameInfo struct
- [ ] Add checkbox to Properties dialog
- [ ] Check if MangoHud is installed
- [ ] Prepend `mangohud` to command
- [ ] Add MangoHud configuration options
- [ ] Persist in database
- [ ] Test with MangoHud

### Open Game Folder
- [ ] Add button to Properties dialog
- [ ] Implement folder opening logic
- [ ] Test on Linux/Windows/macOS

### Verify Game Files
- [ ] Implement file verification logic
- [ ] Get checksums from GOG API
- [ ] Compare local files
- [ ] Show progress dialog
- [ ] Handle missing/corrupted files

### Hide Game
- [ ] Add `bool isHidden` to GameInfo struct
- [ ] Add checkbox to Properties dialog
- [ ] Filter library view
- [ ] Add "Show hidden games" setting
- [ ] Persist in database

---

## Code Examples

### Adding a Toggle to Properties Dialog

```cpp
// In game_details_dialog.h
private:
    QCheckBox* showFpsCheckbox_ = nullptr;
    QCheckBox* useGameModeCheckbox_ = nullptr;
    QCheckBox* useMangoHudCheckbox_ = nullptr;

// In game_details_dialog.cpp constructor
showFpsCheckbox_ = new QCheckBox(tr("Show FPS in game"), compatBox);
showFpsCheckbox_->setChecked(game_.showFpsOverlay);
form->addRow("", showFpsCheckbox_);

useGameModeCheckbox_ = new QCheckBox(tr("Use GameMode"), compatBox);
useGameModeCheckbox_->setChecked(game_.useGameMode);
form->addRow("", useGameModeCheckbox_);

useMangoHudCheckbox_ = new QCheckBox(tr("Use MangoHud"), compatBox);
useMangoHudCheckbox_->setChecked(game_.useMangoHud);
form->addRow("", useMangoHudCheckbox_);

// In onSave()
game_.showFpsOverlay = showFpsCheckbox_->isChecked();
game_.useGameMode = useGameModeCheckbox_->isChecked();
game_.useMangoHud = useMangoHudCheckbox_->isChecked();
```

### Adding Fields to GameInfo

```cpp
// In core/include/opengalaxy/api/models.h
struct GameInfo {
    // ... existing fields ...
    
    // Display options
    bool showFpsOverlay = false;
    bool useGameMode = false;
    bool useMangoHud = false;
    bool isHidden = false;
    bool cloudSavesEnabled = true;
};
```

### Implementing FPS Overlay

```cpp
// In runner execution
if (game_.showFpsOverlay) {
    env << "DXVK_HUD=fps";
}

if (game_.useGameMode) {
    command = "gamemoderun " + command;
}

if (game_.useMangoHud) {
    command = "mangohud " + command;
}
```

---

## Related Documentation

- [FEATURES_STATUS.md](FEATURES_STATUS.md) - Current feature status
- [FUTURE_FEATURES.md](FUTURE_FEATURES.md) - Planned features
- [WINE_PROTON_TOOLS.md](WINE_PROTON_TOOLS.md) - Wine/Proton tools

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-26 | Initial documentation of missing features |

---

## Contributing

To implement any of these features:

1. Check the implementation checklist
2. Follow the code examples
3. Add database migration if needed
4. Update GameInfo struct
5. Add UI elements to Properties dialog
6. Implement the feature logic
7. Test thoroughly
8. Submit pull request

