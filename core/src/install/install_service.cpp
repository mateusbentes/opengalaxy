// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/install/install_service.h"
#include "opengalaxy/util/log.h"
#include "opengalaxy/net/http_client.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QMutexLocker>
#include <map>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
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

void InstallService::installGame(const api::GameInfo& game,
                                 const QString& installDir,
                                 ProgressCallback progressCallback,
                                 CompletionCallback completionCallback)
{
    LOG_INFO(QString("Installing game: %1").arg(game.title));

    if (isInstalling(game.id)) {
        if (completionCallback) {
            completionCallback(util::Result<QString>::error("Game is already being installed"));
        }
        return;
    }

    if (game.downloads.empty()) {
        if (completionCallback) {
            completionCallback(util::Result<QString>::error("No downloads available for this game"));
        }
        return;
    }

    auto task = std::make_unique<InstallTask>();
    task->gameId = game.id;
    task->game = game;
    task->installDir = installDir;
    task->progressCallback = std::move(progressCallback);
    task->completionCallback = std::move(completionCallback);

    InstallTask* taskPtr = task.get();
    {
        QMutexLocker locker(&tasksMutex_);
        activeTasks_[game.id] = std::move(task);
    }

    emit installStarted(game.id);

    QDir().mkpath(installDir);

    // Pick first windows installer by default (we'll improve selection later)
    api::GameInfo::DownloadLink selected = game.downloads.front();
    for (const auto& dl : game.downloads) {
        if (dl.platform.toLower().contains("windows")) {
            selected = dl;
            break;
        }
    }

    // Validate download URL before proceeding
    if (selected.url.isEmpty()) {
        const QString err = "No valid download URL available for this game";
        emit installFailed(game.id, err);
        if (completionCallback) {
            completionCallback(util::Result<QString>::error(err));
        }
        QMutexLocker locker(&tasksMutex_);
        activeTasks_.erase(game.id);
        return;
    }

    // Fix protocol-relative URLs (GOG API may return //cdn.gog.com/...)
    QString downloadUrl = selected.url;
    if (downloadUrl.startsWith("//")) {
        downloadUrl = "https:" + downloadUrl;
    }

    // Step 1: Resolve real download URL (GOG returns JSON with { downlink, checksum })
    net::HttpClient* http = new net::HttpClient(this);

    net::HttpClient::Request req;
    req.url = downloadUrl;
    req.method = "GET";

    http->request(req, [this, taskPtr, selected, http](util::Result<net::HttpClient::Response> result) mutable {
        if (!result.isOk()) {
            emit installFailed(taskPtr->gameId, result.errorMessage());
            if (taskPtr->completionCallback) {
                taskPtr->completionCallback(util::Result<QString>::error(result.errorMessage()));
            }
            QMutexLocker locker(&tasksMutex_);
            activeTasks_.erase(taskPtr->gameId);
            return;
        }

        const QJsonObject obj = QJsonDocument::fromJson(result.value().body).object();
        QString downlink = obj.value("downlink").toString();
        if (downlink.isEmpty()) {
            const QString err = "Missing downlink in download response";
            emit installFailed(taskPtr->gameId, err);
            if (taskPtr->completionCallback) {
                taskPtr->completionCallback(util::Result<QString>::error(err));
            }
            QMutexLocker locker(&tasksMutex_);
            activeTasks_.erase(taskPtr->gameId);
            return;
        }

        // Fix protocol-relative URLs in downlink
        if (downlink.startsWith("//")) {
            downlink = "https:" + downlink;
        }

        // Step 2: Download installer to installDir
        const QString installerPath = taskPtr->installDir + "/" + taskPtr->game.title + ".exe";

        InstallProgress prog;
        prog.gameId = taskPtr->gameId;
        prog.status = "downloading";
        prog.currentFile = installerPath;
        if (taskPtr->progressCallback) taskPtr->progressCallback(prog);

        http->downloadFile(downlink, installerPath,
                          [this, taskPtr, installerPath](util::Result<net::HttpClient::Response> dlRes) mutable {
                              if (!dlRes.isOk()) {
                                  emit installFailed(taskPtr->gameId, dlRes.errorMessage());
                                  if (taskPtr->completionCallback) {
                                      taskPtr->completionCallback(util::Result<QString>::error(dlRes.errorMessage()));
                                  }
                                  QMutexLocker locker(&tasksMutex_);
                                  activeTasks_.erase(taskPtr->gameId);
                                  return;
                              }

                              // Step 3: Run installer with Wine (GUI)
                              InstallProgress prog;
                              prog.gameId = taskPtr->gameId;
                              prog.status = "installing";
                              prog.currentFile = installerPath;
                              prog.percentage = 100;
                              if (taskPtr->progressCallback) taskPtr->progressCallback(prog);

                              // Create an install folder and run installer.
                              const QString installPath = taskPtr->installDir + "/" + taskPtr->game.title;
                              QDir().mkpath(installPath);

                              auto* proc = new QProcess(this);
                              proc->setProgram("/usr/bin/wine");
                              proc->setArguments({installerPath});
                              proc->setWorkingDirectory(installPath);
                              proc->start();

                              connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                                      [this, taskPtr, installPath, proc](int exitCode, QProcess::ExitStatus status) mutable {
                                          proc->deleteLater();

                                          if (status != QProcess::NormalExit || exitCode != 0) {
                                              const QString err = QString("Installer exited with code %1").arg(exitCode);
                                              emit installFailed(taskPtr->gameId, err);
                                              if (taskPtr->completionCallback) {
                                                  taskPtr->completionCallback(util::Result<QString>::error(err));
                                              }
                                              QMutexLocker locker(&tasksMutex_);
                                              activeTasks_.erase(taskPtr->gameId);
                                              return;
                                          }

                                          emit installCompleted(taskPtr->gameId, installPath);
                                          if (taskPtr->completionCallback) {
                                              taskPtr->completionCallback(util::Result<QString>::success(installPath));
                                          }
                                          QMutexLocker locker(&tasksMutex_);
                                          activeTasks_.erase(taskPtr->gameId);
                                      });
                          },
                          [this, taskPtr](qint64 received, qint64 total) {
                              InstallProgress p;
                              p.gameId = taskPtr->gameId;
                              p.downloadedBytes = received;
                              p.totalBytes = total;
                              p.status = "downloading";
                              if (total > 0) {
                                  p.percentage = static_cast<int>((received * 100) / total);
                              }
                              if (taskPtr->progressCallback) taskPtr->progressCallback(p);
                              emit installProgress(taskPtr->gameId, p.percentage);
                          });
    });
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
