# OpenGalaxy Link Systems Documentation

## Overview

OpenGalaxy provides integrated links to external GOG resources directly from the game information dialog. This document describes how the link system works, the URLs used, and how to maintain or extend it.

## Link Types

### 1. GOG Store Page

**Purpose**: Direct link to the game's store page on GOG.com

**URL Format**: 
```
https://www.gog.com/en/game/{slug}
```

**Example**:
```
https://www.gog.com/en/game/battle_brothers
```

**Fallback** (when slug is unavailable):
```
https://www.gog.com/en/games?search={title}
```

**Data Source**: Game slug from GOG API (`embed.gog.com/account/getFilteredProducts`)

**Implementation**: `ui/qt/dialogs/game_information_dialog.cpp:98-104`

---

### 2. Support Page

**Purpose**: Link to GOG support resources for the game

**URL Format**: 
```
https://support.gog.com/hc/en-us/search?query={title}&product=gog
```

**Example**:
```
https://support.gog.com/hc/en-us/search?query=Battle%20Brothers&product=gog
```

**Fallback** (when slug is unavailable):
```
https://support.gog.com/hc/en-us
```

**Data Source**: Game title from GameInfo struct

**Implementation**: `ui/qt/dialogs/game_information_dialog.cpp:107-118`

**Note**: Uses search instead of direct article links because the GOG API does not provide support article IDs. The search with `&product=gog` filter returns GOG-specific support articles.

---

### 3. Forum Page

**Purpose**: Direct link to the game's forum discussion on GOG.com

**URL Format**: 
```
https://www.gog.com/forum/{slug}
```

**Example**:
```
https://www.gog.com/forum/battle_brothers
```

**Fallback** (when slug is unavailable):
```
https://www.gog.com/forum
```

**Data Source**: Game slug from GOG API

**Implementation**: `ui/qt/dialogs/game_information_dialog.cpp:120-131`

---

## Data Flow

### Complete Link System Data Flow

```
┌─────────────────────────────────────────────────────────────┐
│ 1. GOG API (embed.gog.com/account/getFilteredProducts)      │
│    Returns: {id, title, slug, ...}                          │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│ 2. gog_client.cpp (fetchLibrary)                            │
│    Extracts: g.slug = p.value("slug").toString();           │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│ 3. library_service.cpp (cacheGames)                         │
│    Stores: INSERT INTO games (..., slug) VALUES (?, ...)    │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│ 4. Database (library.db)                                    │
│    Table: games (id, title, slug, ...)                      │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│ 5. library_service.cpp (getGame)                            │
│    Retrieves: SELECT ... slug FROM games WHERE id = ?       │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│ 6. GameInfo object (with slug populated)                    │
│    Passed to: GameInformationDialog                         │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│ 7. game_information_dialog.cpp                              │
│    Constructs URLs using slug and title                     │
│    - Store: https://www.gog.com/en/game/{slug}              │
│    - Support: https://support.gog.com/hc/en-us/search?...   │
│    - Forum: https://www.gog.com/forum/{slug}                │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│ 8. QDesktopServices::openUrl()                              │
│    Opens URL in default browser                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Implementation Details

### Files Involved

1. **core/include/opengalaxy/api/models.h**
   - Defines `GameInfo` struct with `slug` field
   - Line 17: `QString slug;  // GOG game slug (for store URLs)`

2. **core/src/api/gog_client.cpp**
   - Extracts slug from GOG API response
   - Line 72: `g.slug = p.value("slug").toString();`

3. **core/src/library/library_service.cpp**
   - Database schema: Adds slug column
   - cacheGames(): Stores slug in database
   - getGame(): Retrieves slug from database
   - loadCachedGames(): Loads slug from cache

4. **ui/qt/dialogs/game_information_dialog.cpp**
   - Constructs URLs using slug and title
   - Opens URLs via QDesktopServices
   - Lines 96-131: Link button implementations

### Database Schema

```sql
CREATE TABLE IF NOT EXISTS games (
    id TEXT PRIMARY KEY,
    title TEXT NOT NULL,
    platform TEXT,
    coverUrl TEXT,
    backgroundUrl TEXT,
    developer TEXT,
    publisher TEXT,
    releaseDate TEXT,
    description TEXT,
    isInstalled INTEGER DEFAULT 0,
    installPath TEXT,
    version TEXT,
    size INTEGER DEFAULT 0,
    preferredRunner TEXT,
    runnerExecutable TEXT,
    runnerArguments TEXT,
    extraEnvironment TEXT,
    slug TEXT  -- Added for GOG store/forum links
)
```

### URL Construction Code

