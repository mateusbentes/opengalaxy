#include "app_window.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>

#include "widgets/game_card.h"
#include "pages/store_page.h"
#include "pages/friends_page.h"

namespace opengalaxy {
namespace ui {

AppWindow::AppWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("OpenGalaxy");
    setMinimumSize(1280, 720);
    resize(1400, 800);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupSidebar();
    mainLayout->addWidget(sidebar);

    stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget);

    session_ = new api::Session(this);

    loginPage = new LoginPage(this);
    libraryPage = new LibraryPage(this);
    storePage = new StorePage(this);
    friendsPage = new FriendsPage(this);
    settingsPage = new SettingsPage(this);

    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(libraryPage);
    stackedWidget->addWidget(storePage);
    stackedWidget->addWidget(friendsPage);
    stackedWidget->addWidget(settingsPage);

    stackedWidget->setCurrentWidget(loginPage);
    sidebar->setVisible(false);

    connect(loginPage, &LoginPage::loginRequested, this, &AppWindow::startPasswordLogin);
    connect(session_, &api::Session::authenticated, this, &AppWindow::onLoginSuccess);
    connect(session_, &api::Session::authenticationFailed, this, [this](const QString& err) {
        QMessageBox::warning(this, "Login failed", err);
    });

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setColor(QColor(0, 0, 0, 160));
    shadow->setOffset(0, 0);
    centralWidget->setGraphicsEffect(shadow);
}

AppWindow::~AppWindow() = default;

void AppWindow::setupSidebar()
{
    sidebar = new QListWidget(this);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(220);
    sidebar->setFrameShape(QFrame::NoFrame);
    sidebar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sidebar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QListWidgetItem* libraryItem = new QListWidgetItem("Library", sidebar);
    libraryItem->setData(Qt::UserRole, 1);

    QListWidgetItem* storeItem = new QListWidgetItem("Store", sidebar);
    storeItem->setData(Qt::UserRole, 2);

    QListWidgetItem* friendsItem = new QListWidgetItem("Friends", sidebar);
    friendsItem->setData(Qt::UserRole, 3);

    QListWidgetItem* settingsItem = new QListWidgetItem("Settings", sidebar);
    settingsItem->setData(Qt::UserRole, 4);

    connect(sidebar, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        int pageIndex = item->data(Qt::UserRole).toInt();
        stackedWidget->setCurrentIndex(pageIndex);
    });

    sidebar->setCurrentRow(0);
}

void AppWindow::onLoginSuccess()
{
    sidebar->setVisible(true);
    stackedWidget->setCurrentWidget(libraryPage);
}

void AppWindow::startPasswordLogin(const QString& username, const QString& password)
{
    session_->loginWithPassword(username, password, [this](util::Result<api::AuthTokens> result) {
        if (!result.isOk()) {
            QMessageBox::warning(this, "Login failed", result.errorMessage());
        }
    });
}

} // namespace ui
} // namespace opengalaxy
