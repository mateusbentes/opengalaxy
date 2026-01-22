// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/runners/runner_manager.h"
#include "opengalaxy/util/log.h"
#include "wrapper_runner.h"
#include <QProcess>
#include <QFileInfo>
#include <QDir>
namespace opengalaxy::runners {

// Simple native runner implementation
class NativeRunner : public Runner {
public:
    QString name() const override { return "Native"; }
    QString version() const override { return "1.0"; }
    bool isAvailable() const override { return true; }
    
    RunnerCapabilities capabilities() const override {
        RunnerCapabilities caps;
        caps.name = "Native";
        caps.version = "1.0";
        caps.executablePath = "";
        caps.supportedPlatform = Platform::Linux;
        caps.hostArch = Architecture::X86_64;
        caps.targetArch = Architecture::X86_64;
        caps.requiresISATranslation = false;
        return caps;
    }
    
    bool canRun(const LaunchConfig& config) const override {
        return config.gamePlatform == Platform::Linux;
    }
    
    std::unique_ptr<QProcess> launch(const LaunchConfig& config) override {
        auto process = std::make_unique<QProcess>();
        process->setProgram(config.gamePath);
        process->setArguments(config.arguments);
        process->setWorkingDirectory(config.workingDirectory);
        
        for (auto it = config.environment.begin(); it != config.environment.end(); ++it) {
            process->setEnvironment(QStringList() << it.key() + "=" + it.value());
        }
        
        process->start();
        
        // Check if process started successfully
        if (!process->waitForStarted(3000)) {
            LOG_ERROR(QString("Failed to start game: %1 - %2")
                .arg(config.gamePath)
                .arg(process->errorString()));
            return nullptr;
        }
        
        return process;
    }
};

RunnerManager::RunnerManager(QObject* parent)
    : QObject(parent)
{
    discoverRunners();
}

RunnerManager::~RunnerManager() = default;

void RunnerManager::discoverRunners()
{
    LOG_INFO("Discovering runners...");

    // Always add native runner
    registerRunner(std::make_unique<NativeRunner>());

    // ISA translators / wrappers (availability checked via executable existence)
#ifdef Q_OS_LINUX
    registerRunner(std::make_unique<WrapperRunner>("Box64", "/usr/bin/box64", Platform::Linux,
                                                  Architecture::ARM64, Architecture::X86_64, true));
    registerRunner(std::make_unique<WrapperRunner>("FEX", "/usr/bin/FEXInterpreter", Platform::Linux,
                                                  Architecture::ARM64, Architecture::X86_64, true));
    registerRunner(std::make_unique<WrapperRunner>("QEMU", "/usr/bin/qemu-x86_64", Platform::Linux,
                                                  Architecture::ARM64, Architecture::X86_64, true));
#endif
#ifdef Q_OS_MACOS
    // Note: Rosetta 2 does not have a stable public wrapper executable;
    // we expose it as a logical runner name and use /usr/bin/arch to request x86_64.
    registerRunner(std::make_unique<WrapperRunner>("Rosetta2", "/usr/bin/arch", Platform::MacOS,
                                                  Architecture::ARM64, Architecture::X86_64, true));
#endif

    emit runnersDiscovered(static_cast<int>(runners_.size()));
}
std::vector<RunnerCapabilities> RunnerManager::availableRunners() const
{
    std::vector<RunnerCapabilities> caps;
    for (const auto& runner : runners_) {
        caps.push_back(runner->capabilities());
    }
    return caps;
}

Runner* RunnerManager::findBestRunner(const LaunchConfig& config)
{
    for (const auto& runner : runners_) {
        if (runner->canRun(config) && runner->isAvailable()) {
            return runner.get();
        }
    }
    return nullptr;
}

Runner* RunnerManager::getRunner(const QString& name)
{
    for (const auto& runner : runners_) {
        if (runner->name() == name) {
            return runner.get();
        }
    }
    return nullptr;
}

void RunnerManager::registerRunner(std::unique_ptr<Runner> runner)
{
    QString name = runner->name();
    runners_.push_back(std::move(runner));
    emit runnerAdded(name);
}

} // namespace opengalaxy::runners
