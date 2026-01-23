// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/api/gog_client.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>
#include <QUrl>
#include "opengalaxy/net/http_client.h"

namespace opengalaxy::api {

GOGClient::GOGClient(Session* session, QObject* parent)
    : QObject(parent)
    , session_(session)
{
    httpClient_ = new net::HttpClient(this);
}

GOGClient::~GOGClient() = default;
QString GOGClient::buildAuthHeader() const
{
    if (session_->isAuthenticated()) {
        return "Bearer " + session_->tokens().accessToken;
    }
    return QString();
}

namespace {
struct LibraryAccumulator {
    std::vector<GameInfo> games;
    int currentPage = 1;
    int totalPages = 1;
    GOGClient::GamesCallback callback;
};
}

void GOGClient::fetchLibrary(GamesCallback callback)
{
    if (!session_->isAuthenticated()) {
        callback(util::Result<std::vector<GameInfo>>::error("Not authenticated"));
        return;
    }

    auto acc = std::make_shared<LibraryAccumulator>();
    acc->callback = std::move(callback);

    const auto fetchPage = [this, acc](auto&& self) -> void {
        net::HttpClient::Request req;
        req.url = QString("https://embed.gog.com/account/getFilteredProducts?mediaType=1&page=%1").arg(acc->currentPage);
        req.method = "GET";
        req.headers["Authorization"] = buildAuthHeader();

        httpClient_->request(req, [this, acc, self](util::Result<net::HttpClient::Response> result) mutable {
            if (!result.isOk()) {
                acc->callback(util::Result<std::vector<GameInfo>>::error(result.errorMessage()));
                return;
            }

            const QJsonObject obj = QJsonDocument::fromJson(result.value().body).object();
            acc->totalPages = obj.value("totalPages").toInt(1);

            const QJsonArray products = obj.value("products").toArray();
            acc->games.reserve(acc->games.size() + static_cast<size_t>(products.size()));

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

                acc->games.push_back(std::move(g));
            }

            if (acc->currentPage >= acc->totalPages) {
                acc->callback(util::Result<std::vector<GameInfo>>::success(std::move(acc->games)));
                return;
            }

            acc->currentPage++;
            self(self);
        });
    };

    fetchPage(fetchPage);
}
void GOGClient::fetchGameDownloads(const QString& gameId, GameCallback callback)
{
    if (!session_->isAuthenticated()) {
        callback(util::Result<GameInfo>::error("Not authenticated"));
        return;
    }

    // Uses api.gog.com product details (MiniGalaxy-style) to get installer entries.
    const QString url = QString("https://api.gog.com/products/%1?locale=%2&expand=downloads")
                            .arg(QString(QUrl::toPercentEncoding(gameId)),
                                 QString(QUrl::toPercentEncoding(locale_)));

    net::HttpClient::Request req;
    req.url = url;
    req.method = "GET";
    req.headers["Authorization"] = buildAuthHeader();

    httpClient_->request(req, [gameId, callback = std::move(callback)](util::Result<net::HttpClient::Response> result) mutable {
        if (!result.isOk()) {
            callback(util::Result<GameInfo>::error(result.errorMessage()));
            return;
        }

        const QJsonObject obj = QJsonDocument::fromJson(result.value().body).object();
        GameInfo game;
        game.id = gameId;
        game.title = obj.value("title").toString();

        const QJsonObject downloads = obj.value("downloads").toObject();
        const QJsonArray installers = downloads.value("installers").toArray();

        for (const auto& v : installers) {
            const QJsonObject inst = v.toObject();
            GameInfo::DownloadLink link;
            link.platform = inst.value("os").toString();
            link.language = inst.value("language").toString();
            link.version = inst.value("version").toString();
            link.url = inst.value("link").toString();
            game.downloads.push_back(link);
        }

        callback(util::Result<GameInfo>::success(game));
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

void GOGClient::searchStore(
    const QString& query,
    std::function<void(util::Result<std::vector<StoreGameInfo>>)> callback)
{
    // Public endpoint used by many clients: api.gog.com product search
    // Note: Pricing may not be present without additional locale/country context.
    const QString url = QString("https://api.gog.com/products?search=%1&limit=30")
                            .arg(QUrl::toPercentEncoding(query));

    net::HttpClient::Request req;
    req.url = url;
    req.method = "GET";

    httpClient_->request(req, [callback = std::move(callback)](util::Result<net::HttpClient::Response> result) mutable {
        if (!result.isOk()) {
            callback(util::Result<std::vector<StoreGameInfo>>::error(result.errorMessage()));
            return;
        }

        const QJsonObject obj = QJsonDocument::fromJson(result.value().body).object();
        const QJsonArray items = obj.value("products").toArray();

        std::vector<StoreGameInfo> games;
        games.reserve(static_cast<size_t>(items.size()));

        for (const auto& v : items) {
            const QJsonObject p = v.toObject();
            StoreGameInfo g;
            g.id = QString::number(p.value("id").toVariant().toLongLong());
            g.title = p.value("title").toString();
            g.coverUrl = p.value("image").toString();

            // pricing often absent; keep empty if not provided
            if (p.contains("price") && p.value("price").isObject()) {
                const auto priceObj = p.value("price").toObject();
                g.price = priceObj.value("finalAmount").toString();
                g.discountPrice = priceObj.value("baseAmount").toString();
                g.discountPercent = priceObj.value("discountPercentage").toInt(0);
            }

            games.push_back(std::move(g));
        }

        callback(util::Result<std::vector<StoreGameInfo>>::success(std::move(games)));
    });
}

void GOGClient::fetchStoreGames(std::function<void(util::Result<std::vector<StoreGameInfo>>)> callback)
{
    // Store list without search is not implemented; UI uses search.
    callback(util::Result<std::vector<StoreGameInfo>>::success({}));
}

void GOGClient::handleApiError(const net::HttpClient::Response& response, const QString& operation) {
    qDebug() << "API Error" << operation << response.statusCode;
}

} // namespace opengalaxy::api
