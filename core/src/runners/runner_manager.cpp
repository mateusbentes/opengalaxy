// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/runners/runner_manager.h"
#include "opengalaxy/util/log.h"

#include "wrapper_runner.h"
#include "wine_runner.h"
#include "proton_runner.h"
#include "proton_discovery.h"

#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QSysInfo>

namespace opengalaxy::runners {

// Helper to convert architecture to string
static QString architectureToString(Architecture arch)
{
    switch (arch) {
        case Architecture::X86: return "x86";
        case Architecture::X86_64: return "x86_64";
        case Architecture::ARM: return "arm";
        case Architecture::ARM64: return "arm64";
        case Architecture::RISCV64: return "riscv64";
        case Architecture::PPC64: return "ppc64";
        case Architecture::MIPS64: return "mips64";
        case Architecture::LoongArch64: return "loongarch64";
        default: return "unknown";
    }
}

// Helper to detect host architecture
static Architecture hostArchitecture()
{
    const QString arch = QSysInfo::currentCpuArchitecture().toLower();
    
    // x86 family
    if (arch == "x86_64" || arch == "amd64") return Architecture::X86_64;
    if (arch == "i386" || arch == "i686" || arch == "x86") return Architecture::X86;
    
    // ARM family
    if (arch == "arm64" || arch == "aarch64") return Architecture::ARM64;
    if (arch.startsWith("arm")) return Architecture::ARM;
    
    // RISC-V
    if (arch == "riscv64" || arch == "riscv") return Architecture::RISCV64;
    
    // PowerPC
    if (arch == "ppc64" || arch == "ppc64le" || arch == "powerpc64" || arch == "powerpc64le") 
        return Architecture::PPC64;
    
    // MIPS
    if (arch == "mips64" || arch == "mips64el") return Architecture::MIPS64;
    
    // LoongArch
    if (arch == "loongarch64") return Architecture::LoongArch64;
    
    return Architecture::Unknown;
}

// Helper to find executable in PATH
static QString findExe(const QStringList& names)
{
    for (const auto& n : names) {
        const QString p = QStandardPaths::findExecutable(n);
        if (!p.isEmpty()) return p;
    }
    return {};
}

// Simple native runner implementation
class NativeRunner : public Runner {
public:
    QString name() const override { return "Native"; }
    QString version() const override { return "1.0"; }
    bool isAvailable() const override { return true; }

    RunnerCapabilities capabilities() const override
    {
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

    bool canRun(const LaunchConfig& config) const override { return config.gamePlatform == Platform::Linux; }

    std::unique_ptr<QProcess> launch(const LaunchConfig& config) override
    {
        auto process = std::make_unique<QProcess>();
        process->setProgram(config.gamePath);
        process->setArguments(config.arguments);
        process->setWorkingDirectory(config.workingDirectory);

        QStringList env = QProcessEnvironment::systemEnvironment().toStringList();
        for (auto it = config.environment.begin(); it != config.environment.end(); ++it) {
            env << (it.key() + "=" + it.value());
        }
        process->setEnvironment(env);

        process->start();

        if (!process->waitForStarted(3000)) {
            LOG_ERROR(QString("Failed to start game: %1 - %2").arg(config.gamePath, process->errorString()));
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

#ifdef Q_OS_LINUX
    // Windows compatibility on Linux
    const QString wine = findExe({"wine", "wine64"});
    if (!wine.isEmpty()) {
        registerRunner(std::make_unique<WineRunner>(wine));
    }

    // Proton-GE (Steam compatibility tools)
    for (const auto& p : discoverProtonGE()) {
        registerRunner(std::make_unique<ProtonRunner>(p.name, p.protonDir));
    }

    // ISA translators / wrappers (register only on ARM64 hosts for x86_64 translation)
    const Architecture hostArch = hostArchitecture();
    if (hostArch == Architecture::ARM64) {
        LOG_INFO("ARM64 host detected, discovering ISA translators...");

        const QString box64 = findExe({"box64"});
        if (!box64.isEmpty()) {
            LOG_INFO(QString("Found Box64: %1").arg(box64));
            registerRunner(std::make_unique<WrapperRunner>("Box64", box64, Platform::Linux,
                                                          Architecture::ARM64, Architecture::X86_64, true));
        }

        const QString fex = findExe({"FEXInterpreter", "FEXLoader"});
        if (!fex.isEmpty()) {
            LOG_INFO(QString("Found FEX-Emu: %1").arg(fex));
            registerRunner(std::make_unique<WrapperRunner>("FEX", fex, Platform::Linux,
                                                          Architecture::ARM64, Architecture::X86_64, true));
        }

        const QString qemu = findExe({"qemu-x86_64", "qemu-x86_64-static"});
        if (!qemu.isEmpty()) {
            LOG_INFO(QString("Found QEMU: %1").arg(qemu));
            registerRunner(std::make_unique<WrapperRunner>("QEMU", qemu, Platform::Linux,
                                                          Architecture::ARM64, Architecture::X86_64, true));
        }
    }
#endif

#ifdef Q_OS_MACOS
    // Rosetta 2 on Apple Silicon (ARM64 only)
    const Architecture hostArch = hostArchitecture();
    if (hostArch == Architecture::ARM64) {
        const QString arch = findExe({"arch"});
        if (!arch.isEmpty()) {
            LOG_INFO(QString("Found Rosetta2 (via arch): %1").arg(arch));
            registerRunner(std::make_unique<WrapperRunner>("Rosetta2", arch, Platform::MacOS,
                                                          Architecture::ARM64, Architecture::X86_64, true));
        }
    }
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
    Runner* best = nullptr;
    int bestScore = -1000000;

    for (const auto& runner : runners_) {
        if (!runner->isAvailable()) continue;
        if (!runner->canRun(config)) continue;

        const auto caps = runner->capabilities();
        int score = 0;

        // Platform match is mandatory, but reward it anyway
        if (caps.supportedPlatform == config.gamePlatform) {
            score += 100;
        }

        // If the game arch is known, prefer exact target arch match
        if (config.gameArch != Architecture::Unknown) {
            if (caps.targetArch == config.gameArch) {
                score += 50;
            } else {
                // Penalize non-matching target arch
                score -= 50;
            }

            // Prefer not translating when unnecessary (native execution)
            if (!caps.requiresISATranslation && caps.hostArch == config.gameArch) {
                score += 10;
            }
            // Slight penalty for translation when not needed
            if (caps.requiresISATranslation && caps.hostArch == config.gameArch) {
                score -= 10;
            }
        }

        // Preference among translators (typical performance: FEX > Box64 > QEMU)
        if (caps.name == "FEX") score += 3;
        if (caps.name == "Box64") score += 2;
        if (caps.name == "QEMU") score += 1;

        // Prefer Proton over Wine for Windows games (better compatibility)
        if (config.gamePlatform == Platform::Windows) {
            if (caps.name.contains("Proton")) score += 5;
        }

        if (score > bestScore) {
            bestScore = score;
            best = runner.get();
        }
    }

    if (best) {
        LOG_INFO(QString("Auto-selected runner: %1 (score: %2)")
                     .arg(best->name()).arg(bestScore));
    }

    return best;
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

void RunnerManager::registerRunner(std::unique_ptr<opengalaxy::runners::Runner> runner)
{
    QString name = runner->name();
    runners_.push_back(std::move(runner));
    emit runnerAdded(name);
}

} // namespace opengalaxy::runners
