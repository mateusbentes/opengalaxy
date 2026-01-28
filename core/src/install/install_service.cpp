// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/install/install_service.h"
#include "opengalaxy/install/installer_detector.h"
#include "opengalaxy/api/session.h"
#include "opengalaxy/net/http_client.h"
#include "opengalaxy/util/dos_detector.h"
#include "opengalaxy/util/log.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QSysInfo>
#include <QTimer>
#include <map>
namespace opengalaxy::install {

struct InstallService::InstallTask {
    QString gameId;
    api::GameInfo game;
    QString installDir;
    ProgressCallback progressCallback;
    CompletionCallback completionCallback;
    QNetworkReply *reply = nullptr;
    qint64 downloadedBytes = 0;
    qint64 totalBytes = 0;
};

InstallService::InstallService(QObject *parent) : QObject(parent) {}

InstallService::~InstallService() = default;

QString InstallService::buildAuthHeader() const {
    if (session_) {
        auto *session = static_cast<api::Session *>(session_);
        if (session->isAuthenticated()) {
            return "Bearer " + session->tokens().accessToken;
        }
    }
    return QString();
}

void InstallService::installGame(const api::GameInfo &game, const QString &installDir,
                                 ProgressCallback progressCallback,
                                 CompletionCallback completionCallback) {
    LOG_INFO(QString("Installing game: %1").arg(game.title));

    if (isInstalling(game.id)) {
        if (completionCallback) {
            completionCallback(util::Result<QString>::error("Game is already being installed"));
        }
        return;
    }

    if (game.downloads.empty()) {
        if (completionCallback) {
            completionCallback(
                util::Result<QString>::error("No downloads available for this game"));
        }
        return;
    }

    auto task = std::make_unique<InstallTask>();
    task->gameId = game.id;
    task->game = game;
    task->installDir = installDir;
    task->progressCallback = std::move(progressCallback);
    task->completionCallback = std::move(completionCallback);

    InstallTask *taskPtr = task.get();
    {
        QMutexLocker locker(&tasksMutex_);
        activeTasks_[game.id] = std::move(task);
    }

    emit installStarted(game.id);

    QDir().mkpath(installDir);

    // Detect current OS and select appropriate download
    QString currentOS;
#ifdef Q_OS_WIN
    currentOS = "windows";
#elif defined(Q_OS_LINUX)
    currentOS = "linux";
#elif defined(Q_OS_MACOS)
    currentOS = "mac";
#else
    currentOS = "windows"; // Default fallback
#endif

    LOG_INFO(QString("Detected OS: %1").arg(currentOS));

    // Select download for current OS
    api::GameInfo::DownloadLink selected;
    bool foundForCurrentOS = false;

    // First, try to find exact match for current OS
    for (const auto &dl : game.downloads) {
        const QString platform = dl.platform.toLower();
        if (platform.contains(currentOS)) {
            selected = dl;
            foundForCurrentOS = true;
            LOG_INFO(QString("Found %1 version for download").arg(currentOS));
            break;
        }
    }

    // If no match for current OS, use first available
    if (!foundForCurrentOS) {
        if (!game.downloads.empty()) {
            selected = game.downloads.front();
            LOG_WARNING(
                QString("No %1 version found, using: %2").arg(currentOS, selected.platform));
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
    net::HttpClient *http = new net::HttpClient(this);

    net::HttpClient::Request req;
    req.url = downloadUrl;
    req.method = "GET";

    // Add authentication header for GOG API
    QString authHeader = buildAuthHeader();
    if (!authHeader.isEmpty()) {
        req.headers["Authorization"] = authHeader;
    }

    http->request(req, [this, gameId = game.id, selected,
                        http](util::Result<net::HttpClient::Response> result) mutable {
        // Check if task still exists (might have been cancelled)
        QMutexLocker locker(&tasksMutex_);
        auto it = activeTasks_.find(gameId);
        if (it == activeTasks_.end()) {
            // Task was cancelled, cleanup and return
            http->deleteLater();
            return;
        }
        InstallTask *taskPtr = it->second.get();
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
        // Extract filename from the ACTUAL download URL (downlink), not the API URL
        QString installerFilename = taskPtr->game.title + ".exe"; // Default fallback

        LOG_INFO(QString("Actual download URL: %1").arg(downlink));

        // Extract filename from downlink URL (before query parameters)
        QString urlPath = downlink;

        // Remove query parameters first
        const int questionMark = urlPath.indexOf('?');
        if (questionMark > 0) {
            urlPath = urlPath.left(questionMark);
        }

        // Extract filename from path
        const int lastSlash = urlPath.lastIndexOf('/');
        if (lastSlash >= 0) {
            const QString urlFilename = urlPath.mid(lastSlash + 1);
            LOG_INFO(QString("Extracted filename from downlink: %1").arg(urlFilename));
            if (!urlFilename.isEmpty() && urlFilename.contains('.')) {
                installerFilename = urlFilename;
                LOG_INFO(QString("Using downlink filename: %1").arg(installerFilename));
            }
        }

        LOG_INFO(QString("Final installer filename: %1").arg(installerFilename));
        const QString installerPath = taskPtr->installDir + "/" + installerFilename;

        InstallProgress prog;
        prog.gameId = taskPtr->gameId;
        prog.status = "downloading";
        prog.currentFile = installerPath;
        if (taskPtr->progressCallback) taskPtr->progressCallback(prog);

        http->downloadFile(
            downlink, installerPath,
            [this, gameId, installerPath](util::Result<net::HttpClient::Response> dlRes) mutable {
                // Check if task still exists
                QMutexLocker locker(&tasksMutex_);
                auto it = activeTasks_.find(gameId);
                if (it == activeTasks_.end()) {
                    return; // Cancelled
                }
                InstallTask *taskPtr = it->second.get();
                locker.unlock();

                if (!dlRes.isOk()) {
                    emit installFailed(taskPtr->gameId, dlRes.errorMessage());
                    if (taskPtr->completionCallback) {
                        taskPtr->completionCallback(
                            util::Result<QString>::error(dlRes.errorMessage()));
                    }
                    QMutexLocker locker2(&tasksMutex_);
                    activeTasks_.erase(taskPtr->gameId);
                    return;
                }

                // Step 3: Run installer with appropriate runner (Wine/Proton for Windows, DOSBox
                // for DOS)
                InstallProgress prog;
                prog.gameId = taskPtr->gameId;
                prog.status = "installing";
                prog.currentFile = installerPath;
                prog.percentage = 100;
                if (taskPtr->progressCallback) taskPtr->progressCallback(prog);

                // Create an install folder and run installer.
                const QString installPath = taskPtr->installDir + "/" + taskPtr->game.title;
                QDir().mkpath(installPath);

                // Check if this is a pure DOS game (not Win32)
                // First check the executable - this is more reliable than metadata
                bool isDOSGame = false;

                // Check file extension
                const QString fileExt = QFileInfo(installerPath).suffix().toLower();
                const bool isShellScript = (fileExt == "sh" || fileExt == "bash");
                const bool isWindowsExe = (fileExt == "exe");
                const bool isMacPkg = (fileExt == "pkg" || fileExt == "dmg");

                // Universal archive formats (work on all platforms)
                const bool isUniversalArchive = (fileExt == "zip" || fileExt == "tar" ||
                                                 fileExt == "gz" || fileExt == "7z");

                // Platform-specific archives
                const bool isPlatformArchive = (fileExt == "rar" || fileExt == "bz2");

                LOG_INFO(QString("Installer file: %1 (extension: %2, type: %3)")
                             .arg(QFileInfo(installerPath).fileName(), fileExt,
                                  isShellScript ? "shell-script"
                                  : isWindowsExe ? "windows-exe"
                                  : isMacPkg ? "macos-pkg"
                                  : isUniversalArchive ? "universal-archive"
                                  : isPlatformArchive ? "platform-archive"
                                  : "unknown"));

                // Check if it's a legacy DOS game (DOS game packaged as Windows executable)
                bool isLegacyDOSGame = false;
                if (isWindowsExe && !isDOSGame) {
                    isLegacyDOSGame = install::InstallerDetector::isLegacyDOSGame(
                        taskPtr->game.title, taskPtr->game.genres);
                    if (isLegacyDOSGame) {
                        LOG_INFO(QString("Legacy DOS game detected (packaged as Windows): %1")
                                     .arg(taskPtr->game.title));
                    }
                }

                // If it's a macOS package, install it
                if (isMacPkg) {
                    LOG_INFO(QString("macOS package detected: %1").arg(installerPath));

#ifdef Q_OS_MACOS
                    auto *proc = new QProcess(this);

                    if (fileExt == "pkg") {
                        // Install .pkg using installer command
                        proc->setProgram("sudo");
                        proc->setArguments({"installer", "-pkg", installerPath, "-target",
                                           QDir::homePath()});
                    } else if (fileExt == "dmg") {
                        // Mount .dmg and copy contents
                        proc->setProgram("hdiutil");
                        proc->setArguments({"attach", installerPath, "-mountpoint", installPath});
                    }

                    proc->start();

                    if (!proc->waitForFinished(120000)) {
                        const QString err = QString("Failed to install macOS package: %1")
                                                .arg(proc->errorString());
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

                    LOG_INFO(QString("macOS package installed successfully: %1").arg(installerPath));

                    if (fileExt == "dmg") {
                        // Unmount DMG after installation
                        auto *unmountProc = new QProcess(this);
                        unmountProc->setProgram("hdiutil");
                        unmountProc->setArguments({"detach", installPath});
                        unmountProc->start();
                        unmountProc->waitForFinished(5000);
                        unmountProc->deleteLater();
                    }

                    proc->deleteLater();

                    emit installCompleted(taskPtr->gameId, taskPtr->installDir, "");
                    if (taskPtr->completionCallback) {
                        taskPtr->completionCallback(
                            util::Result<QString>::success(taskPtr->installDir));
                    }

                    QMutexLocker locker2(&tasksMutex_);
                    activeTasks_.erase(taskPtr->gameId);
                    return;
#else
                    const QString err = "macOS packages (.pkg, .dmg) are only supported on macOS";
                    LOG_ERROR(err);
                    emit installFailed(taskPtr->gameId, err);
                    if (taskPtr->completionCallback) {
                        taskPtr->completionCallback(util::Result<QString>::error(err));
                    }
                    QMutexLocker locker2(&tasksMutex_);
                    activeTasks_.erase(taskPtr->gameId);
                    return;
#endif
                }

                // If it's a universal archive, extract it (works on all platforms)
                if (isUniversalArchive) {
                    LOG_INFO(QString("Universal archive detected, extracting: %1").arg(installerPath));

                    auto *proc = new QProcess(this);

                    if (fileExt == "zip") {
                        proc->setProgram("unzip");
                        proc->setArguments({"-q", installerPath, "-d", installPath});
                    } else if (fileExt == "tar") {
                        proc->setProgram("tar");
                        proc->setArguments({"xf", installerPath, "-C", installPath});
                    } else if (fileExt == "gz") {
                        proc->setProgram("tar");
                        proc->setArguments({"xzf", installerPath, "-C", installPath});
                    } else if (fileExt == "7z") {
                        proc->setProgram("7z");
                        proc->setArguments({"x", installerPath, QString("-o%1").arg(installPath)});
                    }

                    proc->start();

                    if (!proc->waitForFinished(60000)) {
                        const QString err = QString("Failed to extract archive: %1")
                                                .arg(proc->errorString());
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

                    LOG_INFO(QString("Archive extracted successfully: %1").arg(installerPath));
                    proc->deleteLater();

                    emit installCompleted(taskPtr->gameId, taskPtr->installDir, "");
                    if (taskPtr->completionCallback) {
                        taskPtr->completionCallback(
                            util::Result<QString>::success(taskPtr->installDir));
                    }

                    QMutexLocker locker2(&tasksMutex_);
                    activeTasks_.erase(taskPtr->gameId);
                    return;
                }

                // Platform-specific archives (not recommended)
                if (isPlatformArchive) {
                    LOG_WARNING(QString("Platform-specific archive detected: %1 (not recommended)")
                                    .arg(fileExt));
                    // Could add support here if needed
                }

                if (QFile::exists(installerPath)) {
                    isDOSGame = util::DOSDetector::isDOSExecutable(installerPath);
                    LOG_INFO(
                        QString("Executable check for DOS: %1").arg(isDOSGame ? "true" : "false"));
                }

                // If it's a shell script or Windows EXE, don't use metadata for DOS detection
                // Shell scripts are native Linux installers, not DOS
                if (isShellScript) {
                    LOG_INFO("Shell script detected - running as native Linux installer");
                    isDOSGame = false;

                    // Run shell script directly
                    auto *proc = new QProcess(this);
                    proc->setProgram("/bin/bash");
                    proc->setArguments({installerPath});
                    proc->setWorkingDirectory(installPath);

                    // Make script executable
                    QFile scriptFile(installerPath);
                    scriptFile.setPermissions(scriptFile.permissions() | QFile::ExeOwner |
                                              QFile::ExeGroup | QFile::ExeOther);

                    proc->start();

                    if (!proc->waitForStarted(5000)) {
                        const QString err = QString("Failed to start shell script installer: %1")
                                                .arg(proc->errorString());
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

                    LOG_INFO(QString("Shell script installer started: %1").arg(installerPath));

                    // Wait for shell script to finish
                    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                            [this, taskPtr, proc](int exitCode, QProcess::ExitStatus exitStatus) {
                                if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
                                    LOG_INFO(QString("Shell script installer completed: %1")
                                                 .arg(taskPtr->game.title));
                                    emit installCompleted(taskPtr->gameId, taskPtr->installDir, "");
                                    if (taskPtr->completionCallback) {
                                        taskPtr->completionCallback(
                                            util::Result<QString>::success(taskPtr->installDir));
                                    }
                                } else {
                                    const QString err =
                                        QString("Shell script installer failed with exit code: %1")
                                            .arg(exitCode);
                                    LOG_ERROR(err);
                                    emit installFailed(taskPtr->gameId, err);
                                    if (taskPtr->completionCallback) {
                                        taskPtr->completionCallback(
                                            util::Result<QString>::error(err));
                                    }
                                }
                                proc->deleteLater();
                                QMutexLocker locker(&tasksMutex_);
                                activeTasks_.erase(taskPtr->gameId);
                            });

                    return;
                } else if (isWindowsExe && !isDOSGame) {
                    // Windows EXE that's not pure DOS - check if it's a legacy DOS game
                    // Some old DOS games (like The Elder Scrolls: Arena) are packaged as Windows
                    // executables by GOG. We detect these using metadata (genres, title).
                    isDOSGame = util::DOSDetector::isDOSGameByMetadata(taskPtr->game.title,
                                                                       taskPtr->game.genres);
                    if (isDOSGame) {
                        LOG_INFO(QString("Legacy DOS game detected (packaged as Windows): %1")
                                     .arg(taskPtr->game.title));
                    }
                    LOG_INFO(
                        QString("Metadata check for DOS: %1").arg(isDOSGame ? "true" : "false"));
                }

                if (isDOSGame) {
                    // DOS game - but if installer is Windows EXE, use Wine to install first
                    LOG_INFO(QString("Detected DOS game: %1").arg(taskPtr->game.title));

                    if (isWindowsExe) {
                        // Legacy DOS game packaged as Windows installer
                        // Use Wine/Proton to run the installer, then DOSBox for the game
                        LOG_INFO("Legacy DOS game with Windows installer - using Wine/Proton to "
                                 "install");

                        // Find Wine/Proton (same logic as Windows games)
                        QString wineExe;
                        QString runnerName;

                        // Check for Proton-GE
                        QStringList protonGePaths = {
                            QDir::homePath() + "/.steam/steam/compatibilitytools.d/GE-Proton*/proton",
                            QDir::homePath() +
                                "/.local/share/Steam/compatibilitytools.d/GE-Proton*/proton",
                            "/usr/share/steam/compatibilitytools.d/GE-Proton*/proton"};

                        for (const QString &pattern : protonGePaths) {
                            QDir dir(QFileInfo(pattern).path());
                            if (dir.exists()) {
                                QStringList entries = dir.entryList(
                                    QStringList() << "GE-Proton*", QDir::Dirs,
                                    QDir::Name | QDir::Reversed);
                                if (!entries.isEmpty()) {
                                    QString protonPath =
                                        dir.absolutePath() + "/" + entries.first() + "/proton";
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
                                QDir::homePath() +
                                    "/.local/share/Steam/steamapps/common/Proton*/proton"};

                            for (const QString &pattern : protonPaths) {
                                QDir dir(QFileInfo(pattern).path());
                                if (dir.exists()) {
                                    QStringList entries = dir.entryList(
                                        QStringList() << "Proton*", QDir::Dirs,
                                        QDir::Name | QDir::Reversed);
                                    if (!entries.isEmpty()) {
                                        QString protonPath =
                                            dir.absolutePath() + "/" + entries.first() + "/proton";
                                        if (QFile::exists(protonPath)) {
                                            wineExe = protonPath;
                                            runnerName = "Proton";
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        // Check for Wine
                        if (wineExe.isEmpty()) {
                            QStringList winePaths = {QStandardPaths::findExecutable("wine-staging"),
                                                     QStandardPaths::findExecutable("wine-tkg"),
                                                     "/usr/bin/wine-staging",
                                                     "/usr/local/bin/wine-staging",
                                                     QStandardPaths::findExecutable("wine"),
                                                     "/usr/bin/wine",
                                                     "/usr/local/bin/wine",
                                                     "/opt/wine/bin/wine",
                                                     "/opt/wine-staging/bin/wine"};

                            for (const QString &path : winePaths) {
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
                            const QString err =
                                "Wine/Proton not found. Please install Wine or Proton to install "
                                "DOS games.\n\n"
                                "Wine:\n"
                                "  Ubuntu/Debian: sudo apt install wine\n"
                                "  Fedora: sudo dnf install wine\n"
                                "  Arch: sudo pacman -S wine\n\n"
                                "Proton-GE (recommended):\n"
                                "  Download from: "
                                "https://github.com/GloriousEggroll/proton-ge-custom/releases\n"
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

                        LOG_INFO(QString("Running DOS game installer with %1: %2 %3")
                                     .arg(runnerName, wineExe, installerPath));

                        auto *proc = new QProcess(this);

                        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

                        if (runnerName.contains("Proton")) {
                            env.insert("STEAM_COMPAT_DATA_PATH", installPath + "/.proton");
                            env.insert("STEAM_COMPAT_CLIENT_INSTALL_PATH",
                                       QDir::homePath() + "/.steam/steam");

                            proc->setProgram(wineExe);
                            proc->setArguments({"run", installerPath});
                        } else {
                            env.insert("WINEPREFIX", installPath + "/.wine");
                            env.insert("WINEDEBUG", "-all");

                            proc->setProgram(wineExe);
                            proc->setArguments({installerPath});
                        }

                        proc->setProcessEnvironment(env);
                        proc->setWorkingDirectory(installPath);

                        proc->start();

                        if (!proc->waitForStarted(5000)) {
                            const QString err = QString("Failed to start Wine installer: %1")
                                                    .arg(proc->errorString());
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

                        LOG_INFO(QString("Wine installer started for DOS game: %1")
                                     .arg(taskPtr->game.title));

                        connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                                [this, taskPtr, proc](int exitCode, QProcess::ExitStatus exitStatus) {
                                    // Windows installers often exit with codes 0-3 even on success
                                    // (0=success, 1=reboot needed, 2=user cancelled, 3=other)
                                    // For DOS games, we consider 0-3 as acceptable
                                    bool isSuccess = (exitStatus == QProcess::NormalExit && exitCode <= 3);
                                    
                                    if (isSuccess) {
                                        LOG_INFO(QString("DOS game installed successfully: %1 (exit code: %2)")
                                                     .arg(taskPtr->game.title).arg(exitCode));

                                        // Auto-set preferred runner to DOSBox for DOS games
                                        taskPtr->game.preferredRunner = "DOSBox";
                                        LOG_INFO(QString("Auto-set preferred runner to DOSBox for: %1")
                                                     .arg(taskPtr->game.title));

                                        {
                                            QMutexLocker locker(&tasksMutex_);
                                            detectedRunners_[taskPtr->gameId] = "DOSBox";
                                        }

                                        emit installCompleted(taskPtr->gameId, taskPtr->installDir, "DOSBox");
                                        if (taskPtr->completionCallback) {
                                            taskPtr->completionCallback(util::Result<QString>::success(
                                                taskPtr->installDir));
                                        }
                                    } else {
                                        const QString err =
                                            QString("DOS game installer failed with exit code: %1")
                                                .arg(exitCode);
                                        LOG_ERROR(err);
                                        emit installFailed(taskPtr->gameId, err);
                                        if (taskPtr->completionCallback) {
                                            taskPtr->completionCallback(
                                                util::Result<QString>::error(err));
                                        }
                                    }
                                    proc->deleteLater();
                                    QMutexLocker locker(&tasksMutex_);
                                    activeTasks_.erase(taskPtr->gameId);
                                });

                        return;
                    }

                    // Pure DOS game - use DOSBox directly
                    QString dosboxExe = QStandardPaths::findExecutable("dosbox");
                    if (dosboxExe.isEmpty()) {
                        dosboxExe = QStandardPaths::findExecutable("dosbox-x");
                    }

                    if (dosboxExe.isEmpty()) {
                        const QString err =
                            "DOSBox not found. Please install DOSBox to run DOS games.\n\n"
                            "Installation:\n"
                            "  Ubuntu/Debian: sudo apt install dosbox\n"
                            "  Fedora: sudo dnf install dosbox\n"
                            "  Arch: sudo pacman -S dosbox\n"
                            "  macOS: brew install dosbox\n\n"
                            "Download: https://www.dosbox.com/";
                        LOG_ERROR(err);
                        emit installFailed(taskPtr->gameId, err);
                        if (taskPtr->completionCallback) {
                            taskPtr->completionCallback(util::Result<QString>::error(err));
                        }
                        QMutexLocker locker2(&tasksMutex_);
                        activeTasks_.erase(taskPtr->gameId);
                        return;
                    }

                    LOG_INFO(QString("Running pure DOS installer with DOSBox: %1 %2")
                                 .arg(dosboxExe, installerPath));

                    auto *proc = new QProcess(this);
                    proc->setProgram(dosboxExe);

                    // Mount the directory and auto-run the installer
                    const QString installerDir = QFileInfo(installerPath).absolutePath();
                    const QString installerExe = QFileInfo(installerPath).fileName();

                    QStringList args;
                    args << "-c" << QString("mount c: \"%1\"").arg(installerDir);
                    args << "-c" << "c:";
                    args << "-c" << installerExe; // Auto-run the installer
                    args << "-c" << "exit"; // Exit DOSBox when done

                    proc->setArguments(args);
                    proc->setWorkingDirectory(installPath);
                    proc->start();

                    if (!proc->waitForStarted(5000)) {
                        const QString err = QString("Failed to start DOSBox installer: %1")
                                                .arg(proc->errorString());
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

                    // For DOS games, wait for DOSBox to finish
                    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                            [this, taskPtr, proc](int exitCode, QProcess::ExitStatus exitStatus) {
                                // DOSBox may exit with non-zero codes, but we consider it success
                                // if it exited normally (not crashed)
                                bool isSuccess = (exitStatus == QProcess::NormalExit);
                                
                                if (isSuccess) {
                                    LOG_INFO(QString("DOS installer completed: %1 (exit code: %2)")
                                                 .arg(taskPtr->game.title).arg(exitCode));

                                    // Auto-set preferred runner to DOSBox for DOS games
                                    taskPtr->game.preferredRunner = "DOSBox";
                                    LOG_INFO(QString("Auto-set preferred runner to DOSBox for: %1")
                                                 .arg(taskPtr->game.title));

                                    {
                                        QMutexLocker locker(&tasksMutex_);
                                        detectedRunners_[taskPtr->gameId] = "DOSBox";
                                    }

                                    emit installCompleted(taskPtr->gameId, taskPtr->installDir, "DOSBox");
                                    if (taskPtr->completionCallback) {
                                        taskPtr->completionCallback(
                                            util::Result<QString>::success(taskPtr->installDir));
                                    }
                                } else {
                                    const QString err =
                                        QString("DOS installer failed with exit code: %1")
                                            .arg(exitCode);
                                    LOG_ERROR(err);
                                    emit installFailed(taskPtr->gameId, err);
                                    if (taskPtr->completionCallback) {
                                        taskPtr->completionCallback(
                                            util::Result<QString>::error(err));
                                    }
                                }
                                proc->deleteLater();
                                QMutexLocker locker(&tasksMutex_);
                                activeTasks_.erase(taskPtr->gameId);
                            });

                    return;
                }

                // Windows game - use Wine/Proton
                QString wineExe;
                QString runnerName;

                // Check for Proton-GE (best compatibility for games)
                QStringList protonGePaths = {
                    QDir::homePath() + "/.steam/steam/compatibilitytools.d/GE-Proton*/proton",
                    QDir::homePath() + "/.local/share/Steam/compatibilitytools.d/GE-Proton*/proton",
                    "/usr/share/steam/compatibilitytools.d/GE-Proton*/proton"};

                for (const QString &pattern : protonGePaths) {
                    QDir dir(QFileInfo(pattern).path());
                    if (dir.exists()) {
                        QStringList entries = dir.entryList(
                            QStringList() << "GE-Proton*", QDir::Dirs, QDir::Name | QDir::Reversed);
                        if (!entries.isEmpty()) {
                            QString protonPath =
                                dir.absolutePath() + "/" + entries.first() + "/proton";
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
                        QDir::homePath() + "/.local/share/Steam/steamapps/common/Proton*/proton"};

                    for (const QString &pattern : protonPaths) {
                        QDir dir(QFileInfo(pattern).path());
                        if (dir.exists()) {
                            QStringList entries =
                                dir.entryList(QStringList() << "Proton*", QDir::Dirs,
                                              QDir::Name | QDir::Reversed);
                            if (!entries.isEmpty()) {
                                QString protonPath =
                                    dir.absolutePath() + "/" + entries.first() + "/proton";
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
                    QStringList winePaths = {QStandardPaths::findExecutable("wine-staging"),
                                             QStandardPaths::findExecutable("wine-tkg"),
                                             "/usr/bin/wine-staging",
                                             "/usr/local/bin/wine-staging",
                                             QStandardPaths::findExecutable("wine"),
                                             "/usr/bin/wine",
                                             "/usr/local/bin/wine",
                                             "/opt/wine/bin/wine",
                                             "/opt/wine-staging/bin/wine"};

                    for (const QString &path : winePaths) {
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
                    const QString err =
                        "Wine/Proton not found. Please install Wine or Proton to run Windows "
                        "installers.\n\n"
                        "Wine:\n"
                        "  Ubuntu/Debian: sudo apt install wine\n"
                        "  Fedora: sudo dnf install wine\n"
                        "  Arch: sudo pacman -S wine\n\n"
                        "Proton-GE (recommended for games):\n"
                        "  Download from: "
                        "https://github.com/GloriousEggroll/proton-ge-custom/releases\n"
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

                LOG_INFO(QString("Running installer with %1: %2 %3")
                             .arg(runnerName, wineExe, installerPath));

                auto *proc = new QProcess(this);

                // Set environment for Wine/Proton
                QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

                // Configure based on runner type
                if (runnerName.contains("Proton")) {
                    // Proton uses different environment variables
                    env.insert("STEAM_COMPAT_DATA_PATH", installPath + "/.proton");
                    env.insert("STEAM_COMPAT_CLIENT_INSTALL_PATH",
                               QDir::homePath() + "/.steam/steam");

                    // Proton command: proton run installer.exe
                    proc->setProgram(wineExe);
                    proc->setArguments({"run", installerPath});
                } else {
                    // Wine/Wine-Staging command: wine installer.exe
                    env.insert("WINEPREFIX", installPath + "/.wine");
                    env.insert("WINEDEBUG", "-all"); // Reduce Wine debug output

                    proc->setProgram(wineExe);
                    proc->setArguments({installerPath});
                }

                proc->setProcessEnvironment(env);
                proc->setWorkingDirectory(installPath);

                proc->start();

                if (!proc->waitForStarted(5000)) {
                    const QString err =
                        QString("Failed to start Wine installer: %1").arg(proc->errorString());
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
                        [this, gameId, installPath, proc](int exitCode,
                                                          QProcess::ExitStatus status) mutable {
                            proc->deleteLater();

                            // Check if task still exists
                            QMutexLocker locker(&tasksMutex_);
                            auto it = activeTasks_.find(gameId);
                            if (it == activeTasks_.end()) {
                                return; // Cancelled
                            }
                            InstallTask *taskPtr = it->second.get();
                            locker.unlock();

                            // Windows installers often exit with codes 0-3 even on success
                            // (0=success, 1=reboot needed, 2=user cancelled, 3=other)
                            bool isSuccess = (status == QProcess::NormalExit && exitCode <= 3);
                            
                            if (!isSuccess) {
                                const QString err =
                                    QString("Installer exited with code %1").arg(exitCode);
                                emit installFailed(taskPtr->gameId, err);
                                if (taskPtr->completionCallback) {
                                    taskPtr->completionCallback(util::Result<QString>::error(err));
                                }
                                QMutexLocker locker2(&tasksMutex_);
                                activeTasks_.erase(taskPtr->gameId);
                                return;
                            }

                            // Auto-set preferred runner based on what was used for installation
                            // For Windows games, prefer Proton-GE > Proton > Wine
                            if (taskPtr->game.preferredRunner.isEmpty()) {
                                // Check what runner was available during installation
                                QString dosboxExe = QStandardPaths::findExecutable("dosbox");
                                if (dosboxExe.isEmpty()) {
                                    dosboxExe = QStandardPaths::findExecutable("dosbox-x");
                                }

                                // Check for Proton-GE first
                                bool foundProtonGE = false;
                                QStringList protonGePaths = {
                                    QDir::homePath() + "/.steam/steam/compatibilitytools.d/GE-Proton*/proton",
                                    QDir::homePath() +
                                        "/.local/share/Steam/compatibilitytools.d/GE-Proton*/proton"};

                                for (const QString &pattern : protonGePaths) {
                                    QDir dir(QFileInfo(pattern).path());
                                    if (dir.exists()) {
                                        QStringList entries = dir.entryList(
                                            QStringList() << "GE-Proton*", QDir::Dirs,
                                            QDir::Name | QDir::Reversed);
                                        if (!entries.isEmpty()) {
                                            taskPtr->game.preferredRunner = "Proton-GE";
                                            foundProtonGE = true;
                                            break;
                                        }
                                    }
                                }

                                // Fall back to Proton
                                if (!foundProtonGE) {
                                    QStringList protonPaths = {
                                        QDir::homePath() + "/.steam/steam/steamapps/common/Proton*/proton",
                                        QDir::homePath() +
                                            "/.local/share/Steam/steamapps/common/Proton*/proton"};

                                    for (const QString &pattern : protonPaths) {
                                        QDir dir(QFileInfo(pattern).path());
                                        if (dir.exists()) {
                                            QStringList entries = dir.entryList(
                                                QStringList() << "Proton*", QDir::Dirs,
                                                QDir::Name | QDir::Reversed);
                                            if (!entries.isEmpty()) {
                                                taskPtr->game.preferredRunner = "Proton";
                                                break;
                                            }
                                        }
                                    }
                                }

                                // Fall back to Wine
                                if (taskPtr->game.preferredRunner.isEmpty()) {
                                    QStringList winePaths = {
                                        QStandardPaths::findExecutable("wine-staging"),
                                        QStandardPaths::findExecutable("wine-tkg"),
                                        QStandardPaths::findExecutable("wine")};

                                    for (const QString &path : winePaths) {
                                        if (!path.isEmpty()) {
                                            if (path.contains("staging")) {
                                                taskPtr->game.preferredRunner = "Wine-Staging";
                                            } else if (path.contains("tkg")) {
                                                taskPtr->game.preferredRunner = "Wine-TKG";
                                            } else {
                                                taskPtr->game.preferredRunner = "Wine";
                                            }
                                            break;
                                        }
                                    }
                                }

                                if (!taskPtr->game.preferredRunner.isEmpty()) {
                                    LOG_INFO(QString("Auto-set preferred runner to %1 for: %2")
                                                 .arg(taskPtr->game.preferredRunner,
                                                      taskPtr->game.title));
                                }
                            }

                            QString detectedRunner = taskPtr->game.preferredRunner;
                            {
                                QMutexLocker locker2(&tasksMutex_);
                                if (!detectedRunner.isEmpty()) {
                                    detectedRunners_[taskPtr->gameId] = detectedRunner;
                                }
                            }

                            emit installCompleted(taskPtr->gameId, installPath, detectedRunner);
                            if (taskPtr->completionCallback) {
                                taskPtr->completionCallback(
                                    util::Result<QString>::success(installPath));
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
                InstallTask *taskPtr = it->second.get();

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

void InstallService::uninstallGame(const QString &gameId, const QString &installPath,
                                   std::function<void(util::Result<void>)> callback) {
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

void InstallService::cancelInstallation(const QString &gameId) {
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

bool InstallService::isInstalling(const QString &gameId) const {
    QMutexLocker locker(&tasksMutex_);
    return activeTasks_.find(gameId) != activeTasks_.end();
}

QString InstallService::getDetectedRunner(const QString &gameId) const {
    QMutexLocker locker(&tasksMutex_);
    auto it = detectedRunners_.find(gameId);
    if (it != detectedRunners_.end()) {
        return it->second;
    }
    return QString();
}

} // namespace opengalaxy::install
