#include "app_window.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QMouseEvent>
#include <QScreen>
#include <QApplication>

#include "widgets/game_card.h"
#include "pages/store_page.h"
#include "pages/friends_page.h"
#include "dialogs/oauth_login_dialog.h"

namespace opengalaxy {
namespace ui {

AppWindow::AppWindow(TranslationManager* translationManager, QWidget* parent)
    : QMainWindow(parent)
    , translationManager_(translationManager)
{
    setWindowTitle(tr("OpenGalaxy"));
    setMinimumSize(1400, 900);
    resize(1600, 1000);

    setWindowFlags(Qt::FramelessWindowHint);
    // Remove translucent background for solid color
    // setAttribute(Qt::WA_TranslucentBackground);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* outerLayout = new QVBoxLayout(centralWidget);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    setupTitleBar();
    outerLayout->addWidget(titleBar);

    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupSidebar();
    mainLayout->addWidget(sidebar);

    stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget);

    outerLayout->addLayout(mainLayout);

    session_ = new api::Session(this);

    loginPage = new LoginPage(this);
    libraryPage = new LibraryPage(session_, this);
    storePage = new StorePage(translationManager_, session_, this);
    friendsPage = new FriendsPage(this);
    settingsPage = new SettingsPage(translationManager_, this);

    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(libraryPage);
    stackedWidget->addWidget(storePage);
    stackedWidget->addWidget(friendsPage);
    stackedWidget->addWidget(settingsPage);

    stackedWidget->setCurrentWidget(loginPage);
    sidebar->setVisible(false);

    connect(loginPage, &LoginPage::loginRequested, this, &AppWindow::startOAuthLogin);
    connect(session_, &api::Session::authenticated, this, &AppWindow::onLoginSuccess);
    connect(session_, &api::Session::authenticationFailed, this, [this](const QString& err) {
        QMessageBox::warning(this, tr("Login failed"), err);
    });

