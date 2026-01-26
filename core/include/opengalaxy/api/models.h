// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QDateTime>
#include <QMap>
#include <QString>
#include <QStringList>
#include <vector>

namespace opengalaxy::api {

/**
 * @brief Game information from GOG API
 */
struct GameInfo {
    QString id;            // GOG game ID (numeric)
    QString slug;          // GOG game slug (for store URLs)
    QString title;         // Game title
    QString platform;      // Platform (Windows, Linux, macOS)
    QString coverUrl;      // Cover image URL
    QString backgroundUrl; // Background image URL
    QStringList genres;    // Game genres
    QString developer;     // Developer name
    QString publisher;     // Publisher name
    QDateTime releaseDate; // Release date
    QString description;   // Game description
    bool isInstalled = false;
    QString installPath; // Local installation path
    QString version;     // Installed version
    qint64 size = 0;     // Download size in bytes

    // Per-game properties (user overrides)
    // runner: "Auto" (empty) | "Native" | "Wine" | "Proton-GE (...)" | "Box64" | "FEX" | "Rosetta2"
    // | "QEMU"
    QString preferredRunner;
    QString runnerExecutable;
    QStringList runnerArguments;
    QMap<QString, QString> extraEnvironment;

    // Per-game tweaks and toggles
    bool hiddenInLibrary = false;  // Hide game from library
    bool enableMangoHud = false;   // Use MangoHud overlay
    bool enableDxvkHudFps = false; // Show FPS (DXVK_HUD=fps)
    bool enableGameMode = false;   // Use GameMode
    bool enableCloudSaves = true;  // Enable cloud saves (default true)

    // Download URLs
    struct DownloadLink {
        QString url;      // API link (returns JSON with { downlink, checksum })
        QString platform; // windows, linux, mac
        QString language;
        QString version;
        qint64 size = 0;
        QString checksumUrl; // optional: URL to checksum XML
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

    bool isExpired() const { return QDateTime::currentDateTime() >= expiresAt; }

    bool isValid() const { return !accessToken.isEmpty() && !isExpired(); }
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
    QString status;      // online, offline, away, busy
    QString currentGame; // Currently playing game
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
