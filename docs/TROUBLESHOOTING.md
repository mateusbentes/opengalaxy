# OpenGalaxy Troubleshooting Guide

Common issues and solutions.

## 🔐 Login Issues

### Can't Login with Password

**Symptoms**: "Invalid credentials" or "Login failed" error

**Solutions**:
1. **Verify credentials** at https://www.gog.com
2. **Try OAuth login** instead (more reliable)
3. **Check internet connection**
4. **Clear session data**:
   ```bash
   rm ~/.local/share/opengalaxy/session.json
   ```
5. **Check logs** for specific error:
   ```bash
   tail -50 ~/.local/share/opengalaxy/opengalaxy.log | grep ERROR
   ```

### OAuth Login Window Won't Open

**Symptoms**: Clicking "Login with OAuth" does nothing

**Solutions**:
1. **Check Qt WebEngine** is installed:
   ```bash
   # Ubuntu/Debian
   sudo apt install qt6-webengine-dev
   
   # Fedora
   sudo dnf install qt6-qtwebengine-devel
   
   # Arch
   sudo pacman -S qt6-webengine
   ```
2. **Rebuild application** after installing WebEngine
3. **Use password login** as alternative

### Session Expired

**Symptoms**: "Session expired, please login again"

**Solutions**:
- **Normal behavior** - tokens expire after ~1 hour
- **Just login again** - refresh tokens should work automatically
- If refresh fails, **clear session** and login fresh:
  ```bash
  rm ~/.local/share/opengalaxy/session.json
  ```

---

## 📚 Library Issues

### Library Won't Load / Empty Library

**Symptoms**: Library page shows no games or loading spinner forever

**Solutions**:
1. **Check internet connection**
2. **Verify you own games** on GOG.com
3. **Refresh library** manually (click Refresh button)
4. **Clear library cache**:
   ```bash
   rm ~/.local/share/opengalaxy/library.db
   # Restart app and refresh library
   ```
5. **Check logs**:
   ```bash
   grep "library" ~/.local/share/opengalaxy/opengalaxy.log
   ```

### Games Missing from Library

**Symptoms**: Some games don't appear in OpenGalaxy

**Solutions**:
1. **Refresh library** (may take a minute for large libraries)
2. **Check game ownership** on GOG.com
3. **Clear cache and re-sync**:
   ```bash
   rm ~/.local/share/opengalaxy/library.db
   ```
4. **Check if game is hidden** (filter settings)

---

## 💾 Installation Issues

### "Protocol unknown" Error

**Symptoms**: Installation fails with "Protocol '' is unknown"

**Solutions**:
- **Fixed in v1.0.0** - update to latest version
- If still occurs, **report bug** with game ID

### Download Fails / Stalls

**Symptoms**: Download progress stops or fails

**Solutions**:
1. **Check internet connection**
2. **Check disk space**:
   ```bash
   df -h ~/Games
   ```
3. **Try different game** to isolate issue
4. **Check GOG CDN status** (may be temporary)
5. **Restart download** (cancel and try again)

### Installation Directory Not Writable

**Symptoms**: "Permission denied" or "Cannot write to directory"

**Solutions**:
```bash
# Check permissions
ls -ld ~/Games

# Fix permissions
chmod 755 ~/Games

# Or choose different directory in Settings
```

### Installer Hangs (Wine)

**Symptoms**: Windows game installer window appears but doesn't progress

**Solutions**:
1. **Wait** - some installers are slow
2. **Check Wine version**:
   ```bash
   wine --version
   ```
3. **Update Wine**:
   ```bash
   sudo apt install wine-stable
   ```
4. **Try Proton** instead (Settings → Runners)
5. **Manual installation**:
   - Download installer from GOG.com
   - Run manually with Wine

---

## 🎮 Game Launch Issues

### Game Won't Launch

**Symptoms**: Clicking "Play" does nothing or shows error

**Solutions**:
1. **Check game is installed** (not just in library)
2. **Verify runner availability**:
   ```bash
   which wine proton box64
   ```
3. **Check game executable exists**:
   ```bash
   ls ~/Games/GameName/*.exe
   ```
4. **Try different runner** (Game Properties → Runner)
5. **Check logs**:
   ```bash
   tail -100 ~/.local/share/opengalaxy/opengalaxy.log
   ```

### Game Crashes on Launch

**Symptoms**: Game starts but immediately crashes

**Solutions**:
1. **Check system requirements**
2. **Update graphics drivers**
3. **Try different runner**:
   - Wine → Proton
   - Box64 → FEX-Emu
4. **Check Wine/Proton logs**:
   ```bash
   WINEDEBUG=+all wine game.exe 2>&1 | tee wine.log
   ```
5. **Install game dependencies**:
   ```bash
   winetricks vcrun2019 d3dx9 dotnet48
   ```

### Poor Performance / Low FPS

**Symptoms**: Game runs but very slowly

**Solutions**:
1. **Check CPU/GPU usage**:
   ```bash
   htop
   ```
2. **Try different runner** (FEX-Emu usually faster than Box64)
3. **Lower game graphics settings**
4. **Close other applications**
5. **Check if ISA translation is needed** (ARM64 systems)
6. **Use native version** if available

---

## 🖥️ UI Issues

### Window Won't Open / Crashes on Startup

**Symptoms**: Application crashes immediately or window doesn't appear

**Solutions**:
1. **Run from terminal** to see errors:
   ```bash
   ./build/ui/opengalaxy
   ```
2. **Check Qt installation**:
   ```bash
   qmake --version
   ```
3. **Check dependencies**:
   ```bash
   ldd ./build/ui/opengalaxy
   ```
