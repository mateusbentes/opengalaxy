// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/library/library_service.h"
#include "opengalaxy/util/log.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

namespace opengalaxy::library {

class LibraryDatabase {
public:
    LibraryDatabase()
    {
        QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(dbPath);

        db_ = QSqlDatabase::addDatabase("QSQLITE", "library");
        db_.setDatabaseName(dbPath + "/library.db");

        if (!db_.open()) {
            LOG_ERROR("Failed to open library database");
        }
    }

    ~LibraryDatabase() { db_.close(); }

    QSqlDatabase& database() { return db_; }

private:
    QSqlDatabase db_;
};

LibraryService::LibraryService(api::GOGClient* gogClient, QObject* parent)
    : QObject(parent)
    , gogClient_(gogClient)
    , db_(new LibraryDatabase())
{
    initDatabase();
}

LibraryService::~LibraryService()
{
    delete db_;
}

void LibraryService::fetchLibrary(bool forceRefresh, GamesCallback callback)
{
    if (!forceRefresh) {
        auto cached = loadCachedGames();
        if (!cached.empty()) {
            callback(util::Result<std::vector<api::GameInfo>>::success(cached));
            return;
        }
    }

    gogClient_->fetchLibrary([this, callback](util::Result<std::vector<api::GameInfo>> result) {
        if (result.isOk()) {
            cacheGames(result.value());
            emit libraryUpdated(static_cast<int>(result.value().size()));
        }
        callback(result);
    });
}

void LibraryService::getGame(const QString& gameId, GameCallback callback)
{
    QSqlQuery query(db_->database());
    query.prepare(
        "SELECT id, title, platform, coverUrl, isInstalled, installPath, version, preferredRunner, runnerExecutable, "
        "runnerArguments, extraEnvironment, slug, hiddenInLibrary, enableMangoHud, enableDxvkHudFps, enableGameMode, enableCloudSaves "
        "FROM games WHERE id = ?"
    );
    query.addBindValue(gameId);

    if (query.exec() && query.next()) {
        api::GameInfo game;
        game.id = query.value(0).toString();
        game.title = query.value(1).toString();
        game.platform = query.value(2).toString();
        game.coverUrl = query.value(3).toString();
        game.isInstalled = query.value(4).toBool();
        game.installPath = query.value(5).toString();
        game.version = query.value(6).toString();
        game.preferredRunner = query.value(7).toString();
        game.runnerExecutable = query.value(8).toString();
        game.runnerArguments = query.value(9).toString().split('\n', Qt::SkipEmptyParts);

        const QString envJson = query.value(10).toString();
        if (!envJson.isEmpty()) {
            const QJsonDocument doc = QJsonDocument::fromJson(envJson.toUtf8());
            if (doc.isObject()) {
                const auto obj = doc.object();
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    game.extraEnvironment.insert(it.key(), it.value().toString());
                }
            }
        }

        game.slug = query.value(11).toString();
        game.hiddenInLibrary = query.value(12).toInt() != 0;
        game.enableMangoHud = query.value(13).toInt() != 0;
        game.enableDxvkHudFps = query.value(14).toInt() != 0;
        game.enableGameMode = query.value(15).toInt() != 0;
        game.enableCloudSaves = query.value(16).toInt() != 0;

        callback(util::Result<api::GameInfo>::success(game));
    } else {
        callback(util::Result<api::GameInfo>::error("Game not found"));
    }
}

void LibraryService::updateGameInstallation(const QString& gameId, const QString& installPath, const QString& version)
{
    QSqlQuery query(db_->database());
    query.prepare("UPDATE games SET isInstalled = 1, installPath = ?, version = ? WHERE id = ?");
    query.addBindValue(installPath);
    query.addBindValue(version);
    query.addBindValue(gameId);

    if (query.exec()) {
        LOG_INFO(QString("Updated installation for game: %1").arg(gameId));
        emit gameUpdated(gameId);
    } else {
        LOG_ERROR(QString("Failed to update game installation: %1").arg(query.lastError().text()));
    }
}

void LibraryService::removeGameInstallation(const QString& gameId)
{
    QSqlQuery query(db_->database());
    query.prepare("UPDATE games SET isInstalled = 0, installPath = '', version = '' WHERE id = ?");
    query.addBindValue(gameId);

    if (query.exec()) {
        LOG_INFO(QString("Removed installation for game: %1").arg(gameId));
        emit gameUpdated(gameId);
    } else {
        LOG_ERROR(QString("Failed to remove game installation: %1").arg(query.lastError().text()));
    }
}

