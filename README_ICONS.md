# OpenGalaxy Icon Guide

## Quick Answer: YES! ✅

**The icon WILL appear on Windows, macOS, and Linux after installation.**

## How It Works

### 🪟 Windows
- **Icon Format**: `.ico` (multi-resolution)
- **Embedded In**: `opengalaxy.exe` file
- **Shows In**:
  - ✅ Windows Explorer (file icon)
  - ✅ Taskbar (when running)
  - ✅ Window title bar
  - ✅ Alt+Tab switcher
  - ✅ Start menu (if pinned)

**Status**: ✅ **READY** - Icon automatically embedded during build

### 🍎 macOS
- **Icon Format**: `.icns` (Apple format)
- **Embedded In**: `OpenGalaxy.app/Contents/Resources/`
- **Shows In**:
  - ✅ Finder (app icon)
  - ✅ Dock (when running)
  - ✅ Window title bar
  - ✅ Cmd+Tab switcher
  - ✅ Launchpad

**Status**: ⚠️ **NEEDS GENERATION** - Run `./data/generate_icons.sh` on macOS

### 🐧 Linux
- **Icon Format**: `.png` (high-resolution)
- **Installed To**:
  - `/usr/share/icons/hicolor/256x256/apps/opengalaxy.png`
  - `/usr/share/icons/hicolor/scalable/apps/opengalaxy.png`
  - `/usr/share/pixmaps/opengalaxy.png`
- **Shows In**:
  - ✅ Application menu (GNOME, KDE, XFCE, etc.)
  - ✅ Taskbar/panel (when running)
  - ✅ Window title bar
  - ✅ Desktop shortcuts

**Status**: ✅ **READY** - Icon automatically installed with `sudo cmake --install build`

## Installation Instructions

### For End Users

#### Windows
1. Download `opengalaxy.exe`
2. Run it - icon is already embedded!
3. No additional steps needed

#### macOS
1. Download `OpenGalaxy.app`
2. Drag to Applications folder
3. Run it - icon is already embedded!
4. No additional steps needed

#### Linux
1. Install the package:
   ```bash
   sudo cmake --install build
   ```
2. Icon automatically installed to system
3. Launch from application menu
4. No additional steps needed

### For Developers

#### Building on Windows
```bash
# Icon automatically embedded during build
./build.sh
# or
cmake -B build && cmake --build build
```

#### Building on macOS
```bash
# First, generate the .icns file
cd data
./generate_icons.sh

# Then build normally
cd ..
./build.sh
```

#### Building on Linux
```bash
# Icon generation (optional, for Windows .ico)
cd data
./generate_icons.sh

# Build
cd ..
./build.sh

# Install (installs icon to system)
sudo cmake --install build
```

## Icon Files

### Source
- `data/opengalaxyicon.png` - Original high-res PNG (1536x1024, 2.1 MB)

### Generated
- `data/opengalaxyicon.ico` - Windows icon (134 KB) ✅
- `data/opengalaxyicon.icns` - macOS icon (generate on macOS) ⚠️

### Resource
- `ui/qt/resources/resources.qrc` - Embeds PNG for runtime use

## Troubleshooting

### Icon not showing on Windows?
1. Rebuild the project completely
2. Check if `data/opengalaxyicon.ico` exists
3. Verify `data/opengalaxy.rc` is present

### Icon not showing on macOS?
1. Generate the .icns file:
   ```bash
   cd data && ./generate_icons.sh
   ```
2. Rebuild the app
3. Clear icon cache:
   ```bash
   sudo rm -rf /Library/Caches/com.apple.iconservices.store
   killall Dock
   ```

### Icon not showing on Linux?
1. Update icon cache:
   ```bash
   sudo gtk-update-icon-cache /usr/share/icons/hicolor/
   ```
2. Verify installation:
   ```bash
   ls -l /usr/share/icons/hicolor/256x256/apps/opengalaxy.png
   ```
3. Check desktop file:
   ```bash
   desktop-file-validate /usr/share/applications/opengalaxy.desktop
   ```

## Technical Details

### CMake Configuration
The `ui/CMakeLists.txt` automatically:
- Embeds `.ico` in Windows executables
- Bundles `.icns` in macOS app bundles
- Installs `.png` to Linux system directories

### Qt Resources
The icon is also embedded in Qt resources for runtime use:
```cpp
setWindowIcon(QIcon(":/data/opengalaxyicon.png"));
```

This ensures the icon appears in the window title bar on all platforms, even before installation.

## Summary

| Platform | Icon Format | Status | Shows Everywhere? |
|----------|-------------|--------|-------------------|
| Windows  | .ico        | ✅ Ready | ✅ Yes |
| macOS    | .icns       | ⚠️ Generate | ✅ Yes (after generation) |
| Linux    | .png        | ✅ Ready | ✅ Yes |

**Bottom Line**: The icon will appear correctly on all platforms! Windows and Linux are ready to go. macOS just needs the `.icns` file generated on a Mac.

## Need Help?

See detailed documentation:
- `CROSS_PLATFORM_ICONS.md` - Complete technical guide
- `ICON_IMPLEMENTATION_SUMMARY.md` - Implementation details
- `data/generate_icons.sh` - Icon generation script

---

**Made with ❤️ for OpenGalaxy**
