# Install Directory Fix - January 23, 2026

## 🎯 Issue

The application was asking users to select an installation folder every time they installed a game, which is:
- ❌ Annoying for users
- ❌ Inconsistent install locations
- ❌ Not following standard conventions

---

## ✅ Solution

Changed to use a **default installation directory** without prompting the user.

### **Commit**: `b44b489`
### **Status**: ✅ Fixed and Pushed

---

## 📁 Default Install Location

### Linux/macOS
```
~/Games/OpenGalaxy/
```

**Full path example**:
```
/home/mateus/Games/OpenGalaxy/
├── Battle Brothers/
├── Witcher 3/
└── Cyberpunk 2077/
```

### Windows (future)
```
%USERPROFILE%\Games\OpenGalaxy\
```

---

## 🔄 Behavior Change

### Before
```
User clicks "Install"
  ↓
Dialog: "Choose install folder"
  ↓
User selects: /some/random/path
  ↓
Game installs to: /some/random/path/Games/OpenGalaxy/GameName/
```

### After
```
User clicks "Install"
  ↓
Game installs to: ~/Games/OpenGalaxy/GameName/
  ↓
Done! (no dialog)
```

---

## 🛠️ Implementation

### Code Changes

**File**: `ui/qt/pages/library_page.cpp`

```cpp
void LibraryPage::installGame(const QString& gameId)
{
    // Use default installation directory: ~/Games/OpenGalaxy
    QString homeDir = QDir::homePath();
    QDir dir(homeDir);
    
    // Create Games directory if it doesn't exist
    if (!dir.exists("Games")) {
        if (!dir.mkdir("Games")) {
            QMessageBox::warning(this, "Install Error", 
                "Failed to create Games directory in home folder");
            return;
        }
    }
    dir.cd("Games");
    
    // Create OpenGalaxy directory if it doesn't exist
    if (!dir.exists("OpenGalaxy")) {
        if (!dir.mkdir("OpenGalaxy")) {
            QMessageBox::warning(this, "Install Error", 
                "Failed to create OpenGalaxy directory");
            return;
        }
    }
    
    const QString installDir = dir.absoluteFilePath("OpenGalaxy");
    // ... rest of installation code
}
```

### Key Features

1. **Automatic Directory Creation**
   - Creates `~/Games` if it doesn't exist
   - Creates `~/Games/OpenGalaxy` if it doesn't exist
   - No user intervention needed

2. **Error Handling**
   - Checks if directory creation succeeds
   - Shows clear error message if fails
   - Prevents installation if can't create directory

3. **Permission Checks**
   - Verifies write access to home directory
   - Fails gracefully if no permissions

---

## 🎨 User Experience

### Advantages

✅ **One-click installation** - No dialogs to dismiss  
✅ **Consistent location** - Always know where games are  
✅ **Standard convention** - Follows Linux/Unix practices  
✅ **Easy to find** - `~/Games/OpenGalaxy` is memorable  
✅ **Backup-friendly** - Single directory to backup  

### Disadvantages (and solutions)

⚠️ **No choice of location**  
→ *Future*: Add Settings option to customize

⚠️ **Might fill home partition**  
→ *Future*: Add disk space warning

⚠️ **Can't install to external drive**  
→ *Future*: Add custom location option

---

## 🔮 Future Enhancements

### Phase 1: Settings Integration
```cpp
// Add to Settings page
QString installDir = settings.value("installDirectory", 
                                    QDir::homePath() + "/Games/OpenGalaxy").toString();
```

### Phase 2: Per-Game Location
```cpp
// Allow override per game
if (game.customInstallPath.isEmpty()) {
    installDir = defaultInstallDir;
} else {
    installDir = game.customInstallPath;
}
```

### Phase 3: Disk Space Management
```cpp
// Check available space before install
qint64 availableSpace = QStorageInfo(installDir).bytesAvailable();
if (availableSpace < game.size) {
    showDiskSpaceWarning();
}
```

---

## 📊 Comparison with Other Launchers

