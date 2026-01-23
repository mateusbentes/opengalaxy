# Cross-Platform Icon Implementation

## Overview
OpenGalaxy now has proper icon support for **Windows, macOS, and Linux** with platform-specific formats and installation.

## Icon Files

### Source
- **PNG**: `data/opengalaxyicon.png` (1536x1024, 2.1 MB)
  - Used as source for all platform-specific formats
  - Embedded in Qt resources for runtime use

### Platform-Specific Icons

#### Windows
- **File**: `data/opengalaxyicon.ico` (134 KB)
- **Format**: Multi-resolution ICO (256, 128, 64, 48, 32, 16 px)
- **Usage**: 
  - Embedded via `data/opengalaxy.rc` resource file
  - Shows in taskbar, window title, and .exe file icon
  - Automatically included during build

#### macOS
- **File**: `data/opengalaxyicon.icns` (to be generated on macOS)
- **Format**: Apple Icon Image format
- **Usage**:
  - Embedded in app bundle Resources
  - Shows in Dock, Finder, and app switcher
  - Generated using `iconutil` on macOS

#### Linux
- **File**: `data/opengalaxyicon.png` (original PNG)
- **Format**: PNG (any size, will be scaled)
- **Usage**:
  - Installed to multiple locations:
    - `/usr/share/icons/hicolor/256x256/apps/opengalaxy.png`
    - `/usr/share/icons/hicolor/scalable/apps/opengalaxy.png`
    - `/usr/share/pixmaps/opengalaxy.png` (fallback)
  - Referenced in `.desktop` file
  - Shows in application menu, taskbar, and window manager

## How It Works

### At Runtime (All Platforms)
```cpp
// In app_window.cpp
setWindowIcon(QIcon(":/data/opengalaxyicon.png"));
```
- Icon embedded in Qt resources (`resources.qrc`)
- Loaded from memory at runtime
- Shows in window title bar and taskbar

### At Installation

#### Windows
1. **Build Time**: 
   - `opengalaxy.rc` compiled into executable
   - ICO file embedded in .exe
2. **Result**: 
   - .exe file shows icon in Explorer
   - Running app shows icon in taskbar
   - No separate installation needed

#### macOS
1. **Build Time**:
   - App built as bundle (`OpenGalaxy.app`)
   - ICNS file copied to `OpenGalaxy.app/Contents/Resources/`
2. **Result**:
   - App shows icon in Finder
   - Running app shows icon in Dock
   - No separate installation needed

#### Linux
1. **Install Time**:
   ```bash
   sudo cmake --install build
   ```
   - PNG copied to hicolor theme directories
   - `.desktop` file installed to `/usr/share/applications/`
2. **Result**:
   - Icon appears in application menu
   - Desktop file associates icon with app
   - Window manager shows icon in taskbar

## Generating Icons

### Automatic (Recommended)
Run the generation script:
```bash
cd data
./generate_icons.sh
```

This will:
- ✅ Generate Windows .ico (requires ImageMagick)
- ✅ Generate macOS .icns (requires icnsutils or iconutil)
- ✅ Use existing PNG for Linux

### Manual Generation

#### Windows .ico
```bash
# Using ImageMagick
convert opengalaxyicon.png -define icon:auto-resize=256,128,64,48,32,16 opengalaxyicon.ico
```

#### macOS .icns
```bash
# On macOS using iconutil
mkdir opengalaxy.iconset
sips -z 16 16     opengalaxyicon.png --out opengalaxy.iconset/icon_16x16.png
sips -z 32 32     opengalaxyicon.png --out opengalaxy.iconset/icon_16x16@2x.png
sips -z 32 32     opengalaxyicon.png --out opengalaxy.iconset/icon_32x32.png
sips -z 64 64     opengalaxyicon.png --out opengalaxy.iconset/icon_32x32@2x.png
sips -z 128 128   opengalaxyicon.png --out opengalaxy.iconset/icon_128x128.png
sips -z 256 256   opengalaxyicon.png --out opengalaxy.iconset/icon_128x128@2x.png
sips -z 256 256   opengalaxyicon.png --out opengalaxy.iconset/icon_256x256.png
sips -z 512 512   opengalaxyicon.png --out opengalaxy.iconset/icon_256x256@2x.png
sips -z 512 512   opengalaxyicon.png --out opengalaxy.iconset/icon_512x512.png
sips -z 1024 1024 opengalaxyicon.png --out opengalaxy.iconset/icon_512x512@2x.png
iconutil -c icns opengalaxy.iconset
rm -rf opengalaxy.iconset
```

