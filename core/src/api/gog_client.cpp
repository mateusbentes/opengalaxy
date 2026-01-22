// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/api/gog_client.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>
#include "opengalaxy/net/http_client.h"

namespace opengalaxy::api {

GOGClient::GOGClient(Session* session, QObject* parent)
    : QObject(parent), session_(session) {
    httpClient_ = new net::HttpClient(this);
}

GOGClient::~GOGClient() = default;

QString GOGClient::buildAuthHeader() const {
    if (session_->isAuthenticated()) {
        return "Bearer " + session_->tokens().accessToken;
    }
    return QString();
}

void GOGClient::fetchLibrary(GamesCallback callback)
{
    QTimer::singleShot(0, this, [this, callback = std::move(callback)]() mutable {
        if (!session_->isAuthenticated()) {
            callback(util::Result<std::vector<GameInfo>>::error("Not authenticated"));
            return;
        }

        net::HttpClient::Request req;
        req.url = "https://embed.gog.com/account/getFilteredProducts?mediaType=1&page=1";
        req.method = "GET";
        req.headers["Authorization"] = buildAuthHeader();

        httpClient_->request(req, [callback = std::move(callback)](util::Result<net::HttpClient::Response> result) mutable {
            if (!result.isOk()) {
                callback(util::Result<std::vector<GameInfo>>::error(result.errorMessage()));
                return;
            }

            const QJsonObject obj = QJsonDocument::fromJson(result.value().body).object();
            const QJsonArray products = obj.value("products").toArray();

            std::vector<GameInfo> games;
            games.reserve(products.size());

            for (const auto& v : products) {
                const QJsonObject p = v.toObject();
                GameInfo g;
                g.id = QString::number(p.value("id").toVariant().toLongLong());
                g.title = p.value("title").toString();
                g.coverUrl = p.value("image").toString();

                const QJsonObject worksOn = p.value("worksOn").toObject();
                if (worksOn.value("Linux").toBool()) g.platform = "linux";
                else if (worksOn.value("Windows").toBool()) g.platform = "windows";
                else if (worksOn.value("Mac").toBool() || worksOn.value("macOS").toBool()) g.platform = "mac";
                else g.platform = "";

                games.push_back(std::move(g));
            }

            callback(util::Result<std::vector<GameInfo>>::success(std::move(games)));
        });
    });
}

void GOGClient::fetchGameDetails(const QString& gameId, GameCallback callback) {
    QTimer::singleShot(100, this, [this, gameId, callback = std::move(callback)]() mutable {
        GameInfo game;
        game.id = gameId;
        game.title = "Game " + gameId;
        game.platform = "windows";
        callback(util::Result<GameInfo>::success(game));
    });
}

void GOGClient::fetchGameDownloads(const QString& gameId, GameCallback callback) {
    QTimer::singleShot(100, this, [gameId, callback = std::move(callback)]() mutable {
        GameInfo game;
        game.id = gameId;
        game.title = "Downloads for " + gameId;
        GameInfo::DownloadLink link;
        link.url = "https://example.com/download.exe";
        link.platform = "windows";
        game.downloads.push_back(link);
        callback(util::Result<GameInfo>::success(game));
    });
}

void GOGClient::fetchAchievements(const QString& gameId, AchievementsCallback callback) {
    QTimer::singleShot(100, this, [gameId, callback = std::move(callback)]() mutable {
        std::vector<Achievement> achs;
        Achievement ach;
        ach.id = "1";
        ach.name = "Test Achievement";
        ach.unlocked = true;
        achs.push_back(ach);
        callback(util::Result<std::vector<Achievement>>::success(achs));
    });
}

void GOGClient::unlockAchievement(const QString& gameId, const QString& achievementId,
                                std::function<void(util::Result<void>)> callback) {
    QTimer::singleShot(100, this, [gameId, achievementId, callback = std::move(callback)]() mutable {
        callback(util::Result<void>::success());
    });
}

void GOGClient::listCloudSaves(const QString& gameId, CloudSavesCallback callback) {
    QTimer::singleShot(100, this, [gameId, callback = std::move(callback)]() mutable {
        std::vector<CloudSave> saves;
        CloudSave save;
        save.filename = "savegame.sav";
        save.size = 1024;
        saves.push_back(save);
        callback(util::Result<std::vector<CloudSave>>::success(saves));
    });
}

void GOGClient::uploadCloudSave(const QString& gameId, const QString& filePath,
                               std::function<void(util::Result<void>)> callback) {
    QTimer::singleShot(100, this, [callback = std::move(callback)]() mutable {
        callback(util::Result<void>::success());
    });
}

void GOGClient::downloadCloudSave(const QString& gameId, const QString& filename,
                                const QString& destPath,
                                std::function<void(util::Result<void>)> callback) {
    QTimer::singleShot(100, this, [callback = std::move(callback)]() mutable {
        callback(util::Result<void>::success());
    });
}

void GOGClient::searchStore(const QString& query,
                          std::function<void(util::Result<std::vector<StoreGameInfo>>)> callback) {
    QTimer::singleShot(100, this, [query, callback = std::move(callback)]() mutable {
        std::vector<StoreGameInfo> games;
        StoreGameInfo game;
        game.id = "store1";
        game.title = "Store Game: " + query;
        games.push_back(game);
        callback(util::Result<std::vector<StoreGameInfo>>::success(games));
    });
}

void GOGClient::fetchStoreGames(std::function<void(util::Result<std::vector<StoreGameInfo>>)> callback) {
    QTimer::singleShot(100, this, [callback = std::move(callback)]() mutable {
        std::vector<StoreGameInfo> games;
        StoreGameInfo game;
        game.id = "storegame";
        game.title = "Popular Store Game";
        games.push_back(game);
        callback(util::Result<std::vector<StoreGameInfo>>::success(games));
    });
}

void GOGClient::handleApiError(const net::HttpClient::Response& response, const QString& operation) {
    qDebug() << "API Error" << operation << response.statusCode;
}

} // namespace opengalaxy::api
