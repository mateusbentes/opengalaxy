# Future Features & Roadmap

## Overview

This document outlines potential features that could be added to OpenGalaxy in future releases, pending API availability and SDK access.

**Status Update (2026-01-26)**: All missing features have been implemented! Only SDK-dependent features remain. See "Pending GOG SDK" section below.

### Recently Implemented Features ✅

- ✅ **Verify Game Files** - Check game file integrity (2026-01-26)
- ✅ **Repair Game** - Repair corrupted game files (2026-01-26)

---

## Pending GOG SDK Features

These features require the GOG SDK which is not yet publicly available. We are waiting for GOG to release a public SDK with these APIs.

### 1. Cloud Saves Integration

**Current Status (2026-01-26)**: ⏳ UI Toggle Implemented, Sync Pending SDK

**What's Implemented**:
- ✅ Per-game "Enable Cloud Saves" toggle in Properties dialog
- ✅ Toggle state persisted in database
- ✅ UI ready for sync functionality

**What's Pending**:
- Cloud save synchronization engine
- Cloud save status display
- Last sync date and file sizes
- Sync conflicts detection
- Manual sync button
- Sync history
- Encryption and security

**UI Design** (Current - Toggle Only):
```
Game Tweaks Section:
☑ Enable Cloud Saves
  (Tooltip: Sync saves to GOG Cloud - requires GOG SDK)
```

**UI Design** (Future - Full Implementation):
```
┌─ Cloud Saves ───────────────────────┐
│                                     │
│ Status: ✅ Synced                   │
│ Last Sync: 2026-01-25 23:58:37      │
│ Local Size: 2.5 MB                  │
│ Cloud Size: 2.5 MB                  │
│                                     │
│ [🔄 Sync Now] [⚙️ Settings]         │
│                                     │
│ Sync History:                       │
│ ├─ 2026-01-25 23:58 - Success       │
│ ├─ 2026-01-25 20:15 - Success       │
│ └─ 2026-01-25 15:30 - Success       │
│                                     │
└─────────────────────────────────────┘
```

**Requirements**:
- GOG SDK with cloud saves API
- User authentication
- Save file management
- Conflict resolution system

**Status**: ⏳ Waiting for GOG SDK (UI toggle ready)

---

### 2. Game Statistics

**What it would show**:
- Play time (total hours played)
- Last played date
- Achievement progress (X/Y achievements)
- Completion percentage
- Game rating/score

**Requirements**:
- GOG SDK with statistics API
- User session/authentication
- Cloud save integration

**Status**: ⏳ Waiting for GOG SDK

---

### 3. Achievements

**What it would show**:
- Achievement list with icons
- Achievement name and description
- Unlock status (locked/unlocked)
- Unlock date (if unlocked)
- Rarity percentage
- Progress bars for multi-step achievements

**UI Design**:
```
┌─ Achievements ──────────────────────┐
│                                     │
│ [Icon] Achievement Name             │
│        Description                  │
│        Unlocked: 2026-01-25         │
│        Rarity: 5.2%                 │
│                                     │
│ [Icon] Locked Achievement           │
│        Description                  │
│        Locked                       │
│        Rarity: 12.5%                │
│                                     │
└─────────────────────────────────────┘
```

**Requirements**:
- GOG SDK with achievements API
- User authentication
- Achievement icon URLs
- Unlock timestamps

**Status**: ⏳ Waiting for GOG SDK

---

### 4. Leaderboards

**What it would show**:
- Global leaderboards
- Friend leaderboards
- Personal rank
- Top scores/times
- Leaderboard type (score, time, etc.)

**UI Design**:
```
┌─ Leaderboards ──────────────────────┐
│                                     │
│ [Global] [Friends] [Personal]       │
│                                     │
│ Rank | Player      | Score | Date   │
│ ──────────────────────────────────  │
│  1   | Player1     | 9999  | 2026-01│
│  2   | Player2     | 8888  | 2026-01│
│  3   | You         | 7777  | 2026-01│
│  4   | Player3     | 6666  | 2026-01│
│                                     │
└─────────────────────────────────────┘
```

**Requirements**:
- GOG SDK with leaderboards API
- User authentication
- Real-time leaderboard data
- Friend list integration

**Status**: ⏳ Waiting for GOG SDK

---

### 5. Game Reviews & Ratings

**What it would show**:
- User ratings (1-5 stars)
- Community reviews
- Review sorting (helpful, recent, etc.)
- User review submission

**Requirements**:
- GOG SDK with reviews API
- User authentication
- Review moderation system

**Status**: ⏳ Waiting for GOG SDK

---

### 6. Game Recommendations

**What it would show**:
- Similar games
- "Players also bought" section
- Personalized recommendations
- Trending games

**Requirements**:
- GOG recommendation API
- User library data
- Recommendation algorithm

**Status**: ⏳ Waiting for GOG SDK

---

### 7. DLC Management

**What it would show**:
- List installed DLCs
- Download/install DLC
- DLC pricing and availability
- DLC dependencies

**Requirements**:
- GOG SDK with DLC API
- DLC download support
- DLC installation logic

**Status**: ⏳ Waiting for GOG SDK

---

## Non-SDK Features (Can Be Implemented Anytime)

