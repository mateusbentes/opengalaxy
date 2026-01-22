#ifndef GAME_CARD_H
#define GAME_CARD_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

namespace opengalaxy {
namespace ui {

class GameCard : public QWidget
{
    Q_OBJECT

public:
    enum class ActionState {
        Install,
        Installing,
        Play
    };

    explicit GameCard(const QString& gameId,
                      const QString& title,
                      const QString& platform,
                      const QString& coverUrl,
                      QWidget* parent = nullptr);
    ~GameCard();

    void setInstalled(bool installed);
    void setInstalling(bool installing);
    void setInstallProgress(int percent);

signals:
    void playRequested(const QString& gameId);
    void detailsRequested(const QString& gameId);
    void installRequested(const QString& gameId);
    void cancelInstallRequested(const QString& gameId);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void setupAnimations();
    void refreshButton();

    QString gameId_;

    bool installed_ = false;
    bool installing_ = false;
    int installProgress_ = 0;

    QLabel* coverImage = nullptr;
    QLabel* gameTitle = nullptr;
    QLabel* platformLabel = nullptr;

    QPushButton* actionButton_ = nullptr;
    QProgressBar* progressBar_ = nullptr;

    QGraphicsDropShadowEffect* shadowEffect = nullptr;
    QPropertyAnimation* hoverAnimation = nullptr;
    QPropertyAnimation* shadowAnimation = nullptr;
};

} // namespace ui
} // namespace opengalaxy

#endif // GAME_CARD_H
