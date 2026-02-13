#include "game_card.h"

#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>
#include <QRandomGenerator>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <algorithm>

namespace opengalaxy {
namespace ui {

GameCard::GameCard(const QString &gameId, const QString &title, const QString &platform,
                   const QString &coverUrl, const QDateTime &releaseDate, QWidget *parent)
    : QWidget(parent), gameId_(gameId) {
    // Check if game is unreleased (release date is in the future)
    // Also consider games with no downloads as potentially unreleased
    bool isUnreleased = releaseDate.isValid() && releaseDate > QDateTime::currentDateTime();
    setUnreleased(isUnreleased);
    setFixedSize(420, 310);
    setCursor(Qt::PointingHandCursor);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Cover
    auto *coverContainer = new QWidget(this);
    coverContainer->setFixedSize(420, 220);
    coverContainer->setStyleSheet(R"(
        QWidget {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #2d1b4e, stop:1 #3d2b5e);
                border-radius: 12px;
        }
    )");

    auto *coverLayout = new QVBoxLayout(coverContainer);
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
    coverImage->setScaledContents(false); // Don't stretch, maintain aspect ratio
    coverImage->setFixedSize(420, 220);
    coverLayout->addWidget(coverImage);

    // Load cover image if URL is provided (with a small delay to prevent overwhelming the network)
    if (!coverUrl.isEmpty()) {
        // Use a timer to stagger image loading
        QTimer::singleShot(QRandomGenerator::global()->bounded(100, 500), this,
                           [this, coverUrl]() { loadCoverImage(coverUrl); });
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

    // Update button
    updateButton_ = new QPushButton(coverContainer);
    updateButton_->setFixedSize(76, 50);
    updateButton_->setStyleSheet(R"(
        QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #ff9800, stop:1 #f57c00);
                border: none;
                border-radius: 8px;
                color: white;
                font-size: 13px;
                font-weight: 700;
        }
        QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #ffa726, stop:1 #fb8c00);
        }
    )");
    updateButton_->setText("⬆ UPD");
    updateButton_->hide();

