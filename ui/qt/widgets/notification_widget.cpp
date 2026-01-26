#include "notification_widget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

NotificationWidget::NotificationWidget(const QString &message, QWidget *parent)
    : QWidget(parent), messageLabel(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(400, 60);

    setupUI();
    setupAnimations();

    messageLabel->setText(message);
}

NotificationWidget::~NotificationWidget()
{
}

void NotificationWidget::setupUI()
{
    setStyleSheet(
        R"(
        NotificationWidget {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #2a1f4e, stop:1 #3a2b5e);
                border-radius: 12px;
                border: 1px solid rgba(255, 255, 255, 0.1);
        }

        QLabel {
                color: #e0e0ff;
                font-size: 14px;
                font-weight: 500;
                padding: 0 20px;
        }
        )");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 12, 20, 12);

    messageLabel = new QLabel(this);
    layout->addWidget(messageLabel);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 0.5));
    shadow->setOffset(0, 8);
    setGraphicsEffect(shadow);
}

void NotificationWidget::setupAnimations()
{
    slideInAnimation = new QPropertyAnimation(this, "geometry");
    slideInAnimation->setDuration(300);
    slideInAnimation->setEasingCurve(QEasingCurve::OutBack);

    slideOutAnimation = new QPropertyAnimation(this, "windowOpacity");
    slideOutAnimation->setDuration(200);
    slideOutAnimation->setEndValue(0);

    hideTimer = new QTimer(this);
    connect(hideTimer, &QTimer::timeout, this, &NotificationWidget::startHideAnimation);
}

void NotificationWidget::showNotification()
{
    move(parentWidget()->width() - width() - 20, 100);
    show();
    slideInAnimation->setStartValue(QRect(parentWidget()->width(), 100, width(), height()));
    slideInAnimation->setEndValue(QRect(parentWidget()->width() - width() - 20, 100, width(), height()));
    slideInAnimation->start();

    hideTimer->start(3000);
}

void NotificationWidget::startHideAnimation()
{
    hideTimer->stop();
    slideOutAnimation->start();
    connect(slideOutAnimation, &QPropertyAnimation::finished, this, &QWidget::close);
}

void NotificationWidget::showToast(const QString &message, QWidget *parent)
{
    NotificationWidget *toast = new NotificationWidget(message, parent);
    toast->showNotification();
}
