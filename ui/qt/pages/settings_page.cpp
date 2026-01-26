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
#include <QSysInfo>
#include <QDesktopServices>
#include <QUrl>
#include "i18n/translation_manager.h"

SettingsPage::SettingsPage(opengalaxy::ui::TranslationManager* translationManager,
                           opengalaxy::api::Session* session,
                           QWidget *parent)
    : QWidget(parent)
    , translationManager_(translationManager)
    , session_(session)
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
    
    connect(installsBtn, &QPushButton::clicked, this, &SettingsPage::onInstallationFoldersClicked);
    
    contentLayout->addWidget(gameTitle);
    contentLayout->addWidget(gameSubtitle);
    contentLayout->addWidget(installsBtn);

    // Account section
    QLabel *accountTitle = new QLabel(tr("Account"), content);
    accountTitle->setObjectName("sectionTitle");
    QLabel *accountSubtitle = new QLabel(tr("Manage your GOG account"), content);
    accountSubtitle->setObjectName("sectionSubtitle");
    
    QPushButton *logoutBtn = new QPushButton(tr("Logout"), content);
    logoutBtn->setStyleSheet(R"(
        QPushButton {
            background: #e74c3c;
            border: none;
            border-radius: 10px;
            padding: 15px 25px;
            color: white;
            font-size: 15px;
            min-width: 200px;
            margin: 5px 60px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #c0392b;
        }
        QPushButton:pressed {
            background: #a93226;
        }
    )");
    
    connect(logoutBtn, &QPushButton::clicked, this, &SettingsPage::onLogoutClicked);
    
    contentLayout->addWidget(accountTitle);
    contentLayout->addWidget(accountSubtitle);
    contentLayout->addWidget(logoutBtn);

    // About section
    QLabel *aboutTitle = new QLabel(tr("About"), content);
    aboutTitle->setObjectName("sectionTitle");
    QLabel *aboutSubtitle = new QLabel(tr("Information about OpenGalaxy"), content);
    aboutSubtitle->setObjectName("sectionSubtitle");
    
    QPushButton *aboutBtn = new QPushButton(tr("About OpenGalaxy"), content);
    connect(aboutBtn, &QPushButton::clicked, this, &SettingsPage::onAboutClicked);
    
    contentLayout->addWidget(aboutTitle);
    contentLayout->addWidget(aboutSubtitle);
    contentLayout->addWidget(aboutBtn);

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

void SettingsPage::onLogoutClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Logout"),
        tr("Are you sure you want to logout?\n\nYou will need to login again next time."),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (session_) {
            session_->logout();
        }
        emit logoutRequested();
    }
}

void SettingsPage::onAboutClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("About OpenGalaxy"));
    dialog.setMinimumSize(550, 550);
    dialog.setStyleSheet(R"(
        QDialog {
            background: #ffffff;
        }
        QLabel {
            color: #3c3a37;
        }
    )");
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);
    
    // Icon
    QLabel* iconLabel = new QLabel(&dialog);
    QPixmap iconPixmap(":/data/opengalaxyicon.png");
    iconLabel->setPixmap(iconPixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);
    
    // App name
    QLabel* nameLabel = new QLabel(tr("OpenGalaxy"), &dialog);
    nameLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    nameLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(nameLabel);
    
    // Version
    QLabel* versionLabel = new QLabel(tr("Version 1.0.0"), &dialog);
    versionLabel->setStyleSheet("font-size: 14px; color: #5a5855;");
    versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel);
    
    layout->addSpacing(10);
    
    // Description
    QLabel* descLabel = new QLabel(
        tr("OpenGalaxy is a free and open-source multiplatform GOG client.\n\n"
           "It allows you to download, install, and play your GOG games on "
           "Windows, macOS, and Linux with support for Wine, Proton, and native games.\n\n"
           "Inspired by Minigalaxy, OpenGalaxy provides a modern and user-friendly "
           "interface to manage your GOG library across all platforms."),
        &dialog
    );
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("font-size: 13px; line-height: 1.5;");
    descLabel->setAlignment(Qt::AlignLeft);
    layout->addWidget(descLabel);
    
    layout->addSpacing(10);
    
    // Links
    QLabel* linksLabel = new QLabel(
        tr("<b>Project:</b> <a href='https://github.com/mateusbentes/opengalaxy'>github.com/mateusbentes/opengalaxy</a><br>"
           "<b>License:</b> Apache 2.0<br>"
           "<b>Website:</b> <a href='https://www.gog.com'>GOG.com</a>"),
        &dialog
    );
    linksLabel->setOpenExternalLinks(true);
    linksLabel->setStyleSheet("font-size: 12px;");
    linksLabel->setTextFormat(Qt::RichText);
    layout->addWidget(linksLabel);
    
    layout->addSpacing(15);
    
    // System Information
    QLabel* sysInfoTitle = new QLabel(tr("System Information"), &dialog);
    sysInfoTitle->setStyleSheet("font-size: 14px; font-weight: 600; color: #3c3a37;");
    layout->addWidget(sysInfoTitle);
    
    // Get system information
    QString osInfo = QSysInfo::prettyProductName();
    QString cpuArch = QSysInfo::currentCpuArchitecture();
    QString qtVersion = QString("Qt %1").arg(QT_VERSION_STR);
    QString kernelVersion = QSysInfo::kernelVersion();
    
    QString sysInfoText = QString(
        "<b>OS:</b> %1 (%2)<br>"
        "<b>Kernel:</b> %3<br>"
        "<b>Qt Version:</b> %4"
    ).arg(osInfo, cpuArch, kernelVersion, qtVersion);
    
    QLabel* sysInfoLabel = new QLabel(sysInfoText, &dialog);
    sysInfoLabel->setStyleSheet("font-size: 11px; color: #5a5855; padding: 5px; background: #f5f5f5; border-radius: 5px;");
    sysInfoLabel->setTextFormat(Qt::RichText);
    sysInfoLabel->setWordWrap(true);
    layout->addWidget(sysInfoLabel);
    
    layout->addSpacing(10);
    
    // Check for Updates button
    QPushButton* updateButton = new QPushButton(tr("Check for Updates"), &dialog);
    updateButton->setStyleSheet(R"(
        QPushButton {
            background: #3498db;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            color: white;
            font-size: 13px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #2980b9;
        }
        QPushButton:pressed {
            background: #21618c;
        }
    )");
    connect(updateButton, &QPushButton::clicked, this, &SettingsPage::onCheckForUpdates);
    layout->addWidget(updateButton);
    
    layout->addStretch();
    
    // Close button
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    buttonBox->setStyleSheet(R"(
        QPushButton {
            background: #9b4dca;
            border: none;
            border-radius: 8px;
            padding: 10px 30px;
            color: white;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #8a3eb9;
        }
        QPushButton:pressed {
            background: #7a2ea9;
        }
    )");
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::accept);
    layout->addWidget(buttonBox);
    
    dialog.exec();
}

void SettingsPage::onCheckForUpdates()
{
    // Open the GitHub releases page
    QDesktopServices::openUrl(QUrl("https://github.com/mateusbentes/opengalaxy/releases"));
    
    // Show a message to the user
    QMessageBox::information(
        this,
        tr("Check for Updates"),
        tr("Opening the GitHub releases page in your browser.\n\n"
           "Current version: 1.0.0\n\n"
           "Please check if a newer version is available.")
    );
}
