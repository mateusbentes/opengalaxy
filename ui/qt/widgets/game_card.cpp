#include "game_card.h"

#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QMouseEvent>

namespace opengalaxy {
namespace ui {

GameCard::GameCard(const QString& gameId,
                   const QString& title,
                   const QString& platform,
                   const QString& coverUrl,
                   QWidget* parent)
    : QWidget(parent)
    , gameId_(gameId)
{
    Q_UNUSED(coverUrl);

    setFixedSize(280, 400);
    setCursor(Qt::PointingHandCursor);

    // Main layout
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Cover image container
    auto* coverContainer = new QWidget(this);
    coverContainer->setFixedSize(280, 320);
    coverContainer->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #2d1b4e, stop:1 #3d2b5e);
            border-radius: 12px;
        }
    )");

    auto* coverLayout = new QVBoxLayout(coverContainer);
    coverLayout->setContentsMargins(0, 0, 0, 0);

    // Cover image (placeholder)
    coverImage = new QLabel(coverContainer);
    coverImage->setAlignment(Qt::AlignCenter);
    coverImage->setText("🎮");
    coverImage->setStyleSheet(R"(
        QLabel {
            font-size: 64px;
            color: rgba(255, 255, 255, 0.3);
            background: transparent;
        }
    )");
    coverLayout->addWidget(coverImage);

    // Play button overlay (hidden by default)
    playButton = new QPushButton("▶ PLAY", coverContainer);
    playButton->setFixedSize(120, 45);
    playButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00e676, stop:1 #00c853);
            border: none;
            border-radius: 8px;
            color: white;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00ff88, stop:1 #00d65f);
        }
    )");
    playButton->hide();
    connect(playButton, &QPushButton::clicked, this, [this]() { emit playRequested(gameId_); });

    // Position play button in center
    playButton->move((coverContainer->width() - playButton->width()) / 2,
                     (coverContainer->height() - playButton->height()) / 2);

    mainLayout->addWidget(coverContainer);

    // Info section
    auto* infoContainer = new QWidget(this);
    infoContainer->setFixedHeight(80);
    infoContainer->setStyleSheet(R"(
        QWidget {
            background: rgba(45, 27, 78, 0.6);
            border-bottom-left-radius: 12px;
            border-bottom-right-radius: 12px;
        }
    )");

    auto* infoLayout = new QVBoxLayout(infoContainer);
    infoLayout->setContentsMargins(16, 12, 16, 12);
    infoLayout->setSpacing(4);

    gameTitle = new QLabel(title, infoContainer);
    gameTitle->setStyleSheet(R"(
        QLabel {
            color: #ffffff;
            font-size: 16px;
            font-weight: 600;
            background: transparent;
        }
    )");
    gameTitle->setWordWrap(true);
    gameTitle->setMaximumHeight(40);
    infoLayout->addWidget(gameTitle);

    platformLabel = new QLabel(platform, infoContainer);
    platformLabel->setStyleSheet(R"(
        QLabel {
            color: #b8b8d1;
            font-size: 12px;
            background: transparent;
        }
    )");
    infoLayout->addWidget(platformLabel);

    mainLayout->addWidget(infoContainer);

    // Card style
    setStyleSheet(R"(
        GameCard {
            background: transparent;
            border-radius: 12px;
        }
    )");

    // Shadow effect
    shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 100));
    shadowEffect->setOffset(0, 4);
    setGraphicsEffect(shadowEffect);

    setupAnimations();
}

GameCard::~GameCard() = default;

void GameCard::setupAnimations()
{
    hoverAnimation = new QPropertyAnimation(this, "pos");
    hoverAnimation->setDuration(200);
    hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);

    shadowAnimation = new QPropertyAnimation(shadowEffect, "blurRadius");
    shadowAnimation->setDuration(200);
    shadowAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void GameCard::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit detailsRequested(gameId_);
}

void GameCard::enterEvent(QEnterEvent* event)
{
    Q_UNUSED(event);

    QPoint currentPos = pos();
    hoverAnimation->setStartValue(currentPos);
    hoverAnimation->setEndValue(currentPos - QPoint(0, 8));
    hoverAnimation->start();

    shadowAnimation->setStartValue(20);
    shadowAnimation->setEndValue(35);
    shadowAnimation->start();

    playButton->show();

    QWidget::enterEvent(event);
}

void GameCard::leaveEvent(QEvent* event)
{
    QPoint currentPos = pos();
    hoverAnimation->setStartValue(currentPos);
    hoverAnimation->setEndValue(currentPos + QPoint(0, 8));
    hoverAnimation->start();

    shadowAnimation->setStartValue(35);
    shadowAnimation->setEndValue(20);
    shadowAnimation->start();

    playButton->hide();

    QWidget::leaveEvent(event);
}

} // namespace ui
} // namespace opengalaxy
