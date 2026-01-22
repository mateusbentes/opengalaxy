#ifndef GAME_CARD_H
#define GAME_CARD_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPushButton>

namespace opengalaxy {
namespace ui {

class GameCard : public QWidget
{
    Q_OBJECT

public:
    explicit GameCard(const QString &title, const QString &platform, const QString &coverUrl, QWidget *parent = nullptr);
    ~GameCard();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setupAnimations();
    
    QLabel *coverImage;
    QLabel *gameTitle;
    QLabel *platformLabel;
    QPushButton *playButton;
    QGraphicsDropShadowEffect *shadowEffect;
    QPropertyAnimation *hoverAnimation;
    QPropertyAnimation *shadowAnimation;
};

} // namespace ui
} // namespace opengalaxy

#endif // GAME_CARD_H
