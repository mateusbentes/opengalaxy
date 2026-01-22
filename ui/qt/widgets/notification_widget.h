#ifndef NOTIFICATION_WIDGET_H
#define NOTIFICATION_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>

class NotificationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationWidget(const QString &message, QWidget *parent = nullptr);
    ~NotificationWidget();

    void showNotification();
    static void showToast(const QString &message, QWidget *parent = nullptr);

private slots:
    void startHideAnimation();

private:
    void setupUI();
    void setupAnimations();
    
    QLabel *messageLabel;
    QPropertyAnimation *slideInAnimation;
    QPropertyAnimation *slideOutAnimation;
    QTimer *hideTimer;
};

#endif // NOTIFICATION_WIDGET_H
