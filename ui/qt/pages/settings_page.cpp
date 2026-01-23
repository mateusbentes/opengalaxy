#include "settings_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QComboBox>
#include <QMessageBox>
#include "i18n/translation_manager.h"

SettingsPage::SettingsPage(opengalaxy::ui::TranslationManager* translationManager, QWidget *parent)
    : QWidget(parent)
    , translationManager_(translationManager)
{
    setStyleSheet(
        R"(
        SettingsPage {
            background: #f5f3f0;
            color: #3c3a37;
        }
        
        QLabel#sectionTitle {
            font-size: 22px;
            font-weight: 600;
            padding: 30px 60px 20px 60px;
            border-bottom: 1px solid #d0cec9;
            color: #3c3a37;
        }
        
        QLabel#sectionSubtitle {
            font-size: 16px;
            color: #5a5855;
            padding: 0 60px 30px 60px;
        }
        
        QLabel#settingLabel {
            font-size: 15px;
            color: #3c3a37;
            padding: 10px 60px;
        }
        
        QComboBox {
            background: #ffffff;
            border: 2px solid #d0cec9;
            border-radius: 8px;
            padding: 10px 16px;
            color: #3c3a37;
            font-size: 14px;
            margin: 0 60px 20px 60px;
            min-width: 250px;
        }
        
        QComboBox:hover {
            border-color: #9b4dca;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        
        QPushButton {
            background: #ffffff;
            border: 1px solid #d0cec9;
            border-radius: 10px;
            padding: 15px 25px;
            color: #3c3a37;
            font-size: 15px;
            min-width: 200px;
            margin: 5px 60px;
        }
        
        QPushButton:hover {
            background: #fafaf9;
            border-color: #9b4dca;
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
    QLabel *generalTitle = new QLabel(tr("Settings"), content);
    generalTitle->setObjectName("sectionTitle");
    QLabel *generalSubtitle = new QLabel(tr("Manage your OpenGalaxy preferences"), content);
    generalSubtitle->setObjectName("sectionSubtitle");
    
    contentLayout->addWidget(generalTitle);
    contentLayout->addWidget(generalSubtitle);
    
    // Language setting
    QLabel *languageLabel = new QLabel(tr("Language"), content);
    languageLabel->setObjectName("settingLabel");
    contentLayout->addWidget(languageLabel);
    
    languageCombo_ = new QComboBox(content);
    
    // Populate language combo box
    if (translationManager_) {
        QStringList locales = translationManager_->availableLocales();
        for (const QString& locale : locales) {
            QString displayName = translationManager_->localeDisplayName(locale);
            languageCombo_->addItem(displayName, locale);
        }
        
        // Set current language
        QString currentLocale = translationManager_->currentLocale();
        int currentIndex = languageCombo_->findData(currentLocale);
        if (currentIndex >= 0) {
            languageCombo_->setCurrentIndex(currentIndex);
        }
        
        connect(languageCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &SettingsPage::onLanguageChanged);
    }
    
    contentLayout->addWidget(languageCombo_);

    // Game section
    QLabel *gameTitle = new QLabel(tr("Games"), content);
    gameTitle->setObjectName("sectionTitle");
    QLabel *gameSubtitle = new QLabel(tr("Manage your game library and installations"), content);
    gameSubtitle->setObjectName("sectionSubtitle");
    
    QPushButton *installsBtn = new QPushButton(tr("Installation Folders"), content);
    QPushButton *launcherBtn = new QPushButton(tr("Default Launcher Options"), content);
    
    contentLayout->addWidget(gameTitle);
    contentLayout->addWidget(gameSubtitle);
    contentLayout->addWidget(installsBtn);
    contentLayout->addWidget(launcherBtn);

    contentLayout->addStretch();

    scrollArea->setWidget(content);
    mainLayout->addWidget(scrollArea);
}

void SettingsPage::onLanguageChanged(int index)
{
    if (!translationManager_ || index < 0) {
        return;
    }
    
    QString selectedLocale = languageCombo_->itemData(index).toString();
    QString currentLocale = translationManager_->currentLocale();
    
    if (selectedLocale == currentLocale) {
        return;
    }
    
    // Change language
    translationManager_->setLocale(selectedLocale);
    
    // Show restart message
    QMessageBox::information(this,
        tr("Restart Required"),
        tr("Please restart the application for the language change to take effect."));
}
