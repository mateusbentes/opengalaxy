#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSystemTrayIcon>
#include <QMenuBar>

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
    explicit AppWindow(QWidget *parent = nullptr);
    ~AppWindow();

private slots:
    void createMenus();

private:
    QTabWidget *tabWidget;
    QSystemTrayIcon *trayIcon;
    LoginPage *loginPage;
};

} // namespace ui
} // namespace opengalaxy

#endif // APP_WINDOW_H
