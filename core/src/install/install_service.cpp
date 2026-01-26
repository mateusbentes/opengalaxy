// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/install/install_service.h"
#include "opengalaxy/util/log.h"
#include "opengalaxy/net/http_client.h"
#include "opengalaxy/api/session.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QProcessEnvironment>
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

QString InstallService::buildAuthHeader() const
{
    if (session_) {
        auto* session = static_cast<api::Session*>(session_);
        if (session->isAuthenticated()) {
            return "Bearer " + session->tokens().accessToken;
        }
    }
    return QString();
}

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

    // Add authentication header for GOG API
    QString authHeader = buildAuthHeader();
    if (!authHeader.isEmpty()) {
        req.headers["Authorization"] = authHeader;
    }

    http->request(req, [this, gameId = game.id, selected, http](util::Result<net::HttpClient::Response> result) mutable {
        // Check if task still exists (might have been cancelled)
        QMutexLocker locker(&tasksMutex_);
        auto it = activeTasks_.find(gameId);
        if (it == activeTasks_.end()) {
            // Task was cancelled, cleanup and return
            http->deleteLater();
            return;
        }
        InstallTask* taskPtr = it->second.get();
        locker.unlock();

        if (!result.isOk()) {
            emit installFailed(taskPtr->gameId, result.errorMessage());
            if (taskPtr->completionCallback) {
                taskPtr->completionCallback(util::Result<QString>::error(result.errorMessage()));
            }
            QMutexLocker locker2(&tasksMutex_);
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
                          [this, gameId, installerPath](util::Result<net::HttpClient::Response> dlRes) mutable {
                              // Check if task still exists
                              QMutexLocker locker(&tasksMutex_);
                              auto it = activeTasks_.find(gameId);
                              if (it == activeTasks_.end()) {
                                  return; // Cancelled
                              }
                              InstallTask* taskPtr = it->second.get();
                              locker.unlock();

                              if (!dlRes.isOk()) {
                                  emit installFailed(taskPtr->gameId, dlRes.errorMessage());
                                  if (taskPtr->completionCallback) {
                                      taskPtr->completionCallback(util::Result<QString>::error(dlRes.errorMessage()));
                                  }
                                  QMutexLocker locker2(&tasksMutex_);
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

                              // Find Wine/Proton executable (prefer Proton-GE > Proton > Wine-Staging > Wine)
                              QString wineExe;
                              QString runnerName;

                              // Check for Proton-GE (best compatibility for games)
                              QStringList protonGePaths = {
                                  QDir::homePath() + "/.steam/steam/compatibilitytools.d/GE-Proton*/proton",
                                  QDir::homePath() + "/.local/share/Steam/compatibilitytools.d/GE-Proton*/proton",
                                  "/usr/share/steam/compatibilitytools.d/GE-Proton*/proton"
                              };

                              for (const QString& pattern : protonGePaths) {
                                  QDir dir(QFileInfo(pattern).path());
                                  if (dir.exists()) {
                                      QStringList entries = dir.entryList(QStringList() << "GE-Proton*", QDir::Dirs, QDir::Name | QDir::Reversed);
                                      if (!entries.isEmpty()) {
                                          QString protonPath = dir.absolutePath() + "/" + entries.first() + "/proton";
                                          if (QFile::exists(protonPath)) {
                                              wineExe = protonPath;
                                              runnerName = "Proton-GE";
                                              break;
                                          }
                                      }
                                  }
                              }

                              // Check for regular Proton
                              if (wineExe.isEmpty()) {
                                  QStringList protonPaths = {
                                      QDir::homePath() + "/.steam/steam/steamapps/common/Proton*/proton",
                                      QDir::homePath() + "/.local/share/Steam/steamapps/common/Proton*/proton"
                                  };

                                  for (const QString& pattern : protonPaths) {
                                      QDir dir(QFileInfo(pattern).path());
                                      if (dir.exists()) {
                                          QStringList entries = dir.entryList(QStringList() << "Proton*", QDir::Dirs, QDir::Name | QDir::Reversed);
                                          if (!entries.isEmpty()) {
                                              QString protonPath = dir.absolutePath() + "/" + entries.first() + "/proton";
                                              if (QFile::exists(protonPath)) {
                                                  wineExe = protonPath;
                                                  runnerName = "Proton";
                                                  break;
                                              }
                                          }
                                      }
                                  }
                              }

                              // Check for Wine variants (Wine-Staging, Wine-TKG, etc.)
                              if (wineExe.isEmpty()) {
                                  QStringList winePaths = {
                                      QStandardPaths::findExecutable("wine-staging"),
                                      QStandardPaths::findExecutable("wine-tkg"),
                                      "/usr/bin/wine-staging",
                                      "/usr/local/bin/wine-staging",
                                      QStandardPaths::findExecutable("wine"),
                                      "/usr/bin/wine",
                                      "/usr/local/bin/wine",
                                      "/opt/wine/bin/wine",
                                      "/opt/wine-staging/bin/wine"
                                  };

                                  for (const QString& path : winePaths) {
                                      if (!path.isEmpty() && QFile::exists(path)) {
                                          wineExe = path;
                                          if (path.contains("staging")) {
                                              runnerName = "Wine-Staging";
                                          } else if (path.contains("tkg")) {
                                              runnerName = "Wine-TKG";
                                          } else {
                                              runnerName = "Wine";
                                          }
                                          break;
                                      }
                                  }
                              }

                              if (wineExe.isEmpty()) {
                                  const QString err = "Wine/Proton not found. Please install Wine or Proton to run Windows installers.\n\n"
                                                     "Wine:\n"
                                                     "  Ubuntu/Debian: sudo apt install wine\n"
                                                     "  Fedora: sudo dnf install wine\n"
                                                     "  Arch: sudo pacman -S wine\n\n"
                                                     "Proton-GE (recommended for games):\n"
                                                     "  Download from: https://github.com/GloriousEggroll/proton-ge-custom/releases\n"
                                                     "  Extract to: ~/.steam/steam/compatibilitytools.d/";
                                  LOG_ERROR(err);
                                  emit installFailed(taskPtr->gameId, err);
                                  if (taskPtr->completionCallback) {
                                      taskPtr->completionCallback(util::Result<QString>::error(err));
                                  }
                                  QMutexLocker locker2(&tasksMutex_);
                                  activeTasks_.erase(taskPtr->gameId);
                                  return;
                              }

                              LOG_INFO(QString("Running installer with %1: %2 %3").arg(runnerName, wineExe, installerPath));

                              auto* proc = new QProcess(this);

                              // Set environment for Wine/Proton
                              QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

                              // Configure based on runner type
                              if (runnerName.contains("Proton")) {
                                  // Proton uses different environment variables
                                  env.insert("STEAM_COMPAT_DATA_PATH", installPath + "/.proton");
                                  env.insert("STEAM_COMPAT_CLIENT_INSTALL_PATH", QDir::homePath() + "/.steam/steam");

                                  // Proton command: proton run installer.exe
                                  proc->setProgram(wineExe);
                                  proc->setArguments({"run", installerPath});
                              } else {
                                  // Wine/Wine-Staging command: wine installer.exe
                                  env.insert("WINEPREFIX", installPath + "/.wine");
                                  env.insert("WINEDEBUG", "-all");  // Reduce Wine debug output

                                  proc->setProgram(wineExe);
                                  proc->setArguments({installerPath});
                              }

                              proc->setProcessEnvironment(env);
                              proc->setWorkingDirectory(installPath);

                              proc->start();

                              if (!proc->waitForStarted(5000)) {
                                  const QString err = QString("Failed to start Wine installer: %1").arg(proc->errorString());
                                  LOG_ERROR(err);
                                  emit installFailed(taskPtr->gameId, err);
                                  if (taskPtr->completionCallback) {
                                      taskPtr->completionCallback(util::Result<QString>::error(err));
                                  }
                                  proc->deleteLater();
                                  QMutexLocker locker2(&tasksMutex_);
                                  activeTasks_.erase(taskPtr->gameId);
                                  return;
                              }

                              LOG_INFO(QString("Wine installer started for: %1").arg(taskPtr->game.title));

                              connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                                      [this, gameId, installPath, proc](int exitCode, QProcess::ExitStatus status) mutable {
                                          proc->deleteLater();

                                          // Check if task still exists
                                          QMutexLocker locker(&tasksMutex_);
                                          auto it = activeTasks_.find(gameId);
                                          if (it == activeTasks_.end()) {
                                              return; // Cancelled
                                          }
                                          InstallTask* taskPtr = it->second.get();
                                          locker.unlock();

                                          if (status != QProcess::NormalExit || exitCode != 0) {
                                              const QString err = QString("Installer exited with code %1").arg(exitCode);
                                              emit installFailed(taskPtr->gameId, err);
                                              if (taskPtr->completionCallback) {
                                                  taskPtr->completionCallback(util::Result<QString>::error(err));
                                              }
                                              QMutexLocker locker2(&tasksMutex_);
                                              activeTasks_.erase(taskPtr->gameId);
                                              return;
                                          }

                                          emit installCompleted(taskPtr->gameId, installPath);
                                          if (taskPtr->completionCallback) {
                                              taskPtr->completionCallback(util::Result<QString>::success(installPath));
                                          }
                                          QMutexLocker locker2(&tasksMutex_);
                                          activeTasks_.erase(taskPtr->gameId);
                                      });
                          },
                          [this, gameId](qint64 received, qint64 total) {
                              // Check if task still exists
                              QMutexLocker locker(&tasksMutex_);
                              auto it = activeTasks_.find(gameId);
                              if (it == activeTasks_.end()) {
                                  return; // Cancelled
                              }
                              InstallTask* taskPtr = it->second.get();

                              InstallProgress p;
                              p.gameId = taskPtr->gameId;
                              p.downloadedBytes = received;
                              p.totalBytes = total;
                              p.status = "downloading";
                              if (total > 0) {
                                  p.percentage = static_cast<int>((received * 100) / total);
                              }
                              locker.unlock();

                              if (taskPtr->progressCallback) taskPtr->progressCallback(p);
                              emit installProgress(gameId, p.percentage);
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
    LOG_INFO(QString("Cancelling installation: %1").arg(gameId));

    QMutexLocker locker(&tasksMutex_);

    auto it = activeTasks_.find(gameId);
    if (it == activeTasks_.end()) {
        LOG_INFO(QString("Installation not found for cancellation: %1").arg(gameId));
        return;
    }

    // Abort any ongoing network request
    if (it->second->reply) {
        it->second->reply->abort();
        it->second->reply = nullptr;
    }

    // Remove the task - this will trigger cleanup
    // Note: The task's callbacks should check if task still exists before accessing it
    activeTasks_.erase(it);

    locker.unlock(); // Unlock before emitting signal

    LOG_INFO(QString("Installation cancelled: %1").arg(gameId));
    emit installCancelled(gameId);
}

bool InstallService::isInstalling(const QString& gameId) const
{
    QMutexLocker locker(&tasksMutex_);
    return activeTasks_.find(gameId) != activeTasks_.end();
}

} // namespace opengalaxy::install
