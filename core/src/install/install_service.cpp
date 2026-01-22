// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/install/install_service.h"
#include "opengalaxy/util/log.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QTimer>

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

    auto* task = new InstallTask();
    task->gameId = game.id;
    task->game = game;
    task->installDir = installDir;
    task->progressCallback = progressCallback;
    task->completionCallback = completionCallback;
    
    activeTasks_[game.id] = task;
    
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
    
    connect(timer, &QTimer::timeout, [this, task, timer, currentProgress]() mutable {
        currentProgress += 10;
        
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
            
            activeTasks_.remove(task->gameId);
            
            LOG_INFO(QString("Installation complete: %1").arg(installPath));
            emit installCompleted(task->gameId, installPath);
            
            if (task->completionCallback) {
                task->completionCallback(util::Result<QString>::success(installPath));
            }
            
            delete task;
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
    if (!activeTasks_.contains(gameId)) {
        return;
    }
    
    auto* task = activeTasks_[gameId];
    
    if (task->reply) {
        task->reply->abort();
    }
    
    activeTasks_.remove(gameId);
    delete task;
    
    LOG_INFO(QString("Installation cancelled: %1").arg(gameId));
    emit installCancelled(gameId);
}

bool InstallService::isInstalling(const QString& gameId) const
{
    return activeTasks_.contains(gameId);
}

} // namespace opengalaxy::install
