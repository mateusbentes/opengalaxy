// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QObject>
#include <vector>
#include <functional>
#include "../api/models.h"
#include "../api/gog_client.h"
#include "../util/result.h"

namespace opengalaxy::library {

/**
 * @brief Library service with local caching
 */
class LibraryService : public QObject {
    Q_OBJECT

public:
    explicit LibraryService(api::GOGClient* gogClient, QObject* parent = nullptr);
    ~LibraryService() override;

    using GamesCallback = std::function<void(util::Result<std::vector<api::GameInfo>>)>;
    using GameCallback = std::function<void(util::Result<api::GameInfo>)>;

    // Fetch library (from cache or API)
    void fetchLibrary(bool forceRefresh, GamesCallback callback);
    
    // Get single game
    void getGame(const QString& gameId, GameCallback callback);
    
    // Update game installation status
    void updateGameInstallation(const QString& gameId, const QString& installPath, const QString& version);
    void removeGameInstallation(const QString& gameId);

    // Update per-game properties
    void updateGameProperties(const api::GameInfo& game);

    // Search and filter
    std::vector<api::GameInfo> searchGames(const QString& query);
    std::vector<api::GameInfo> filterByPlatform(const QString& platform);
signals:
    void libraryUpdated(int gameCount);
    void gameUpdated(const QString& gameId);

private:
    api::GOGClient* gogClient_;
    class LibraryDatabase* db_;

    void initDatabase();
    void cacheGames(const std::vector<api::GameInfo>& games);

public:
    // Exposed for UI convenience (e.g., offline/demo mode)
    std::vector<api::GameInfo> loadCachedGames();
};

} // namespace opengalaxy::library
