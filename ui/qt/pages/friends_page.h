#ifndef FRIENDS_PAGE_H
#define FRIENDS_PAGE_H

#include "opengalaxy/api/gog_client.h"
#include "opengalaxy/api/session.h"
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace opengalaxy {
namespace ui {

class FriendsPage : public QWidget {
    Q_OBJECT

  public:
    explicit FriendsPage(api::Session *session, QWidget *parent = nullptr);
    ~FriendsPage();

  public slots:
    void refreshFriends();

  private:
    void setupUI();

    api::Session *session_;
    api::GOGClient gogClient_;
    QListWidget *friendsView;
    QLabel *statusLabel;
};

} // namespace ui
} // namespace opengalaxy

#endif // FRIENDS_PAGE_H
