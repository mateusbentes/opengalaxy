# Future Features & Roadmap

## Overview

This document outlines potential features that could be added to OpenGalaxy in future releases, pending API availability and SDK access.

---

## Game Information Dialog - Future Enhancements

### Currently Implemented ✅

The Game Information Dialog currently displays:
- Game title and platform
- Game description
- Release date
- GOG Store link
- GOG Support link
- GOG Forum link

### Planned Features (Pending GOG SDK)

#### 1. Game Statistics

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

#### 2. Achievements

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

#### 3. Leaderboards

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

#### 4. Cloud Saves Integration

**What it would show**:
- Cloud save status
- Last sync date
- Save file size
- Sync conflicts (if any)
- Manual sync button
- Sync history

**UI Design**:
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

**Status**: ⏳ Waiting for GOG SDK

---

## Cloud Save Synchronization - Detailed Guide

### Overview

Cloud save synchronization allows players to:
- Automatically backup game saves to GOG Cloud
- Sync saves across multiple devices
- Restore saves if local files are lost
- Manage save conflicts

### How Cloud Saves Work

#### 1. Local Storage

**Location**: Game installation directory
```
~/.local/share/OpenGalaxy/games/
├── game_id_1/
│   ├── saves/
│   │   ├── save_1.sav
│   │   ├── save_2.sav
│   │   └── save_3.sav
│   └── config/
└── game_id_2/
    └── saves/
```

**Supported Formats**:
- Windows: `%APPDATA%\game_name\saves\`
- Linux: `~/.local/share/game_name/saves/`
- macOS: `~/Library/Application Support/game_name/saves/`

#### 2. Cloud Storage

**Provider**: GOG Cloud (when SDK available)
```
GOG Cloud Storage
├── game_id_1/
│   ├── saves/
│   │   ├── save_1.sav (timestamp)
│   │   ├── save_2.sav (timestamp)
│   │   └── save_3.sav (timestamp)
│   └── metadata.json
└── game_id_2/
    └── saves/
```

#### 3. Synchronization Process

**Upload Flow**:
```
Local Save File
    ↓
Calculate Hash
    ↓
Compare with Cloud
    ↓
If Different:
    ├─ Compress
    ├─ Encrypt
    └─ Upload to Cloud
    ↓
Update Metadata
    ↓
✅ Sync Complete
```

**Download Flow**:
```
Cloud Save File
    ↓
Check Timestamp
    ↓
Compare with Local
    ↓
If Newer:
    ├─ Download
    ├─ Decrypt
    ├─ Decompress
    └─ Save Locally
    ↓
Update Metadata
    ↓
✅ Sync Complete
```

### Sync Strategies

#### 1. Automatic Sync (Recommended)

**When to sync**:
- After game exit
- Every 5 minutes (configurable)
- On application startup
- On network connection

**Configuration**:
```
Cloud Saves Settings
├─ Enable Cloud Saves: [✓]
├─ Auto Sync: [✓]
├─ Sync Interval: [5 minutes ▼]
├─ Sync on Exit: [✓]
├─ Sync on Startup: [✓]
└─ Bandwidth Limit: [Unlimited ▼]
```

#### 2. Manual Sync

**User-initiated sync**:
- Click "Sync Now" button
- Right-click game → "Sync Saves"
- Menu → File → Sync All Saves

#### 3. Scheduled Sync

**Background sync**:
- Sync at specific times
- Sync on network change
- Sync on battery status change

### Conflict Resolution

#### Scenario 1: Local Newer Than Cloud

```
Local Save: 2026-01-25 23:58:37
Cloud Save: 2026-01-25 20:15:00

Action: Upload local to cloud
Result: ✅ Cloud updated
```

#### Scenario 2: Cloud Newer Than Local

```
Local Save: 2026-01-25 20:15:00
Cloud Save: 2026-01-25 23:58:37

Action: Download cloud to local
Result: ✅ Local updated
```

#### Scenario 3: Conflict (Both Modified)

```
Local Save: 2026-01-25 23:58:37
Cloud Save: 2026-01-25 23:55:00

Conflict Detected!
Options:
├─ Keep Local (Upload)
├─ Keep Cloud (Download)
├─ Keep Both (Create Backup)
└─ Manual Selection
```

**UI for Conflict Resolution**:
```
┌─ Sync Conflict ─────────────────────┐
│                                     │
│ Save file has conflicting versions  │
│                                     │
│ Local:  2026-01-25 23:58:37         │
│ Cloud:  2026-01-25 23:55:00         │
│                                     │
│ [Keep Local] [Keep Cloud] [Cancel]  │
│                                     │
└─────────────────────────────────────┘
```

### Storage Management

#### Quota System

```
Cloud Storage Quota
├─ Total: 10 GB
├─ Used: 2.5 GB
├─ Available: 7.5 GB
└─ Usage: [████████░░░░░░░░░░] 25%
```

#### Cleanup Options

```
Storage Management
├─ Delete Old Saves
│  └─ Keep last 5 saves per game
├─ Compress Saves
│  └─ Reduce size by ~30%
├─ Remove Unused Games
│  └─ Delete saves for uninstalled games
└─ Manual Cleanup
   └─ Select saves to delete
