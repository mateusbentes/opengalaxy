// SPDX-License-Identifier: Apache-2.0
#include "wrapper_runner.h"
#include "opengalaxy/util/log.h"

#include <QFileInfo>
#include <QProcessEnvironment>

namespace opengalaxy::runners {
static QString platformToString(Platform p) {
    switch (p) {
    case Platform::Windows:
        return "Windows";
    case Platform::Linux:
        return "Linux";
    case Platform::MacOS:
        return "macOS";
    case Platform::DOS:
        return "DOS";
    default:
        return "Unknown";
    }
}

WrapperRunner::WrapperRunner(QString runnerName, QString wrapperExecutable,
                             Platform supportedPlatform, Architecture hostArch,
                             Architecture targetArch, bool requiresISATranslation)
    : runnerName_(std::move(runnerName)), wrapperExecutable_(std::move(wrapperExecutable)),
      supportedPlatform_(supportedPlatform), hostArch_(hostArch), targetArch_(targetArch),
      requiresISATranslation_(requiresISATranslation) {}

QString WrapperRunner::name() const { return runnerName_; }

QString WrapperRunner::version() const { return "1.0"; }

bool WrapperRunner::isAvailable() const { return QFileInfo::exists(wrapperExecutable_); }

RunnerCapabilities WrapperRunner::capabilities() const {
    RunnerCapabilities caps;
    caps.name = runnerName_;
    caps.version = version();
    caps.executablePath = wrapperExecutable_;
    caps.supportedPlatform = supportedPlatform_;
    caps.hostArch = hostArch_;
    caps.targetArch = targetArch_;
    caps.requiresISATranslation = requiresISATranslation_;
    return caps;
}

bool WrapperRunner::canRun(const LaunchConfig &config) const {
    // For now, runners are selected explicitly by name in UI (preferredRunner).
    // As a safety net, require matching platform.
    return config.gamePlatform == supportedPlatform_;
}

std::unique_ptr<QProcess> WrapperRunner::launch(const LaunchConfig &config) {
    auto process = std::make_unique<QProcess>();

    // Use custom executable if provided, otherwise use auto-detected one
    const QString chosenWrapper = config.runnerExecutableOverride.trimmed().isEmpty()
                                      ? wrapperExecutable_
                                      : config.runnerExecutableOverride.trimmed();

    if (chosenWrapper.isEmpty() || !QFileInfo::exists(chosenWrapper)) {
        LOG_ERROR(
            QString("Wrapper executable not found for %1: '%2'").arg(runnerName_, chosenWrapper));
        return nullptr;
    }

    // env: start from current environment and override
    QStringList env = QProcessEnvironment::systemEnvironment().toStringList();
    for (auto it = config.environment.begin(); it != config.environment.end(); ++it) {
        env << (it.key() + "=" + it.value());
    }
    process->setEnvironment(env);

    QStringList args;

    if (runnerName_ == "Rosetta2") {
        // Use: arch -x86_64 [runnerArgs...] <game> [gameArgs...]
        args << "-x86_64";
        args << config.runnerArguments; // Custom wrapper args
        args << config.gamePath;
        args << config.arguments; // Game args
    } else {
        // Use: <wrapper> [runnerArgs...] <game> [gameArgs...]
        args << config.runnerArguments; // Custom wrapper args first
        args << config.gamePath;
        args << config.arguments; // Game args after
    }

    process->setProgram(chosenWrapper);
    process->setArguments(args);
    process->setWorkingDirectory(config.workingDirectory);

    process->start();

    if (!process->waitForStarted(3000)) {
        LOG_ERROR(QString("Failed to start game via %1 (%2): %3")
                      .arg(runnerName_, chosenWrapper, process->errorString()));
        return nullptr;
    }

    LOG_INFO(QString("Launched game via %1 on %2 (executable: %3)")
                 .arg(runnerName_, platformToString(config.gamePlatform), chosenWrapper));

    return process;
}

} // namespace opengalaxy::runners
