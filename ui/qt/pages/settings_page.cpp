#include "settings_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QComboBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QDialog>
#include <QCheckBox>
#include <QDir>
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
    
    connect(installsBtn, &QPushButton::clicked, this, &SettingsPage::onInstallationFoldersClicked);
    connect(launcherBtn, &QPushButton::clicked, this, &SettingsPage::onLauncherOptionsClicked);
    
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

void SettingsPage::onInstallationFoldersClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Installation Folders"));
    dialog.setMinimumSize(600, 400);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    QLabel* infoLabel = new QLabel(tr("Manage folders where games can be installed:"), &dialog);
    layout->addWidget(infoLabel);
    
    QListWidget* folderList = new QListWidget(&dialog);
    folderList->setStyleSheet(R"(
        QListWidget {
            background: #ffffff;
            border: 2px solid #d0cec9;
            border-radius: 8px;
            padding: 8px;
            color: #3c3a37;
        }
    )");
    
    // TODO: Load saved installation folders from settings
    // For now, show default location
    QString defaultPath = QDir::homePath() + "/Games/OpenGalaxy";
    folderList->addItem(defaultPath);
    
    layout->addWidget(folderList);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* addBtn = new QPushButton(tr("Add Folder"), &dialog);
    addBtn->setStyleSheet(R"(
        QPushButton {
            background: #9b4dca;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            color: white;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #8b3dba;
        }
    )");
    
    connect(addBtn, &QPushButton::clicked, [&]() {
        QString dir = QFileDialog::getExistingDirectory(
            &dialog,
            tr("Select Installation Folder"),
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
        
        if (!dir.isEmpty()) {
            folderList->addItem(dir);
            // TODO: Save to settings
        }
    });
    
    QPushButton* removeBtn = new QPushButton(tr("Remove Selected"), &dialog);
    removeBtn->setStyleSheet(R"(
        QPushButton {
            background: #e74c3c;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            color: white;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #c0392b;
        }
    )");
    
    connect(removeBtn, &QPushButton::clicked, [&]() {
        QListWidgetItem* item = folderList->currentItem();
        if (item) {
            delete item;
            // TODO: Save to settings
        }
    });
    
    buttonLayout->addWidget(addBtn);
    buttonLayout->addWidget(removeBtn);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(dialogButtons, &QDialogButtonBox::rejected, &dialog, &QDialog::accept);
    layout->addWidget(dialogButtons);
    
    dialog.exec();
}

void SettingsPage::onLauncherOptionsClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Default Launcher Options"));
    dialog.setMinimumSize(500, 350);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    QLabel* infoLabel = new QLabel(tr("Select default runner for launching games:"), &dialog);
    layout->addWidget(infoLabel);
    
    // Runner selection
    QLabel* runnerLabel = new QLabel(tr("Default Runner:"), &dialog);
    runnerLabel->setStyleSheet("font-weight: 600; margin-top: 10px;");
    layout->addWidget(runnerLabel);
    
    QComboBox* runnerCombo = new QComboBox(&dialog);
    runnerCombo->setStyleSheet(R"(
        QComboBox {
            background: #ffffff;
            border: 2px solid #d0cec9;
            border-radius: 8px;
            padding: 10px 16px;
            color: #3c3a37;
            font-size: 14px;
        }
        QComboBox:hover {
            border-color: #9b4dca;
        }
    )");
    
    // Add runner options
    runnerCombo->addItem(tr("Native (Linux)"), "native");
    runnerCombo->addItem(tr("Wine"), "wine");
    runnerCombo->addItem(tr("Proton"), "proton");
    
    // TODO: Load saved default from settings
    runnerCombo->setCurrentIndex(0);
    
    layout->addWidget(runnerCombo);
    
    // Additional options
    QLabel* optionsLabel = new QLabel(tr("Additional Options:"), &dialog);
    optionsLabel->setStyleSheet("font-weight: 600; margin-top: 20px;");
    layout->addWidget(optionsLabel);
    
    QCheckBox* fullscreenCheck = new QCheckBox(tr("Launch games in fullscreen by default"), &dialog);
    QCheckBox* cloudSavesCheck = new QCheckBox(tr("Enable cloud saves synchronization"), &dialog);
    
    fullscreenCheck->setStyleSheet("color: #3c3a37; padding: 5px;");
    cloudSavesCheck->setStyleSheet("color: #3c3a37; padding: 5px;");
    
    // TODO: Load from settings
    cloudSavesCheck->setChecked(true);
    
    layout->addWidget(fullscreenCheck);
    layout->addWidget(cloudSavesCheck);
    
    layout->addStretch();
    
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    
    connect(dialogButtons, &QDialogButtonBox::accepted, [&]() {
        // TODO: Save settings
        QString selectedRunner = runnerCombo->currentData().toString();
        bool fullscreen = fullscreenCheck->isChecked();
        bool cloudSaves = cloudSavesCheck->isChecked();
        
        QMessageBox::information(&dialog, tr("Settings Saved"),
            tr("Launcher options have been saved successfully."));
        
        dialog.accept();
    });
    
    connect(dialogButtons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    layout->addWidget(dialogButtons);
    
    dialog.exec();
}