These features don't require GOG SDK and can be implemented independently.

### 1. Game Collections

**What it does**:
- Create custom game groups
- Organize games by category
- Quick access to collections
- Drag-and-drop organization

**UI Design**:
```
Collections:
├─ Favorites
├─ Wishlist
├─ Completed
├─ In Progress
└─ Multiplayer
```

**Priority**: Medium  
**Difficulty**: Medium  
**Status**: ⏳ Not Started

---

### 2. Advanced Filtering

**What it does**:
- Filter by genre
- Filter by platform
- Filter by installation status
- Filter by play time
- Filter by rating
- Combine multiple filters

**UI Design**:
```
Filters:
├─ Genre: [Action ▼] [RPG ▼]
├─ Platform: [Windows ✓] [Linux ✓]
├─ Status: [Installed ✓] [Not Installed]
└─ Play Time: [> 10 hours]
```

**Priority**: Medium  
**Difficulty**: Low  
**Status**: ⏳ Not Started

---

### 3. Game Comparison

**What it does**:
- Compare stats between games
- Compare achievements
- Compare playtime
- Side-by-side comparison view

**Priority**: Low  
**Difficulty**: Medium  
**Status**: ⏳ Not Started

---

### 4. Game Card Enhancements

**What it does**:
- Hover preview (screenshot carousel)
- Quick stats overlay
- Achievement progress indicator
- Play time display
- Last played date

**Priority**: Medium  
**Difficulty**: Medium  
**Status**: ⏳ Not Started

---

### 5. Batch Operations

**What it does**:
- Hide/show multiple games at once
- Batch install/uninstall
- Batch update
- Bulk property changes

**Priority**: Low  
**Difficulty**: Low  
**Status**: ⏳ Not Started

---

## UI/UX Improvements

### 1. Dark Mode

**What it does**:
- Complete dark theme
- Automatic theme switching
- Theme customization

**Priority**: Low  
**Difficulty**: Low  
**Status**: ⏳ Not Started

---

### 2. Customizable Layout

**What it does**:
- Resizable panels
- Customizable columns
- Save layout preferences
- Multiple layout presets

**Priority**: Low  
**Difficulty**: Medium  
**Status**: ⏳ Not Started

---

### 3. Game Search Improvements

**What it does**:
- Advanced search syntax
- Search history
- Saved searches
- Fuzzy matching

**Priority**: Low  
**Difficulty**: Low  
**Status**: ⏳ Not Started

---

## Performance & Optimization

### 1. Caching System

**What it does**:
- Cache achievement data
- Cache leaderboard data
- Cache statistics
- Offline mode support

**Priority**: Medium  
**Difficulty**: Medium  
**Status**: ⏳ Not Started

---

### 2. Background Sync

**What it does**:
- Auto-sync cloud saves
- Auto-update statistics
- Background achievement tracking
- Scheduled sync

**Priority**: Medium  
**Difficulty**: Medium  
**Status**: ⏳ Not Started

---

## GOG SDK Status

### Current Situation
- GOG does not currently provide a public SDK for third-party applications
- Statistics, achievements, and leaderboards APIs are not publicly available
- These features are only available through GOG Galaxy client

### How to Track SDK Availability

1. **GOG Developer Portal**: https://www.gog.com/developer
2. **GOG Community Forums**: https://www.gog.com/forum
3. **GitHub Issues**: Track feature requests in OpenGalaxy repository

### What We're Waiting For

- [ ] Public GOG SDK release
- [ ] Statistics API documentation
- [ ] Achievements API documentation
- [ ] Leaderboards API documentation
- [ ] Cloud saves API documentation
- [ ] User authentication API
- [ ] DLC management API

---

## Implementation Priority

### Phase 1 (Current) ✅
- [x] Game information dialog
- [x] GOG links (Store, Support, Forum)
- [x] Wine/Proton tools
- [x] Game properties/configuration
- [x] Game tweaks (FPS, MangoHud, GameMode)
- [x] Hidden games filtering

### Phase 2 (Pending SDK)
- [ ] Game statistics
- [ ] Achievements
- [ ] Cloud saves integration
- [ ] Leaderboards

### Phase 3 (Can Implement Anytime)
- [ ] Game collections
- [ ] Advanced filtering
- [ ] Game comparison
- [ ] Batch operations

### Phase 4 (Future)
- [ ] Game recommendations
- [ ] DLC management
- [ ] Dark mode
- [ ] Customizable layout

---

## How to Contribute

If you want to help implement these features:

1. **Monitor GOG SDK**: Check for SDK releases
2. **Research APIs**: Document available APIs
3. **Create Issues**: Open GitHub issues for feature requests
4. **Submit PRs**: Implement features when APIs become available
5. **Test**: Help test new features

---

## Related Documentation

- [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md) - Implemented features
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture
- [FEATURES_STATUS.md](FEATURES_STATUS.md) - Current feature status

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 2.0 | 2026-01-26 | Reorganized - separated SDK features from non-SDK features |
| 1.0 | 2026-01-25 | Initial roadmap document |

---

## Contact & Support

For questions about future features:
1. Check this document
2. Search GitHub issues
3. Ask in community forums
4. Monitor GOG SDK announcements
