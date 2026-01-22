// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QString>
#include <QDebug>

namespace opengalaxy::util {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

class Logger {
public:
    static Logger& instance();

    void setLevel(LogLevel level);
    LogLevel level() const { return level_; }

    void debug(const QString& message, const QString& category = "");
    void info(const QString& message, const QString& category = "");
    void warning(const QString& message, const QString& category = "");
    void error(const QString& message, const QString& category = "");
    void critical(const QString& message, const QString& category = "");

private:
    Logger() = default;
    LogLevel level_ = LogLevel::Info;
    void log(LogLevel level, const QString& message, const QString& category);
};

// Convenience macros
#define LOG_DEBUG(msg) opengalaxy::util::Logger::instance().debug(msg)
#define LOG_INFO(msg) opengalaxy::util::Logger::instance().info(msg)
#define LOG_WARNING(msg) opengalaxy::util::Logger::instance().warning(msg)
#define LOG_ERROR(msg) opengalaxy::util::Logger::instance().error(msg)
#define LOG_CRITICAL(msg) opengalaxy::util::Logger::instance().critical(msg)

} // namespace opengalaxy::util
