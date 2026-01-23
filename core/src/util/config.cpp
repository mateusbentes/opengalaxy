// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/util/config.h"
#include <QCoreApplication>
#include <QDebug>

namespace opengalaxy::util {

Config& Config::instance() {
    static Config instance;
    return instance;
}

void Config::initialize() {
    // Set application metadata for QSettings
    QCoreApplication::setOrganizationName("OpenGalaxy");
    QCoreApplication::setOrganizationDomain("opengalaxy.org");
    QCoreApplication::setApplicationName("OpenGalaxy");

    // Initialize paths
    instance().initializePaths();
    instance().ensureDirectoriesExist();

    qDebug() << "Config initialized:";
    qDebug() << "  Data dir:" << instance().dataDir();
    qDebug() << "  Config dir:" << instance().configDir();
    qDebug() << "  Games dir:" << instance().gamesDirectory();
}

Config::Config()
    : settings_(QSettings::IniFormat, QSettings::UserScope, "OpenGalaxy", "OpenGalaxy")
{
    initializePaths();
}

void Config::initializePaths() {
    // Data directory (for session, library, logs)
    dataDir_ = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    
    // Config directory (for settings)
    configDir_ = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/OpenGalaxy";
    
    // Default games directory
    defaultGamesDir_ = getDefaultGamesDirectory();
}

QString Config::getDefaultGamesDirectory() const {
    // Try to use a sensible default for each platform
#ifdef Q_OS_WIN
    // Windows: %USERPROFILE%\Games
    QString userProfile = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    return userProfile + "/Games";
#elif defined(Q_OS_MACOS)
    // macOS: ~/Games
    QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    return home + "/Games";
#else
    // Linux: ~/Games
    QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    return home + "/Games";
#endif
}

void Config::ensureDirectoriesExist() {
    QDir().mkpath(dataDir_);
    QDir().mkpath(configDir_);
    
    // Create games directory if user hasn't changed it
    QString gamesDir = gamesDirectory();
    if (!gamesDir.isEmpty()) {
        QDir().mkpath(gamesDir);
    }
}

// Path getters
QString Config::dataDir() const {
    return dataDir_;
}

QString Config::configDir() const {
    return configDir_;
}

QString Config::sessionFilePath() const {
    return dataDir_ + "/session.json";
}

QString Config::libraryDbPath() const {
    return dataDir_ + "/library.db";
}

QString Config::logFilePath() const {
    return dataDir_ + "/opengalaxy.log";
}

QString Config::defaultGamesDir() const {
    return defaultGamesDir_;
}

// Settings accessors
QString Config::gamesDirectory() const {
    return settings_.value("install/gamesDirectory", defaultGamesDir_).toString();
}

void Config::setGamesDirectory(const QString& path) {
    settings_.setValue("install/gamesDirectory", path);
    settings_.sync();
    
    // Create directory if it doesn't exist
    QDir().mkpath(path);
}

QString Config::language() const {
    return settings_.value("ui/locale", "").toString();
}

void Config::setLanguage(const QString& locale) {
    settings_.setValue("ui/locale", locale);
    settings_.sync();
}

QString Config::preferredRunner() const {
    return settings_.value("runners/preferred", "").toString();
}

void Config::setPreferredRunner(const QString& runner) {
    settings_.setValue("runners/preferred", runner);
    settings_.sync();
}

bool Config::autoRefreshLibrary() const {
    return settings_.value("library/autoRefresh", true).toBool();
}

void Config::setAutoRefreshLibrary(bool enabled) {
    settings_.setValue("library/autoRefresh", enabled);
    settings_.sync();
}

bool Config::enableCloudSaves() const {
    return settings_.value("features/cloudSaves", false).toBool();
}

void Config::setEnableCloudSaves(bool enabled) {
    settings_.setValue("features/cloudSaves", enabled);
    settings_.sync();
}

QByteArray Config::windowGeometry() const {
    return settings_.value("window/geometry").toByteArray();
}

void Config::setWindowGeometry(const QByteArray& geometry) {
    settings_.setValue("window/geometry", geometry);
    settings_.sync();
}

QByteArray Config::windowState() const {
    return settings_.value("window/state").toByteArray();
}

void Config::setWindowState(const QByteArray& state) {
    settings_.setValue("window/state", state);
    settings_.sync();
}

void Config::clear() {
    settings_.clear();
    settings_.sync();
}

} // namespace opengalaxy::util
