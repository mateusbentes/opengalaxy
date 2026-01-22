#ifndef GAME_CARD_H
#define GAME_CARD_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QLabel>

class GameCard : public QWidget
{
    Q_OBJECT

public:
    explicit GameCard(const QString &gameName, QWidget *parent = nullptr);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setupAnimations();
    
    QLabel *coverImage;
    QLabel *gameTitle;
    QLabel *playButton;
    QGraphicsDropShadowEffect *shadowEffect;
    QPropertyAnimation *hoverAnimation;
    QPropertyAnimation *shadowAnimation;
};

#endif // GAME_CARD_H
