// SPDX-License-Identifier: Apache-2.0
#include "wine_runner.h"
#include "opengalaxy/util/log.h"

#include <QFileInfo>
#include <QProcessEnvironment>

namespace opengalaxy::runners {

WineRunner::WineRunner(QString winePath)
    : winePath_(std::move(winePath))
{
}

QString WineRunner::name() const { return "Wine"; }

QString WineRunner::version() const { return "system"; }

bool WineRunner::isAvailable() const { return QFileInfo::exists(winePath_); }

RunnerCapabilities WineRunner::capabilities() const
{
    RunnerCapabilities caps;
    caps.name = name();
    caps.version = version();
    caps.executablePath = winePath_;
    caps.supportedPlatform = Platform::Linux;
    caps.hostArch = Architecture::X86_64;
    caps.targetArch = Architecture::X86_64;
    caps.requiresISATranslation = false;
    caps.supportedExtensions = {".exe", ".msi"};
    return caps;
}

bool WineRunner::canRun(const LaunchConfig& config) const
{
    // Wine is for Windows games on Linux.
    return config.gamePlatform  ==  Platform::Windows;
}

std::unique_ptr<QProcess> WineRunner::launch(const LaunchConfig& config)
{
    auto process = std::make_unique<QProcess>();

    // env: start from system and append overrides
    QStringList env = QProcessEnvironment::systemEnvironment().toStringList();
    for (auto it = config.environment.begin(); it  !=  config.environment.end(); ++it) {
        env << (it.key() + "=" + it.value());
    }
    process->setEnvironment(env);

    process->setProgram(winePath_);

    QStringList args;
    args << config.gamePath;
    args << config.arguments;

    process->setArguments(args);
    process->setWorkingDirectory(config.workingDirectory);

    process->start();

    if (!process->waitForStarted(3000)) {
        LOG_ERROR(QString("Failed to start game via Wine (%1): %2")
                      .arg(winePath_, process->errorString()));
        return nullptr;
    }

    return process;
}

} // namespace opengalaxy::runners
