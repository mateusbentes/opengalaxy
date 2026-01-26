// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/api/gog_client.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QTimer>
#include <QUrl>
#include <QDebug>
#include <QRegularExpression>
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
        callback(
                util::Result<std::vector<GameInfo>>::error("Not authenticated"));
        return;
    }

    auto acc = std::make_shared<LibraryAccumulator>();
    acc->callback = std::move(callback);

    const auto fetchPage = [this, acc](auto &&  self) -> void {
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
                g.slug = p.value("slug").toString();

                // GOG API returns protocol-relative URLs (//images-X.gog.com/...)
                // or full HTTPS URLs. Both need size suffix for CDN.
                QString imageUrl = p.value("image").toString();
                if (!imageUrl.isEmpty()) {
                    // Add https: prefix if protocol-relative
                    if (imageUrl.startsWith("//")) {
                                imageUrl = "https:" + imageUrl;
                    }
                    // Add size suffix if not already present
                    if (!imageUrl.contains(".jpg")  &&  !imageUrl.contains(".png")  && 
                                !imageUrl.contains(".webp")  && 
                                !imageUrl.contains(".gif")) {
                                g.coverUrl = imageUrl + "_196.jpg";
                    } else {
                                g.coverUrl = imageUrl;
                    }
                }

                const QJsonObject worksOn = p.value("worksOn").toObject();
                if (worksOn.value("Linux").toBool()) g.platform = "linux";
                else if (worksOn.value("Windows").toBool()) g.platform = "windows";
                else if (worksOn.value("Mac").toBool()  ||  worksOn.value("macOS").toBool()) g.platform = "mac";
                else g.platform = "";

                acc->games.push_back(std::move(g));
                }

                if (acc->currentPage  >=  acc->totalPages) {
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
    const QString url =
        QString("https://api.gog.com/products/%1?locale=%2&expand=downloads")
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

        const QJsonObject obj =
                QJsonDocument::fromJson(result.value().body).object();
        GameInfo game;
        game.id = gameId;
        game.title = obj.value("title").toString();

        // Debug: Print the entire response to see structure
        qDebug() << "=== GOG API Response for game:" << gameId << "===";
        qDebug() << "Response keys:" << obj.keys();

        const QJsonObject downloads = obj.value("downloads").toObject();
        qDebug() << "Downloads object keys:" << downloads.keys();

        const QJsonArray installers = downloads.value("installers").toArray();
        qDebug() << "Found" << installers.size() << "installers";

        // If no installers, print the full downloads object for debugging
        if (installers.isEmpty()) {
                qDebug() << "Downloads object:" << QJsonDocument(downloads).toJson(QJsonDocument::Compact);
        }

        for (int i = 0; i < installers.size(); ++i) {
                const QJsonObject inst = installers[i].toObject();

                qDebug() << "  Installer" << i << "keys:" << inst.keys();
                qDebug() << "  OS:" << inst.value("os").toString();
                qDebug() << "  Language:" << inst.value("language").toString();

                // Get the files array - this contains the actual download links
                const QJsonArray files = inst.value("files").toArray();
                qDebug() << "  Files array size:" << files.size();

                if (files.isEmpty()) {
                qDebug() << "  WARNING: Installer has no files, skipping";
                qDebug() << "  Full installer object:" << QJsonDocument(inst).toJson(QJsonDocument::Compact);
                continue;
                }

                // Use the first file's downlink URL
                const QJsonObject firstFile = files[0].toObject();
                qDebug() << "  First file keys:" << firstFile.keys();

                QString downlinkUrl = firstFile.value("downlink").toString();

                if (downlinkUrl.isEmpty()) {
                qDebug() << "  WARNING: No downlink URL found";
                qDebug() << "  First file object:" << QJsonDocument(firstFile).toJson(QJsonDocument::Compact);
                continue;
                }

                GameInfo::DownloadLink link;
                link.platform = inst.value("os").toString();
                link.language = inst.value("language").toString();
                link.version = inst.value("version").toString();
                link.url = downlinkUrl;
                link.size = firstFile.value("size").toVariant().toLongLong();

                qDebug() << "  ✓ Valid installer found:";
                qDebug() << "    Platform:" << link.platform;
                qDebug() << "    Language:" << link.language;
                qDebug() << "    Version:" << link.version;
                qDebug() << "    Downlink:" << link.url;
                qDebug() << "    Size:" << link.size << "bytes";

                game.downloads.push_back(link);
        }

        if (game.downloads.empty()) {
                qDebug() << "WARNING: No valid downloads found for game" << gameId;
                callback(util::Result<GameInfo>::error("No downloads available for this game. The game may not have installers yet."));
                return;
        }

        callback(util::Result<GameInfo>::success(game));
    });
}

void GOGClient::listCloudSaves(const QString& gameId, CloudSavesCallback callback) {
    // NOTE: GOG does not provide a public REST API for cloud saves.
    // Cloud saves are handled by the official Galaxy client using the GOG Galaxy SDK (IStorage interface).
    // This would require either:
    // 1. Integrating the proprietary GOG Galaxy SDK (C++ library)
    // 2. Reverse-engineering the Galaxy client's cloud save protocol
    // 3. Waiting for GOG to provide a public API
    //
    // For now, this returns an empty list. Users should use the official Galaxy client
    // for cloud save functionality, or manually sync saves using external tools.
    callback(util::Result<std::vector<CloudSave>>::success({}));
}