    // Note: Shadow effect can cause clipping issues with frameless windows
    // Removed to ensure all content is visible
    // If shadow is needed, consider using a custom painted shadow or platform-specific effects
}

AppWindow::~AppWindow() = default;

void AppWindow::setupTitleBar()
{
    titleBar = new QWidget(this);
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(40);
    titleBar->setStyleSheet(
        "QWidget#titleBar {"
        "   background: rgba(26, 15, 46, 0.95);"
        "   border-bottom: 1px solid rgba(124, 77, 255, 0.2);"
        "}"
    );

    QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(15, 0, 0, 0);
    titleLayout->setSpacing(0);

    QLabel* titleLabel = new QLabel(tr("OpenGalaxy"), titleBar);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #ffffff;");
    titleLayout->addWidget(titleLabel);

    titleLayout->addStretch();

    QPushButton* minimizeBtn = new QPushButton("−", titleBar);
    minimizeBtn->setObjectName("minimizeButton");
    minimizeBtn->setFixedSize(46, 40);
    minimizeBtn->setStyleSheet(
        "QPushButton#minimizeButton {"
        "   background: transparent;"
        "   border: none;"
        "   border-radius: 0px;"
        "   color: #ffffff;"
        "   font-size: 20px;"
        "   font-weight: bold;"
        "   padding: 0px;"
        "}"
        "QPushButton#minimizeButton:hover {"
        "   background: rgba(255, 255, 255, 0.1);"
        "}"
        "QPushButton#minimizeButton:pressed {"
        "   background: rgba(255, 255, 255, 0.15);"
        "}"
    );
    connect(minimizeBtn, &QPushButton::clicked, this, &AppWindow::onMinimizeClicked);
    titleLayout->addWidget(minimizeBtn);

    QPushButton* maximizeBtn = new QPushButton("□", titleBar);
    maximizeBtn->setObjectName("maximizeButton");
    maximizeBtn->setFixedSize(46, 40);
    maximizeBtn->setStyleSheet(
        "QPushButton#maximizeButton {"
        "   background: transparent;"
        "   border: none;"
        "   border-radius: 0px;"
        "   color: #ffffff;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   padding: 0px;"
        "}"
        "QPushButton#maximizeButton:hover {"
        "   background: rgba(255, 255, 255, 0.1);"
        "}"
        "QPushButton#maximizeButton:pressed {"
        "   background: rgba(255, 255, 255, 0.15);"
        "}"
    );
    connect(maximizeBtn, &QPushButton::clicked, this, &AppWindow::onMaximizeClicked);
    titleLayout->addWidget(maximizeBtn);

    QPushButton* closeBtn = new QPushButton("×", titleBar);
    closeBtn->setObjectName("closeButton");
    closeBtn->setFixedSize(46, 40);
    closeBtn->setStyleSheet(
        "QPushButton#closeButton {"
        "   background: transparent;"
        "   border: none;"
        "   border-radius: 0px;"
        "   color: #ffffff;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   padding: 0px;"
        "}"
        "QPushButton#closeButton:hover {"
        "   background: #e81123;"
        "   color: #ffffff;"
        "}"
        "QPushButton#closeButton:pressed {"
        "   background: #c50712;"
        "   color: #ffffff;"
        "}"
    );
    connect(closeBtn, &QPushButton::clicked, this, &AppWindow::onCloseClicked);
    titleLayout->addWidget(closeBtn);
}

void AppWindow::setupSidebar()
{
    sidebar = new QListWidget(this);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(220);
    sidebar->setFrameShape(QFrame::NoFrame);
    sidebar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sidebar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QListWidgetItem* libraryItem = new QListWidgetItem(tr("Library"), sidebar);
    libraryItem->setData(Qt::UserRole, 1);

    QListWidgetItem* storeItem = new QListWidgetItem(tr("Store"), sidebar);
    storeItem->setData(Qt::UserRole, 2);

    QListWidgetItem* friendsItem = new QListWidgetItem(tr("Friends"), sidebar);
    friendsItem->setData(Qt::UserRole, 3);

    QListWidgetItem* settingsItem = new QListWidgetItem(tr("Settings"), sidebar);
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
    
    // Force refresh library from GOG (not cache) after login
    libraryPage->refreshLibrary(true);
}

void AppWindow::startOAuthLogin(const QString& username, const QString& password)
{
#ifdef HAVE_WEBENGINE
    // Show OAuth login dialog with auto-fill
    auto* dialog = new OAuthLoginDialog(username, password, this);
    connect(dialog, &OAuthLoginDialog::authorizationReceived, this, [this](const QString& code) {
        // Exchange authorization code for tokens
        session_->loginWithAuthCode(code, [this](util::Result<api::AuthTokens> result) {
            if (!result.isOk()) {
                QMessageBox::warning(this, tr("Login failed"), result.errorMessage());
            }
        });
    });
    dialog->exec();
    dialog->deleteLater();
#else
    QMessageBox::warning(this, tr("OAuth Not Available"),
                        tr("OAuth login requires Qt WebEngine.\n\n"
                           "Please install: sudo apt install qt6-webengine\n\n"
                           "Then rebuild the application."));
#endif
}

void AppWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QWidget* childWidget = childAt(event->pos());
        
        // Check if click is on title bar or its children (but not buttons)
        bool onTitleBar = false;
        QWidget* widget = childWidget;
        while (widget && widget != this) {
            if (widget == titleBar) {
                onTitleBar = true;
                break;
            }
            widget = widget->parentWidget();
        }
        
        // Don't drag if clicking on buttons
        if (onTitleBar && childWidget && 
            childWidget->objectName() != "minimizeButton" &&
            childWidget->objectName() != "maximizeButton" &&
            childWidget->objectName() != "closeButton") {
            isDragging = true;
            dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        }
    }
    QMainWindow::mousePressEvent(event);
}

void AppWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        if (isMaximized) {
            // If maximized, restore to normal size when dragging
            onMaximizeClicked();
            // Adjust drag position for the new window size
            dragPosition = QPoint(width() / 2, 20);
        }
        move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
    }
    QMainWindow::mouseMoveEvent(event);
}

void AppWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
    QMainWindow::mouseReleaseEvent(event);
}

void AppWindow::onMinimizeClicked()
{
    showMinimized();
}

void AppWindow::onMaximizeClicked()
{
    if (isMaximized) {
        showNormal();
        isMaximized = false;
        
        // Update maximize button symbol
        QPushButton* maxBtn = titleBar->findChild<QPushButton*>("maximizeButton");
        if (maxBtn) {
            maxBtn->setText("□");
        }
    } else {
        showMaximized();
        isMaximized = true;
        
        // Update maximize button symbol
        QPushButton* maxBtn = titleBar->findChild<QPushButton*>("maximizeButton");
        if (maxBtn) {
            maxBtn->setText("❐");
        }
    }
}

void AppWindow::onCloseClicked()
{
    close();
}

} // namespace ui
} // namespace opengalaxy