    // Repair button
    repairButton_ = new QPushButton(coverContainer);
    repairButton_->setFixedSize(76, 50);
    repairButton_->setStyleSheet(R"(
        QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #e91e63, stop:1 #c2185b);
                border: none;
                border-radius: 8px;
                color: white;
                font-size: 13px;
                font-weight: 700;
        }
        QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #f06292, stop:1 #ec407a);
        }
    )");
    repairButton_->setText("🔧 REP");
    repairButton_->hide();

    // Progress bar with animated gradient effect
    progressBar_ = new QProgressBar(coverContainer);
    progressBar_->setFixedSize(240, 14);
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    progressBar_->setTextVisible(true);
    progressBar_->setFormat("%p%");
    progressBar_->setStyleSheet(R"(
        QProgressBar {
                background: rgba(0, 0, 0, 0.5);
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
                        (coverContainer->height() - actionButton_->height()) / 2 - 30);

    // Position Update and Repair buttons side-by-side
    int buttonY = actionButton_->y() + actionButton_->height() + 10;
    int totalWidth = 160 + 160 + 8; // Two buttons + spacing
    int startX = (coverContainer->width() - totalWidth) / 2;

    updateButton_->move(startX, buttonY);
    repairButton_->move(startX + 160 + 8, buttonY);

    progressBar_->move((coverContainer->width() - progressBar_->width()) / 2, buttonY + 50 + 12);

    connect(actionButton_, &QPushButton::clicked, this, [this]() {
        if (installing_ || updating_) {
            emit cancelInstallRequested(gameId_);
            return;
        }
        if (installed_) {
            emit playRequested(gameId_);
        } else {
            emit installRequested(gameId_);
        }
    });

    connect(updateButton_, &QPushButton::clicked, this,
            [this]() { emit updateRequested(gameId_); });

    connect(repairButton_, &QPushButton::clicked, this,
            [this]() { emit repairRequested(gameId_); });

    mainLayout->addWidget(coverContainer);

    // Info
    auto *infoContainer = new QWidget(this);
    infoContainer->setFixedHeight(90);
    infoContainer->setStyleSheet(R"(
        QWidget {
                background: rgba(45, 27, 78, 0.6);
                border-bottom-left-radius: 12px;
                border-bottom-right-radius: 12px;
        }
    )");

    auto *infoLayout = new QVBoxLayout(infoContainer);
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

void GameCard::setInstalled(bool installed) {
    installed_ = installed;
    refreshButton();
}

void GameCard::setInstalling(bool installing) {
    installing_ = installing;
    refreshButton();
}

void GameCard::setInstallProgress(int percent) {
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

    if (installing_ || updating_) {
        progressBar_->show();
    }
}

void GameCard::setUpdateAvailable(bool available, const QString &newVersion) {
    updateAvailable_ = available;
    newVersion_ = newVersion;
    refreshButton();
}

void GameCard::setUpdating(bool updating) {
    updating_ = updating;
    refreshButton();
}

void GameCard::refreshButton() {
    if (installing_ || updating_ || repairing_) {
        actionButton_->setText("CANCEL");
        updateButton_->hide();
        repairButton_->hide();
        progressBar_->show();
        return;
    }

    progressBar_->hide();

    if (installed_) {
        actionButton_->setText("▶ PLAY");
        if (updateAvailable_) {
            updateButton_->show();
            repairButton_->hide();
            if (!newVersion_.isEmpty()) {
                updateButton_->setToolTip(tr("Update to version %1").arg(newVersion_));
            }
        } else if (repairNeeded_) {
            updateButton_->hide();
            repairButton_->show();
            repairButton_->setToolTip(tr("Repair corrupted game files"));
        } else {
            updateButton_->hide();
            repairButton_->hide();
        }
    } else {
        actionButton_->setText("⬇ INSTALL");
        updateButton_->hide();
        repairButton_->hide();
    }
}

void GameCard::setupAnimations() {
    hoverAnimation = new QPropertyAnimation(this, "pos");
    hoverAnimation->setDuration(200);
    hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);

    shadowAnimation = new QPropertyAnimation(shadowEffect, "blurRadius");
    shadowAnimation->setDuration(200);
    shadowAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void GameCard::mouseDoubleClickEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    emit detailsRequested(gameId_);
}

void GameCard::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu;
    menu.setStyleSheet(R"(
        QMenu {
                background: #f8f7f5;
                color: #3c3a37;
                border: 1px solid #e8e6e3;
                border-radius: 6px;
                padding: 4px;
        }
        QMenu::item {
                padding: 8px 16px;
                border-radius: 4px;
                color: #3c3a37;
        }
        QMenu::item:hover {
                background: #e8e6e3;
                color: #3c3a37;
        }
        QMenu::item:selected {
                background: #6c5ce7;
                color: white;
        }
    )");

    QAction *informationAction = menu.addAction(tr("Information"));
    QAction *propertiesAction = menu.addAction(tr("Properties"));

    QAction *selectedAction = menu.exec(event->globalPos());

    if (selectedAction == informationAction) {
        emit informationRequested(gameId_);
    } else if (selectedAction == propertiesAction) {
        emit propertiesRequested(gameId_);
    }
}

void GameCard::enterEvent(QEnterEvent *event) {
    Q_UNUSED(event);

    QPoint currentPos = pos();
    hoverAnimation->setStartValue(currentPos);
    hoverAnimation->setEndValue(currentPos - QPoint(0, 8));
    hoverAnimation->start();

    shadowAnimation->setStartValue(20);
    shadowAnimation->setEndValue(35);
    shadowAnimation->start();

    actionButton_->show();
    if (updateAvailable_ && installed_ && !installing_ && !updating_) {
        updateButton_->show();
    }
    if (installing_ || updating_) {
        progressBar_->show();
    }

    QWidget::enterEvent(event);
}

