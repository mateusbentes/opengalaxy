#ifndef NOTIFICATION_WIDGET_H
#define NOTIFICATION_WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>

namespace opengalaxy {
namespace ui {

class NotificationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationWidget(QWidget *parent = nullptr);
    ~NotificationWidget();

private:
    QListWidget *listWidget;
};

} // namespace ui
} // namespace opengalaxy

#endif // NOTIFICATION_WIDGET_H
