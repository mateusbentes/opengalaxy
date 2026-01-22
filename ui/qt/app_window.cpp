#include "app_window.h"

namespace opengalaxy {
namespace ui {

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("OpenGalaxy");
    resize(800, 600);

    tabWidget = new QTabWidget(this);
    loginPage = new LoginPage(tabWidget);
    tabWidget->addTab(loginPage, "Login");

    setCentralWidget(tabWidget);
    trayIcon = new QSystemTrayIcon(this);
    createMenus();
}

AppWindow::~AppWindow()
{
}

void AppWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Exit", this, &QWidget::close);
}

} // namespace ui
} // namespace opengalaxy
