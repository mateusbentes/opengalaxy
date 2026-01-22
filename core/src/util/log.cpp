// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/util/log.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <iostream>

namespace opengalaxy::util {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::setLevel(LogLevel level) {
    level_ = level;
}

void Logger::debug(const QString& message, const QString& category) {
    log(LogLevel::Debug, message, category);
}

void Logger::info(const QString& message, const QString& category) {
    log(LogLevel::Info, message, category);
}

void Logger::warning(const QString& message, const QString& category) {
    log(LogLevel::Warning, message, category);
}

void Logger::error(const QString& message, const QString& category) {
    log(LogLevel::Error, message, category);
}

void Logger::critical(const QString& message, const QString& category) {
    log(LogLevel::Critical, message, category);
}

void Logger::log(LogLevel level, const QString& message, const QString& category) {
    if (level < level_) {
        return;
    }

    QString levelStr;
    switch (level) {
        case LogLevel::Debug:    levelStr = "DEBUG"; break;
        case LogLevel::Info:     levelStr = "INFO"; break;
        case LogLevel::Warning:  levelStr = "WARN"; break;
        case LogLevel::Error:    levelStr = "ERROR"; break;
        case LogLevel::Critical: levelStr = "CRITICAL"; break;
    }

    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    QString categoryStr = category.isEmpty() ? "" : QString("[%1] ").arg(category);
    QString logLine = QString("%1 [%2] %3%4")
                         .arg(timestamp)
                         .arg(levelStr)
                         .arg(categoryStr)
                         .arg(message);

    // Output to console
    if (level >= LogLevel::Error) {
        std::cerr << logLine.toStdString() << std::endl;
    } else {
        std::cout << logLine.toStdString() << std::endl;
    }

    // Also write to log file
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);
    QString logPath = logDir + "/opengalaxy.log";

    QFile logFile(logPath);
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&logFile);
        stream << logLine << "\n";
        logFile.close();
    }
}

} // namespace opengalaxy::util
