// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/install/install_service.h"
#include "opengalaxy/util/log.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QMutexLocker>
#include <map>

namespace opengalaxy::install {

struct InstallService::InstallTask {
    QString gameId;
    api::GameInfo game;
    QString installDir;
    ProgressCallback progressCallback;
    CompletionCallback completionCallback;
    QNetworkReply* reply = nullptr;
    qint64 downloadedBytes = 0;
    qint64 totalBytes = 0;
};

InstallService::InstallService(QObject* parent)
    : QObject(parent)
{
}

InstallService::~InstallService() = default;

void InstallService::installGame(const api::GameInfo& game, const QString& installDir,
                                 ProgressCallback progressCallback, CompletionCallback completionCallback)
{
    LOG_INFO(QString("Installing game: %1").arg(game.title));
    
    if (isInstalling(game.id)) {
        completionCallback(util::Result<QString>::error("Game is already being installed"));
        return;
    }

    auto task = std::make_unique<InstallTask>();
    task->gameId = game.id;
    task->game = game;
    task->installDir = installDir;
    task->progressCallback = progressCallback;
    task->completionCallback = completionCallback;
    
    InstallTask* taskPtr = task.get();
    
    {
        QMutexLocker locker(&tasksMutex_);
        activeTasks_[game.id] = std::move(task);
    }
    
    emit installStarted(game.id);
    
    // Create install directory
    QDir().mkpath(installDir);
    
    // Simulate download and installation
    InstallProgress progress;
    progress.gameId = game.id;
    progress.status = "downloading";
    progress.percentage = 0;
    
    if (progressCallback) {
        progressCallback(progress);
    }
    
    // Simulate progress
    QTimer* timer = new QTimer(this);
    int currentProgress = 0;
    QString gameId = game.id;
    
    connect(timer, &QTimer::timeout, [this, gameId, timer, currentProgress]() mutable {
        currentProgress += 10;
        
        InstallTask* task = nullptr;
        {
            QMutexLocker locker(&tasksMutex_);
            auto it = activeTasks_.find(gameId);
            if (it == activeTasks_.end()) {
                timer->stop();
                timer->deleteLater();
                return;
            }
            task = it->second.get();
        }
        
        InstallProgress progress;
        progress.gameId = task->gameId;
        progress.percentage = currentProgress;
        
        if (currentProgress < 50) {
            progress.status = "downloading";
        } else if (currentProgress < 90) {
            progress.status = "extracting";
        } else {
            progress.status = "verifying";
        }
        
        if (task->progressCallback) {
            task->progressCallback(progress);
        }
        
        emit installProgress(task->gameId, currentProgress);
        
        if (currentProgress >= 100) {
            timer->stop();
            timer->deleteLater();
            
            QString installPath = task->installDir + "/" + task->game.title;
            QDir().mkpath(installPath);
            
            CompletionCallback completionCallback = task->completionCallback;
            QString taskGameId = task->gameId;
            
            {
                QMutexLocker locker(&tasksMutex_);
                activeTasks_.erase(taskGameId);
            }
            
            LOG_INFO(QString("Installation complete: %1").arg(installPath));
            emit installCompleted(taskGameId, installPath);
            
            if (completionCallback) {
                completionCallback(util::Result<QString>::success(installPath));
            }
        }
    });
    
    timer->start(500); // Update every 500ms
}

void InstallService::uninstallGame(const QString& gameId, const QString& installPath,
                                   std::function<void(util::Result<void>)> callback)
{
    LOG_INFO(QString("Uninstalling game: %1").arg(gameId));
    
    QDir dir(installPath);
    if (!dir.exists()) {
        callback(util::Result<void>::error("Install path does not exist"));
        return;
    }
    
    if (dir.removeRecursively()) {
        LOG_INFO(QString("Uninstall complete: %1").arg(gameId));
        callback(util::Result<void>::success());
    } else {
        callback(util::Result<void>::error("Failed to remove installation directory"));
    }
}

void InstallService::cancelInstallation(const QString& gameId)
{
    QMutexLocker locker(&tasksMutex_);
    
    auto it = activeTasks_.find(gameId);
    if (it == activeTasks_.end()) {
        return;
    }
    
    if (it->second->reply) {
        it->second->reply->abort();
    }
    
    activeTasks_.erase(it);
    
    LOG_INFO(QString("Installation cancelled: %1").arg(gameId));
    emit installCancelled(gameId);
}

bool InstallService::isInstalling(const QString& gameId) const
{
    QMutexLocker locker(&tasksMutex_);
    return activeTasks_.find(gameId) != activeTasks_.end();
}

} // namespace opengalaxy::install