void GOGClient::uploadCloudSave(const QString& gameId, const QString& filePath,
                               std::function<void(util::Result<void>)> callback) {
    // Cloud saves not implemented - see listCloudSaves() for details
    callback(util::Result<void>::error("Cloud saves require GOG Galaxy SDK integration"));
}

void GOGClient::downloadCloudSave(const QString& gameId, const QString& filename,
                                const QString& destPath,
                                std::function<void(util::Result<void>)> callback) {
    // Cloud saves not implemented - see listCloudSaves() for details
    callback(util::Result<void>::error("Cloud saves require GOG Galaxy SDK integration"));
}

void GOGClient::searchStore(
    const QString& query,
    std::function<void(util::Result<std::vector<StoreGameInfo>>)> callback)
{
    // Use the public search API endpoint (no auth required)
    // This is what the GOG website uses for public searches
    QString searchQuery = query.isEmpty() ? "*" : query;

    // Build search URL with proper parameters
    const QString url = QString("https://embed.gog.com/en/games/ajax/filtered?"
                                "mediaType=game&"
                                "page=1&"
                                "limit=30&"
                                "search=%1")
                            .arg(QString(QUrl::toPercentEncoding(searchQuery)));

    qDebug() << "Store search URL:" << url;

    net::HttpClient::Request req;
    req.url = url;
    req.method = "GET";
    req.headers["Accept"] = "application/json, text/javascript, */*; q=0.01";
    req.headers["Accept-Language"] = "en-US, en;q=0.9";
    req.headers["X-Requested-With"] = "XMLHttpRequest";
    req.headers["Referer"] = "https://www.gog.com/";
    req.headers["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36";

    httpClient_->request(req, [callback = std::move(callback)](util::Result<net::HttpClient::Response> result) mutable {
        if (!result.isOk()) {
                callback(util::Result<std::vector<StoreGameInfo>>::error(result.errorMessage()));
                return;
        }

        // Parse JSON response with error handling
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(result.value().body, &parseError);

        if (parseError.error  !=  QJsonParseError::NoError) {
                QString errorMsg = QString("Failed to parse store response: %1").arg(parseError.errorString());
                qDebug() << errorMsg;
                qDebug() << "Response body:" << result.value().body.left(500);
                callback(util::Result<std::vector<StoreGameInfo>>::error(errorMsg));
                return;
        }

        if (!doc.isObject()) {
                qDebug() << "Store response is not a JSON object";
                qDebug() << "Response body:" << result.value().body.left(500);
                callback(util::Result<std::vector<StoreGameInfo>>::error("Invalid JSON response from store API"));
                return;
        }

        const QJsonObject obj = doc.object();
        qDebug() << "Store API response keys:" << obj.keys();
        qDebug() << "Total results:" << obj.value("totalResults").toInt();
        qDebug() << "Total games found:" << obj.value("totalGamesFound").toInt();
        qDebug() << "Total pages:" << obj.value("totalPages").toInt();

        const QJsonArray items = obj.value("products").toArray();
        qDebug() << "Found" << items.size() << "products in response";

        // If products array is empty, log the full response for debugging
        if (items.isEmpty()) {
                qDebug() << "Empty products array. Full response:" << QString::fromUtf8(result.value().body);
        }

        std::vector<StoreGameInfo> games;
        games.reserve(static_cast<size_t>(items.size()));

        for (const auto& v : items) {
                const QJsonObject p = v.toObject();
                StoreGameInfo g;

                // Embed API uses numeric IDs
                g.id = QString::number(p.value("id").toVariant().toLongLong());
                g.title = p.value("title").toString();

                // Get cover image - embed API uses "image" field
                QString imageUrl = p.value("image").toString();

                if (!imageUrl.isEmpty()) {
                // Embed API returns protocol-relative URLs
                if (imageUrl.startsWith("//")) {
                    g.coverUrl = "https:" + imageUrl;
                } else if (!imageUrl.startsWith("http")) {
                    g.coverUrl = "https://" + imageUrl;
                } else {
                    g.coverUrl = imageUrl;
                }

                // Add size suffix if needed
                if (!g.coverUrl.contains(".jpg")  &&  !g.coverUrl.contains(".png")) {
                    g.coverUrl += "_196.jpg";
                }
                }

                // Get pricing information from embed API format
                if (p.contains("price")  &&  p.value("price").isObject()) {
                const auto priceObj = p.value("price").toObject();

                // Embed API has "amount" and "baseAmount"
                QString amount = priceObj.value("amount").toString();
                QString baseAmount = priceObj.value("baseAmount").toString();

                if (!amount.isEmpty()) {
                    g.price = amount;
                }

                if (!baseAmount.isEmpty()  &&  baseAmount  !=  amount) {
                    g.discountPrice = baseAmount;
                    // Calculate discount percentage
                    if (!amount.isEmpty()) {
                                double finalPrice = amount.remove(QRegularExpression("[^0-9.]")).toDouble();
                                double originalPrice = baseAmount.remove(QRegularExpression("[^0-9.]")).toDouble();
                                if (originalPrice > 0) {
                            g.discountPercent = qRound((1.0 - finalPrice / originalPrice) * 100);
                                }
                    }
                }
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
