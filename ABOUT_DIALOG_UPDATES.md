# About Dialog - New Features Added

## ✅ New Features Implemented

### 1. System Information Display

The About dialog now shows detailed system information:

#### Information Displayed
- **Operating System**: Full OS name and version
  - Example: "Ubuntu 24.04.3 LTS"
  - Example: "Windows 11 (10.0)"
  - Example: "macOS Sonoma (14.0)"
- **CPU Architecture**: System architecture
  - Example: "x86_64", "arm64", "i386"
- **Kernel Version**: OS kernel version
  - Linux: "6.8.0-51-generic"
  - Windows: "10.0.22631"
  - macOS: "23.0.0"
- **Qt Version**: Qt framework version used to build the app
  - Example: "Qt 6.8.1"

#### Visual Design
- **Title**: "System Information" (bold, 14px)
- **Background**: Light gray (#f5f5f5)
- **Border**: Rounded corners (5px radius)
- **Text**: Small font (11px), gray color
- **Format**: HTML with bold labels

#### Example Display
```
System Information
┌─────────────────────────────────────────┐
│ OS: Ubuntu 24.04.3 LTS (x86_64)        │
│ Kernel: 6.8.0-51-generic               │
│ Qt Version: Qt 6.8.1                   │
└─────────────────────────────────────────┘
```

### 2. Check for Updates Button

A new button that helps users check for new versions:

#### Features
- **Button Text**: "Check for Updates"
- **Color**: Blue (#3498db) - distinct from other buttons
- **Action**: Opens GitHub releases page in default browser
- **Feedback**: Shows information dialog confirming action

#### Behavior
1. User clicks "Check for Updates"
2. Opens: https://github.com/mateusbentes/opengalaxy/releases
3. Shows message box:
   ```
   Opening the GitHub releases page in your browser.
   
   Current version: 1.0.0
   
   Please check if a newer version is available.
   ```

#### Visual Design
- **Background**: Blue (#3498db)
- **Hover**: Darker blue (#2980b9)
- **Pressed**: Even darker (#21618c)
- **Text**: White, 13px, bold
- **Padding**: 10px vertical, 20px horizontal
- **Border Radius**: 8px

## Updated About Dialog Layout

```
┌─────────────────────────────────────────────┐
│                                             │
│              [OpenGalaxy Icon]              │
│                 128x128                     │
│                                             │
│              OpenGalaxy                     │
│             Version 1.0.0                   │
│                                             │
│  OpenGalaxy is a free and open-source      │
│  multiplatform GOG client.                 │
│                                             │
│  It allows you to download, install, and   │
│  play your GOG games on Windows, macOS,    │
│  and Linux with support for Wine, Proton,  │
│  and native games.                         │
│                                             │
│  Inspired by Minigalaxy, OpenGalaxy        │
│  provides a modern and user-friendly       │
│  interface to manage your GOG library      │
│  across all platforms.                     │
│                                             │
│  Project: github.com/mateusbentes/opengalaxy│
│  License: Apache 2.0                       │
│  Website: GOG.com                          │
│                                             │
│  System Information                        │ ← NEW!
│  ┌───────────────────────────────────────┐ │
│  │ OS: Ubuntu 24.04.3 LTS (x86_64)      │ │
│  │ Kernel: 6.8.0-51-generic             │ │
│  │ Qt Version: Qt 6.8.1                 │ │
│  └───────────────────────────────────────┘ │
│                                             │
│          [Check for Updates]                │ ← NEW!
│                                             │
│                  [Close]                    │
│                                             │
└─────────────────────────────────────────────┘
```

## Technical Implementation

### System Information Collection

Uses Qt's `QSysInfo` class:

```cpp
#include <QSysInfo>

// Get system information
QString osInfo = QSysInfo::prettyProductName();      // "Ubuntu 24.04.3 LTS"
QString cpuArch = QSysInfo::currentCpuArchitecture(); // "x86_64"
QString qtVersion = QString("Qt %1").arg(QT_VERSION_STR); // "Qt 6.8.1"
QString kernelVersion = QSysInfo::kernelVersion();    // "6.8.0-51-generic"
```

### Check for Updates Implementation

Uses Qt's `QDesktopServices` to open URLs:

```cpp
#include <QDesktopServices>
#include <QUrl>

void SettingsPage::onCheckForUpdates()
{
    // Open GitHub releases page
    QDesktopServices::openUrl(QUrl("https://github.com/mateusbentes/opengalaxy/releases"));
    
    // Show confirmation message
    QMessageBox::information(
        this,
        tr("Check for Updates"),
        tr("Opening the GitHub releases page in your browser.\n\n"
           "Current version: 1.0.0\n\n"
           "Please check if a newer version is available.")
    );
}
```

## Files Modified

### Header File
- **File**: `ui/qt/pages/settings_page.h`
- **Changes**: Added `onCheckForUpdates()` slot

### Implementation File
- **File**: `ui/qt/pages/settings_page.cpp`
- **Changes**:
  - Added `#include <QSysInfo>`
  - Added `#include <QDesktopServices>`
  - Added `#include <QUrl>`
  - Updated `onAboutClicked()` to include system info and update button
  - Implemented `onCheckForUpdates()` function
  - Increased dialog minimum size to 550x550

## Platform-Specific Information

### Linux
```
OS: Ubuntu 24.04.3 LTS (x86_64)
Kernel: 6.8.0-51-generic
Qt Version: Qt 6.8.1
```

### Windows
```
OS: Windows 11 (10.0) (x86_64)
Kernel: 10.0.22631
Qt Version: Qt 6.8.1
```

### macOS
```
OS: macOS Sonoma (14.0) (arm64)
Kernel: 23.0.0
Qt Version: Qt 6.8.1
```

## Benefits

### For Users
1. **Transparency**: See exactly what system they're running on
2. **Debugging**: Easier to report issues with system info
3. **Updates**: Easy way to check for new versions
4. **Trust**: Shows technical details about the application

### For Developers
1. **Bug Reports**: Users can easily provide system information
2. **Compatibility**: See what Qt version is being used
3. **Support**: Easier to diagnose platform-specific issues
4. **Updates**: Direct users to releases page

## Testing Checklist

- [x] System information displays correctly
- [x] OS name shows properly
- [x] CPU architecture is correct
- [x] Qt version matches build
- [x] Kernel version is accurate
- [x] "Check for Updates" button is visible
- [x] Button opens GitHub releases page
- [x] Confirmation message appears
- [x] Dialog size accommodates new content
- [x] All text is translatable (uses tr())
- [x] Styling matches app theme
- [x] Build successful

## Future Enhancements

Potential improvements:
- [ ] Automatic update checking on startup
- [ ] Download updates directly from app
- [ ] Show changelog in dialog
- [ ] Compare current version with latest
- [ ] Notify user if update available
- [ ] Add "Copy System Info" button
- [ ] Include build date/commit hash
- [ ] Show GPU information
- [ ] Display memory information

## User Experience

### Before
- Basic about dialog
- No system information
- Manual update checking required

### After
- ✅ Complete system information displayed
- ✅ One-click update checking
- ✅ Direct link to releases page
- ✅ Better debugging support
- ✅ More professional appearance

## Summary

The About dialog now includes:
1. ✅ **System Information**
   - Operating System
   - CPU Architecture
   - Kernel Version
   - Qt Version

2. ✅ **Check for Updates Button**
   - Opens GitHub releases
   - Shows confirmation message
   - Easy version checking

3. ✅ **Enhanced Layout**
   - Larger dialog (550x550)
   - Better organized
   - Professional appearance

**Status**: ✅ **Complete and Working**

All features implemented, tested, and ready to use!
