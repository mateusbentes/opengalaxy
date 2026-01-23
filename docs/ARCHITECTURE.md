# OpenGalaxy Architecture

High-level system design and component overview.

## 🏗️ Overview

OpenGalaxy follows a modular, layered architecture with clear separation between core logic, UI, and CLI interfaces.

```
┌─────────────────────────────────────────────────────────┐
│                    User Interfaces                       │
│  ┌──────────────────────┐    ┌──────────────────────┐  │
│  │     Qt6 GUI App      │    │     CLI Tool         │  │
│  │  (ui/qt/)            │    │  (cli/)              │  │
│  └──────────────────────┘    └──────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│                    Core Library                          │
│  ┌──────────────────────────────────────────────────┐  │
│  │  API Layer (core/src/api/)                       │  │
│  │  • Session (auth, tokens)                        │  │
│  │  • GOGClient (API calls)                         │  │
│  │  • HttpClient (networking)                       │  │
│  └──────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Service Layer (core/src/)                       │  │
│  │  • LibraryService (game management)              │  │
│  │  • InstallService (downloads, installation)      │  │
│  └──────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Runner System (core/src/runners/)               │  │
│  │  • RunnerManager (detection, selection)          │  │
│  │  • Wine/Proton/Native/Wrapper runners            │  │
│  └──────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Utilities (core/src/util/)                      │  │
│  │  • Logging, error handling, helpers              │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│              External Dependencies                       │
│  • Qt6 (Network, Widgets, SQL, WebEngine)               │
│  • OpenSSL (HTTPS, crypto)                              │
│  • SQLite (local database)                              │
│  • GOG API (game data, downloads)                       │
└─────────────────────────────────────────────────────────┘
```

---

## 📦 Core Components

### 1. Session Management (`core/src/api/session.cpp`)

**Purpose**: Handle authentication and token lifecycle

**Key Features**:
- OAuth 2.0 with PKCE flow
- Password-based authentication
- Automatic token refresh
- Secure token storage
- Session persistence

**API**:
```cpp
class Session {
    Result<void> loginWithPassword(QString email, QString password);
    Result<void> loginWithOAuth();
    Result<void> refreshToken();
    void logout();
    bool isAuthenticated();
    QString getAccessToken();
};
```

**Data Flow**:
```
User Credentials → Session::login() → GOG Auth API
                                    ↓
                              Access Token
                                    ↓
                         Save to session.json
                                    ↓
                         Use for API requests
```

---

### 2. GOG API Client (`core/src/api/gog_client.cpp`)

**Purpose**: Interface with GOG's REST API

**Key Features**:
- Library fetching (owned games)
- Store browsing and search
- Game details and metadata
- Download URL resolution
- Achievements tracking
- Cloud saves (stub)

**API**:
```cpp
class GOGClient {
    Result<QVector<Game>> getOwnedGames();
    Result<QVector<Game>> searchStore(QString query);
    Result<GameDetails> getGameDetails(QString gameId);
    Result<QVector<DownloadLink>> getDownloadLinks(QString gameId);
    Result<QVector<Achievement>> getAchievements(QString gameId);
};
```

**Endpoints Used**:
- `https://embed.gog.com/account/getFilteredProducts` - Library
- `https://catalog.gog.com/v1/catalog` - Store search
- `https://api.gog.com/products/{id}` - Game details
- `https://api.gog.com/products/{id}/downloads` - Download links

---

### 3. HTTP Client (`core/src/net/http_client.cpp`)

**Purpose**: Async HTTP requests with retry and progress

**Key Features**:
- Async/non-blocking operations
- Automatic retry on failure
- Timeout handling
- Progress tracking (downloads)
- HTTPS with SSL verification
- Bearer token authentication

**API**:
```cpp
class HttpClient {
    void get(QString url, Callback callback);
    void post(QString url, QByteArray data, Callback callback);
    void download(QString url, QString path, ProgressCallback progress);
    void setAuthToken(QString token);
    void setTimeout(int ms);
};
```

**Features**:
- Uses Qt's `QNetworkAccessManager`
- Emits signals for progress updates
- Thread-safe operation
- Automatic JSON parsing

---

### 4. Library Service (`core/src/library/library_service.cpp`)

