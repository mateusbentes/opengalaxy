#ifndef GAME_CARD_H
#define GAME_CARD_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace opengalaxy {
namespace ui {

class GameCard : public QWidget
{
    Q_OBJECT

public:
    explicit GameCard(QWidget *parent = nullptr);
    ~GameCard();

private slots:
    void onPlayClicked();

private:
    QLabel *titleLabel;
    QLabel *iconLabel;
    QPushButton *playButton;
};

} // namespace ui
} // namespace opengalaxy

#endif // GAME_CARD_H
