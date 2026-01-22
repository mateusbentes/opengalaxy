#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
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
    ~AppWindow();

private slots:
    void onLoginSuccess();

private:
    void setupSidebar();

    QStackedWidget* stackedWidget;
    QListWidget* sidebar;
    
    LoginPage* loginPage;
    LibraryPage* libraryPage;
    StorePage* storePage;
    FriendsPage* friendsPage;
    SettingsPage* settingsPage;
};

} // namespace ui
} // namespace opengalaxy

#endif // APP_WINDOW_H
