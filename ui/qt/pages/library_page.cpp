#include "library_page.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

LibraryPage::LibraryPage(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(
        R"(
        LibraryPage {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1e1a3a, stop:1 #2a1f4e);
            color: #e0e0ff;
        }
        
        QLabel {
            font-size: 28px;
            font-weight: 700;
            margin: 40px 60px 30px 60px;
        }
        )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QLabel *title = new QLabel("Your Library", this);
    mainLayout->addWidget(title);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { border: none; } QScrollBar:vertical { background: transparent; width: 12px; }");

    QWidget *scrollWidget = new QWidget();
    gridLayout = new QGridLayout(scrollWidget);
    gridLayout->setSpacing(25);
    gridLayout->setContentsMargins(60, 30, 60, 60);

    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);

    setupGrid();
}

void LibraryPage::setupGrid()
{
    QStringList games = {"Cyberpunk 2077", "Witcher 3", "DOOM Eternal", 
                        "Stardew Valley", "Hades", "Hollow Knight"};
    
    int row = 0, col = 0;
    for (const QString &game : games) {
        GameCard *card = new GameCard(game, this);
        gridLayout->addWidget(card, row, col);
        
        col++;
        if (col > 2) {
            col = 0;
            row++;
        }
    }
}