```

### Security & Privacy

#### Encryption

**In Transit**:
- HTTPS/TLS 1.3
- AES-256 encryption
- Certificate pinning

**At Rest**:
- AES-256 encryption
- Server-side encryption
- User-controlled keys (optional)

#### Privacy

```
Privacy Settings
├─ Encryption: [AES-256 ✓]
├─ Server Logs: [Disabled ✓]
├─ Data Retention: [90 days ▼]
├─ Share with GOG: [Disabled ✓]
└─ Analytics: [Disabled ✓]
```

### Bandwidth Management

#### Throttling

```
Bandwidth Settings
├─ Upload Speed: [Unlimited ▼]
├─ Download Speed: [Unlimited ▼]
├─ Metered Connection: [Pause Sync ✓]
└─ Battery Saver: [Pause Sync ✓]
```

#### Data Usage

```
Data Usage (This Month)
├─ Uploaded: 125 MB
├─ Downloaded: 89 MB
├─ Total: 214 MB
└─ Limit: Unlimited
```

### Troubleshooting

#### Common Issues

**Issue 1: Sync Fails**
```
Error: Failed to sync saves
Possible Causes:
├─ No internet connection
├─ GOG server down
├─ Insufficient storage quota
├─ File permissions issue
└─ Corrupted save file

Solutions:
├─ Check internet connection
├─ Retry sync
├─ Check storage quota
├─ Check file permissions
└─ Restore from backup
```

**Issue 2: Conflict Loop**
```
Error: Continuous sync conflicts
Possible Causes:
├─ Clock skew between devices
├─ Simultaneous edits
├─ Network latency
└─ Corrupted metadata

Solutions:
├─ Sync system time
├─ Wait before playing on other device
├─ Check network stability
└─ Clear sync history
```

**Issue 3: Storage Full**
```
Error: Cloud storage quota exceeded
Possible Causes:
├─ Too many save files
├─ Large save files
├─ Old backups not deleted
└─ Other data using quota

Solutions:
├─ Delete old saves
├─ Compress saves
├─ Remove unused games
└─ Upgrade storage plan
```

### Implementation Checklist

When GOG SDK becomes available:

- [ ] Implement cloud saves API integration
- [ ] Create sync engine
- [ ] Implement conflict resolution
- [ ] Add UI for cloud saves settings
- [ ] Add sync status indicator
- [ ] Implement encryption
- [ ] Add bandwidth throttling
- [ ] Create backup system
- [ ] Add sync history
- [ ] Implement error handling
- [ ] Add logging and debugging
- [ ] Write comprehensive tests
- [ ] Create user documentation
- [ ] Add troubleshooting guide

---

## Other Potential Features

### Game Library Enhancements

#### 1. Game Reviews & Ratings
- User ratings (1-5 stars)
- Community reviews
- Review sorting (helpful, recent, etc.)

**Requirements**: GOG SDK with reviews API

#### 2. Game Recommendations
- Similar games
- "Players also bought" section
- Personalized recommendations

**Requirements**: GOG recommendation API

#### 3. DLC Management
- List installed DLCs
- Download/install DLC
- DLC pricing and availability

**Requirements**: GOG SDK with DLC API

#### 4. Game Updates
- Check for game updates
- Update history
- Patch notes

**Requirements**: GOG SDK with updates API

---

### UI/UX Improvements

#### 1. Game Card Enhancements
- Hover preview (screenshot carousel)
- Quick stats overlay
- Achievement progress indicator
- Play time display

#### 2. Advanced Filtering
- Filter by achievement completion
- Filter by play time
- Filter by rating
- Filter by DLC status

#### 3. Game Comparison
- Compare stats between games
- Compare achievements
- Compare playtime

---

### Performance & Optimization

#### 1. Caching
- Cache achievement data
- Cache leaderboard data
- Cache statistics
- Offline mode support

#### 2. Background Sync
- Auto-sync cloud saves
- Auto-update statistics
- Background achievement tracking

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

---

## Implementation Priority

### Phase 1 (Current) ✅
- [x] Game information dialog
- [x] GOG links (Store, Support, Forum)
- [x] Wine/Proton tools
- [x] Game properties/configuration

### Phase 2 (Pending SDK)
- [ ] Game statistics
- [ ] Achievements
- [ ] Cloud saves integration

### Phase 3 (Pending SDK)
- [ ] Leaderboards
- [ ] Game reviews
- [ ] DLC management

### Phase 4 (Future)
- [ ] Game recommendations
- [ ] Advanced filtering
- [ ] Game comparison

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

- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture
- [FEATURES_STATUS.md](FEATURES_STATUS.md) - Current feature status
- [LINK_SYSTEMS.md](LINK_SYSTEMS.md) - GOG links implementation

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-25 | Initial roadmap document |

---

## Contact & Support

For questions about future features:
1. Check this document
2. Search GitHub issues
3. Ask in community forums
4. Monitor GOG SDK announcements
