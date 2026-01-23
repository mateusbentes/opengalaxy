// SPDX-License-Identifier: Apache-2.0
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QDir>
#include <iostream>
#include "opengalaxy/api/session.h"
#include "opengalaxy/api/gog_client.h"
#include "opengalaxy/library/library_service.h"
#include "opengalaxy/install/install_service.h"
#include "opengalaxy/runners/runner_manager.h"
#include "opengalaxy/util/log.h"
#include "opengalaxy/util/config.h"
// main.moc is included by CMake's AUTOGEN; do not include it manually in a .cpp

using namespace opengalaxy;

class CLI {
public:
    CLI(QCoreApplication* app)
        : app_(app)
    {
        session_ = new api::Session();
        gogClient_ = new api::GOGClient(session_);
        libraryService_ = new library::LibraryService(gogClient_);
        installService_ = new install::InstallService();
        runnerManager_ = new runners::RunnerManager();
    }

    ~CLI()
    {
        delete runnerManager_;
        delete installService_;
        delete libraryService_;
        delete gogClient_;
        delete session_;
    }

    void login(const QString& username, const QString& password)
    {
        std::cout << "Logging in..." << std::endl;

        session_->loginWithPassword(username, password, [this](util::Result<api::AuthTokens> result) {
            if (result.isOk()) {
                std::cout << "Login successful!" << std::endl;
                app_->quit();
            } else {
                std::cerr << "Login failed: " << result.errorMessage().toStdString() << std::endl;
                app_->exit(1);
            }
        });
    }

    void listGames()
    {
        if (!session_->isAuthenticated()) {
            std::cerr << "Not logged in. Please login first." << std::endl;
            app_->exit(1);
            return;
        }

        std::cout << "Fetching library..." << std::endl;

        libraryService_->fetchLibrary(false, [this](util::Result<std::vector<api::GameInfo>> result) {
            if (result.isOk()) {
                const auto& games = result.value();
                std::cout << "\nYour library (" << games.size() << " games):\n" << std::endl;

                for (const auto& game : games) {
                    std::cout << "  " << game.title.toStdString();
                    if (game.isInstalled) {
                        std::cout << " [INSTALLED]";
                    }
                    std::cout << std::endl;
                    std::cout << "    ID: " << game.id.toStdString() << std::endl;
                    std::cout << "    Platform: " << game.platform.toStdString() << std::endl;
                    std::cout << std::endl;
                }

                app_->quit();
            } else {
                std::cerr << "Failed to fetch library: " << result.errorMessage().toStdString() << std::endl;
                app_->exit(1);
            }
        });
    }

    void installGame(const QString& gameId, const QString& installDir)
    {
        if (!session_->isAuthenticated()) {
            std::cerr << "Not logged in. Please login first." << std::endl;
            app_->exit(1);
            return;
        }

        std::cout << "Fetching game details..." << std::endl;

        libraryService_->getGame(gameId, [this, installDir](util::Result<api::GameInfo> result) {
            if (!result.isOk()) {
                std::cerr << "Game not found: " << result.errorMessage().toStdString() << std::endl;
                app_->exit(1);
                return;
            }

            const auto& game = result.value();
            std::cout << "Installing: " << game.title.toStdString() << std::endl;

            installService_->installGame(
                game,
                installDir,
                [](const install::InstallService::InstallProgress& progress) {
                    std::cout << "\r[" << progress.percentage << "%] " << progress.status.toStdString() << "..." << std::flush;
                },
                [this](util::Result<QString> result) {
                    std::cout << std::endl;
                    if (result.isOk()) {
                        std::cout << "Installation complete: " << result.value().toStdString() << std::endl;
                        app_->quit();
                    } else {
                        std::cerr << "Installation failed: " << result.errorMessage().toStdString() << std::endl;
                        app_->exit(1);
                    }
                });
        });
    }

    void launchGame(const QString& gameId)
    {
        if (!session_->isAuthenticated()) {
            std::cerr << "Not logged in. Please login first." << std::endl;
            app_->exit(1);
            return;
        }

        libraryService_->getGame(gameId, [this](util::Result<api::GameInfo> result) {
            if (!result.isOk()) {
                std::cerr << "Game not found: " << result.errorMessage().toStdString() << std::endl;
                app_->exit(1);
                return;
            }

            const auto& game = result.value();

            if (!game.isInstalled) {
                std::cerr << "Game is not installed." << std::endl;
                app_->exit(1);
                return;
            }

            std::cout << "Launching: " << game.title.toStdString() << std::endl;

            runners::LaunchConfig config;
            config.gamePath = game.installPath;
            config.workingDirectory = QFileInfo(game.installPath).absolutePath();
            
            // Detect platform and architecture from binary
            config.gamePlatform = runners::Runner::detectPlatform(game.installPath);
            config.gameArch = runners::Runner::detectArchitecture(game.installPath);
            
            // Use game-specific settings if available
            config.runnerExecutableOverride = game.runnerExecutable.trimmed();
            config.runnerArguments = game.runnerArguments; // Already a QStringList
            config.arguments = {}; // Game arguments (can be extended later)

            auto* runner = runnerManager_->findBestRunner(config);
            if (!runner) {
                std::cerr << "No suitable runner found." << std::endl;
                app_->exit(1);
                return;
            }

            std::cout << "Using runner: " << runner->name().toStdString() << std::endl;

            auto process = runner->launch(config);
            if (!process) {
                std::cerr << "Failed to launch game." << std::endl;
                app_->exit(1);
                return;
            }

            std::cout << "Game launched successfully." << std::endl;

            process->setParent(app_);
            process.release();
            app_->quit();
        });
    }

