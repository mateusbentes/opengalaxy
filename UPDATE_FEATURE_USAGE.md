# Game Update Feature - User Guide

## How to Update Games in OpenGalaxy

### Automatic Update Detection

When you open your library, OpenGalaxy automatically checks for updates for all installed games. This happens in the background without any user action required.

### Visual Indicators

**Update Available:**
- When an update is available, hover over the game card
- You'll see an orange "⬆ UPDATE" button appear below the "▶ PLAY" button
- Hover over the update button to see the new version number in a tooltip

**No Update Available:**
- Only the "▶ PLAY" button appears (purple)
- No update button means you have the latest version

### Updating a Game

1. **Start Update:**
   - Hover over the game card with an available update
   - Click the orange "⬆ UPDATE" button
   - A toast notification will appear: "Updating game..."

2. **During Update:**
   - The update button changes to a "CANCEL" button
   - A progress bar appears showing download/install progress
   - The progress bar updates in real-time (0% to 100%)
   - You can still browse your library while updating

3. **Cancel Update (Optional):**
   - Click the "CANCEL" button to stop the update
   - Partial downloads will be cleaned up
   - The game remains in its previous state

4. **Update Complete:**
   - A toast notification appears: "Update completed"
   - The update button disappears
   - The game is now at the latest version
   - You can immediately play the updated game

### Multiple Game Updates

- You can update multiple games simultaneously
- Each game shows its own progress bar
- Updates are independent and don't affect each other
- No limit on concurrent updates (system resources permitting)

### Update Behavior

**Installation Directory:**
- Updates install to the same directory as the original game
- No need to choose a new location
- Existing game files are replaced with new versions

**Game Data:**
- Save files are typically preserved (depends on game)
- Settings and configurations usually remain intact
- Cloud saves are not affected

**Version Tracking:**
- OpenGalaxy tracks the installed version
- Compares with the latest version from GOG
- Only shows update button when versions differ

### Troubleshooting

**Update Button Doesn't Appear:**
- Make sure the game is installed
- Refresh your library (close and reopen OpenGalaxy)
- Check your internet connection
- Verify you're logged into GOG

**Update Fails:**
- Check available disk space
- Verify internet connection is stable
- Try cancelling and restarting the update
- Check the error message in the toast notification

**Update Stuck:**
- Click "CANCEL" to stop the update
- Close and reopen OpenGalaxy
- Try updating again
- Check system resources (CPU, RAM, disk)

**Version Still Shows as Old:**
- Refresh the library page
- Close and reopen the game details
- The version updates after successful installation

### Tips

1. **Check for Updates Regularly:**
   - Open your library to trigger automatic update checks
   - Updates are checked each time you load the library

2. **Update Before Playing:**
   - Updates often include bug fixes and improvements
   - New features may be added in updates
   - Performance improvements are common

3. **Stable Internet Required:**
   - Updates can be large (several GB)
   - Ensure stable connection before starting
   - Use wired connection for best results

4. **Disk Space:**
   - Ensure sufficient free space (2x game size recommended)
   - Updates may temporarily use extra space
   - Old files are replaced after download completes

### Keyboard Shortcuts

Currently, there are no keyboard shortcuts for updates. Use mouse clicks to interact with the update button.

### Future Features (Planned)

- **Update All:** One-click to update all games
- **Auto-Update:** Automatically download updates in background
- **Update Notifications:** Desktop alerts for new updates
- **Changelog:** View what's new in each update
- **Scheduled Updates:** Set times for automatic updates
- **Bandwidth Limiting:** Control download speed
- **Update History:** See past updates and versions

### Technical Details

**Version Comparison:**
- Compares installed version with latest GOG version
- Uses version strings from game metadata
- Simple string comparison (exact match required)

**Download Process:**
- Uses same system as initial installation
- Downloads from GOG CDN servers
- Supports resume on connection interruption
- Verifies file integrity after download

**Update Size:**
- Full game download (not delta/patch)
- Same size as fresh installation
- Future versions may support smaller delta updates

### Privacy & Security

- No personal data collected during updates
- Uses your existing GOG authentication
- Downloads are verified for integrity
- No third-party servers involved

### Support

If you encounter issues with updates:
1. Check the console output for error messages
2. Report bugs on the OpenGalaxy GitHub repository
3. Include game name, version numbers, and error messages
4. Provide system information (OS, disk space, etc.)

---

**Note:** This feature requires an active internet connection and valid GOG account authentication.
