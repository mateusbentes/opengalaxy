// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/runners/dosbox_runner.h"
#include "opengalaxy/runners/dosbox_manager.h"
#include "opengalaxy/util/log.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTextStream>
#include <algorithm>

namespace opengalaxy::runners {

DOSBoxRunner::DOSBoxRunner(const QString &dosboxPath) : dosboxPath_(dosboxPath) {}

QString DOSBoxRunner::name() const { return "DOSBox"; }

QString DOSBoxRunner::version() const {
    // Try to get DOSBox version
    QProcess process;
    process.setProgram(dosboxPath_);
    process.setArguments({"-version"});
    process.start();

    if (process.waitForFinished(2000)) {
        const QString output = QString::fromUtf8(process.readAllStandardOutput());
        // Parse version from output like "DOSBox version 0.74-3"
        if (output.contains("version")) {
            const QStringList parts = output.split(" ");
            for (int i = 0; i < parts.size() - 1; ++i) {
                if (parts[i] == "version") {
                    return parts[i + 1].trimmed();
                }
            }
        }
    }

    return "unknown";
}

bool DOSBoxRunner::isAvailable() const {
    return QFileInfo(dosboxPath_).exists() && QFileInfo(dosboxPath_).isExecutable();
}

RunnerCapabilities DOSBoxRunner::capabilities() const {
    RunnerCapabilities caps;
    caps.name = "DOSBox";
    caps.version = version();
    caps.executablePath = dosboxPath_;
    caps.supportedPlatform = Platform::DOS;
    caps.hostArch = Architecture::X86_64; // DOSBox runs on host architecture
    caps.targetArch = Architecture::X86;  // DOS games are x86
    caps.requiresISATranslation = false;
    caps.supportedExtensions = {".exe", ".com", ".bat"};

    return caps;
}

bool DOSBoxRunner::canRun(const LaunchConfig &config) const {
    return config.gamePlatform == Platform::DOS && isAvailable();
}

QString DOSBoxRunner::createDOSBoxConfig(const LaunchConfig &config) {
    // Create a temporary DOSBox configuration file
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        LOG_ERROR("Failed to create temporary directory for DOSBox config");
        return {};
    }

    const QString configPath = tempDir.path() + "/dosbox.conf";
    QFile configFile(configPath);

    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to create DOSBox config: %1").arg(configPath));
        return {};
    }

    QTextStream out(&configFile);

    // DOSBox configuration
    out << "[cpu]\n";
    out << "core=auto\n";
    out << "cputype=auto\n";
    out << "cycles=" << cpuCycles_ << "\n";
    out << "\n";

    out << "[render]\n";
    out << "frameskip=0\n";
    out << "scaler=" << renderScaler_ << "\n";
    out << "\n";

    out << "[mixer]\n";
    out << "rate=44100\n";
    out << "nosound=false\n";
    out << "\n";

    out << "[dos]\n";
    out << "xms=true\n";
    out << "ems=true\n";
    out << "umb=true\n";
    out << "\n";

    out << "[cpu]\n";
    out << "core=auto\n";
    out << "\n";

    // Mount the game directory
    const QString gameDir = QFileInfo(config.gamePath).absolutePath();
    out << "[autoexec]\n";
    out << "@echo off\n";
    out << "mount c: \"" << gameDir << "\"\n";
    out << "c:\n";

    // Run the game executable
    const QString gameExe = QFileInfo(config.gamePath).fileName();
    out << gameExe;

    // Add game arguments
    for (const auto &arg : config.arguments) {
        out << " " << arg;
    }
    out << "\n";
    out << "exit\n";

    configFile.close();

    LOG_INFO(QString("Created DOSBox config: %1").arg(configPath));
    return configPath;
}

