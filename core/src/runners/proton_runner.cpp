// SPDX-License-Identifier: Apache-2.0
#include "proton_runner.h"
#include "opengalaxy/util/log.h"

#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>

namespace opengalaxy::runners {

ProtonRunner::ProtonRunner(QString name, QString protonDir)
    : runnerName_(std::move(name))
    , protonDir_(std::move(protonDir))
{
}

QString ProtonRunner::name() const { return runnerName_; }

QString ProtonRunner::version() const { return "unknown"; }

QString ProtonRunner::protonScriptPath_() const
{
    return QDir(protonDir_).filePath("proton");
}

bool ProtonRunner::isAvailable() const
{
    return QFileInfo::exists(protonScriptPath_());
}

RunnerCapabilities ProtonRunner::capabilities() const
{
    RunnerCapabilities caps;
    caps.name = runnerName_;
    caps.version = version();
    caps.executablePath = protonScriptPath_();
    caps.supportedPlatform = Platform::Linux;
    caps.hostArch = Architecture::X86_64;
    caps.targetArch = Architecture::X86_64;
    caps.requiresISATranslation = false;
    caps.supportedExtensions = {".exe", ".msi"};
    return caps;
}

bool ProtonRunner::canRun(const LaunchConfig& config) const
{
    return config.gamePlatform == Platform::Windows;
}

std::unique_ptr<QProcess> ProtonRunner::launch(const LaunchConfig& config)
{
    auto process = std::make_unique<QProcess>();

    // Merge env
    QStringList env = QProcessEnvironment::systemEnvironment().toStringList();
    for (auto it = config.environment.begin(); it != config.environment.end(); ++it) {
        env << (it.key() + "=" + it.value());
    }

    // Required for non-Steam Proton usage: compat data path provides a prefix location.
    // If user did not specify one, we fallback to workingDirectory/.opengalaxy-proton-prefix
    QString compatPath;
    for (const QString& e : env) {
        if (e.startsWith("STEAM_COMPAT_DATA_PATH=")) {
            compatPath = e.mid(QString("STEAM_COMPAT_DATA_PATH=").size());
            break;
        }
    }
    if (compatPath.isEmpty()) {
        compatPath = QDir(config.workingDirectory).filePath(".opengalaxy-proton-prefix");
        env << ("STEAM_COMPAT_DATA_PATH=" + compatPath);
    }

    process->setEnvironment(env);

    process->setProgram(protonScriptPath_());
    QStringList args;
    args << "run";
    args << config.gamePath;
    args << config.arguments;
    process->setArguments(args);
    process->setWorkingDirectory(config.workingDirectory);

    process->start();

    if (!process->waitForStarted(3000)) {
        LOG_ERROR(QString("Failed to start game via %1 (%2): %3")
                      .arg(runnerName_, protonScriptPath_(), process->errorString()));
        return nullptr;
    }

    return process;
}

} // namespace opengalaxy::runners
