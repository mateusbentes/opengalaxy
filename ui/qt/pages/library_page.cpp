#include "library_page.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QDebug>

#include "opengalaxy/util/config.h"
#include "../widgets/game_card.h"
#include "../widgets/notification_widget.h"
#include "../dialogs/game_details_dialog.h"
#include "../dialogs/game_information_dialog.h"

namespace opengalaxy {
namespace ui {

LibraryPage::LibraryPage(api::Session* session, QWidget* parent)
    : QWidget(parent)
    , session_(session)
    , gogClient_(session_, this)
    , libraryService_(&gogClient_, this)
    , runnerManager_(this)
    , installService_(this)
{
    // Set session for authenticated downloads
    installService_.setSession(session_);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->setSpacing(30);

    // Header
    QHBoxLayout* headerLayout = new QHBoxLayout();

    QLabel* titleLabel = new QLabel("My Library", this);
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #ffffff;
            font-size: 28px;
            font-weight: 700;
        }
    )");
    headerLayout->addWidget(titleLabel);

    headerLayout->addStretch();

    searchBox_ = new QLineEdit(this);
    searchBox_->setPlaceholderText("Search games...");
    searchBox_->setFixedWidth(300);
    searchBox_->setStyleSheet(R"(
        QLineEdit {
            background: #ffffff;
            border: 2px solid #d0cec9;
            border-radius: 8px;
            padding: 10px 16px;
            color: #3c3a37;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #9b4dca;
            background: #ffffff;
        }
        QLineEdit::placeholder {
            color: #8a8884;
        }
    )");
    headerLayout->addWidget(searchBox_);

    // Connect search box to filter function
    connect(searchBox_, &QLineEdit::textChanged, this, &LibraryPage::filterGames);

    mainLayout->addLayout(headerLayout);

    // Scroll area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* scrollWidget = new QWidget(scrollArea);
    gameGrid = new QGridLayout(scrollWidget);
    gameGrid->setSpacing(30);  // Spacing between cards
    gameGrid->setContentsMargins(0, 0, 0, 20);  // Add bottom margin
    gameGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);  // Align to top-left

    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea, 1);  // Give scroll area stretch factor

    // Background
    setStyleSheet(R"(
        LibraryPage {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a0f2e, stop:1 #2d1b4e);
        }
    )");
}

LibraryPage::~LibraryPage() = default;

