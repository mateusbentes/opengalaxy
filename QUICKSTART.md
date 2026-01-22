# OpenGalaxy Quick Start

## 🚀 Get Started in 5 Minutes

### Step 1: Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt install build-essential cmake ninja-build qt6-base-dev qt6-webengine-dev libssl-dev
```

**Fedora:**
```bash
sudo dnf install gcc-c++ cmake ninja-build qt6-qtbase-devel qt6-qtwebengine-devel openssl-devel
```

**Arch:**
```bash
sudo pacman -S base-devel cmake ninja qt6-base qt6-webengine openssl
```

### Step 2: Build

```bash
cd /home/mateus/opengalaxy
./build.sh
```

Or manually:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -GNinja
ninja -j$(nproc)
```

### Step 3: Run

**GUI:**
```bash
./build/ui/opengalaxy
```

**CLI:**
```bash
./build/cli/opengalaxy-cli --help
```

### Step 4: Test

```bash
cd build
ctest --output-on-failure
```

## 📖 Usage Examples

### Login
```bash
# GUI: Click "Login" and enter credentials
# CLI:
./build/cli/opengalaxy-cli login -u your@email.com -p yourpassword
```

### List Games
```bash
./build/cli/opengalaxy-cli list
```

### Install Game
```bash
./build/cli/opengalaxy-cli install --game GAME_ID --dir ~/Games
```

### Launch Game
```bash
./build/cli/opengalaxy-cli launch --game GAME_ID
```

## 🔧 Troubleshooting

### Qt6 not found
```bash
# Specify Qt path
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64
```

### Build fails
```bash
# Clean and rebuild
rm -rf build
./build.sh
```

### Missing dependencies
See [BUILD.md](BUILD.md) for detailed instructions.

## 📚 Documentation

- [README.md](README.md) - Overview and features
- [BUILD.md](BUILD.md) - Detailed build instructions
- [CONTRIBUTING.md](CONTRIBUTING.md) - How to contribute

## 💬 Need Help?

- Issues: https://github.com/mateusbentes/opengalaxy/issues
- Discussions: https://github.com/mateusbentes/opengalaxy/discussions
- Email: mateusbentes@tuta.io