void LibraryService::updateGameProperties(const api::GameInfo& game)
{
    QSqlQuery query(db_->database());

    QJsonObject envObj;
    for (auto it = game.extraEnvironment.begin(); it != game.extraEnvironment.end(); ++it) {
        envObj.insert(it.key(), it.value());
    }
    const QString envJson = QString::fromUtf8(QJsonDocument(envObj).toJson(QJsonDocument::Compact));

    query.prepare(
        "UPDATE games SET preferredRunner = ?, runnerExecutable = ?, runnerArguments = ?, extraEnvironment = ?, "
        "hiddenInLibrary = ?, enableMangoHud = ?, enableDxvkHudFps = ?, enableGameMode = ?, enableCloudSaves = ? WHERE id = ?"
    );
    query.addBindValue(game.preferredRunner);
    query.addBindValue(game.runnerExecutable);
    query.addBindValue(game.runnerArguments.join("\n"));
    query.addBindValue(envJson);
    query.addBindValue(game.hiddenInLibrary ? 1 : 0);
    query.addBindValue(game.enableMangoHud ? 1 : 0);
    query.addBindValue(game.enableDxvkHudFps ? 1 : 0);
    query.addBindValue(game.enableGameMode ? 1 : 0);
    query.addBindValue(game.enableCloudSaves ? 1 : 0);
    query.addBindValue(game.id);

    if (query.exec()) {
        LOG_INFO(QString("Updated properties for game: %1").arg(game.id));
        emit gameUpdated(game.id);
    } else {
        LOG_ERROR(QString("Failed to update game properties: %1").arg(query.lastError().text()));
    }
}

std::vector<api::GameInfo> LibraryService::searchGames(const QString& query)
{
    std::vector<api::GameInfo> results;

    QSqlQuery sqlQuery(db_->database());
    sqlQuery.prepare("SELECT id, title, platform, coverUrl, isInstalled, installPath, version FROM games WHERE title LIKE ?");
    sqlQuery.addBindValue("%" + query + "%");

    if (sqlQuery.exec()) {
        while (sqlQuery.next()) {
            api::GameInfo game;
            game.id = sqlQuery.value(0).toString();
            game.title = sqlQuery.value(1).toString();
            game.platform = sqlQuery.value(2).toString();
            game.coverUrl = sqlQuery.value(3).toString();
            game.isInstalled = sqlQuery.value(4).toBool();
            game.installPath = sqlQuery.value(5).toString();
            game.version = sqlQuery.value(6).toString();
            results.push_back(game);
        }
    }

    return results;
}

std::vector<api::GameInfo> LibraryService::filterByPlatform(const QString& platform)
{
    std::vector<api::GameInfo> results;

    QSqlQuery query(db_->database());
    query.prepare("SELECT id, title, platform, coverUrl, isInstalled, installPath, version FROM games WHERE platform = ?");
    query.addBindValue(platform);

    if (query.exec()) {
        while (query.next()) {
            api::GameInfo game;
            game.id = query.value(0).toString();
            game.title = query.value(1).toString();
            game.platform = query.value(2).toString();
            game.coverUrl = query.value(3).toString();
            game.isInstalled = query.value(4).toBool();
            game.installPath = query.value(5).toString();
            game.version = query.value(6).toString();
            results.push_back(game);
        }
    }

    return results;
}

void LibraryService::initDatabase()
{
    QSqlQuery query(db_->database());

    query.exec(R"(
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
            slug TEXT,
            hiddenInLibrary INTEGER DEFAULT 0,
            enableMangoHud INTEGER DEFAULT 0,
            enableDxvkHudFps INTEGER DEFAULT 0,
            enableGameMode INTEGER DEFAULT 0,
            enableCloudSaves INTEGER DEFAULT 1
        )
    )");

    const auto tryAddColumn = [&](const QString& ddl) {
        if (!query.exec(ddl)) {
            const QString err = query.lastError().text().toLower();
            if (!err.contains("duplicate column") && !err.contains("already exists")) {
                LOG_ERROR(QString("Migration failed: %1 (%2)").arg(ddl, query.lastError().text()));
            }
        }
    };

    tryAddColumn("ALTER TABLE games ADD COLUMN preferredRunner TEXT");
    tryAddColumn("ALTER TABLE games ADD COLUMN runnerExecutable TEXT");
    tryAddColumn("ALTER TABLE games ADD COLUMN runnerArguments TEXT");
    tryAddColumn("ALTER TABLE games ADD COLUMN extraEnvironment TEXT");
    tryAddColumn("ALTER TABLE games ADD COLUMN slug TEXT");
    tryAddColumn("ALTER TABLE games ADD COLUMN hiddenInLibrary INTEGER DEFAULT 0");
    tryAddColumn("ALTER TABLE games ADD COLUMN enableMangoHud INTEGER DEFAULT 0");
    tryAddColumn("ALTER TABLE games ADD COLUMN enableDxvkHudFps INTEGER DEFAULT 0");
    tryAddColumn("ALTER TABLE games ADD COLUMN enableGameMode INTEGER DEFAULT 0");
    tryAddColumn("ALTER TABLE games ADD COLUMN enableCloudSaves INTEGER DEFAULT 1");
}

