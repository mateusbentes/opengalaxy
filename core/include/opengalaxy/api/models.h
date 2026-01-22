// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QString>
#include <QDateTime>
#include <QStringList>
#include <QMap>
#include <vector>
namespace opengalaxy::api {

/**
 * @brief Game information from GOG API
 */
struct GameInfo {
    QString id;              // GOG game ID
    QString title;           // Game title
    QString platform;        // Platform (Windows, Linux, macOS)
    QString coverUrl;        // Cover image URL
    QString backgroundUrl;   // Background image URL
    QStringList genres;      // Game genres
    QString developer;       // Developer name
    QString publisher;       // Publisher name
    QDateTime releaseDate;   // Release date
    QString description;     // Game description
    bool isInstalled = false;
    QString installPath;     // Local installation path
    QString version;         // Installed version
    qint64 size = 0;         // Download size in bytes

    // Per-game properties (user overrides)
    // runner: "Auto" (empty) | "Native" | "Box64" | "FEX" | "Rosetta2" | "QEMU"
    QString preferredRunner;
    // Optional wrapper executable path (e.g., /usr/bin/box64, /usr/bin/qemu-x86_64)
    QString runnerExecutable;
    // Extra arguments passed to the runner/wrapper (not the game)
    QStringList runnerArguments;
    // Extra environment variables (KEY=VALUE) applied on launch
    QMap<QString, QString> extraEnvironment;

    // Download URLs
    struct DownloadLink {
        QString url;
        QString platform;     // windows, linux, mac
        QString language;
        QString version;
        qint64 size = 0;
    };
    std::vector<DownloadLink> downloads;
};

/**
 * @brief User session information
 */
struct UserInfo {
    QString userId;
    QString username;
    QString email;
    QString avatarUrl;
    QString country;
    QDateTime createdAt;
};

/**
 * @brief OAuth tokens
 */
struct AuthTokens {
    QString accessToken;
    QString refreshToken;
    QString tokenType = "Bearer";
    QDateTime expiresAt;
    
    bool isExpired() const {
        return QDateTime::currentDateTime() >= expiresAt;
    }
    
    bool isValid() const {
        return !accessToken.isEmpty() && !isExpired();
    }
};

/**
 * @brief Achievement information
 */
struct Achievement {
    QString id;
    QString name;
    QString description;
    QString iconUrl;
    QString iconLockedUrl;
    bool unlocked = false;
    QDateTime unlockedAt;
    double rarityPercent = 0.0;
};

/**
 * @brief Cloud save information
 */
struct CloudSave {
    QString filename;
    qint64 size = 0;
    QDateTime modified;
    QString checksum;
    QString downloadUrl;
};

/**
 * @brief Friend information
 */
struct FriendInfo {
    QString userId;
    QString username;
    QString avatarUrl;
    QString status;          // online, offline, away, busy
    QString currentGame;     // Currently playing game
    QDateTime lastSeen;
};

/**
 * @brief Store game information
 */
struct StoreGameInfo {
    QString id;
    QString title;
    QString price;
    QString discountPrice;
    int discountPercent = 0;
    QString coverUrl;
    QStringList platforms;
    double rating = 0.0;
    int reviewCount = 0;
};

} // namespace opengalaxy::api
