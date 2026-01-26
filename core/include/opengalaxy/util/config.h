// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QString>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>

namespace opengalaxy::util {

/**
 * @brief Cross-platform configuration manager
 *
 * Manages application settings and data paths for Windows, macOS, and Linux.
 * Uses platform-specific standard locations:
 *
 * Linux:
 *   - Config: ~/.config/OpenGalaxy/
 *   - Data: ~/.local/share/opengalaxy/
 *   - Games: ~/Games/
 *
 * Windows:
 *   - Config: %APPDATA%\OpenGalaxy\
 *   - Data: %APPDATA%\OpenGalaxy\
 *   - Games: %USERPROFILE%\Games\
 *
 * macOS:
 *   - Config: ~/Library/Preferences/OpenGalaxy/
 *   - Data: ~/Library/Application Support/opengalaxy/
 *   - Games: ~/Games/
 */
class Config {
public:
    /**
     * @brief Get the singleton instance
     */
    static Config& instance();

    /**
     * @brief Initialize the configuration system
     * Must be called once at application startup
     */
    static void initialize();

    // Path getters
    QString dataDir() const;           // Application data directory
    QString configDir() const;         // Configuration directory
    QString sessionFilePath() const;   // Session file path
    QString libraryDbPath() const;     // Library database path
    QString logFilePath() const;       // Log file path
    QString defaultGamesDir() const;   // Default games installation directory

    // Settings accessors
    QString gamesDirectory() const;
    void setGamesDirectory(const QString& path);

    QString language() const;
    void setLanguage(const QString& locale);

    QString preferredRunner() const;
    void setPreferredRunner(const QString& runner);

    bool autoRefreshLibrary() const;
    void setAutoRefreshLibrary(bool enabled);

    bool enableCloudSaves() const;
    void setEnableCloudSaves(bool enabled);

    bool showHiddenGames() const;
    void setShowHiddenGames(bool enabled);

    // Window state
    QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray& geometry);

    QByteArray windowState() const;
    void setWindowState(const QByteArray& state);

    // Ensure directories exist
    void ensureDirectoriesExist();

    // Clear all settings (for testing/reset)
    void clear();

private:
    Config();
    ~Config() = default;

    // Disable copy/move
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config && ) = delete;
    Config& operator=(Config && ) = delete;

    QSettings settings_;
    QString dataDir_;
    QString configDir_;
    QString defaultGamesDir_;

    void initializePaths();
    QString getDefaultGamesDirectory() const;
};

} // namespace opengalaxy::util