void LibraryService::cacheGames(const std::vector<api::GameInfo>& games)
{
    QSqlQuery query(db_->database());

    if (!db_->database().transaction()) {
        LOG_ERROR("Failed to start database transaction");
        return;
    }

    bool hasError = false;
    for (const auto& game : games) {
        query.prepare(R"(
            INSERT OR REPLACE INTO games (id, title, platform, coverUrl, backgroundUrl, developer, publisher, description, size, slug,
                                         preferredRunner, runnerExecutable, runnerArguments, extraEnvironment,
                                         hiddenInLibrary, enableMangoHud, enableDxvkHudFps, enableGameMode, enableCloudSaves)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        )");
        query.addBindValue(game.id);
        query.addBindValue(game.title);
        query.addBindValue(game.platform);
        query.addBindValue(game.coverUrl);
        query.addBindValue(game.backgroundUrl);
        query.addBindValue(game.developer);
        query.addBindValue(game.publisher);
        query.addBindValue(game.description);
        query.addBindValue(game.size);
        query.addBindValue(game.slug);

        // Keep user properties when refreshing from API (these fields are user-owned)
        query.addBindValue(game.preferredRunner);
        query.addBindValue(game.runnerExecutable);
        query.addBindValue(game.runnerArguments.join("\n"));

        // Add new toggle fields
        query.addBindValue(game.hiddenInLibrary ? 1 : 0);
        query.addBindValue(game.enableMangoHud ? 1 : 0);
        query.addBindValue(game.enableDxvkHudFps ? 1 : 0);
        query.addBindValue(game.enableGameMode ? 1 : 0);
        query.addBindValue(game.enableCloudSaves ? 1 : 0);

        QJsonObject envObj;
        for (auto it = game.extraEnvironment.begin(); it != game.extraEnvironment.end(); ++it) {
            envObj.insert(it.key(), it.value());
        }
        query.addBindValue(QString::fromUtf8(QJsonDocument(envObj).toJson(QJsonDocument::Compact)));

        if (!query.exec()) {
            LOG_ERROR(QString("Failed to cache game: %1").arg(query.lastError().text()));
            hasError = true;
            break;
        }
    }

    if (hasError) {
        db_->database().rollback();
        LOG_ERROR("Database transaction rolled back due to errors");
    } else {
        if (!db_->database().commit()) {
            LOG_ERROR("Failed to commit database transaction");
            db_->database().rollback();
        }
    }
}

std::vector<api::GameInfo> LibraryService::loadCachedGames()
{
    std::vector<api::GameInfo> games;

    QSqlQuery query(db_->database());
    if (query.exec("SELECT id, title, platform, coverUrl, backgroundUrl, developer, "
                   "publisher, description, isInstalled, installPath, version, size, "
                   "preferredRunner, runnerExecutable, runnerArguments, extraEnvironment, "
                   "slug, hiddenInLibrary, enableMangoHud, enableDxvkHudFps, "
                   "enableGameMode, enableCloudSaves FROM games")) {
        while (query.next()) {
            api::GameInfo game;
            game.id = query.value(0).toString();
            game.title = query.value(1).toString();
            game.platform = query.value(2).toString();
            game.coverUrl = query.value(3).toString();
            game.backgroundUrl = query.value(4).toString();
            game.developer = query.value(5).toString();
            game.publisher = query.value(6).toString();
            game.description = query.value(7).toString();
            game.isInstalled = query.value(8).toBool();
            game.installPath = query.value(9).toString();
            game.version = query.value(10).toString();
            game.size = query.value(11).toLongLong();
            game.preferredRunner = query.value(12).toString();
            game.runnerExecutable = query.value(13).toString();
            game.runnerArguments = query.value(14).toString().split('\n', Qt::SkipEmptyParts);

            const QString envJson = query.value(15).toString();
            game.slug = query.value(16).toString();
            game.hiddenInLibrary = query.value(17).toInt() != 0;
            game.enableMangoHud = query.value(18).toInt() != 0;
            game.enableDxvkHudFps = query.value(19).toInt() != 0;
            game.enableGameMode = query.value(20).toInt() != 0;
            game.enableCloudSaves = query.value(21).toInt() != 0;
            if (!envJson.isEmpty()) {
                const QJsonDocument doc = QJsonDocument::fromJson(envJson.toUtf8());
                if (doc.isObject()) {
                    const auto obj = doc.object();
                    for (auto it = obj.begin(); it != obj.end(); ++it) {
                        game.extraEnvironment.insert(it.key(), it.value().toString());
                    }
                }
            }

            games.push_back(game);
        }
    }

    return games;
}

} // namespace opengalaxy::library
