#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include <QListWidget>
#include <QMainWindow>
#include <QPoint>
#include <QStackedWidget>

#include "opengalaxy/api/session.h"

#include "i18n/translation_manager.h"
#include "pages/friends_page.h"
#include "pages/library_page.h"
#include "pages/login_page.h"
#include "pages/settings_page.h"
#include "pages/store_page.h"

namespace opengalaxy {
namespace ui {

class AppWindow : public QMainWindow {
    Q_OBJECT

  public:
    AppWindow(TranslationManager *translationManager, QWidget *parent = nullptr);
    ~AppWindow() override;

  protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

  private slots:
    void onLoginSuccess();
    void onLogout();
    void startOAuthLogin(const QString &username, const QString &password);
    void onMinimizeClicked();
    void onMaximizeClicked();
    void onCloseClicked();

  private:
    void setupSidebar();
    void setupTitleBar();

    QWidget *titleBar = nullptr;
    QStackedWidget *stackedWidget = nullptr;
    QListWidget *sidebar = nullptr;

    api::Session *session_ = nullptr;
    TranslationManager *translationManager_ = nullptr;

    LoginPage *loginPage = nullptr;
    LibraryPage *libraryPage = nullptr;
    StorePage *storePage = nullptr;
    FriendsPage *friendsPage = nullptr;
    SettingsPage *settingsPage = nullptr;

    QPoint dragPosition;
    bool isDragging = false;
    bool isMaximized = false;
};

} // namespace ui
} // namespace opengalaxy

#endif // APP_WINDOW_H
