#include "game_details_dialog.h"

namespace opengalaxy {
namespace ui {

GameDetailsDialog::GameDetailsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Game Details");
    detailsLabel = new QLabel("Game details here", this);
    closeButton = new QPushButton("Close", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(detailsLabel);
    layout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, &GameDetailsDialog::onClose);
}

GameDetailsDialog::~GameDetailsDialog()
{
}

void GameDetailsDialog::onClose()
{
    accept();
}

} // namespace ui
} // namespace opengalaxy
