# Issues to Fix

## 1. Library showing stub games instead of real games ✅ INVESTIGATING
- Disco Elysium, Hades, Celeste are showing
- These might be from cached database
- Need to check if session is actually authenticated when refreshLibrary() is called
- Add logging to see what's happening

## 2. Store not loading ⏳ TODO
- Need to investigate why store page isn't working
- Check if session is authenticated
- Check if GOG API calls are working

## 3. Library window too small ⏳ TODO
- Current window is cramped
- Need to increase minimum window size
- Adjust layout spacing

## 4. Settings buttons don't work ⏳ TODO
- "Installation Folders" button does nothing
- "Default Launcher Options" button does nothing
- Need to implement these dialogs

## Quick Fixes Needed

### Fix 1: Clear cached library on logout
The stub games might be from a cached database. We should clear it.

### Fix 2: Add debug logging
Add console output to see:
- When refreshLibrary() is called
- If session is authenticated
- What games are returned from API

### Fix 3: Increase window size
Change minimum size from 1280x720 to 1400x900

### Fix 4: Implement Settings dialogs
- Installation folder picker
- Runner selection dialog

## Testing Steps

1. Delete cached database: `rm ~/.local/share/opengalaxy/library.db`
2. Restart app and login
3. Check if real games load
4. Check console output for errors