void LibraryPage::refreshLibrary(bool forceRefresh)
{
    // Only load if authenticated
    if (!session_ || !session_->isAuthenticated()) {
        qDebug() << "LibraryPage::refreshLibrary() - Session not authenticated";
        return;
    }

    // Prevent double-loading
    if (isLoading_) {
        qDebug() << "LibraryPage::refreshLibrary() - Already loading, skipping";
        return;
    }

    qDebug() << "LibraryPage::refreshLibrary() - Loading library (forceRefresh=" << forceRefresh << ")";
    isLoading_ = true;

    // Load library
    libraryService_.fetchLibrary(forceRefresh, [this](opengalaxy::util::Result<std::vector<api::GameInfo>> result) {
        isLoading_ = false;  // Reset loading flag
        
        if (!result.isOk()) {
            qDebug() << "LibraryPage - Failed to load library:" << result.errorMessage();
            QMessageBox::warning(this, "Library", result.errorMessage());
            return;
        }

        qDebug() << "LibraryPage - Loaded" << result.value().size() << "games";
        
        // Store all games for filtering (deduplicate by ID)
        allGames_.clear();
        QSet<QString> seenIds;
        
        for (const auto& game : result.value()) {
            // Skip duplicates (same game ID)
            if (seenIds.contains(game.id)) {
                qDebug() << "  - Skipping duplicate:" << game.title << "(" << game.id << ")";
                continue;
            }
            
            seenIds.insert(game.id);
            allGames_.append(game);
            qDebug() << "  -" << game.title << "(" << game.id << ")" << game.platform;
        }

        qDebug() << "LibraryPage - After deduplication:" << allGames_.size() << "unique games";

        // Clear existing cards completely
        qDebug() << "Clearing" << cardsById_.size() << "existing cards";
        
        // First, hide and remove all card widgets
        for (auto it = cardsById_.begin(); it != cardsById_.end(); ++it) {
            GameCard* card = it.value();
            gameGrid->removeWidget(card);  // Remove from layout first
            card->setVisible(false);       // Hide immediately
            card->setParent(nullptr);      // Remove parent relationship
            delete card;                   // Delete immediately
        }
        cardsById_.clear();
        
        // Then clear any remaining layout items
        while (QLayoutItem* item = gameGrid->takeAt(0)) {
            delete item;
        }
        
        // Force layout update
        gameGrid->update();
        gameGrid->parentWidget()->update();
        
        qDebug() << "All cards cleared";

        // Create cards for all games
        for (const auto& game : allGames_) {
            auto* card = new GameCard(game.id, game.title, game.platform, game.coverUrl, gameGrid->parentWidget());
            card->setInstalled(game.isInstalled);
            card->show();  // Explicitly show the card

            connect(card, &GameCard::detailsRequested, this, &LibraryPage::openGameDetails);
            connect(card, &GameCard::playRequested, this, &LibraryPage::launchGame);
            connect(card, &GameCard::installRequested, this, &LibraryPage::installGame);
            connect(card, &GameCard::cancelInstallRequested, this, &LibraryPage::cancelInstall);
            connect(card, &GameCard::updateRequested, this, &LibraryPage::updateGame);
            connect(card, &GameCard::informationRequested, this, &LibraryPage::showGameInformation);
            connect(card, &GameCard::propertiesRequested, this, &LibraryPage::openGameProperties);

            cardsById_.insert(game.id, card);
            
            // Check for updates if game is installed
            if (game.isInstalled) {
                checkForUpdate(game.id);
            }
        }

        qDebug() << "Created" << cardsById_.size() << "game cards";

        // Update grid layout with all cards
        updateGridLayout();
        
        qDebug() << "Grid layout updated";
        
        // Apply current search filter if any
        if (searchBox_ && !searchBox_->text().isEmpty()) {
            filterGames(searchBox_->text());
        } else {
            qDebug() << "No search filter, showing all" << cardsById_.size() << "games";
        }
    });

    // Install progress
    connect(&installService_, &install::InstallService::installStarted, this, [this](const QString& gameId) {
        if (cardsById_.contains(gameId)) {
            cardsById_[gameId]->setInstalling(true);
            cardsById_[gameId]->setInstallProgress(0);
        }
        NotificationWidget::showToast("Installing...", this);
    });

    connect(&installService_, &install::InstallService::installProgress, this, [this](const QString& gameId, int percentage) {
        if (cardsById_.contains(gameId)) {
            cardsById_[gameId]->setInstallProgress(percentage);
        }
    });

    connect(&installService_, &install::InstallService::installCompleted, this, [this](const QString& gameId, const QString& installPath) {
        if (cardsById_.contains(gameId)) {
            cardsById_[gameId]->setInstalling(false);
            cardsById_[gameId]->setInstalled(true);
        }
        libraryService_.updateGameInstallation(gameId, installPath, "");
        NotificationWidget::showToast("Install completed", this);
    });

    connect(&installService_, &install::InstallService::installFailed, this, [this](const QString& gameId, const QString& error) {
        if (cardsById_.contains(gameId)) {
            cardsById_[gameId]->setInstalling(false);
            cardsById_[gameId]->setInstallProgress(0);
        }
        NotificationWidget::showToast("Install failed: " + error, this);
    });
}

