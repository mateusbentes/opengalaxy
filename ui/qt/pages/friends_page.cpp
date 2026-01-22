#include "friends_page.h"

namespace opengalaxy {
namespace ui {

FriendsPage::FriendsPage(QWidget *parent)
    : QWidget(parent)
{
    friendsView = new QListView(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(friendsView);
}

FriendsPage::~FriendsPage()
{
}

} // namespace ui
} // namespace opengalaxy
