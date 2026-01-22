#include "library_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include "../widgets/game_card.h"

namespace opengalaxy {
namespace ui {

LibraryPage::LibraryPage(QWidget *parent)
    : QWidget(parent)
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
    
    QWidget* scrollWidget = new QWidget();
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
    
    // Load demo games
    loadGames();
}

LibraryPage::~LibraryPage()
{
}

void LibraryPage::loadGames()
{
    // Demo games data
    struct GameData {
        QString title;
        QString platform;
        QString coverUrl;
    };
    
    QList<GameData> demoGames = {
        {"Cyberpunk 2077", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3.jpg"},
        {"The Witcher 3", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3.jpg"},
        {"Baldur's Gate 3", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3.jpg"},
        {"Stardew Valley", "Linux", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3.jpg"},
        {"Hollow Knight", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3.jpg"},
        {"Terraria", "Linux", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3.jpg"},
        {"Disco Elysium", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3.jpg"},
        {"Hades", "Windows", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3.jpg"},
        {"Celeste", "Linux", "https://images.gog.com/5f05cf8fc3f4b6e6d6b1c3c3c3c3c3c3.jpg"},
    };
    
    int row = 0;
    int col = 0;
    const int columns = 3;
    
    for (const auto& game : demoGames) {
        GameCard* card = new GameCard(game.title, game.platform, game.coverUrl, this);
        gameGrid->addWidget(card, row, col);
        
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
    
    // Add stretch to push cards to top
    gameGrid->setRowStretch(row + 1, 1);
}

} // namespace ui
} // namespace opengalaxy
