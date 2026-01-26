# Game Tweaks & Settings Guide

## Quick Reference

This guide explains all the game tweaks and settings available in OpenGalaxy.

---

## Game Properties Dialog

Access game properties by right-clicking any game card and selecting **Properties**.

### Game Tweaks Section

#### 1. Hide game from library
**What it does:** Removes the game from your library view

**When to use:**
- Hide unfinished games
- Hide test/demo games
- Organize your library

**How to use:**
1. Open game Properties
2. Check "Hide game from library"
3. Click Save
4. Game disappears from library

**To show hidden games:**
- Go to Settings → Games
- Check "Show hidden games in library"

---

#### 2. Show FPS in game
**What it does:** Displays a real-time FPS counter in the top-left corner of the game

**Requirements:**
- Game must use DXVK or VKD3D (Windows games on Linux/Mac)
- Does not work with native Linux/Mac games

**When to use:**
- Monitor game performance
- Troubleshoot performance issues
- Verify frame rate consistency

**How to use:**
1. Open game Properties
2. Check "Show FPS in game"
3. Click Save
4. Launch game - FPS counter appears in top-left

**Note:** The FPS counter is provided by DXVK and shows:
- Current FPS
- Average FPS
- Frame time

---

#### 3. Use MangoHud
**What it does:** Displays a comprehensive performance overlay with FPS, CPU, GPU, temperature, and RAM usage

**Requirements:**
- MangoHud must be installed on your system
- Works with most games (Windows and Linux)

**Installation:**
```bash
# Ubuntu/Debian
sudo apt install mangohud

# Fedora
sudo dnf install mangohud

# Arch
sudo pacman -S mangohud
```

**When to use:**
- Monitor detailed system performance
- Track CPU/GPU usage
- Monitor temperatures
- Troubleshoot performance bottlenecks

**How to use:**
1. Install MangoHud (see above)
2. Open game Properties
3. Check "Use MangoHud"
4. Click Save
5. Launch game - overlay appears in top-left

**Customization:**
MangoHud can be configured via `~/.config/MangoHud/MangoHud.conf`

---

#### 4. Use GameMode
**What it does:** Optimizes system resources for better gaming performance

**Requirements:**
- GameMode must be installed on your system
- Works on Linux systems

**Installation:**
```bash
# Ubuntu/Debian
sudo apt install gamemode

# Fedora
sudo dnf install gamemode

# Arch
sudo pacman -S gamemode
```

**When to use:**
- Improve frame rates
- Reduce input latency
- Optimize CPU/GPU performance
- Improve frame consistency

**How to use:**
1. Install GameMode (see above)
2. Open game Properties
3. Check "Use GameMode"
4. Click Save
5. Launch game - GameMode automatically optimizes system

**What GameMode does:**
- Increases CPU/GPU priority
- Reduces CPU frequency scaling
- Disables power saving features
- Optimizes I/O scheduler
- Improves overall gaming performance

---

## Settings Page

Access Settings by clicking the Settings tab in the main window.

### Games Section

#### Show hidden games in library
**What it does:** Controls whether hidden games appear in your library

**Default:** Unchecked (hidden games are not shown)

**How to use:**
1. Go to Settings
2. In the Games section, check "Show hidden games in library"
3. Setting is saved automatically
4. Hidden games now appear in library

**Note:** Individual games can still be hidden via Properties dialog

---

## Environment Variables

Advanced users can set custom environment variables for each game:

1. Open game Properties
2. Click "Environment Variables" section
3. Add custom variables (e.g., `WINE_CPU_TOPOLOGY=4:2`)
4. Click Save

**Common variables:**
- `DXVK_HUD=fps` - Show FPS counter
- `DXVK_HUD=memory` - Show memory usage
- `WINE_CPU_TOPOLOGY=4:2` - Set CPU topology
- `STAGING_SHARED_MEMORY=1` - Enable shared memory

---

## Runner Selection

Choose which runner to use for each game:

1. Open game Properties
2. In "Runner" section, select preferred runner
3. Options: Auto, Native, Wine, Proton, Box64, FEX, etc.
4. Click Save

**Auto (Recommended):** OpenGalaxy automatically selects the best runner

---

## Troubleshooting

### Game won't launch with tweaks enabled
1. Disable all tweaks
2. Try launching game
3. If it works, enable tweaks one by one to find the culprit
4. Check that required tools are installed (MangoHud, GameMode)

### FPS counter not showing
- Ensure "Show FPS in game" is enabled
- Game must use DXVK/VKD3D (Windows games)
- Native Linux/Mac games don't support DXVK_HUD

### MangoHud overlay not showing
- Ensure MangoHud is installed: `which mangohud`
- Check MangoHud configuration: `~/.config/MangoHud/MangoHud.conf`
- Try running game from terminal to see errors

### GameMode not working
- Ensure GameMode is installed: `which gamemoderun`
- Check GameMode is running: `systemctl --user status gamemoded`
- Try starting GameMode: `systemctl --user start gamemoded`

---

## Performance Tips

### For Maximum Performance
1. Enable GameMode (if available)
2. Use MangoHud to monitor performance
3. Select appropriate runner (Proton for Windows games)
4. Disable unnecessary background processes
5. Monitor temperatures with MangoHud

### For Troubleshooting
1. Enable FPS counter to see frame rate
2. Enable MangoHud to see CPU/GPU usage
3. Check temperatures to avoid thermal throttling
4. Monitor RAM usage for memory leaks

### For Compatibility
1. Try different runners (Wine, Proton, etc.)
2. Set custom environment variables if needed
3. Check game-specific guides on ProtonDB
4. Report issues on GitHub

---

## Advanced Usage

### Custom Environment Variables
Set game-specific environment variables:

```
DXVK_HUD=fps,memory
WINE_CPU_TOPOLOGY=4:2
STAGING_SHARED_MEMORY=1
```

### Custom Runner Arguments
Pass additional arguments to the runner:

```
--no-esync
--no-fsync
-d3d11
```

### Custom Runner Executable
Use a custom runner binary:

```
/path/to/custom/wine
/path/to/proton-ge/proton
```

---

## FAQ

**Q: Will tweaks affect game performance?**
A: Tweaks generally improve performance. FPS counter and MangoHud have minimal overhead.

**Q: Can I use multiple tweaks at once?**
A: Yes! You can enable multiple tweaks simultaneously.

**Q: Do tweaks persist across updates?**
A: Yes, all settings are saved in the database and persist across game updates.

**Q: Can I hide games permanently?**
A: Yes, hidden games stay hidden until you uncheck "Hide game from library" in Properties.

**Q: What if a tweak breaks my game?**
A: Simply disable the tweak in Properties and try again.

---

## See Also

- [MISSING_FEATURES.md](MISSING_FEATURES.md) - Feature status
- [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md) - Technical details
- [WINE_PROTON_TOOLS.md](WINE_PROTON_TOOLS.md) - Wine/Proton tools guide
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - General troubleshooting

---

## Version

This guide applies to OpenGalaxy 1.0.0 and later.

Last updated: 2026-01-26