bool DOSBoxRunner::isDOSGame(const QString &executablePath) {
    QFile file(executablePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Read first 2 bytes (MZ header for DOS/Windows executables)
    const QByteArray header = file.read(2);
    file.close();

    // DOS executables start with "MZ" (0x4D5A)
    if (header.size() >= 2 && header[0] == 'M' && header[1] == 'Z') {
        // Check if it's a DOS executable (not Windows PE)
        // Windows PE has additional headers at offset 0x3C
        file.open(QIODevice::ReadOnly);
        file.seek(0x3C);
        const QByteArray peOffset = file.read(4);
        file.close();

        if (peOffset.size() == 4) {
            // If PE offset is very small, it's likely a DOS executable
            const uint32_t offset = *reinterpret_cast<const uint32_t *>(peOffset.data());
            if (offset < 64) {
                return true; // DOS executable
            }
        }
    }

    return false;
}

std::unique_ptr<QProcess> DOSBoxRunner::launch(const LaunchConfig &config) {
    if (!isAvailable()) {
        LOG_ERROR("DOSBox is not available");
        return nullptr;
    }

    // Determine the actual game path
    // config.gamePath might be a directory (install path) or an executable
    QString gamePath = config.gamePath;
    QFileInfo pathInfo(gamePath);
    
    // If gamePath is a directory, find the main executable
    if (pathInfo.isDir()) {
        LOG_INFO(QString("Game path is a directory, searching for executable: %1").arg(gamePath));
        
        // Search in multiple locations:
        // 1. Direct game directory
        // 2. Wine prefix (.wine/drive_c)
        // 3. Proton prefix (.proton/pfx/drive_c)
        // 4. Recursively in subdirectories
        
        QStringList searchPaths;
        searchPaths << gamePath;  // Direct directory
        searchPaths << gamePath + "/.wine/drive_c";  // Wine prefix
        searchPaths << gamePath + "/.proton/pfx/drive_c";  // Proton prefix
        
        QFileInfoList exeFiles;
        
        // Helper lambda to recursively find executables
        auto findExecutablesRecursive = [](const QString &rootPath) -> QFileInfoList {
            QFileInfoList allExes;
            QStringList exeFilters = {"*.exe", "*.com", "*.bat"};
            
            QDirIterator it(rootPath, exeFilters, QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                allExes.append(QFileInfo(it.next()));
            }
            
            // Sort by size (largest first)
            std::sort(allExes.begin(), allExes.end(), 
                [](const QFileInfo &a, const QFileInfo &b) {
                    return a.size() > b.size();
                });
            
            return allExes;
        };
        
        for (const QString &searchPath : searchPaths) {
            QDir searchDir(searchPath);
            if (!searchDir.exists()) {
                LOG_INFO(QString("Search path does not exist: %1").arg(searchPath));
                continue;
            }
            
            LOG_INFO(QString("Searching for executables in: %1").arg(searchPath));
            
            // Recursive search for executables
            exeFiles = findExecutablesRecursive(searchPath);
            
            // If found in this path, use it
            if (!exeFiles.isEmpty()) {
                LOG_INFO(QString("Found %1 executable files in: %2").arg(exeFiles.size()).arg(searchPath));
                break;
            }
        }
        
        if (!exeFiles.isEmpty()) {
            // Filter out setup/installer executables
            QStringList installerPatterns = {"setup", "install", "uninstall", "patch", "update"};
            
            // First pass: Look for DOS executables that are NOT installers
            for (const auto &fileInfo : exeFiles) {
                if (!fileInfo.isFile()) continue;
                
                QString fileName = fileInfo.fileName().toLower();
                bool isInstaller = false;
                for (const auto &pattern : installerPatterns) {
                    if (fileName.contains(pattern)) {
                        isInstaller = true;
                        break;
                    }
                }
                
                if (!isInstaller && isDOSGame(fileInfo.absoluteFilePath())) {
                    gamePath = fileInfo.absoluteFilePath();
                    LOG_INFO(QString("Found DOS executable (non-installer): %1").arg(gamePath));
                    break;
                }
            }
            
            // Second pass: Look for any DOS executable (including installers)
            if (gamePath == config.gamePath) {
                for (const auto &fileInfo : exeFiles) {
                    if (!fileInfo.isFile()) continue;
                    
                    if (isDOSGame(fileInfo.absoluteFilePath())) {
                        gamePath = fileInfo.absoluteFilePath();
                        LOG_INFO(QString("Found DOS executable: %1").arg(gamePath));
                        break;
                    }
                }
            }
            
            // Third pass: Use largest non-installer file
            if (gamePath == config.gamePath) {
                for (const auto &fileInfo : exeFiles) {
                    if (!fileInfo.isFile()) continue;
                    
                    QString fileName = fileInfo.fileName().toLower();
                    bool isInstaller = false;
                    for (const auto &pattern : installerPatterns) {
                        if (fileName.contains(pattern)) {
                            isInstaller = true;
                            break;
                        }
                    }
                    
                    if (!isInstaller) {
                        gamePath = fileInfo.absoluteFilePath();
                        LOG_WARNING(QString("Using largest non-installer file: %1").arg(gamePath));
                        break;
                    }
                }
            }
            
            // Last resort: Use largest file (even if installer)
            if (gamePath == config.gamePath && !exeFiles.isEmpty()) {
                for (const auto &fileInfo : exeFiles) {
                    if (fileInfo.isFile()) {
                        gamePath = fileInfo.absoluteFilePath();
                        LOG_WARNING(QString("No suitable executable found, using largest file: %1").arg(gamePath));
                        break;
                    }
                }
            }
        } else {
            LOG_ERROR(QString("No executable files found in game directory or Wine/Proton prefix: %1").arg(gamePath));
            return nullptr;
        }
    }
    
    // Create a modified config with the actual executable path
    LaunchConfig modifiedConfig = config;
    modifiedConfig.gamePath = gamePath;

    // Clean up old DOSBox processes before launching new one
    LOG_INFO("Cleaning up old DOSBox processes...");
    const auto oldPids = DOSBoxManager::findRunningDOSBoxProcesses();
    for (const auto &pid : oldPids) {
        if (DOSBoxManager::likelyHasUnsavedProgress(pid)) {
            LOG_WARNING(QString("Old DOSBox process %1 may have unsaved progress (uptime: %2s)")
                            .arg(pid)
                            .arg(DOSBoxManager::getProcessUptime(pid)));
        }
        if (DOSBoxManager::gracefullyTerminate(pid, 3000)) {
            LOG_INFO(QString("Terminated old DOSBox process: %1").arg(pid));
        } else if (DOSBoxManager::forceKill(pid)) {
            LOG_WARNING(QString("Force killed old DOSBox process: %1").arg(pid));
        }
    }

    // Create DOSBox configuration
    const QString configPath = createDOSBoxConfig(modifiedConfig);
    if (configPath.isEmpty()) {
        LOG_ERROR("Failed to create DOSBox configuration");
        return nullptr;
    }

    auto process = std::make_unique<QProcess>();
    process->setProgram(dosboxPath_);

    // DOSBox arguments: -conf <config_file> -fullscreen (optional)
    QStringList args;
    args << "-conf" << configPath;

    if (fullscreen_) {
        args << "-fullscreen";
    }

    process->setArguments(args);
    process->setWorkingDirectory(QFileInfo(modifiedConfig.gamePath).absolutePath());

    // Set environment
    QStringList env = QProcessEnvironment::systemEnvironment().toStringList();
    for (auto it = modifiedConfig.environment.begin(); it != modifiedConfig.environment.end(); ++it) {
        env << (it.key() + "=" + it.value());
    }
    process->setEnvironment(env);

    process->start();

    if (!process->waitForStarted(3000)) {
        LOG_ERROR(QString("Failed to start DOSBox: %1").arg(process->errorString()));
        return nullptr;
    }

    LOG_INFO(QString("DOSBox started for game: %1").arg(modifiedConfig.gamePath));
    return process;
}

QStringList DOSBoxRunner::configOptions() const {
    return {"cpuCycles", "renderScaler", "fullscreen"};
}

void DOSBoxRunner::setConfigOption(const QString &key, const QString &value) {
    if (key == "cpuCycles") {
        cpuCycles_ = value;
        LOG_INFO(QString("DOSBox CPU cycles set to: %1").arg(value));
    } else if (key == "renderScaler") {
        renderScaler_ = value;
        LOG_INFO(QString("DOSBox render scaler set to: %1").arg(value));
    } else if (key == "fullscreen") {
        fullscreen_ = (value.toLower() == "true");
        LOG_INFO(QString("DOSBox fullscreen: %1").arg(fullscreen_ ? "enabled" : "disabled"));
    }
}

} // namespace opengalaxy::runners
