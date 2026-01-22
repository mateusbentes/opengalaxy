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
    explicit GameCard(const QString& gameId,
                      const QString &title,
                      const QString &platform,
                      const QString &coverUrl,
                      QWidget *parent = nullptr);
    ~GameCard();

signals:
    void playRequested(const QString& gameId);
    void detailsRequested(const QString& gameId);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void setupAnimations();

    QString gameId_;

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
