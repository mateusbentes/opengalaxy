// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QObject>
#include <QString>
#include <QMutex>
#include <functional>
#include "../util/result.h"
#include "../api/models.h"

namespace opengalaxy::install {

/**
 * @brief Game installation service
 */
class InstallService : public QObject {
    Q_OBJECT

public:
    explicit InstallService(QObject* parent = nullptr);
    ~InstallService() override;

    // Set the session for authenticated downloads (forward declared)
    void setSession(void* session) { session_ = session; }

    struct InstallProgress {
        QString gameId;
        qint64 downloadedBytes = 0;
        qint64 totalBytes = 0;
        QString currentFile;
        QString status;  // downloading, extracting, verifying, complete
        int percentage = 0;
    };

    using ProgressCallback = std::function<void(const InstallProgress&)>;
    using CompletionCallback = std::function<void(util::Result<QString>)>;  // Returns install path

    // Install game
    void installGame(const api::GameInfo& game, const QString& installDir,
                    ProgressCallback progressCallback, CompletionCallback completionCallback);

    // Uninstall game
    void uninstallGame(const QString& gameId, const QString& installPath,
                      std::function<void(util::Result<void>)> callback);

    // Cancel ongoing installation
    void cancelInstallation(const QString& gameId);

    // Check if game is being installed
    bool isInstalling(const QString& gameId) const;

signals:
    void installStarted(const QString& gameId);
    void installProgress(const QString& gameId, int percentage);
    void installCompleted(const QString& gameId, const QString& installPath);
    void installFailed(const QString& gameId, const QString& error);
    void installCancelled(const QString& gameId);

private:
    struct InstallTask;
    std::map<QString, std::unique_ptr<InstallTask>> activeTasks_;
    mutable QMutex tasksMutex_;
    void* session_ = nullptr;  // api::Session* (void* to avoid forward declaration issues)

    void downloadAndExtract(InstallTask* task);
    bool verifyChecksum(const QString& filePath, const QString& expectedChecksum);
    bool extractArchive(const QString& archivePath, const QString& destPath, ProgressCallback progressCallback);
    QString buildAuthHeader() const;
};

} // namespace opengalaxy::install
