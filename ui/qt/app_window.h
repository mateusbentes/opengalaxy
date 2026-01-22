#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>

#include "opengalaxy/api/session.h"

#include "pages/login_page.h"
#include "pages/library_page.h"
#include "pages/store_page.h"
#include "pages/friends_page.h"
#include "pages/settings_page.h"

namespace opengalaxy {
namespace ui {

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppWindow(QWidget *parent = nullptr);
    ~AppWindow() override;

private slots:
    void onLoginSuccess();
    void startPasswordLogin(const QString& username, const QString& password);

private:
    void setupSidebar();

    QStackedWidget* stackedWidget = nullptr;
    QListWidget* sidebar = nullptr;

    api::Session* session_ = nullptr;

    LoginPage* loginPage = nullptr;
    LibraryPage* libraryPage = nullptr;
    StorePage* storePage = nullptr;
    FriendsPage* friendsPage = nullptr;
    SettingsPage* settingsPage = nullptr;
};

} // namespace ui
} // namespace opengalaxy

#endif // APP_WINDOW_H