void LibraryPage::openGameDetails(const QString& gameId)
{
    libraryService_.getGame(gameId, [this](auto result) {
        if (!result.isOk()) {
            QMessageBox::warning(this, "Error", result.errorMessage());
            return;
        }
        GameDetailsDialog* dlg = new GameDetailsDialog(result.value(), &libraryService_, &runnerManager_, this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
    });
}

void LibraryPage::openGameProperties(const QString& gameId)
{
    qDebug() << "Opening properties for game:" << gameId;
    libraryService_.getGame(gameId, [this](auto result) {
        if (!result.isOk()) {
            qDebug() << "Error getting game:" << result.errorMessage();
            QMessageBox::warning(this, "Error", result.errorMessage());
            return;
        }
        qDebug() << "Got game info:" << result.value().title;
        GameDetailsDialog* dlg = new GameDetailsDialog(result.value(), &libraryService_, &runnerManager_, this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
    });
}

void LibraryPage::showGameInformation(const QString& gameId)
{
    qDebug() << "Opening information for game:" << gameId;
    libraryService_.getGame(gameId, [this](auto result) {
        if (!result.isOk()) {
            qDebug() << "Error getting game:" << result.errorMessage();
            QMessageBox::warning(this, "Error", result.errorMessage());
            return;
        }
        qDebug() << "Got game info:" << result.value().title;
        GameInformationDialog* dlg = new GameInformationDialog(result.value(), this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
    });
}

void LibraryPage::launchGame(const QString& gameId)
{
    libraryService_.getGame(gameId, [this](auto result) {
        if (!result.isOk()) {
            QMessageBox::warning(this, "Error", result.errorMessage());
            return;
        }

        api::GameInfo game = result.value();
        if (game.installPath.isEmpty()) {
            QMessageBox::information(this, "Not installed", "Game is not installed yet.");
            return;
        }

        runners::LaunchConfig cfg;
        cfg.gamePath = game.installPath;
        cfg.workingDirectory = QFileInfo(game.installPath).absolutePath();
        cfg.arguments = {}; // Game arguments (can be extended later)
        cfg.environment = game.extraEnvironment;

        const QString p = game.platform.toLower();
        if (p.contains("windows")) cfg.gamePlatform = runners::Platform::Windows;
        else if (p.contains("mac")) cfg.gamePlatform = runners::Platform::MacOS;
        else cfg.gamePlatform = runners::Platform::Linux;

        // Detect game architecture from binary
        cfg.gameArch = runners::Runner::detectArchitecture(game.installPath);

        // Wire up custom runner settings
        cfg.runnerExecutableOverride = game.runnerExecutable.trimmed();
        cfg.runnerArguments = game.runnerArguments; // Already a QStringList

        runners::Runner* runner = nullptr;
        if (!game.preferredRunner.trimmed().isEmpty()) {
            runner = runnerManager_.getRunner(game.preferredRunner.trimmed());
        }
        if (!runner) {
            runner = runnerManager_.findBestRunner(cfg);
        }

        if (!runner) {
            QMessageBox::warning(this, "No runner", "No suitable runner found for this game.");
            return;
        }

        auto proc = runner->launch(cfg);
        if (!proc) {
            QMessageBox::warning(this, "Launch failed", "Failed to start game process.");
            return;
        }

        proc.release();
    });
}

void LibraryPage::installGame(const QString& gameId)
{
    // Use default installation directory: ~/Games/OpenGalaxy
    QString homeDir = QDir::homePath();
    QDir dir(homeDir);
    
    // Create Games directory if it doesn't exist
    if (!dir.exists("Games")) {
        if (!dir.mkdir("Games")) {
            QMessageBox::warning(this, "Install Error", 
                "Failed to create Games directory in home folder");
            return;
        }
    }
    dir.cd("Games");
    
    // Create OpenGalaxy directory if it doesn't exist
    if (!dir.exists("OpenGalaxy")) {
        if (!dir.mkdir("OpenGalaxy")) {
            QMessageBox::warning(this, "Install Error", 
                "Failed to create OpenGalaxy directory");
            return;
        }
    }
    
    const QString installDir = dir.absoluteFilePath("OpenGalaxy");

    gogClient_.fetchGameDownloads(gameId, [this, installDir, gameId](opengalaxy::util::Result<api::GameInfo> result) {
        if (!result.isOk()) {
            QMessageBox::warning(this, "Install", result.errorMessage());
            return;
        }

        api::GameInfo game = result.value();

        // Keep title/platform from cached library row
        libraryService_.getGame(gameId, [this, installDir, game, gameId](auto cached) mutable {
            if (cached.isOk()) {
                game.platform = cached.value().platform;
                game.title = cached.value().title;
            }

            // Set up progress callback
            auto progressCallback = [this, gameId](const install::InstallService::InstallProgress& progress) {
                if (cardsById_.contains(gameId)) {
                    cardsById_[gameId]->setInstallProgress(progress.percentage);
                }
            };

            // Set up completion callback (signals handle the rest)
            auto completionCallback = [](util::Result<QString> result) {
                // Signals installCompleted/installFailed are already emitted by InstallService
                Q_UNUSED(result);
            };

            installService_.installGame(game, installDir, progressCallback, completionCallback);
        });
    });
}

void LibraryPage::cancelInstall(const QString& gameId)
{
    installService_.cancelInstallation(gameId);
    if (cardsById_.contains(gameId)) {
        cardsById_[gameId]->setInstalling(false);
        cardsById_[gameId]->setUpdating(false);
        cardsById_[gameId]->setInstallProgress(0);
    }
    NotificationWidget::showToast("Install cancelled", this);
}

void LibraryPage::checkForUpdate(const QString& gameId)
{
    // Get current game info
    libraryService_.getGame(gameId, [this, gameId](auto result) {
        if (!result.isOk()) {
            return;
        }
        
        api::GameInfo currentGame = result.value();
        
        // Only check if game is installed and has a version
        if (!currentGame.isInstalled || currentGame.version.isEmpty()) {
            return;
        }
        
        // Fetch latest download info from GOG
        gogClient_.fetchGameDownloads(gameId, [this, gameId, currentGame](opengalaxy::util::Result<api::GameInfo> downloadResult) {
            if (!downloadResult.isOk()) {
                return;
            }
            
            api::GameInfo latestInfo = downloadResult.value();
            
            // Find the latest version from downloads
            QString latestVersion;
            for (const auto& download : latestInfo.downloads) {
                if (!download.version.isEmpty()) {
                    // Compare versions (simple string comparison for now)
                    if (latestVersion.isEmpty() || download.version > latestVersion) {
                        latestVersion = download.version;
                    }
                }
            }
            
            // Check if update is available
            if (!latestVersion.isEmpty() && latestVersion != currentGame.version) {
                qDebug() << "Update available for" << currentGame.title 
                         << "- Current:" << currentGame.version 
                         << "Latest:" << latestVersion;
                
                if (cardsById_.contains(gameId)) {
                    cardsById_[gameId]->setUpdateAvailable(true, latestVersion);
                }
            }
        });
    });
}

void LibraryPage::updateGame(const QString& gameId)
{
    // Get current game info
    libraryService_.getGame(gameId, [this, gameId](auto result) {
        if (!result.isOk()) {
            QMessageBox::warning(this, "Update Error", result.errorMessage());
            return;
        }
        
        api::GameInfo currentGame = result.value();
        
        if (!currentGame.isInstalled) {
            QMessageBox::information(this, "Update", "Game is not installed.");
            return;
        }
        
        // Mark as updating
        if (cardsById_.contains(gameId)) {
            cardsById_[gameId]->setUpdating(true);
            cardsById_[gameId]->setInstallProgress(0);
        }
        
        NotificationWidget::showToast("Updating game...", this);
        
        // Use the existing install directory
        QString installDir = QFileInfo(currentGame.installPath).absolutePath();
        
        // Fetch latest downloads
        gogClient_.fetchGameDownloads(gameId, [this, installDir, gameId, currentGame](opengalaxy::util::Result<api::GameInfo> result) {
            if (!result.isOk()) {
                if (cardsById_.contains(gameId)) {
                    cardsById_[gameId]->setUpdating(false);
                }
                QMessageBox::warning(this, "Update Error", result.errorMessage());
                return;
            }
            
            api::GameInfo game = result.value();
            
            // Keep title/platform from current game
            game.platform = currentGame.platform;
            game.title = currentGame.title;
            
            // Set up progress callback
            auto progressCallback = [this, gameId](const install::InstallService::InstallProgress& progress) {
                if (cardsById_.contains(gameId)) {
                    cardsById_[gameId]->setInstallProgress(progress.percentage);
                }
            };
            
            // Set up completion callback
            auto completionCallback = [this, gameId](util::Result<QString> result) {
                if (cardsById_.contains(gameId)) {
                    cardsById_[gameId]->setUpdating(false);
                    
                    if (result.isOk()) {
                        cardsById_[gameId]->setUpdateAvailable(false);
                        NotificationWidget::showToast("Update completed", this);
                        
                        // Refresh game info to get new version
                        checkForUpdate(gameId);
                    } else {
                        NotificationWidget::showToast("Update failed: " + result.errorMessage(), this);
                    }
                }
            };
            
            // Use install service to download and install the update
            installService_.installGame(game, installDir, progressCallback, completionCallback);
        });
    });
}

void LibraryPage::filterGames(const QString& searchText)
{
    QString search = searchText.trimmed().toLower();
    
    // Load current setting
    showHiddenGames_ = opengalaxy::util::Config::instance().showHiddenGames();
    
    qDebug() << "Filtering games with search:" << search << "showHiddenGames:" << showHiddenGames_;
    qDebug() << "Total cards:" << cardsById_.size();
    
    int visibleCount = 0;
    
    // Show/hide cards based on search and hidden status
    for (auto it = cardsById_.begin(); it != cardsById_.end(); ++it) {
        GameCard* card = it.value();
        
        // Find the game info
        bool isHidden = false;
        QString gameTitle;
        for (const auto& game : allGames_) {
            if (game.id == it.key()) {
                gameTitle = game.title;
                isHidden = game.hiddenInLibrary;
                break;
            }
        }
        
        // Check if game should be visible
        bool shouldShow = true;
        
        // Check hidden status
        if (isHidden && !showHiddenGames_) {
            shouldShow = false;
        }
        
        // Check search filter
        if (shouldShow && !search.isEmpty()) {
            shouldShow = gameTitle.toLower().contains(search);
        }
        
        card->setVisible(shouldShow);
        if (shouldShow) {
            if (!search.isEmpty()) {
                qDebug() << "  Match:" << gameTitle;
            }
            visibleCount++;
        }
    }
    
    qDebug() << "Visible cards after filter:" << visibleCount;
    
    // Update grid layout to reflow visible cards
    updateGridLayout();
}

void LibraryPage::updateGridLayout()
{
    qDebug() << "updateGridLayout() called";
    qDebug() << "Total games in allGames_:" << allGames_.size();
    qDebug() << "Total cards in cardsById_:" << cardsById_.size();
    
    // Remove all widgets from grid (but don't delete them)
    QList<GameCard*> visibleCards;
    
    // Collect visible cards in order
    for (const auto& game : allGames_) {
        if (cardsById_.contains(game.id)) {
            GameCard* card = cardsById_[game.id];
            if (card->isVisible()) {
                visibleCards.append(card);
            }
        }
    }
    
    qDebug() << "Visible cards to add to grid:" << visibleCards.size();
    
    // Remove all items from layout
    while (QLayoutItem* item = gameGrid->takeAt(0)) {
        delete item;  // Delete the layout item, not the widget
    }
    
    // Clear all row/column stretches
    for (int i = 0; i < 100; ++i) {
        gameGrid->setRowStretch(i, 0);
        gameGrid->setColumnStretch(i, 0);
    }
    
    // Re-add visible cards to grid
    int row = 0;
    int col = 0;
    const int columns = 3;  // 3 cards per row
    
    for (GameCard* card : visibleCards) {
        gameGrid->addWidget(card, row, col, Qt::AlignTop | Qt::AlignLeft);
        qDebug() << "  Added card at row" << row << "col" << col;
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
    
    qDebug() << "Grid now has" << (row * columns + col) << "cards in" << (row + 1) << "rows";
    
    // Add vertical stretch after last row to push cards to top
    gameGrid->setRowStretch(row + 1, 1);
}

} // namespace ui
} // namespace opengalaxy