**Purpose**: Manage game library with local caching

**Key Features**:
- SQLite database for caching
- Automatic sync with GOG
- Search and filtering
- Installation tracking
- Metadata management

**API**:
```cpp
class LibraryService {
    Result<void> syncLibrary();
    QVector<Game> getGames();
    QVector<Game> searchGames(QString query);
    Result<Game> getGame(QString gameId);
    void markInstalled(QString gameId, QString path);
};
```

**Database Schema**:
```sql
CREATE TABLE games (
    id TEXT PRIMARY KEY,
    title TEXT NOT NULL,
    platform TEXT,
    cover_url TEXT,
    installed BOOLEAN DEFAULT 0,
    install_path TEXT,
    last_sync INTEGER
);
```

---

### 5. Install Service (`core/src/install/install_service.cpp`)

**Purpose**: Download and install games

**Key Features**:
- Async downloads with progress
- Wine installer execution
- Installation tracking
- Cancel/resume support
- Error handling and retry

**API**:
```cpp
class InstallService {
    void installGame(QString gameId, QString installDir);
    void cancelInstall(QString gameId);
    void uninstallGame(QString gameId);
    float getProgress(QString gameId);
};
```

**Installation Flow**:
```
1. Get download links from GOG API
2. Select appropriate installer (OS, language)
3. Download installer file
4. Extract/run installer (Wine for Windows games)
5. Update library database
6. Emit completion signal
```

---

### 6. Runner System (`core/src/runners/`)

**Purpose**: Execute games with appropriate compatibility layers

**Components**:
- `RunnerManager` - Discovery and selection
- `Runner` - Base class for all runners
- `WineRunner` - Windows games via Wine
- `ProtonRunner` - Windows games via Proton
- `WrapperRunner` - ISA translation (Box64, FEX, QEMU)
- `NativeRunner` - Direct execution

**Runner Selection Logic**:
```cpp
Runner* RunnerManager::findBestRunner(Game game) {
    // 1. Check user preference
    if (game.hasPreferredRunner())
        return getRunner(game.preferredRunner);
    
    // 2. Native runner if available
    if (game.platform == currentPlatform())
        return nativeRunner;
    
    // 3. Wine/Proton for Windows games
    if (game.platform == "windows") {
        if (protonAvailable) return protonRunner;
        if (wineAvailable) return wineRunner;
    }
    
    // 4. ISA translation if needed
    if (needsTranslation(game.arch, hostArch())) {
        if (fexAvailable) return fexRunner;
        if (box64Available) return box64Runner;
        if (qemuAvailable) return qemuRunner;
    }
    
    return nullptr;
}
```

**Runner Interface**:
```cpp
class Runner {
    virtual bool canRun(Game game) = 0;
    virtual Result<void> launch(Game game, QString path) = 0;
    virtual bool isAvailable() = 0;
    virtual QString name() = 0;
};
```

---

## 🎨 UI Architecture (Qt6)

### Page-Based Navigation

```
AppWindow (main window)
├── Sidebar (navigation)
└── QStackedWidget (pages)
    ├── LoginPage
    ├── LibraryPage
    ├── StorePage
    ├── FriendsPage
    └── SettingsPage
```

### Key UI Components

**1. AppWindow** (`ui/qt/app_window.cpp`)
- Main application window
- Sidebar navigation
- Page management
- System tray integration

**2. LibraryPage** (`ui/qt/pages/library_page.cpp`)
- Game grid with cards
- Search and filters
- Install/launch actions
- Progress tracking

**3. GameCard** (`ui/qt/widgets/game_card.cpp`)
- Game cover art
- Title and metadata
- Action buttons (Play/Install/Details)
- Progress bar (during install)

**4. Dialogs**
- `GameDetailsDialog` - Game info and runner selection
- `CloudSavesDialog` - Cloud save management
- `OAuthLoginDialog` - OAuth authentication

### Signal/Slot Architecture

```cpp
// Example: Install flow
LibraryPage::onInstallClicked(gameId) 
    → InstallService::installGame(gameId)
    → InstallService::progressUpdated(gameId, progress)
    → GameCard::updateProgress(progress)
    → InstallService::installCompleted(gameId)
    → GameCard::showPlayButton()
```

