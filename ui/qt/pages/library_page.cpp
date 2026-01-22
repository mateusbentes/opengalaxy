#include "library_page.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QMessageBox>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "../widgets/game_card.h"
#include "../dialogs/game_details_dialog.h"

namespace opengalaxy {
namespace ui {

LibraryPage::LibraryPage(QWidget* parent)
    : QWidget(parent)
    , session_(this)
    , gogClient_(&session_, this)
    , libraryService_(&gogClient_, this)
    , runnerManager_(this)
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

    // Search box
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

    // Scroll area for games
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

    // Set page background
    setStyleSheet(R"(
        LibraryPage {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a0f2e, stop:1 #2d1b4e);
        }
    )");

    loadGames();
}

LibraryPage::~LibraryPage() = default;

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
            QMessageBox::information(this, "Not installed", "This demo entry has no install path set.");
            return;
        }

        runners::LaunchConfig cfg;
        cfg.gamePath = game.installPath;
        cfg.workingDirectory = QFileInfo(game.installPath).absolutePath();
        cfg.arguments = {}; // game args not modeled yet
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

void LibraryPage::loadGames()
{
    struct GameData {
        QString id;
        QString title;
        QString platform;
        QString coverUrl;
        QString installPath;
    };

    QList<GameData> demoGames = {
        {"gog_cyberpunk_2077", "Cyberpunk 2077", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3c3.jpg", ""},
        {"gog_witcher_3", "The Witcher 3", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3c3.jpg", ""},
        {"steam_baldurs_gate_3", "Baldur's Gate 3", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3c3.jpg", ""},
        {"gog_stardew_valley", "Stardew Valley", "Linux", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3c3.jpg", ""},
        {"steam_hollow_knight", "Hollow Knight", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3c3.jpg", ""},
        {"gog_terraria", "Terraria", "Linux", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3c3.jpg", ""},
        {"gog_disco_elysium", "Disco Elysium", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3c3.jpg", ""},
        {"gog_hades", "Hades", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3c3.jpg", ""},
        {"gog_celeste", "Celeste", "Linux", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3c3.jpg", ""},
    };

    // Ensure demo IDs exist in DB so properties can be saved.
    {
        QSqlDatabase db = QSqlDatabase::database("library");
        if (db.isOpen()) {
            QSqlQuery q(db);
            for (const auto& g : demoGames) {
                q.prepare("INSERT OR IGNORE INTO games (id, title, platform, coverUrl) VALUES (?, ?, ?, ?)");
                q.addBindValue(g.id);
                q.addBindValue(g.title);
                q.addBindValue(g.platform);
                q.addBindValue(g.coverUrl);
                q.exec();
            }
        }
    }

    int row = 0;
    int col = 0;
    const int columns = 3;

    for (const auto& game : demoGames) {
        auto* card = new GameCard(game.id, game.title, game.platform, game.coverUrl, this);
        connect(card, &GameCard::detailsRequested, this, &LibraryPage::openGameDetails);
        connect(card, &GameCard::playRequested, this, &LibraryPage::launchGame);
        gameGrid->addWidget(card, row, col);

        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }

    gameGrid->setRowStretch(row + 1, 1);
}

} // namespace ui
} // namespace opengalaxy
