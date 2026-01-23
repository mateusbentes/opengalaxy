#include "game_card.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QUrl>
#include <QDebug>
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
    setFixedSize(420, 310);
    setCursor(Qt::PointingHandCursor);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Cover
    auto* coverContainer = new QWidget(this);
    coverContainer->setFixedSize(420, 220);
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
            font-size: 72px;
            color: rgba(255, 255, 255, 0.3);
            background: transparent;
        }
    )");
    coverImage->setScaledContents(false);  // Don't stretch, maintain aspect ratio
    coverImage->setFixedSize(420, 220);
    coverLayout->addWidget(coverImage);

    // Load cover image if URL is provided
    if (!coverUrl.isEmpty()) {
        loadCoverImage(coverUrl);
    }

    // Action button
    actionButton_ = new QPushButton(coverContainer);
    actionButton_->setFixedSize(160, 50);
    actionButton_->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #7c4dff, stop:1 #5a3aff);
            border: none;
            border-radius: 8px;
            color: white;
            font-size: 15px;
            font-weight: 700;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #8c5dff, stop:1 #6a4aff);
        }
    )");
    actionButton_->hide();

    // Progress bar with animated gradient effect
    progressBar_ = new QProgressBar(coverContainer);
    progressBar_->setFixedSize(240, 14);
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    progressBar_->setTextVisible(true);
    progressBar_->setFormat("%p%");
    progressBar_->setStyleSheet(R"(
        QProgressBar {
            background: rgba(0,0,0,0.5);
            border: 2px solid rgba(124, 77, 255, 0.3);
            border-radius: 7px;
            color: white;
            font-size: 10px;
            font-weight: bold;
            text-align: center;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00e676, stop:0.5 #00c853, stop:1 #00e676);
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
    infoContainer->setFixedHeight(90);
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
            font-size: 18px;
            font-weight: 600;
            background: transparent;
        }
    )");
    gameTitle->setWordWrap(true);
    gameTitle->setMaximumHeight(50);
    infoLayout->addWidget(gameTitle);

    platformLabel = new QLabel(platform, infoContainer);
    platformLabel->setStyleSheet(R"(
        QLabel {
            color: #b8b8d1;
            font-size: 14px;
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
    int newProgress = std::clamp(percent, 0, 100);
    
    // Animate progress bar smoothly
    if (!progressAnimation) {
        progressAnimation = new QPropertyAnimation(progressBar_, "value", this);
        progressAnimation->setDuration(300); // 300ms smooth transition
        progressAnimation->setEasingCurve(QEasingCurve::OutCubic);
    }
    
    progressAnimation->stop();
    progressAnimation->setStartValue(progressBar_->value());
    progressAnimation->setEndValue(newProgress);
    progressAnimation->start();
    
    installProgress_ = newProgress;
    
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

void GameCard::loadCoverImage(const QString& url)
{
    // Don't attempt to load if URL is empty or invalid
    if (url.isEmpty() || url.trimmed().isEmpty()) {
        return;
    }

    // Validate URL has a protocol
    QUrl qurl(url);
    if (!qurl.isValid() || qurl.scheme().isEmpty()) {
        qDebug() << "Invalid cover URL (no protocol):" << url;
        return;
    }

    // Create network manager
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    
    // Make request
    QNetworkRequest request(qurl);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    
    QNetworkReply* reply = manager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            // Only log if it's not an empty URL issue
            if (reply->error() != QNetworkReply::ProtocolUnknownError) {
                qDebug() << "Failed to load cover image:" << reply->errorString();
            }
            return;
        }
        
        QByteArray imageData = reply->readAll();
        QPixmap pixmap;
        
        if (pixmap.loadFromData(imageData)) {
            // Scale to fit within the cover container while maintaining aspect ratio
            QPixmap scaled = pixmap.scaled(420, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            coverImage->setPixmap(scaled);
            coverImage->setStyleSheet("background: transparent;"); // Remove placeholder styling
        } else {
            qDebug() << "Failed to parse cover image data";
        }
    });
}

} // namespace ui
} // namespace opengalaxy
