// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/library/library_service.h"
#include "opengalaxy/util/log.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>

namespace opengalaxy::library {

class LibraryDatabase {
public:
    LibraryDatabase() {
        QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(dbPath);
        
        db_ = QSqlDatabase::addDatabase("QSQLITE", "library");
        db_.setDatabaseName(dbPath + "/library.db");
        
        if (!db_.open()) {
            LOG_ERROR("Failed to open library database");
        }
    }
    
    ~LibraryDatabase() {
        db_.close();
    }
    
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

LibraryService::~LibraryService() {
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
    query.prepare("SELECT id, title, platform, coverUrl, isInstalled, installPath, version FROM games WHERE id = ?");
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
            size INTEGER DEFAULT 0
        )
    )");
}

void LibraryService::cacheGames(const std::vector<api::GameInfo>& games)
{
    QSqlQuery query(db_->database());
    
    db_->database().transaction();
    
    for (const auto& game : games) {
        query.prepare(R"(
            INSERT OR REPLACE INTO games (id, title, platform, coverUrl, backgroundUrl, developer, publisher, description, size)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
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
        
        if (!query.exec()) {
            LOG_ERROR(QString("Failed to cache game: %1").arg(query.lastError().text()));
        }
    }
    
    db_->database().commit();
}

std::vector<api::GameInfo> LibraryService::loadCachedGames()
{
    std::vector<api::GameInfo> games;
    
    QSqlQuery query(db_->database());
    if (query.exec("SELECT id, title, platform, coverUrl, backgroundUrl, developer, publisher, description, isInstalled, installPath, version, size FROM games")) {
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
            games.push_back(game);
        }
    }
    
    return games;
}

} // namespace opengalaxy::library
