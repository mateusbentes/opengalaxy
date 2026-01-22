#include <QApplication>
#include <QStyleFactory>

#include "app_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));

    opengalaxy::ui::AppWindow window;
    window.show();

    return app.exec();
}