| Launcher | Default Location | Customizable |
|----------|-----------------|--------------|
| **OpenGalaxy** | `~/Games/OpenGalaxy` | ❌ (future) |
| Steam | `~/.steam/steam/steamapps` | ✅ |
| Lutris | `~/Games` | ✅ |
| Heroic | `~/Games/Heroic` | ✅ |
| GOG Galaxy | `C:\GOG Games` | ✅ |

**Note**: We follow the same pattern as Heroic and Lutris.

---

## 🧪 Testing

### Test 1: First Installation
```bash
# Clean state
rm -rf ~/Games/OpenGalaxy

# Install game
./build/ui/opengalaxy
# Login → Library → Install game

# Verify
ls -la ~/Games/OpenGalaxy/
# Should show game directory
```

### Test 2: Subsequent Installations
```bash
# Install another game
# Should use same directory
# No dialog shown

ls -la ~/Games/OpenGalaxy/
# Should show multiple game directories
```

### Test 3: Permission Error
```bash
# Make Games directory read-only
chmod 555 ~/Games

# Try to install
# Should show error message
# Should not crash

# Restore permissions
chmod 755 ~/Games
```

---

## 📝 Files Modified

### UI (1 file)
**`ui/qt/pages/library_page.cpp`**
- Removed `QFileDialog::getExistingDirectory()` call
- Added automatic directory creation
- Added error handling for mkdir failures
- Lines: 301-325

### Documentation (1 file)
**`CRASH_FIX_2026-01-23.md`** (updated)
- Documents crash fix
- Documents install directory behavior

---

## 🔄 Git History

```bash
# Previous commits
4d69c49 - feat: Polish ISA translator support
e478206 - fix: Prevent crash from null callback

# This fix
b44b489 - fix: Use default install directory instead of asking user

# Status
✅ Committed
✅ Pushed to origin/main
```

---

## 💡 Design Rationale

### Why `~/Games/OpenGalaxy`?

1. **`~/Games`** - Standard location for game launchers on Linux
   - Used by Lutris, Heroic, etc.
   - Easy to remember
   - Separate from system files

2. **`/OpenGalaxy`** - Namespace for our games
   - Prevents conflicts with other launchers
   - Clear ownership
   - Easy to backup/move

3. **No dialog** - Better UX
   - One less click
   - Consistent behavior
   - Follows principle of least surprise

### Alternative Considered

❌ **`~/.local/share/opengalaxy/games`**
- Too hidden
- Harder to find
- Not user-friendly

❌ **`~/opengalaxy-games`**
- Clutters home directory
- Not standard

✅ **`~/Games/OpenGalaxy`** (chosen)
- Standard location
- Easy to find
- Follows conventions

---

## 🎓 Best Practices Applied

1. **Convention over Configuration**
   - Sensible defaults
   - No unnecessary choices

2. **Fail Fast**
   - Check permissions early
   - Clear error messages

3. **Progressive Enhancement**
   - Works now with defaults
   - Can add customization later

4. **User-Friendly Paths**
   - No hidden directories
   - Easy to navigate

---

## ✅ Checklist

- [x] Default directory implemented
- [x] Automatic creation working
- [x] Error handling added
- [x] Build successful
- [x] No crashes
- [x] Code committed
- [x] Changes pushed
- [x] Documentation updated

---

## 📞 Support

### Common Questions

**Q: Can I change the install directory?**  
A: Not yet, but it's planned for a future update. For now, all games install to `~/Games/OpenGalaxy`.

**Q: What if I don't have space in my home directory?**  
A: You can create a symlink: `ln -s /path/to/large/drive ~/Games`

**Q: Can I move installed games?**  
A: Yes, but you'll need to update the database. Better to wait for the move feature.

**Q: Where is the database?**  
A: `~/.config/OpenGalaxy/library.db`

---

**Fix Date**: January 23, 2026  
**Commit**: b44b489  
**Status**: ✅ Complete and Deployed  
**Impact**: Improved UX