```cpp
// GOG Store link
if (!game_.slug.isEmpty()) {
    url = QString("https://www.gog.com/en/game/%1").arg(game_.slug);
} else {
    QString searchQuery = game_.title.replace(" ", "%20");
    url = QString("https://www.gog.com/en/games?search=%1").arg(searchQuery);
}

// Support link
if (!game_.slug.isEmpty()) {
    url = QString("https://support.gog.com/hc/en-us/search?query=%1&product=gog")
        .arg(game_.title.replace(" ", "%20"));
} else {
    url = "https://support.gog.com/hc/en-us";
}

// Forum link
if (!game_.slug.isEmpty()) {
    url = QString("https://www.gog.com/forum/%1").arg(game_.slug);
} else {
    url = "https://www.gog.com/forum";
}
```

---

## Slug Format

### What is a Slug?

A slug is a URL-friendly identifier for a game. It's derived from the game title and uses underscores to separate words.

### Examples

| Game Title | Slug |
|-----------|------|
| Battle Brothers | `battle_brothers` |
| Assassin's Creed®: Director's Cut | `assassins_creed_directors_cut` |
| The Witcher 3 | `the_witcher_3` |
| Baldur's Gate 3 | `baldurs_gate_3` |

### Slug Characteristics

- Uses lowercase letters
- Replaces spaces with underscores
- Removes special characters (®, ™, etc.)
- Unique per game on GOG

---

## User Interface

### Information Dialog

The Information dialog is accessed by:
1. Right-clicking a game card in the library
2. Selecting "Information" from the context menu

The dialog displays:
- Game title
- Game description
- Release date and platform
- Three link buttons:
  - 🔗 GOG Store Page
  - ❓ Support
  - 💬 Forum

### Link Button Styling

```cpp
QPushButton {
    background: #6c5ce7;
    border: none;
    border-radius: 6px;
    padding: 10px 18px;
    color: white;
    font-weight: 600;
    font-size: 13px;
}

QPushButton:hover {
    background: #5f3dc4;
}

QPushButton:pressed {
    background: #4c2d9f;
}
```

---

## Debugging

### Debug Output

When a link is clicked, the application outputs debug information:

```
Opening GOG Store: https://www.gog.com/en/game/battle_brothers Slug: "battle_brothers"
Opening Support: https://support.gog.com/hc/en-us/search?query=Battle%20Brothers&product=gog Slug: "battle_brothers"
Opening Forum: https://www.gog.com/forum/battle_brothers Slug: "battle_brothers"
```

### Checking Database

To verify slugs are stored correctly:

```bash
sqlite3 ~/.local/share/OpenGalaxy/OpenGalaxy/library.db \
  "SELECT id, title, slug FROM games LIMIT 10;"
```

Expected output:
```
1590012242|Battle Brothers|battle_brothers
1207659023|Assassin's Creed®: Director's Cut|assassins_creed_directors_cut
```

---

## Limitations & Constraints

### API Limitations

1. **Support Article IDs**: The GOG API does not provide support article IDs, so we use search instead of direct links
2. **Slug Availability**: Some games may not have slugs in the API response (rare)
3. **URL Encoding**: Spaces in titles must be encoded as `%20` for URLs

### Fallback Behavior

When slug is unavailable:
- **Store**: Search by game title
- **Support**: Link to main support page
- **Forum**: Link to main forum page

---

## Maintenance & Updates

### Adding New Link Types

To add a new link type:

1. Add a new button in `game_information_dialog.cpp`
2. Implement the URL construction logic
3. Connect the button to `openLink()` slot
4. Add debug output with `qDebug()`

Example:
```cpp
QPushButton* newBtn = new QPushButton(tr("🔗 New Link"), content);
connect(newBtn, &QPushButton::clicked, this, [this]() {
    QString url = QString("https://example.com/%1").arg(game_.slug);
    qDebug() << "Opening New Link:" << url << "Slug:" << game_.slug;
    openLink(url);
});
linksLayout->addWidget(newBtn);
```

### Updating URL Formats

If GOG changes their URL structure:

1. Update the URL format in `game_information_dialog.cpp`
2. Test with actual games
3. Update this documentation
4. Commit changes with clear message

### Testing Links

1. Run the application: `build/ui/opengalaxy`
2. Right-click a game card
3. Click "Information"
4. Click each link button
5. Verify browser opens correct page
6. Check console for debug output

---

## Related Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md) - Overall system architecture
- [CONFIGURATION.md](CONFIGURATION.md) - Configuration and data storage
- [FEATURES_STATUS.md](FEATURES_STATUS.md) - Feature status and roadmap

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-25 | Initial implementation with GOG Store, Support, and Forum links |

---

## Contact & Support

For issues or questions about the link system:
1. Check the debug output in console
2. Verify database contains slug data
3. Test URL format in browser directly
4. Report issues on GitHub with debug output
