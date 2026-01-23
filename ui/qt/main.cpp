#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QTextStream>
#include <QFontDatabase>

#include "app_window.h"
#include "i18n/translation_manager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Application metadata
    app.setApplicationName("OpenGalaxy");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("OpenGalaxy");
    
    // Set Fusion style as base
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Load beautiful dark theme stylesheet
    QFile styleFile(":/styles/dark_theme.qss");
    if (!styleFile.exists()) {
        // Fallback: try to load from source directory
        QString sourcePath = QCoreApplication::applicationDirPath() + "/../ui/qt/resources/styles/dark_theme.qss";
        styleFile.setFileName(sourcePath);
    }
    
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        QString styleSheet = stream.readAll();
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }
    
    // Set application font
    QFont appFont("Segoe UI", 10);
    app.setFont(appFont);
    
    // Initialize translation system
    opengalaxy::ui::TranslationManager translationManager(&app);
    translationManager.loadFromSettings();
    
    // Create and show main window
    opengalaxy::ui::AppWindow window(&translationManager);
    window.show();

    return app.exec();
}
