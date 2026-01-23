# Game Update Feature Implementation

## Overview
Implemented automatic game update detection and one-click update functionality for OpenGalaxy.

## Features Added

### 1. Update Detection
- Automatically checks for updates when library is loaded
- Compares installed game version with latest available version from GOG
- Only shows update button when an update is actually available

### 2. Update Button
- Orange gradient button (⬆ UPDATE) appears on game cards when update is available
- Shows tooltip with new version number
- Positioned below the Play button on hover
- Only visible for installed games with available updates

### 3. Update Process
- Click update button to start downloading the latest version
- Shows progress bar during download and installation
- Updates in-place to the existing installation directory
- Automatically refreshes version info after completion
- Can be cancelled like regular installations

### 4. Visual Feedback
- Update button uses distinctive orange color (#ff9800 to #f57c00)
- Progress bar shows during update process
- Toast notifications for update start, completion, and errors
- Button positions adjust when both Play and Update buttons are visible

## Modified Files

### UI Components
1. **ui/qt/widgets/game_card.h**
   - Added `updateButton_` widget
   - Added `updateAvailable_` and `updating_` state flags
   - Added `newVersion_` to store available version
   - Added signals: `updateRequested()`
   - Added methods: `setUpdateAvailable()`, `setUpdating()`

2. **ui/qt/widgets/game_card.cpp**
   - Created and styled update button with orange gradient
   - Positioned update button below action button
   - Connected update button to emit `updateRequested` signal
   - Updated `refreshButton()` to show/hide update button based on state
   - Updated hover events to show update button when available
   - Modified button positioning to accommodate both buttons

3. **ui/qt/pages/library_page.h**
   - Added `updateGame()` method
   - Added `checkForUpdate()` method

4. **ui/qt/pages/library_page.cpp**
   - Connected `updateRequested` signal from game cards
   - Implemented `checkForUpdate()` to compare versions
   - Implemented `updateGame()` to handle update process
   - Integrated with existing `InstallService` for downloads
   - Added automatic update checking after library refresh
   - Updated cancel logic to handle update cancellation

## How It Works

### Update Detection Flow
1. When library loads, for each installed game:
   - Fetch current game info from local database
   - Fetch latest download info from GOG API
   - Compare installed version with latest available version
   - If versions differ, mark update as available

2. Game card displays update button when:
   - Game is installed
   - Update is available
   - Not currently installing or updating
   - User hovers over the card

### Update Process Flow
1. User clicks "UPDATE" button
2. Card enters "updating" state (shows progress bar)
3. Fetch latest game downloads from GOG
4. Use `InstallService` to download and install to existing directory
5. Progress bar updates during download
6. On completion:
   - Mark update as complete
   - Hide update button
   - Show success notification
   - Refresh version info

### Version Comparison
- Uses simple string comparison for version numbers
- Checks all available downloads for the latest version
- Only shows update if version strings differ
- Future enhancement: semantic version comparison

## User Experience

### Before Update
```
[Game Card]
  Cover Image
  [▶ PLAY]        <- Main action button
  [⬆ UPDATE]      <- Orange update button (when available)
  Game Title
  Platform
```

### During Update
```
[Game Card]
  Cover Image
  [CANCEL]        <- Can cancel update
  [████████░░] 80% <- Progress bar
  Game Title
  Platform
```

### After Update
```
[Game Card]
  Cover Image
  [▶ PLAY]        <- Update button hidden
  Game Title
  Platform
```

## Technical Details

### Button Styling
- **Update Button**: Orange gradient (#ff9800 to #f57c00)
- **Play Button**: Purple gradient (#7c4dff to #5a3aff)
- **Size**: 160x50 pixels
- **Position**: Centered, stacked vertically

### State Management
- `updateAvailable_`: Boolean flag for update availability
- `updating_`: Boolean flag for update in progress
- `newVersion_`: String storing the available version number
- States are mutually exclusive with installing state

### API Integration
- Uses `GOGClient::fetchGameDownloads()` to get latest versions
- Uses `LibraryService::getGame()` to get current version
- Uses `InstallService::installGame()` for download and installation
- Reuses existing progress and completion callbacks

## Future Enhancements

1. **Semantic Versioning**
   - Implement proper version comparison (1.2.3 vs 1.2.10)
   - Support different version formats

2. **Update Notifications**
   - Show badge count of available updates
   - Desktop notifications for new updates
   - Auto-check for updates on startup

3. **Update History**
   - Track update history and changelog
   - Show what's new in the update
   - Rollback to previous versions

4. **Batch Updates**
   - "Update All" button for multiple games
   - Queue multiple updates
   - Priority-based update scheduling

5. **Smart Updates**
   - Only download changed files (delta updates)
   - Verify file integrity before updating
   - Backup before update with rollback option

## Testing Checklist

- [x] Update button appears when update is available
- [x] Update button hidden when no update available
- [x] Update button hidden for non-installed games
- [x] Progress bar shows during update
- [x] Update can be cancelled
- [x] Update button disappears after successful update
- [x] Tooltip shows new version number
- [x] Multiple games can be updated independently
- [x] Error handling for failed updates
- [x] Toast notifications work correctly

## Notes

- Update uses the same installation directory as the original game
- Update process is essentially a reinstall with the latest version
- Version information comes from GOG API's download metadata
- Update button only appears on hover to keep UI clean
- Orange color distinguishes update from install/play actions
