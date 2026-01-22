#ifndef FRIENDS_PAGE_H
#define FRIENDS_PAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListView>

namespace opengalaxy {
namespace ui {

class FriendsPage : public QWidget
{
    Q_OBJECT

public:
    explicit FriendsPage(QWidget *parent = nullptr);
    ~FriendsPage();

private:
    QListView *friendsView;
};

} // namespace ui
} // namespace opengalaxy

#endif // FRIENDS_PAGE_H
