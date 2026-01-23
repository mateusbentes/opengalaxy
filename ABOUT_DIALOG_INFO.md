# About Dialog - Final Information

## About OpenGalaxy Dialog Content

### Icon
- **Size**: 128x128 pixels
- **Source**: `:/data/opengalaxyicon.png`
- **Display**: Centered at top of dialog

### Application Name
**OpenGalaxy**
- Font: 24px, bold
- Color: #3c3a37
- Alignment: Center

### Version
**Version 1.0.0**
- Font: 14px
- Color: #5a5855 (gray)
- Alignment: Center

### Description
```
OpenGalaxy is a free and open-source multiplatform GOG client.

It allows you to download, install, and play your GOG games on 
Windows, macOS, and Linux with support for Wine, Proton, and native games.

Inspired by Minigalaxy, OpenGalaxy provides a modern and user-friendly 
interface to manage your GOG library across all platforms.
```

**Key Points**:
- ✅ Emphasizes **multiplatform** support
- ✅ Lists all three platforms: **Windows, macOS, and Linux**
- ✅ Mentions Wine, Proton, and native games
- ✅ Credits **Minigalaxy** as inspiration
- ✅ Highlights cross-platform library management

### Links

**Project**
- URL: https://github.com/mateusbentes/opengalaxy
- Clickable link
- Opens in default browser

**License**
- Type: **Apache 2.0**
- Not clickable (just text)

**Website**
- URL: https://www.gog.com
- Clickable link
- Opens in default browser

### Styling

**Dialog**
- Background: #ffffff (white)
- Minimum size: 500x400 pixels
- Padding: 30px all around
- Spacing: 15px between elements

**Close Button**
- Background: #9b4dca (purple - matches app theme)
- Hover: #8a3eb9 (darker purple)
- Pressed: #7a2ea9 (even darker)
- Text: White, 14px, bold
- Padding: 10px 30px
- Border radius: 8px

## Access Path

1. Launch OpenGalaxy
2. Navigate to **Settings** page (sidebar)
3. Scroll to **About** section
4. Click **"About OpenGalaxy"** button
5. Dialog appears

## Comparison with Minigalaxy

| Feature | Minigalaxy | OpenGalaxy |
|---------|------------|------------|
| Icon display | ✅ Large icon | ✅ Large icon (128x128) |
| App name | ✅ Centered | ✅ Centered, bold |
| Version | ✅ Shown | ✅ Shown (1.0.0) |
| Description | ✅ Brief | ✅ Detailed, multiplatform |
| Links | ✅ GitHub, license | ✅ GitHub, license, GOG.com |
| Styling | ✅ Clean | ✅ Clean, purple theme |
| Credits | ✅ Contributors | ✅ Credits Minigalaxy |

## Translation Support

All text in the About dialog uses Qt's `tr()` function for translation:
- `tr("About OpenGalaxy")`
- `tr("OpenGalaxy")`
- `tr("Version 1.0.0")`
- `tr("OpenGalaxy is a free and open-source...")`
- `tr("<b>Project:</b> ...")`

This allows the About dialog to be translated into multiple languages.

## Technical Implementation

### Code Location
- **File**: `ui/qt/pages/settings_page.cpp`
- **Function**: `SettingsPage::onAboutClicked()`
- **Trigger**: "About OpenGalaxy" button in Settings

### Key Components
```cpp
// Icon
QLabel* iconLabel = new QLabel(&dialog);
QPixmap iconPixmap(":/data/opengalaxyicon.png");
iconLabel->setPixmap(iconPixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));

// Description
QLabel* descLabel = new QLabel(
    tr("OpenGalaxy is a free and open-source multiplatform GOG client.\n\n"
       "It allows you to download, install, and play your GOG games on "
       "Windows, macOS, and Linux with support for Wine, Proton, and native games.\n\n"
       "Inspired by Minigalaxy, OpenGalaxy provides a modern and user-friendly "
       "interface to manage your GOG library across all platforms."),
    &dialog
);

// Links
QLabel* linksLabel = new QLabel(
    tr("<b>Project:</b> <a href='https://github.com/mateusbentes/opengalaxy'>github.com/mateusbentes/opengalaxy</a><br>"
       "<b>License:</b> Apache 2.0<br>"
       "<b>Website:</b> <a href='https://www.gog.com'>GOG.com</a>"),
    &dialog
);
linksLabel->setOpenExternalLinks(true);
```

## Screenshots Description

### Expected Appearance

```
┌─────────────────────────────────────────────┐
│                                             │
│              [OpenGalaxy Icon]              │
│                 128x128                     │
│                                             │
│              OpenGalaxy                     │
│             Version 1.0.0                   │
│                                             │
│  OpenGalaxy is a free and open-source      │
│  multiplatform GOG client.                 │
│                                             │
│  It allows you to download, install, and   │
│  play your GOG games on Windows, macOS,    │
│  and Linux with support for Wine, Proton,  │
│  and native games.                         │
│                                             │
│  Inspired by Minigalaxy, OpenGalaxy        │
│  provides a modern and user-friendly       │
│  interface to manage your GOG library      │
│  across all platforms.                     │
│                                             │
│  Project: github.com/mateusbentes/opengalaxy│
│  License: Apache 2.0                       │
│  Website: GOG.com                          │
│                                             │
│                  [Close]                    │
│                                             │
└─────────────────────────────────────────────┘
```

## Verification Checklist

- [x] Icon displays correctly (128x128)
- [x] App name is bold and centered
- [x] Version number is shown
- [x] Description mentions multiplatform support
- [x] All three platforms listed (Windows, macOS, Linux)
- [x] Minigalaxy credited as inspiration
- [x] GitHub link is correct (mateusbentes/opengalaxy)
- [x] License is Apache 2.0
- [x] GOG.com link is present
- [x] Links are clickable
- [x] Close button works
- [x] Dialog is styled with purple theme
- [x] All text is translatable

## Future Enhancements

Potential improvements for future versions:
- [ ] Add contributors list
- [ ] Add "Check for Updates" button
- [ ] Show build date/commit hash
- [ ] Add system information (Qt version, OS)
- [ ] Add "Report Bug" link
- [ ] Add social media links
- [ ] Add donation/support links

---

**Status**: ✅ **Complete and Accurate**

The About dialog now correctly represents OpenGalaxy as a multiplatform application with the correct GitHub repository and Apache 2.0 license.
