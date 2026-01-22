// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QString>
#include <QStringList>
#include <QProcess>
#include <QMap>
#include <memory>

namespace opengalaxy::runners {

/**
 * @brief Architecture types for ISA translation
 */
enum class Architecture {
    Unknown,
    X86,        // 32-bit x86
    X86_64,     // 64-bit x86
    ARM,        // 32-bit ARM
    ARM64       // 64-bit ARM (aarch64)
};

/**
 * @brief Platform types
 */
enum class Platform {
    Unknown,
    Windows,
    Linux,
    MacOS,
    DOS
};

/**
 * @brief Runner capabilities
 */
struct RunnerCapabilities {
    QString name;
    QString version;
    QString executablePath;
    Platform supportedPlatform;
    Architecture hostArch;
    Architecture targetArch;
    bool requiresISATranslation = false;
    QStringList supportedExtensions;  // .exe, .sh, etc.
};

/**
 * @brief Launch configuration
 */
struct LaunchConfig {
    QString gamePath;
    QString workingDirectory;
    QStringList arguments;
    QMap<QString, QString> environment;
    Platform gamePlatform;
    Architecture gameArch;
};

/**
 * @brief Base class for game runners
 */
class Runner {
public:
    virtual ~Runner() = default;

    // Runner information
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual bool isAvailable() const = 0;
    virtual RunnerCapabilities capabilities() const = 0;

    // Check if this runner can run the given game
    virtual bool canRun(const LaunchConfig& config) const = 0;

    // Launch game
    virtual QProcess* launch(const LaunchConfig& config) = 0;

    // Configuration
    virtual QStringList configOptions() const { return {}; }
    virtual void setConfigOption(const QString& key, const QString& value) { Q_UNUSED(key); Q_UNUSED(value); }

protected:
    // Helper to detect architecture from executable
    static Architecture detectArchitecture(const QString& executablePath);
    static Platform detectPlatform(const QString& executablePath);
};

} // namespace opengalaxy::runners