4. **Clear settings**:
   ```bash
   rm ~/.local/share/opengalaxy/settings.conf
   ```
5. **Reinstall application**

### UI Elements Not Displaying Correctly

**Symptoms**: Buttons missing, text cut off, layout broken

**Solutions**:
1. **Resize window** (may be too small)
2. **Check Qt theme**:
   ```bash
   QT_QPA_PLATFORMTHEME=qt5ct opengalaxy
   ```
3. **Reset window settings**:
   ```bash
   rm ~/.local/share/opengalaxy/settings.conf
   ```
4. **Update Qt**:
   ```bash
   sudo apt update && sudo apt upgrade
   ```

### Language Not Changing

**Symptoms**: UI still in English after changing language

**Solutions**:
1. **Restart application** (required for language change)
2. **Check translation files exist**:
   ```bash
   ls ui/qt/resources/i18n/*.qm
   ```
3. **Rebuild application** if translations missing
4. See [MULTILINGUAL_GUIDE.md](MULTILINGUAL_GUIDE.md)

---

## 🌐 Network Issues

### "Connection refused" / "Network error"

**Symptoms**: Can't connect to GOG servers

**Solutions**:
1. **Check internet connection**:
   ```bash
   ping gog.com
   ```
2. **Check firewall**:
   ```bash
   sudo ufw status
   ```
3. **Check proxy settings** (if using proxy)
4. **Try different network** (mobile hotspot, etc.)
5. **Check GOG status**: https://status.gog.com

### SSL/TLS Errors

**Symptoms**: "SSL handshake failed" or certificate errors

**Solutions**:
1. **Update OpenSSL**:
   ```bash
   sudo apt install openssl libssl-dev
   ```
2. **Update CA certificates**:
   ```bash
   sudo update-ca-certificates
   ```
3. **Check system time** (must be accurate for SSL)
4. **Disable antivirus** temporarily (may intercept SSL)

---

## 🔧 Build Issues

### CMake Configuration Fails

**Symptoms**: `cmake ..` fails with errors

**Solutions**:
1. **Check CMake version**:
   ```bash
   cmake --version  # Must be 3.21+
   ```
2. **Install missing dependencies**:
   ```bash
   sudo apt install build-essential cmake qt6-base-dev qt6-webengine-dev libssl-dev
   ```
3. **Clear CMake cache**:
   ```bash
   rm -rf build
   mkdir build && cd build
   cmake ..
   ```
4. **Specify Qt path**:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64
   ```

### Compilation Errors

**Symptoms**: `make` or `ninja` fails with errors

**Solutions**:
1. **Check compiler version**:
   ```bash
   g++ --version  # Must support C++20
   ```
2. **Update compiler**:
   ```bash
   sudo apt install g++-10
   ```
3. **Clean and rebuild**:
   ```bash
   rm -rf build
   ./build.sh
   ```
4. **Check error message** and search GitHub issues

### Linker Errors

**Symptoms**: "undefined reference" errors during linking

**Solutions**:
1. **Install missing libraries**:
   ```bash
   sudo apt install libqt6sql6-sqlite libssl-dev
   ```
2. **Check library paths**:
   ```bash
   ldconfig -p | grep Qt6
   ```
3. **Rebuild from scratch**:
   ```bash
   rm -rf build
   ./build.sh
   ```

---

## 📊 Performance Issues

### High CPU Usage

**Symptoms**: OpenGalaxy uses excessive CPU

**Solutions**:
1. **Check for background tasks** (downloads, library sync)
2. **Close unused tabs/pages**
3. **Disable animations** (if option available)
4. **Update to latest version** (may have optimizations)
5. **Report bug** if persistent

### High Memory Usage

**Symptoms**: OpenGalaxy uses excessive RAM

**Solutions**:
1. **Normal for Qt WebEngine** (OAuth login)
2. **Close OAuth window** after login
3. **Restart application** periodically
4. **Check for memory leaks** (report if found)

### Slow Startup

**Symptoms**: Application takes long to start

**Solutions**:
1. **Normal on first run** (library sync)
2. **Check disk I/O**:
   ```bash
   iotop
   ```
3. **Move data to SSD** if on HDD
4. **Disable startup library sync** (if option available)

---

## 🐛 Reporting Bugs

If none of these solutions work:

1. **Collect information**:
   ```bash
   # System info
   uname -a
   
   # OpenGalaxy version
   ./build/ui/opengalaxy --version
   
   # Recent logs
   tail -200 ~/.local/share/opengalaxy/opengalaxy.log > debug.log
   ```

2. **Create GitHub issue**: https://github.com/mateusbentes/opengalaxy/issues

3. **Include**:
   - Operating system and version
   - OpenGalaxy version
   - Steps to reproduce
   - Error messages
   - Relevant logs (debug.log)
   - Screenshots (if UI issue)

---

## 📚 Additional Resources

- [README.md](README.md) - Feature overview
- [BUILD.md](BUILD.md) - Build instructions
- [COMPATIBILITY_GUIDE.md](COMPATIBILITY_GUIDE.md) - Runner setup
- [DATA_LOCATIONS.md](DATA_LOCATIONS.md) - Data storage
- [GitHub Issues](https://github.com/mateusbentes/opengalaxy/issues) - Known issues

---

## 💬 Getting Help

- **GitHub Discussions**: https://github.com/mateusbentes/opengalaxy/discussions
- **Email**: mateusbentes@tuta.io
- **IRC**: #opengalaxy on Libera.Chat (coming soon)

---

**Last Updated**: 2026-01-23