---

## 🖥️ CLI Architecture

Simple command-based interface:

```
main.cpp
├── Parse arguments
├── Initialize core library
├── Execute command
│   ├── login
│   ├── list
│   ├── install
│   ├── launch
│   └── runners
└── Print results
```

**Example**:
```cpp
// cli/main.cpp
if (command == "install") {
    InstallService installer;
    installer.installGame(gameId, installDir);
    
    // Show progress
    connect(&installer, &InstallService::progressUpdated,
            [](float progress) {
                std::cout << "Progress: " << progress << "%\n";
            });
}
```

---

## 🔄 Data Flow Examples

### 1. User Login

```
User enters credentials
    ↓
LoginPage::onLoginClicked()
    ↓
Session::loginWithPassword(email, password)
    ↓
HttpClient::post("https://auth.gog.com/token", credentials)
    ↓
GOG Auth API returns tokens
    ↓
Session saves tokens to session.json
    ↓
Session::authenticated() signal
    ↓
AppWindow switches to LibraryPage
    ↓
LibraryService::syncLibrary()
```

### 2. Game Installation

```
User clicks "Install" on game card
    ↓
LibraryPage::onInstallClicked(gameId)
    ↓
InstallService::installGame(gameId, installDir)
    ↓
GOGClient::getDownloadLinks(gameId)
    ↓
HttpClient::download(url, path)
    ↓
[Progress updates via signals]
    ↓
InstallService executes installer (Wine if Windows)
    ↓
LibraryService::markInstalled(gameId, path)
    ↓
InstallService::installCompleted() signal
    ↓
GameCard updates UI (Install → Play button)
```

### 3. Game Launch

```
User clicks "Play"
    ↓
LibraryPage::onPlayClicked(gameId)
    ↓
RunnerManager::findBestRunner(game)
    ↓
Runner::launch(game, installPath)
    ↓
QProcess::start(executable, args)
    ↓
Game runs
```

---

## 🔒 Security Considerations

### Token Storage
- Tokens stored in `~/.local/share/opengalaxy/session.json`
- File permissions: 0600 (user read/write only)
- Tokens encrypted at rest (future enhancement)

### HTTPS
- All API calls use HTTPS
- SSL certificate verification enabled
- Uses system CA certificates

### Input Validation
- All user inputs sanitized
- SQL injection prevention (parameterized queries)
- Path traversal prevention

---

## 🧪 Testing Architecture

### Unit Tests (`tests/`)

```
tests/
├── api_tests.cpp       # Session, GOGClient, HttpClient
├── core_tests.cpp      # LibraryService, InstallService
└── runner_tests.cpp    # RunnerManager, runners
```

**Test Framework**: Qt Test

**Example**:
```cpp
void ApiTests::testLogin() {
    Session session;
    auto result = session.loginWithPassword("test@example.com", "password");
    QVERIFY(result.isOk());
    QVERIFY(session.isAuthenticated());
}
```

---

## 📊 Performance Considerations

### Async Operations
- All network calls are async (non-blocking UI)
- Uses Qt's signal/slot mechanism
- Background threads for heavy operations

### Caching
- Library cached in SQLite (fast local access)
- Cover images cached on disk
- API responses cached with TTL

### Memory Management
- Qt's parent-child ownership (automatic cleanup)
- Smart pointers for complex objects
- Lazy loading of game details

---

## 🔮 Future Enhancements

### Planned Improvements
- [ ] Plugin system for extensibility
- [ ] Multi-platform integration (Steam, Epic)
- [ ] Advanced cloud save sync
- [ ] In-game overlay
- [ ] Mod manager integration

### Architecture Changes
- [ ] Move to async/await pattern (C++20 coroutines)
- [ ] Implement event sourcing for state management
- [ ] Add GraphQL layer for flexible queries
- [ ] Microservices for scalability

---

## 📚 Additional Resources

- [Qt6 Documentation](https://doc.qt.io/qt-6/)
- [GOG API (Unofficial)](https://gogapidocs.readthedocs.io/)
- [C++20 Features](https://en.cppreference.com/w/cpp/20)

---

**Last Updated**: 2026-01-23
