#include "game_card.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>

GameCard::GameCard(const QString &gameName, QWidget *parent)
    : QWidget(parent), gameTitle(nullptr)
{
    setFixedSize(280, 380);
    setStyleSheet(
        R"(
        GameCard {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 rgba(255,255,255,0.1), stop:1 rgba(50,50,80,0.4));
            border-radius: 16px;
            backdrop-filter: blur(20px);
        }
        
        QLabel#gameTitle {
            color: #ffffff;
            font-size: 18px;
            font-weight: 700;
            padding: 20px;
            background: rgba(0,0,0,0.3);
            border-radius: 0 0 16px 16px;
        }
        
        QLabel#playButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #7c4dff, stop:1 #5a3aff);
            color: white;
            padding: 12px 24px;
            border-radius: 25px;
            font-weight: 600;
        }
        )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // Cover image placeholder
    coverImage = new QLabel(this);
    coverImage->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #667eea, stop:1 #764ba2); border-radius: 16px 16px 0 0;");
    coverImage->setFixedHeight(280);
    coverImage->setAlignment(Qt::AlignCenter);

    gameTitle = new QLabel(gameName, this);
    gameTitle->setObjectName("gameTitle");
    gameTitle->setAlignment(Qt::AlignCenter);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(20, 0, 20, 20);
    
    playButton = new QLabel("PLAY", this);
    playButton->setObjectName("playButton");
    playButton->setAlignment(Qt::AlignCenter);
    playButton->setFixedSize(80, 36);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(playButton);
    buttonLayout->addStretch();

    layout->addWidget(coverImage);
    layout->addWidget(gameTitle);
    layout->addLayout(buttonLayout);

    setupAnimations();
}

void GameCard::setupAnimations()
{
    shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(25);
    shadowEffect->setColor(QColor(124, 77, 255, 0.4));
    shadowEffect->setOffset(0, 12);
    setGraphicsEffect(shadowEffect);

    hoverAnimation = new QPropertyAnimation(this, "geometry", this);
    hoverAnimation->setDuration(250);
    hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);

    shadowAnimation = new QPropertyAnimation(shadowEffect, "blurRadius", this);
    shadowAnimation->setDuration(250);
}

void GameCard::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    shadowEffect->setBlurRadius(35);
    shadowEffect->setColor(QColor(124, 77, 255, 0.6));
}

void GameCard::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    shadowEffect->setBlurRadius(25);
    shadowEffect->setColor(QColor(124, 77, 255, 0.4));
}
