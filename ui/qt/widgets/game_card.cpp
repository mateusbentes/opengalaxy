#include "game_card.h"

namespace opengalaxy {
namespace ui {

GameCard::GameCard(QWidget *parent)
    : QWidget(parent)
{
    titleLabel = new QLabel("Game Title", this);
    iconLabel = new QLabel(this);
    playButton = new QPushButton("Play", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(playButton);

    connect(playButton, &QPushButton::clicked, this, &GameCard::onPlayClicked);
}

GameCard::~GameCard()
{
}

void GameCard::onPlayClicked()
{
}

} // namespace ui
} // namespace opengalaxy