#### Linux PNG
No conversion needed - use the original PNG file.

## CMake Configuration

The `ui/CMakeLists.txt` now includes:

### Windows
```cmake
if(WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_SOURCE_DIR}/data/opengalaxy.rc")
    set_target_properties(opengalaxy PROPERTIES WIN32_EXECUTABLE TRUE)
    target_sources(opengalaxy PRIVATE ${APP_ICON_RESOURCE})
endif()
```

### macOS
```cmake
elseif(APPLE)
    set(MACOSX_ICON_FILE "${CMAKE_SOURCE_DIR}/data/opengalaxyicon.icns")
    if(EXISTS ${MACOSX_ICON_FILE})
        set_target_properties(opengalaxy PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_ICON_FILE opengalaxyicon.icns
        )
        set_source_files_properties(${MACOSX_ICON_FILE} PROPERTIES
            MACOSX_PACKAGE_LOCATION Resources
        )
        target_sources(opengalaxy PRIVATE ${MACOSX_ICON_FILE})
    endif()
endif()
```

### Linux
```cmake
if(UNIX AND NOT APPLE)
    set(ICON_SOURCE "${CMAKE_SOURCE_DIR}/data/opengalaxyicon.png")
    if(EXISTS ${ICON_SOURCE})
        install(FILES ${ICON_SOURCE}
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME opengalaxy.png
        )
        install(FILES ${ICON_SOURCE}
            DESTINATION share/icons/hicolor/scalable/apps
            RENAME opengalaxy.png
        )
        install(FILES ${ICON_SOURCE}
            DESTINATION share/pixmaps
            RENAME opengalaxy.png
        )
    endif()
endif()
```

## Testing

### Windows
1. Build the project
2. Check if `opengalaxy.exe` has an icon in Explorer
3. Run the app and verify taskbar icon
4. Check window title bar icon

### macOS
1. Build the project
2. Check if `OpenGalaxy.app` has an icon in Finder
3. Run the app and verify Dock icon
4. Check window title bar icon

### Linux
1. Build and install:
   ```bash
   ./build.sh
   sudo cmake --install build
   ```
2. Check application menu for OpenGalaxy icon
3. Run the app and verify taskbar icon
4. Check window title bar icon
5. Verify icon files installed:
   ```bash
   ls -l /usr/share/icons/hicolor/256x256/apps/opengalaxy.png
   ls -l /usr/share/pixmaps/opengalaxy.png
   ```

## Dependencies

### For Icon Generation

#### Linux
```bash
# For Windows .ico generation
sudo apt install imagemagick

# For macOS .icns generation (optional on Linux)
sudo apt install icnsutils
```

#### macOS
```bash
# iconutil is included with Xcode Command Line Tools
xcode-select --install
```

#### Windows
- ImageMagick: https://imagemagick.org/script/download.php#windows
- Or use online converters

## Troubleshooting

### Icon not showing on Linux
1. Update icon cache:
   ```bash
   sudo gtk-update-icon-cache /usr/share/icons/hicolor/
   ```
2. Verify .desktop file:
   ```bash
   desktop-file-validate /usr/share/applications/opengalaxy.desktop
   ```

### Icon not showing on Windows
1. Rebuild the project completely
2. Check if `opengalaxy.rc` is in the build
3. Verify `opengalaxyicon.ico` exists in `data/`

### Icon not showing on macOS
1. Verify ICNS file exists
2. Check bundle structure:
   ```bash
   ls -la OpenGalaxy.app/Contents/Resources/
   ```
3. Clear icon cache:
   ```bash
   sudo rm -rf /Library/Caches/com.apple.iconservices.store
   killall Dock
   ```

## Summary

✅ **Windows**: Icon embedded in .exe via .rc file  
✅ **macOS**: Icon embedded in app bundle as .icns  
✅ **Linux**: Icon installed to system directories  
✅ **Runtime**: Icon loaded from Qt resources for window  
✅ **Cross-platform**: Single PNG source, platform-specific formats generated  

The icon will appear correctly on all platforms after installation!
