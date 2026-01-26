// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../net/http_client.h"
#include "../util/result.h"
#include "models.h"
#include "session.h"
#include <QObject>
#include <functional>
#include <vector>

namespace opengalaxy::api {

/**
 * @brief GOG API client for game library and store operations
 */
class GOGClient : public QObject {
    Q_OBJECT

  public:
    explicit GOGClient(Session *session, QObject *parent = nullptr);
    ~GOGClient() override;

    using GamesCallback = std::function<void(util::Result<std::vector<GameInfo>>)>;
    using GameCallback = std::function<void(util::Result<GameInfo>)>;
    using AchievementsCallback = std::function<void(util::Result<std::vector<Achievement>>)>;
    using CloudSavesCallback = std::function<void(util::Result<std::vector<CloudSave>>)>;

    // Library operations
    void fetchLibrary(GamesCallback callback);
    void fetchGameDetails(const QString &gameId, GameCallback callback);
    void fetchGameDownloads(const QString &gameId, GameCallback callback);

    // Achievements
    void fetchAchievements(const QString &gameId, AchievementsCallback callback);
    void unlockAchievement(const QString &gameId, const QString &achievementId,
                           std::function<void(util::Result<void>)> callback);

    // Cloud saves
    void listCloudSaves(const QString &gameId, CloudSavesCallback callback);
    void uploadCloudSave(const QString &gameId, const QString &filePath,
                         std::function<void(util::Result<void>)> callback);
    void downloadCloudSave(const QString &gameId, const QString &filename, const QString &destPath,
                           std::function<void(util::Result<void>)> callback);

    // Store operations
    void searchStore(const QString &query,
                     std::function<void(util::Result<std::vector<StoreGameInfo>>)> callback);
    void fetchStoreGames(std::function<void(util::Result<std::vector<StoreGameInfo>>)> callback);

    // Locale settings
    void setLocale(const QString &locale) { locale_ = locale; }
    QString locale() const { return locale_; }

  signals:
    void libraryUpdated();
    void gameDetailsUpdated(const QString &gameId);

  private:
    Session *session_;
    net::HttpClient *httpClient_;
    QString locale_ = "en-US";

    // API endpoints
    static constexpr const char *API_BASE = "https://api.gog.com";
    static constexpr const char *EMBED_BASE = "https://embed.gog.com";
    static constexpr const char *AUTH_BASE = "https://auth.gog.com";

    QString buildAuthHeader() const;
    void handleApiError(const net::HttpClient::Response &response, const QString &operation);
};

} // namespace opengalaxy::api
