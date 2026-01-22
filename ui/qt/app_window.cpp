#include "app_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QGraphicsDropShadowEffect>
#include "widgets/game_card.h"
#include "pages/store_page.h"
#include "pages/friends_page.h"

namespace opengalaxy {
namespace ui {

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Set window properties
    setWindowTitle("OpenGalaxy");
    setMinimumSize(1280, 720);
    resize(1400, 800);
    
    // Remove default window frame for modern look
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Create central widget with main layout
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create sidebar
    setupSidebar();
    mainLayout->addWidget(sidebar);
    
    // Create stacked widget for pages
    stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget);
    
    // Create pages
    loginPage = new LoginPage(this);
    libraryPage = new LibraryPage(this);
    storePage = new StorePage(this);
    friendsPage = new FriendsPage(this);
    settingsPage = new SettingsPage(this);
    
    // Add pages to stack
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(libraryPage);
    stackedWidget->addWidget(storePage);
    stackedWidget->addWidget(friendsPage);
    stackedWidget->addWidget(settingsPage);
    
    // Start with login page
    stackedWidget->setCurrentWidget(loginPage);
    sidebar->setVisible(false); // Hide sidebar until logged in
    
    // Connect signals
    connect(loginPage, &LoginPage::loginSuccess, this, &AppWindow::onLoginSuccess);
    
    // Apply shadow effect to window
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setColor(QColor(0, 0, 0, 160));
    shadow->setOffset(0, 0);
    centralWidget->setGraphicsEffect(shadow);
}

AppWindow::~AppWindow()
{
}

void AppWindow::setupSidebar()
{
    sidebar = new QListWidget(this);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(220);
    sidebar->setFrameShape(QFrame::NoFrame);
    sidebar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sidebar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Add navigation items
    QListWidgetItem* libraryItem = new QListWidgetItem("Library", sidebar);
    libraryItem->setData(Qt::UserRole, 1); // Page index
    
    QListWidgetItem* storeItem = new QListWidgetItem("Store", sidebar);
    storeItem->setData(Qt::UserRole, 2);
    
    QListWidgetItem* friendsItem = new QListWidgetItem("Friends", sidebar);
    friendsItem->setData(Qt::UserRole, 3);
    
    QListWidgetItem* settingsItem = new QListWidgetItem("Settings", sidebar);
    settingsItem->setData(Qt::UserRole, 4);
    
    // Connect navigation
    connect(sidebar, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        int pageIndex = item->data(Qt::UserRole).toInt();
        stackedWidget->setCurrentIndex(pageIndex);
    });
    
    // Select library by default
    sidebar->setCurrentRow(0);
}

void AppWindow::onLoginSuccess()
{
    // Show sidebar and switch to library
    sidebar->setVisible(true);
    stackedWidget->setCurrentWidget(libraryPage);
}

} // namespace ui
} // namespace opengalaxy
