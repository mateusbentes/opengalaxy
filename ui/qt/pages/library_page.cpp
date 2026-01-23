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

#include "../widgets/game_card.h"
#include "../widgets/notification_widget.h"
#include "../dialogs/game_details_dialog.h"

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

    QLineEdit* searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("Search games...");
    searchBox->setFixedWidth(300);
    searchBox->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.08);
            border: 2px solid rgba(124, 77, 255, 0.3);
            border-radius: 8px;
            padding: 10px 16px;
            color: #ffffff;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #7c4dff;
        }
        QLineEdit::placeholder {
            color: rgba(255, 255, 255, 0.4);
        }
    )");
    headerLayout->addWidget(searchBox);

    mainLayout->addLayout(headerLayout);

    // Scroll area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    QWidget* scrollWidget = new QWidget(scrollArea);
    gameGrid = new QGridLayout(scrollWidget);
    gameGrid->setSpacing(20);
    gameGrid->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
    mainLayout->addStretch(1);

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

    qDebug() << "LibraryPage::refreshLibrary() - Loading library (forceRefresh=" << forceRefresh << ")";

    // Load library
    libraryService_.fetchLibrary(forceRefresh, [this](opengalaxy::util::Result<std::vector<api::GameInfo>> result) {
        if (!result.isOk()) {
            qDebug() << "LibraryPage - Failed to load library:" << result.errorMessage();
            QMessageBox::warning(this, "Library", result.errorMessage());
            return;
        }

        qDebug() << "LibraryPage - Loaded" << result.value().size() << "games";
        for (const auto& game : result.value()) {
            qDebug() << "  -" << game.title << "(" << game.id << ")" << game.platform;
        }

        // Clear grid
        cardsById_.clear();
        while (QLayoutItem* item = gameGrid->takeAt(0)) {
            if (QWidget* w = item->widget()) w->deleteLater();
            delete item;
        }

        int row = 0;
        int col = 0;
        const int columns = 3;

        for (const auto& game : result.value()) {
            auto* card = new GameCard(game.id, game.title, game.platform, game.coverUrl, this);
            card->setInstalled(game.isInstalled);

            connect(card, &GameCard::detailsRequested, this, &LibraryPage::openGameDetails);
            connect(card, &GameCard::playRequested, this, &LibraryPage::launchGame);
            connect(card, &GameCard::installRequested, this, &LibraryPage::installGame);
            connect(card, &GameCard::cancelInstallRequested, this, &LibraryPage::cancelInstall);

            cardsById_.insert(game.id, card);
            gameGrid->addWidget(card, row, col);

            col++;
            if (col >= columns) {
                col = 0;
                row++;
            }
        }

        gameGrid->setRowStretch(row + 1, 1);
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
        GameDetailsDialog dlg(result.value(), &libraryService_, &runnerManager_, this);
        dlg.exec();
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
        cfg.arguments = {};
        cfg.environment = game.extraEnvironment;

        const QString p = game.platform.toLower();
        if (p.contains("windows")) cfg.gamePlatform = runners::Platform::Windows;
        else if (p.contains("mac")) cfg.gamePlatform = runners::Platform::MacOS;
        else cfg.gamePlatform = runners::Platform::Linux;

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
    const QString installDir = QFileDialog::getExistingDirectory(this, "Choose install folder");
    if (installDir.isEmpty()) return;

    gogClient_.fetchGameDownloads(gameId, [this, installDir, gameId](opengalaxy::util::Result<api::GameInfo> result) {
        if (!result.isOk()) {
            QMessageBox::warning(this, "Install", result.errorMessage());
            return;
        }

        api::GameInfo game = result.value();

        // Keep title/platform from cached library row
        libraryService_.getGame(gameId, [this, installDir, game](auto cached) mutable {
            if (cached.isOk()) {
                game.platform = cached.value().platform;
                game.title = cached.value().title;
            }
            installService_.installGame(game, installDir, nullptr, [](util::Result<QString>) {});
        });
    });
}

void LibraryPage::cancelInstall(const QString& gameId)
{
    installService_.cancelInstallation(gameId);
    if (cardsById_.contains(gameId)) {
        cardsById_[gameId]->setInstalling(false);
        cardsById_[gameId]->setInstallProgress(0);
    }
    NotificationWidget::showToast("Install cancelled", this);
}

} // namespace ui
} // namespace opengalaxy
