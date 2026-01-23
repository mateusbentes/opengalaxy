# OpenGalaxy Quick Reference

## 🚀 Quick Start

```bash
# Build
cd /home/mateus/opengalaxy
./build.sh

# Run GUI
./build/ui/opengalaxy

# Run CLI
./build/cli/opengalaxy-cli --help
```

---

## ✅ What Works

| Feature | Status | Command/Location |
|---------|--------|------------------|
| **Game Installation** | ✅ Working | Library → Install button |
| **ISA Translators** | ✅ Working | Settings → Runners |
| **Wine/Proton** | ✅ Working | Auto-detected |
| **Library Management** | ✅ Working | Library tab |
| **Store Browse** | ✅ Working | Store tab |
| **Game Launching** | ✅ Working | Library → Play button |

---

## ⚠️ What Doesn't Work

| Feature | Status | Alternative |
|---------|--------|-------------|
| **Cloud Saves** | ❌ Not Available | Use official GOG Galaxy client |
| **Achievements** | ⚠️ Partial | API works, UI incomplete |
| **Friends** | ⚠️ Partial | API works, UI incomplete |

---

## 🔧 ISA Translators (ARM64 Support)

### Install on ARM64 Linux:
```bash
# Box64 (Recommended)
sudo apt install box64

# FEX-Emu (Faster)
sudo add-apt-repository ppa:fex-emu/fex
sudo apt install fex-emu

# QEMU (Fallback)
sudo apt install qemu-user-static
```

### Install on ARM64 macOS:
```bash
# Rosetta 2 (Usually pre-installed)
softwareupdate --install-rosetta
```

### Check Available:
```bash
which box64 fex-emu qemu-x86_64
```

---

## 🐛 Troubleshooting

### Installation Fails
```bash
# Check logs
./build/ui/opengalaxy 2>&1 | tee opengalaxy.log

# Verify Wine
which wine

# Test download manually
curl -I https://api.gog.com/products/GAME_ID
```

### Game Won't Launch
```bash
# Check runner
ls -l /usr/bin/wine /usr/bin/box64

# Verify game file
file /path/to/game.exe

# Check permissions
chmod +x /path/to/game
```

### Poor Performance (ARM64)
1. Try FEX instead of Box64
2. Lower game graphics settings
3. Check CPU usage with `htop`

---

## 📚 Documentation

| File | Purpose |
|------|---------|
| `FEATURES_STATUS.md` | Complete feature overview |
| `COMPATIBILITY_GUIDE.md` | ISA translator setup guide |
| `FIXES_2026-01-23.md` | Recent bug fixes |
| `SESSION_SUMMARY_2026-01-23.md` | What was accomplished |
| `QUICK_REFERENCE.md` | This file |

---

## 🎮 Common Tasks

### Install a Game
```bash
# GUI
./build/ui/opengalaxy
# Library → Find game → Install

# CLI
./build/cli/opengalaxy-cli install --game GAME_ID --dir ~/Games
```

### Change Runner
```bash
# GUI
./build/ui/opengalaxy
# Library → Right-click game → Properties → Runner
```

### Check Cloud Saves
```
Not available - use official GOG Galaxy client
```

### Update Game
```bash
# Check for updates
./build/cli/opengalaxy-cli update --game GAME_ID
```

---

## 🔍 System Info

```bash
# Architecture
uname -m

# OS
lsb_release -a

# Available runners
which wine proton box64 fex-emu qemu-x86_64

# OpenGalaxy version
./build/ui/opengalaxy --version
```

---

## 📊 Performance Expectations

| Configuration | Performance | Use Case |
|---------------|-------------|----------|
| x86_64 Native | 100% | Standard |
| ARM64 + FEX | 80-95% | Modern ARM CPUs |
| ARM64 + Box64 | 60-80% | Older ARM CPUs |
| ARM64 + QEMU | 30-50% | Compatibility only |
| macOS Rosetta2 | 90-97% | Apple Silicon |

---

## 🆘 Getting Help

1. **Read docs**: Start with `FEATURES_STATUS.md`
2. **Check logs**: Run from terminal to see output
3. **Verify system**: Check architecture and runners
4. **Report issues**: Include logs and system info

---

## 🎯 Key Points

✅ **Game installation works** (HTTP error fixed)  
✅ **ISA translators supported** (Box64, FEX, QEMU, Rosetta2)  
❌ **Cloud saves not available** (use official Galaxy client)  
✅ **Wine/Proton auto-detected**  
✅ **Full library management**  

---

## 📝 License

Apache-2.0