    void listRunners()
    {
        std::cout << "Discovering runners..." << std::endl;
        runnerManager_->discoverRunners();

        auto runners = runnerManager_->availableRunners();
        std::cout << "\nAvailable runners (" << runners.size() << "): \n" << std::endl;

        for (const auto& runner : runners) {
            std::cout << "  " << runner.name.toStdString() << " " << runner.version.toStdString() << std::endl;
            std::cout << "    Path: " << runner.executablePath.toStdString() << std::endl;
            std::cout << "    Platform: ";
            switch (runner.supportedPlatform) {
                case runners::Platform::Windows: std::cout << "Windows"; break;
                case runners::Platform::Linux: std::cout << "Linux"; break;
                case runners::Platform::MacOS: std::cout << "macOS"; break;
                case runners::Platform::DOS: std::cout << "DOS"; break;
                default: std::cout << "Unknown"; break;
            }
            std::cout << std::endl;

            if (runner.requiresISATranslation) {
                std::cout << "    ISA Translation: ";
                switch (runner.hostArch) {
                    case runners::Architecture::X86: std::cout << "x86"; break;
                    case runners::Architecture::X86_64: std::cout << "x86_64"; break;
                    case runners::Architecture::ARM: std::cout << "ARM"; break;
                    case runners::Architecture::ARM64: std::cout << "ARM64"; break;
                    default: std::cout << "Unknown"; break;
                }
                std::cout << " -> ";
                switch (runner.targetArch) {
                    case runners::Architecture::X86: std::cout << "x86"; break;
                    case runners::Architecture::X86_64: std::cout << "x86_64"; break;
                    case runners::Architecture::ARM: std::cout << "ARM"; break;
                    case runners::Architecture::ARM64: std::cout << "ARM64"; break;
                    default: std::cout << "Unknown"; break;
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        app_->quit();
    }

private:
    QCoreApplication* app_;
    api::Session* session_;
    api::GOGClient* gogClient_;
    library::LibraryService* libraryService_;
    install::InstallService* installService_;
    runners::RunnerManager* runnerManager_;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // Initialize configuration system
    util::Config::initialize();
    
    QCoreApplication::setApplicationName("OpenGalaxy CLI");
    QCoreApplication::setApplicationVersion("1.0.0");

    util::Logger::instance().setLevel(util::LogLevel::Warning);

    QCommandLineParser parser;
    parser.setApplicationDescription("OpenGalaxy command-line interface");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("command", "Command to execute: login, list, install, launch, runners");

    QCommandLineOption usernameOption(QStringList() << "u" << "username", "Username for login", "username");
    QCommandLineOption passwordOption(QStringList() << "p" << "password", "Password for login", "password");
    QCommandLineOption gameIdOption(QStringList() << "g" << "game", "Game ID", "gameId");
    QCommandLineOption installDirOption(QStringList() << "d" << "dir", "Installation directory", "dir");

    parser.addOption(usernameOption);
    parser.addOption(passwordOption);
    parser.addOption(gameIdOption);
    parser.addOption(installDirOption);

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        parser.showHelp(1);
        return 1;
    }

    QString command = args.first();
    CLI cli(&app);

    if (command == "login") {
        if (!parser.isSet(usernameOption) || !parser.isSet(passwordOption)) {
            std::cerr << "Error: --username and --password required for login" << std::endl;
            return 1;
        }
        cli.login(parser.value(usernameOption), parser.value(passwordOption));
    }
    else if (command == "list") {
        cli.listGames();
    }
    else if (command == "install") {
        if (!parser.isSet(gameIdOption)) {
            std::cerr << "Error: --game required for install" << std::endl;
            return 1;
        }
        QString installDir = parser.value(installDirOption);
        if (installDir.isEmpty()) {
            // Use configured games directory
            installDir = util::Config::instance().gamesDirectory();
        }
        cli.installGame(parser.value(gameIdOption), installDir);
    }
    else if (command == "launch") {
        if (!parser.isSet(gameIdOption)) {
            std::cerr << "Error: --game required for launch" << std::endl;
            return 1;
        }
        cli.launchGame(parser.value(gameIdOption));
    }
    else if (command == "runners") {
        cli.listRunners();
    }
    else {
        std::cerr << "Unknown command: " << command.toStdString() << std::endl;
        parser.showHelp(1);
        return 1;
    }

    return app.exec();
}
