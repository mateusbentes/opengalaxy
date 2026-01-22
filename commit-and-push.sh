#!/bin/bash
# SPDX-License-Identifier: Apache-2.0
# Script to commit and push the OpenGalaxy rewrite

set -e

echo "🚀 OpenGalaxy - Preparing First Commit"
echo "======================================"
echo ""

# Check if we're in a git repository
if [ ! -d .git ]; then
    echo "❌ Error: Not a git repository"
    exit 1
fi

# Show current status
echo "📊 Current Git Status:"
git status --short
echo ""

# Ask for confirmation
read -p "Do you want to stage all files? (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Aborted."
    exit 1
fi

# Stage all files
echo "📦 Staging files..."
git add .

# Show what will be committed
echo ""
echo "📝 Files to be committed:"
git status --short
echo ""

# Ask for final confirmation
read -p "Proceed with commit? (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Aborted. You can manually commit with:"
    echo "  git commit -m 'Your message'"
    exit 1
fi

# Commit with the prepared message
echo "💾 Committing..."
git commit -F - << 'EOF'
feat: Complete rewrite - Production-ready GOG Galaxy 2.0 alternative

Complete architectural rewrite of OpenGalaxy with modern C++20 and Qt6.

BREAKING CHANGE: Complete rewrite from scratch.

### New Architecture
- Clean separation: core library, UI, CLI, tests
- Proper header/implementation structure
- Modern C++20 with Qt6 framework
- Cross-platform support (Linux, Windows, macOS)

### Core Features
- Session management with OAuth 2.0 PKCE + password auth
- Async HTTP client with timeout/retry
- GOG API client (library, store, achievements, cloud saves)
- Runner system (Wine, Proton, DOSBox, ISA translators)
- ISA translation (Box64, FEX-Emu, QEMU)
- Library service with SQLite caching
- Install service with resumable downloads
- Comprehensive logging
- Result<T> error handling

### UI Features
- Modern Qt6 interface with dark/light themes
- Login, Library, Store, Friends, Settings pages
- Game cards with cover art
- Notification system with system tray
- Localization support (EN, PT, ES, FR, DE)

### CLI Tool
- Commands: login, list, install, launch, runners
- Progress tracking
- Scriptable operations

### Documentation
- Comprehensive README
- BUILD guide for all platforms
- CONTRIBUTING guide
- Quick start guide
- CHANGELOG

### Build System
- Modern CMake 3.21+
- Qt6 integration with AUTOMOC
- Packaging support (AppImage, Flatpak, NSIS, DMG)
- Build automation script

### Technical Stack
- C++20, Qt 6.5+, CMake 3.21+
- Platforms: Linux, Windows, macOS
- Architectures: x86_64, ARM64
EOF

echo "✅ Committed successfully!"
echo ""

# Ask about pushing
read -p "Push to origin/main? (y/n) " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "🚀 Pushing to origin/main..."
    git push origin main
    echo "✅ Pushed successfully!"
    echo ""
    echo "🎉 OpenGalaxy rewrite is now live!"
else
    echo "⏸️  Not pushed. You can push later with:"
    echo "  git push origin main"
fi

echo ""
echo "📚 Next steps:"
echo "  1. Build the project: ./build.sh"
echo "  2. Run tests: cd build && ctest"
echo "  3. Try the GUI: ./build/ui/opengalaxy"
echo "  4. Try the CLI: ./build/cli/opengalaxy-cli --help"
echo ""
