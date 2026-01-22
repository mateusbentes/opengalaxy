#include "settings_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(
        R"(
        SettingsPage {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1e1a3a, stop:1 #2a1f4e);
            color: #e0e0ff;
        }
        
        QLabel#sectionTitle {
            font-size: 22px;
            font-weight: 600;
            padding: 30px 60px 20px 60px;
            border-bottom: 1px solid rgba(255,255,255,0.1);
        }
        
        QLabel#sectionSubtitle {
            font-size: 16px;
            color: #b0b0cc;
            padding: 0 60px 30px 60px;
        }
        
        QPushButton {
            background: rgba(255,255,255,0.05);
            border: 1px solid rgba(255,255,255,0.1);
            border-radius: 10px;
            padding: 15px 25px;
            color: #e0e0ff;
            font-size: 15px;
            min-width: 200px;
        }
        
        QPushButton:hover {
            background: rgba(255,255,255,0.1);
            border-color: #7c4dff;
        }
        )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("border: none;");

    QWidget *content = new QWidget(scrollArea);
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 40);
    contentLayout->setSpacing(0);

    // General section
    QLabel *generalTitle = new QLabel("General", content);
    generalTitle->setObjectName("sectionTitle");
    QLabel *generalSubtitle = new QLabel("Manage your OpenGalaxy preferences", content);
    generalSubtitle->setObjectName("sectionSubtitle");
    
    QPushButton *themeBtn = new QPushButton("Theme Settings", content);
    QPushButton *languageBtn = new QPushButton("Language", content);
    
    contentLayout->addWidget(generalTitle);
    contentLayout->addWidget(generalSubtitle);
    contentLayout->addWidget(themeBtn);
    contentLayout->addWidget(languageBtn);

    // Game section
    QLabel *gameTitle = new QLabel("Games", content);
    gameTitle->setObjectName("sectionTitle");
    QLabel *gameSubtitle = new QLabel("Manage your game library and installations", content);
    gameSubtitle->setObjectName("sectionSubtitle");
    
    QPushButton *installsBtn = new QPushButton("Installation Folders", content);
    QPushButton *launcherBtn = new QPushButton("Default Launcher Options", content);
    
    contentLayout->addWidget(gameTitle);
    contentLayout->addWidget(gameSubtitle);
    contentLayout->addWidget(installsBtn);
    contentLayout->addWidget(launcherBtn);

    contentLayout->addStretch();

    scrollArea->setWidget(content);
    mainLayout->addWidget(scrollArea);
}