void GameCard::leaveEvent(QEvent *event) {
    QPoint currentPos = pos();
    hoverAnimation->setStartValue(currentPos);
    hoverAnimation->setEndValue(currentPos + QPoint(0, 8));
    hoverAnimation->start();

    shadowAnimation->setStartValue(35);
    shadowAnimation->setEndValue(20);
    shadowAnimation->start();

    actionButton_->hide();
    updateButton_->hide();
    progressBar_->hide();

    QWidget::leaveEvent(event);
}

void GameCard::loadCoverImage(const QString &url) {
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

    // Create network manager (reuse if possible)
    static QNetworkAccessManager *sharedManager = nullptr;
    if (!sharedManager) {
        sharedManager = new QNetworkAccessManager();
        // Configure for better connection handling
        sharedManager->setTransferTimeout(10000); // 10 second timeout
    }

    // Make request with proper headers
    QNetworkRequest request(qurl);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setRawHeader("User-Agent", "OpenGalaxy/0.1.0");
    request.setRawHeader("Accept", "image/*");

    // Set connection keep-alive
    request.setRawHeader("Connection", "keep-alive");

    QNetworkReply *reply = sharedManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, url]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            // Silently ignore connection errors for cover images
            // They're not critical and will just show the placeholder
            // Common errors that are safe to ignore:
            // - ProtocolUnknownError: Invalid URL
            // - RemoteHostClosedError: Server closed connection
            // - OperationCanceledError: Request was cancelled
            // - ContentNotFoundError: 404 errors
            // - UnknownContentError: Server replied with error
            if (reply->error() != QNetworkReply::ProtocolUnknownError &&
                reply->error() != QNetworkReply::RemoteHostClosedError &&
                reply->error() != QNetworkReply::OperationCanceledError &&
                reply->error() != QNetworkReply::ContentNotFoundError &&
                reply->error() != QNetworkReply::UnknownContentError) {
                qDebug() << "Failed to load cover image for" << gameId_ << ":"
                         << reply->errorString();
            }
            return;
        }

        QByteArray imageData = reply->readAll();
        if (imageData.isEmpty()) {
            return;
        }

        QPixmap pixmap;
        if (pixmap.loadFromData(imageData)) {
            // Scale to fit within the cover container while maintaining aspect ratio
            QPixmap scaled = pixmap.scaled(420, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            coverImage->setPixmap(scaled);
            coverImage->setStyleSheet("background: transparent;"); // Remove placeholder styling
        }
    });

    // Handle errors during download
    connect(reply, &QNetworkReply::errorOccurred, this,
            [this, reply](QNetworkReply::NetworkError error) {
                // Silently ignore common network errors for cover images
                Q_UNUSED(error);
            });
}

void GameCard::setRepairNeeded(bool needed) {
    repairNeeded_ = needed;
    refreshButton();
}

void GameCard::setRepairing(bool repairing) {
    repairing_ = repairing;
    refreshButton();
}

void GameCard::setUnreleased(bool unreleased) {
    unreleased_ = unreleased;

    if (!unreleasedWarning_) {
        // Create warning label if it doesn't exist
        unreleasedWarning_ = new QLabel(this);
        unreleasedWarning_->setStyleSheet(R"(
            QLabel {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #ff9800, stop:1 #f57c00);
                color: white;
                font-size: 14px;
                font-weight: 700;
                padding: 8px 16px;
                border-radius: 8px;
                qproperty-alignment: AlignCenter;
            }
        )");
        unreleasedWarning_->setText("⏳ UNRELEASED");
        unreleasedWarning_->setFixedSize(180, 40);
        unreleasedWarning_->hide();

        // Position it in the top right corner of the cover
        if (coverImage) {
            unreleasedWarning_->move(coverImage->width() - unreleasedWarning_->width() - 12, 12);
        }
    }

    if (unreleasedWarning_) {
        unreleasedWarning_->setVisible(unreleased);
    }
}

} // namespace ui
} // namespace opengalaxy
