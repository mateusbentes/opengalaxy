#include "game_card.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <algorithm>

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

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Cover
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

    // Action button
    actionButton_ = new QPushButton(coverContainer);
    actionButton_->setFixedSize(140, 45);
    actionButton_->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #7c4dff, stop:1 #5a3aff);
            border: none;
            border-radius: 8px;
            color: white;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #8c5dff, stop:1 #6a4aff);
        }
    )");
    actionButton_->hide();

    // Progress
    progressBar_ = new QProgressBar(coverContainer);
    progressBar_->setFixedSize(220, 10);
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    progressBar_->setTextVisible(false);
    progressBar_->setStyleSheet(R"(
        QProgressBar {
            background: rgba(0,0,0,0.35);
            border: none;
            border-radius: 5px;
        }
        QProgressBar::chunk {
            background: #00e676;
            border-radius: 5px;
        }
    )");
    progressBar_->hide();

    // Position overlay widgets
    actionButton_->move((coverContainer->width() - actionButton_->width()) / 2,
                        (coverContainer->height() - actionButton_->height()) / 2);
    progressBar_->move((coverContainer->width() - progressBar_->width()) / 2,
                       actionButton_->y() + actionButton_->height() + 12);

    connect(actionButton_, &QPushButton::clicked, this, [this]() {
        if (installing_) {
            emit cancelInstallRequested(gameId_);
            return;
        }
        if (installed_) {
            emit playRequested(gameId_);
        } else {
            emit installRequested(gameId_);
        }
    });

    mainLayout->addWidget(coverContainer);

    // Info
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

    // Shadow
    shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 100));
    shadowEffect->setOffset(0, 4);
    setGraphicsEffect(shadowEffect);

    setupAnimations();
    refreshButton();
}

GameCard::~GameCard() = default;

void GameCard::setInstalled(bool installed)
{
    installed_ = installed;
    refreshButton();
}

void GameCard::setInstalling(bool installing)
{
    installing_ = installing;
    refreshButton();
}

void GameCard::setInstallProgress(int percent)
{
    installProgress_ = std::clamp(percent, 0, 100);
    progressBar_->setValue(installProgress_);
    if (installing_) {
        progressBar_->show();
    }
}

void GameCard::refreshButton()
{
    if (installing_) {
        actionButton_->setText("CANCEL");
        progressBar_->show();
        return;
    }

    progressBar_->hide();

    if (installed_) {
        actionButton_->setText("▶ PLAY");
    } else {
        actionButton_->setText("⬇ INSTALL");
    }
}

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

    actionButton_->show();
    if (installing_) {
        progressBar_->show();
    }

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

    actionButton_->hide();
    progressBar_->hide();

    QWidget::leaveEvent(event);
}

} // namespace ui
} // namespace opengalaxy
