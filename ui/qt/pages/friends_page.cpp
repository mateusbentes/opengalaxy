#include "friends_page.h"

namespace opengalaxy {
namespace ui {

FriendsPage::FriendsPage(QWidget *parent)
    : QWidget(parent)
{
    friendsView = new QListView(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 30, 40, 40);  // Add proper margins, especially bottom
    layout->setSpacing(0);
    layout->addWidget(friendsView);
}

FriendsPage::~FriendsPage()
{
}

} // namespace ui
} // namespace opengalaxy
