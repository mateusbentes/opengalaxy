#include "notification_widget.h"

namespace opengalaxy {
namespace ui {

NotificationWidget::NotificationWidget(QWidget *parent)
    : QWidget(parent)
{
    listWidget = new QListWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(listWidget);
}

NotificationWidget::~NotificationWidget()
{
}

} // namespace ui
} // namespace opengalaxy
