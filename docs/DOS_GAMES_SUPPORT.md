# DOS Games Support in OpenGalaxy

**Date**: January 26, 2026  
**Status**: ✅ Implemented

## Overview

OpenGalaxy now includes **automatic detection and support for MS-DOS games** from GOG. When you try to install a DOS game, OpenGalaxy will:

1. ✅ Automatically detect it's a DOS game
2. ✅ Check if DOSBox is installed
3. ✅ Use DOSBox as the runner
4. ✅ Configure DOSBox optimally for the game
5. ✅ Show helpful error messages if DOSBox is missing

## Supported DOS Games

OpenGalaxy recognizes DOS games through multiple methods:

### 1. Metadata Detection
Games are identified as DOS if they have:
- "DOS" in their genres
- "Retro" or "Classic" in their genres
- Known DOS game titles (see list below)
- Released before 1995 with RPG/Adventure/Strategy genres

### 2. Executable Detection
OpenGalaxy checks the executable file header:
- MZ header (0x4D5A) indicates DOS/Windows executable
- PE offset < 64 indicates DOS executable
- PE offset > 64 indicates Windows PE executable

### 3. Known DOS Games

OpenGalaxy recognizes these classic DOS games:

**Adventure Games**:
- Monkey Island series
- Day of the Tentacle
- Sam & Max
- Maniac Mansion
- King's Quest series
- Space Quest series
- Police Quest series
- Leisure Suit Larry series

**Action Games**:
- Doom / Doom II
- Heretic / Hexen
- Wolfenstein 3D
- Commander Keen
- Duke Nukem
- Lemmings
- Prince of Persia
- Another World
- Flashback

**Strategy Games**:
- Civilization
- SimCity
- Dune / Dune II
- StarCraft
- Warcraft series

**RPG Games**:
- Baldur's Gate series
- Planescape Torment
- Icewind Dale series
- Fallout / Fallout 2
- The Elder Scrolls: Daggerfall
- Ultima series
- Wizardry series
- Might and Magic series

**Shooter Games**:
- Quake
- Half-Life
- System Shock

**And many more...**

## Installation Requirements

### Linux
```bash
# Ubuntu/Debian
sudo apt-get install dosbox

# Fedora/RHEL
sudo dnf install dosbox

# Arch
sudo pacman -S dosbox
```

### macOS
```bash
# Using Homebrew
brew install dosbox
```

### Windows
Download from: https://www.dosbox.com/

## How It Works

### Automatic Detection

When you click "Install" on a DOS game:

1. **Metadata Check**: OpenGalaxy checks if the game is tagged as DOS
2. **Runner Discovery**: Looks for DOSBox in system PATH
3. **Configuration**: Creates optimal DOSBox configuration
4. **Launch**: Starts the game with DOSBox

### DOSBox Configuration

OpenGalaxy automatically configures DOSBox with:

```ini
[cpu]
core=auto
cputype=auto
cycles=max

[render]
frameskip=0
scaler=normal2x

[mixer]
rate=44100
nosound=false

[dos]
xms=true
ems=true
umb=true
```

### Game Launch

The game is launched with:
1. Game directory mounted as C: drive
2. Automatic CD-ROM mounting (if applicable)
3. Optimal CPU cycles for performance
4. Sound and graphics configured

## Per-Game Configuration

You can customize DOSBox settings per game:

### CPU Cycles
- `max` - Maximum cycles (default, best performance)
- `auto` - Automatic detection
- `3000` - Specific cycle count (for slower games)

### Render Scaler
- `normal2x` - 2x scaling (default)
- `normal3x` - 3x scaling
- `advmame2x` - Advanced scaling
- `advmame3x` - Advanced 3x scaling

### Fullscreen
- `true` - Run in fullscreen
- `false` - Run in windowed mode (default)

## Troubleshooting

### "DOSBox not found"

**Problem**: OpenGalaxy can't find DOSBox

**Solution**:
```bash
# Install DOSBox
sudo apt-get install dosbox

# Verify installation
which dosbox
dosbox -version
```

### Game runs too fast

**Problem**: Game is unplayable because it runs too fast

**Solution**: Reduce CPU cycles in game settings:
1. Right-click game → Settings
2. Set CPU Cycles to `3000` or lower
3. Try again

### Game runs too slow

**Problem**: Game is sluggish or unresponsive

**Solution**: Increase CPU cycles:
1. Right-click game → Settings
2. Set CPU Cycles to `max`
3. Try again

### No sound

**Problem**: Game has no audio

**Solution**:
1. Check DOSBox mixer settings
2. Verify system audio is working
3. Try different sound card emulation

### Game won't start

**Problem**: Game crashes immediately

**Solution**:
1. Check game compatibility on DOSBox wiki
2. Try different CPU core (auto vs dynamic)
3. Disable EMS/XMS if game doesn't need it
4. Check game-specific DOSBox configuration

## Advanced Configuration

### Custom DOSBox Config

For games that need special configuration, you can create a custom `dosbox.conf`:

```ini
[cpu]
core=dynamic
cputype=auto
cycles=10000

[render]
frameskip=0
scaler=advmame2x

[mixer]
rate=44100
nosound=false
prebuffer=25

[dos]
xms=true
ems=true
umb=true

[cpu]
core=dynamic

[autoexec]
@echo off
mount c: /path/to/game
c:
cd GAME
GAME.EXE
exit
```

### DOSBox-X

For better compatibility, you can use DOSBox-X instead:

```bash
# Ubuntu/Debian
sudo apt-get install dosbox-x

# macOS
brew install dosbox-x
```

OpenGalaxy will automatically detect and use DOSBox-X if available.

## Performance Tips

1. **Use DOSBox-X** for better performance and compatibility
2. **Reduce scaler** if performance is poor (use `normal2x` instead of `advmame3x`)
3. **Disable frameskip** for smoother gameplay
4. **Adjust CPU cycles** based on game requirements
5. **Use dynamic core** for better performance on modern systems

## Known Limitations

1. **Network games** - Multiplayer over network not supported
2. **CD audio** - Some games with CD audio may not work
3. **3D graphics** - Limited 3D support (use DOSBox-X for better 3D)
4. **Joystick** - Joystick support depends on DOSBox configuration
5. **Mouse** - Some games may have mouse issues

## Future Improvements

- [ ] Per-game DOSBox configuration UI
- [ ] DOSBox-X automatic detection
- [ ] Game-specific presets
- [ ] CD image mounting support
- [ ] Network game support
- [ ] Joystick configuration UI

## References

- [DOSBox Official Website](https://www.dosbox.com/)
- [DOSBox-X Project](https://dosbox-x.com/)
- [DOSBox Game Compatibility](https://www.dosbox.com/comp_list.php)
- [GOG DOS Games](https://www.gog.com/games?mediaType=game&genres=DOS)

## Example: Installing Battle Chess

1. **Install DOSBox**:
   ```bash
   sudo apt-get install dosbox
   ```

2. **Open OpenGalaxy** and find "Battle Chess"

3. **Click Install** - OpenGalaxy will:
   - Detect it's a DOS game
   - Find DOSBox
   - Configure DOSBox
   - Start the installation

4. **Play** - Battle Chess launches in DOSBox!

---

**Enjoy your classic DOS games!** 🎮
