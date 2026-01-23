#!/bin/bash
# Script to generate platform-specific icon files from PNG

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_PNG="$SCRIPT_DIR/opengalaxyicon.png"

echo "Generating platform-specific icons from: $SOURCE_PNG"

# Check if source exists
if [ ! -f "$SOURCE_PNG" ]; then
    echo "Error: Source PNG not found: $SOURCE_PNG"
    exit 1
fi

# Generate Windows .ico file (requires ImageMagick)
if command -v convert &> /dev/null; then
    echo "Generating Windows .ico file..."
    convert "$SOURCE_PNG" -define icon:auto-resize=256,128,64,48,32,16 "$SCRIPT_DIR/opengalaxyicon.ico"
    echo "✓ Created: opengalaxyicon.ico"
else
    echo "⚠ ImageMagick not found. Skipping .ico generation."
    echo "  Install with: sudo apt install imagemagick"
fi

# Generate macOS .icns file (requires png2icns or iconutil)
if command -v png2icns &> /dev/null; then
    echo "Generating macOS .icns file..."
    png2icns "$SCRIPT_DIR/opengalaxyicon.icns" "$SOURCE_PNG"
    echo "✓ Created: opengalaxyicon.icns"
elif command -v iconutil &> /dev/null; then
    echo "Generating macOS .icns file using iconutil..."
    # Create iconset directory
    ICONSET_DIR="$SCRIPT_DIR/opengalaxy.iconset"
    mkdir -p "$ICONSET_DIR"
    
    # Generate different sizes
    for size in 16 32 128 256 512; do
        convert "$SOURCE_PNG" -resize ${size}x${size} "$ICONSET_DIR/icon_${size}x${size}.png"
        convert "$SOURCE_PNG" -resize $((size*2))x$((size*2)) "$ICONSET_DIR/icon_${size}x${size}@2x.png"
    done
    
    # Convert to icns
    iconutil -c icns "$ICONSET_DIR" -o "$SCRIPT_DIR/opengalaxyicon.icns"
    rm -rf "$ICONSET_DIR"
    echo "✓ Created: opengalaxyicon.icns"
else
    echo "⚠ png2icns or iconutil not found. Skipping .icns generation."
    echo "  Install with: sudo apt install icnsutils (Linux)"
    echo "  or use iconutil on macOS"
fi

echo ""
echo "Icon generation complete!"
echo ""
echo "Generated files:"
ls -lh "$SCRIPT_DIR"/*.{ico,icns} 2>/dev/null || echo "  (No platform-specific icons generated)"
echo ""
echo "Note: The PNG icon will be used on Linux automatically."
