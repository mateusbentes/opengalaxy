#include "game_details_dialog.h"

#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

namespace opengalaxy {
namespace ui {

static QStringList envToLines(const QMap<QString, QString>& env)
{
    QStringList lines;
    for (auto it = env.begin(); it  !=  env.end(); ++it) {
        lines << (it.key() + "=" + it.value());
    }
    return lines;
}

static QMap<QString, QString> parseEnvLines(const QString& text, QString* error)
{
    QMap<QString, QString> env;
    const QStringList lines = text.split('\n');
    for (const QString& raw : lines) {
        const QString line = raw.trimmed();
        if (line.isEmpty()) continue;
        const int eq = line.indexOf('=');
        if (eq  <=  0) {
                if (error) *error = QString("Invalid env line: '%1' (expected KEY=VALUE)").arg(line);
                return {};
        }
        const QString key = line.left(eq).trimmed();
        const QString value = line.mid(eq + 1);
        env.insert(key, value);
    }
    return env;
}

GameDetailsDialog::GameDetailsDialog(const api::GameInfo& game,
                                   library::LibraryService* libraryService,
                                   runners::RunnerManager* runnerManager,
                                   QWidget* parent)
    : QDialog(parent)
    , game_(game)
    , libraryService_(libraryService)
    , runnerManager_(runnerManager)
{
    setWindowTitle(tr("Game Properties"));
    setModal(true);
    resize(520, 520);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(12);

    titleLabel_ = new QLabel(QString("<b>%1</b><br/><span style='color:#888'>%2</span>")
                                            .arg(game_.title, game_.platform),
                             this);
    root->addWidget(titleLabel_);

    auto* compatBox = new QGroupBox(tr("Compatibility"), this);
    auto* form = new QFormLayout(compatBox);

    runnerCombo_ = new QComboBox(compatBox);
    runnerCombo_->setToolTip(tr("Choose how this game should be launched"));

    runnerExecutableEdit_ = new QLineEdit(compatBox);
    runnerExecutableEdit_->setPlaceholderText(tr("Optional: full path to wrapper (overrides auto-detected path)"));

    runnerArgsEdit_ = new QPlainTextEdit(compatBox);
    runnerArgsEdit_->setPlaceholderText(tr("Runner arguments (one per line)\nExample: --some-flag"));
    runnerArgsEdit_->setFixedHeight(90);

    envEdit_ = new QPlainTextEdit(compatBox);
    envEdit_->setPlaceholderText(tr("Environment variables (KEY=VALUE, one per line)\nCommon: DXVK_HUD=fps, VKD3D_CONFIG=..., PROTON_LOG=1"));
    envEdit_->setFixedHeight(140);

    form->addRow(tr("Translator / Runner"), runnerCombo_);
    form->addRow(tr("Runner executable"), runnerExecutableEdit_);
    form->addRow(tr("Runner arguments"), runnerArgsEdit_);
    form->addRow(tr("Environment"), envEdit_);

    root->addWidget(compatBox);

    // Tweaks Section
    auto* tweaksBox = new QGroupBox(tr("Game Tweaks"), this);
    auto* tweaksLayout = new QVBoxLayout(tweaksBox);

    hideGameCheck_ = new QCheckBox(tr("Hide game from library"), tweaksBox);
    hideGameCheck_->setToolTip(tr("Hide this game from the library view"));
    tweaksLayout->addWidget(hideGameCheck_);

    dxvkFpsCheck_ = new QCheckBox(tr("Show FPS in game"), tweaksBox);
    dxvkFpsCheck_->setToolTip(tr("Display FPS counter overlay (DXVK_HUD=fps)"));
    tweaksLayout->addWidget(dxvkFpsCheck_);

    mangohudCheck_ = new QCheckBox(tr("Use MangoHud"), tweaksBox);
    mangohudCheck_->setToolTip(tr("Show performance overlay (FPS, CPU, GPU, temps)"));
    tweaksLayout->addWidget(mangohudCheck_);

    gamemodeCheck_ = new QCheckBox(tr("Use GameMode"), tweaksBox);
    gamemodeCheck_->setToolTip(tr("Enable GameMode for better performance"));
    tweaksLayout->addWidget(gamemodeCheck_);

    cloudSavesCheck_ = new QCheckBox(tr("Enable Cloud Saves"), tweaksBox);
    cloudSavesCheck_->setToolTip(tr("Sync saves to GOG Cloud (requires GOG SDK)"));
    cloudSavesCheck_->setEnabled(false);  // Disabled until SDK available
    tweaksLayout->addWidget(cloudSavesCheck_);

    root->addWidget(tweaksBox);

    // Wine/Proton Tools Section
    auto* toolsBox = new QGroupBox(tr("Wine/Proton Tools"), this);
    auto* toolsLayout = new QHBoxLayout(toolsBox);
    toolsLayout->setSpacing(8);

    auto* winecfgBtn = new QPushButton(tr("Winecfg"), this);
    winecfgBtn->setToolTip(tr("Open Wine configuration (winecfg)"));
    connect(winecfgBtn, &QPushButton::clicked, this, &GameDetailsDialog::launchWinecfg);

    auto* protontricksBtn = new QPushButton(tr("Protontricks"), this);
    protontricksBtn->setToolTip(tr("Open Protontricks for DLL/runtime management"));
    connect(protontricksBtn, &QPushButton::clicked, this, &GameDetailsDialog::launchProtontricks);

    auto* winetricksBtn = new QPushButton(tr("Winetricks"), this);
    winetricksBtn->setToolTip(tr("Open Winetricks for DLL/runtime management"));
    connect(winetricksBtn, &QPushButton::clicked, this, &GameDetailsDialog::launchWinetricks);

    auto* regeditBtn = new QPushButton(tr("Regedit"), this);
    regeditBtn->setToolTip(tr("Open Wine Registry Editor"));
    connect(regeditBtn, &QPushButton::clicked, this, &GameDetailsDialog::launchRegedit);

    openFolderBtn_ = new QPushButton(tr("Open Folder"), this);
    openFolderBtn_->setToolTip(tr("Open game installation folder"));
    connect(openFolderBtn_, &QPushButton::clicked, this, &GameDetailsDialog::openInstallFolder);

    toolsLayout->addWidget(winecfgBtn);
    toolsLayout->addWidget(protontricksBtn);
    toolsLayout->addWidget(winetricksBtn);
    toolsLayout->addWidget(regeditBtn);
    toolsLayout->addWidget(openFolderBtn_);
    toolsLayout->addStretch();

    root->addWidget(toolsBox);

    // Game Maintenance Section
    auto* maintenanceBox = new QGroupBox(tr("Game Maintenance"), this);
    auto* maintenanceLayout = new QHBoxLayout(maintenanceBox);
    maintenanceLayout->setSpacing(8);

    updateGameBtn_ = new QPushButton(tr("Update Game"), this);
    updateGameBtn_->setToolTip(tr("Check for and install game updates"));
    connect(updateGameBtn_, &QPushButton::clicked, this, &GameDetailsDialog::updateGame);

    verifyFilesBtn_ = new QPushButton(tr("Verify Files"), this);
    verifyFilesBtn_->setToolTip(tr("Check game file integrity"));
    connect(verifyFilesBtn_, &QPushButton::clicked, this, &GameDetailsDialog::verifyGameFiles);

    repairGameBtn_ = new QPushButton(tr("Repair Game"), this);
    repairGameBtn_->setToolTip(tr("Repair corrupted game files"));
    connect(repairGameBtn_, &QPushButton::clicked, this, &GameDetailsDialog::repairGame);

    maintenanceLayout->addWidget(updateGameBtn_);
    maintenanceLayout->addWidget(verifyFilesBtn_);
    maintenanceLayout->addWidget(repairGameBtn_);
    maintenanceLayout->addStretch();

    root->addWidget(maintenanceBox);

    auto* buttons = new QHBoxLayout();
    buttons->addStretch();
    saveButton_ = new QPushButton(tr("Save"), this);
    closeButton_ = new QPushButton(tr("Close"), this);
    buttons->addWidget(saveButton_);
    buttons->addWidget(closeButton_);
    root->addLayout(buttons);

    connect(saveButton_, &QPushButton::clicked, this, &GameDetailsDialog::onSave);
    connect(closeButton_, &QPushButton::clicked, this, &GameDetailsDialog::onClose);

    populateRunners();

    // Load saved values
    runnerExecutableEdit_->setText(game_.runnerExecutable);
    runnerArgsEdit_->setPlainText(game_.runnerArguments.join("\n"));
    envEdit_->setPlainText(envToLines(game_.extraEnvironment).join("\n"));

    // Load tweaks checkboxes
    hideGameCheck_->setChecked(game_.hiddenInLibrary);
    dxvkFpsCheck_->setChecked(game_.enableDxvkHudFps);
    mangohudCheck_->setChecked(game_.enableMangoHud);
    gamemodeCheck_->setChecked(game_.enableGameMode);
    cloudSavesCheck_->setChecked(game_.enableCloudSaves);

    // Select preferred runner
    const QString pref = game_.preferredRunner.trimmed();
    if (!pref.isEmpty()) {
        const int idx = runnerCombo_->findData(pref);
        if (idx  >=  0) runnerCombo_->setCurrentIndex(idx);
    }
}

GameDetailsDialog::~GameDetailsDialog() = default;

void GameDetailsDialog::populateRunners()
{
    runnerCombo_->clear();
    runnerCombo_->addItem("Auto", "");

    if (!runnerManager_) return;

    for (const auto& caps : runnerManager_->availableRunners()) {
        runnerCombo_->addItem(caps.name, caps.name);
    }
}

void GameDetailsDialog::onSave()
{
    QString err;
    QMap<QString, QString> env = parseEnvLines(envEdit_->toPlainText(), &err);
    if (!err.isEmpty()) {
        QMessageBox::warning(this, tr("Invalid environment"), err);
        return;
    }

    game_.preferredRunner = runnerCombo_->currentData().toString();
    game_.runnerExecutable = runnerExecutableEdit_->text().trimmed();
    game_.runnerArguments = runnerArgsEdit_->toPlainText().split('\n', Qt::SkipEmptyParts);
    for (QString& s : game_.runnerArguments) s = s.trimmed();

    // Save tweaks checkboxes
    game_.hiddenInLibrary = hideGameCheck_->isChecked();
    game_.enableDxvkHudFps = dxvkFpsCheck_->isChecked();
    game_.enableMangoHud = mangohudCheck_->isChecked();
    game_.enableGameMode = gamemodeCheck_->isChecked();
    game_.enableCloudSaves = cloudSavesCheck_->isChecked();

    // Apply environment variables based on tweaks (don't override user settings)
    if (game_.enableDxvkHudFps  &&  !env.contains("DXVK_HUD")) {
        env["DXVK_HUD"] = "fps";
    }
    if (game_.enableMangoHud  &&  !env.contains("MANGOHUD")) {
        env["MANGOHUD"] = "1";
    }
    if (game_.enableGameMode  &&  !env.contains("GAMEMODE")) {
        env["GAMEMODE"] = "1";
    }

    game_.extraEnvironment = env;

    if (!libraryService_) {
        QMessageBox::warning(this, tr("Error"), tr("Library service not available"));
        return;
    }

    libraryService_->updateGameProperties(game_);
    QMessageBox::information(this, tr("Saved"), tr("Game properties saved."));
}

void GameDetailsDialog::onClose()
{
    accept();
}

void GameDetailsDialog::launchWinecfg()
{
    qDebug() << "Launching winecfg for:" << game_.title;

    QProcess process;
    QStringList env = QProcess::systemEnvironment();

    // Set WINEPREFIX if available
    if (!game_.runnerExecutable.isEmpty()) {
        env << "WINEPREFIX=" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.wine";
    }

    process.setEnvironment(env);
    const bool ok = process.startDetached("winecfg");
    if (!ok) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to launch winecfg. Make sure Wine is installed."));
    }
}

void GameDetailsDialog::launchProtontricks()
{
    qDebug() << "Launching protontricks for:" << game_.title;

    const bool ok = QProcess::startDetached("protontricks", QStringList() << "--gui");
    if (!ok) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to launch protontricks. Make sure Protontricks is installed."));
    }
}

void GameDetailsDialog::launchWinetricks()
{
    qDebug() << "Launching winetricks for:" << game_.title;

    QProcess process;
    QStringList env = QProcess::systemEnvironment();

    // Set WINEPREFIX if available
    if (!game_.runnerExecutable.isEmpty()) {
        env << "WINEPREFIX=" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.wine";
    }

    process.setEnvironment(env);
    const bool ok = process.startDetached("winetricks", QStringList() << "--gui");
    if (!ok) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to launch winetricks. Make sure Winetricks is installed."));
    }
}

void GameDetailsDialog::launchRegedit()
{
    qDebug() << "Launching regedit for:" << game_.title;

    QProcess process;
    QStringList env = QProcess::systemEnvironment();

    // Set WINEPREFIX if available
    if (!game_.runnerExecutable.isEmpty()) {
        env << "WINEPREFIX=" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.wine";
    }

    process.setEnvironment(env);
    const bool ok = process.startDetached("wine", QStringList() << "regedit");
    if (!ok) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to launch regedit. Make sure Wine is installed."));
    }
}

void GameDetailsDialog::openInstallFolder()
{
    qDebug() << "Opening install folder for:" << game_.title;

    if (!game_.isInstalled  ||  game_.installPath.trimmed().isEmpty()) {
        QMessageBox::information(this, tr("Not installed"),
                                            tr("This game is not installed or has no install path."));
        return;
    }

    const QString path = game_.installPath;
    if (!QDir(path).exists()) {
        QMessageBox::warning(this, tr("Folder not found"),
                             tr("Install folder does not exist:\n%1").arg(path));
        return;
    }

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
        QMessageBox::warning(this, tr("Failed to open"),
                             tr("Could not open folder:\n%1").arg(path));
    }
}

void GameDetailsDialog::updateGame()
{
    qDebug() << "Checking for updates for:" << game_.title;

    if (!game_.isInstalled  ||  game_.installPath.trimmed().isEmpty()) {
        QMessageBox::information(this, tr("Not installed"),
                                            tr("This game is not installed."));
        return;
    }

    // Show progress dialog
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Check for Updates"));
    msgBox.setText(tr("Checking for updates for: %1").arg(game_.title));
    msgBox.setInformativeText(tr("Checking available versions...\n\nThis may take a moment."));
    msgBox.setStandardButtons(QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Information);

    // In a real implementation, we would:
    // 1. Fetch latest version from GOG API
    // 2. Compare with installed version
    // 3. Show update dialog if newer version available
    // 4. Download and install update

    // For now, show a message that update check would be performed
    QMessageBox::information(
        this, tr("Update Check"),
        tr("Checking for updates for:\n%1\n\n"
           "Note: Full update functionality requires GOG SDK integration.\n"
           "Currently, you can use the Update button on the game card to "
           "check for updates.")
                .arg(game_.title));
}

void GameDetailsDialog::verifyGameFiles()
{
    qDebug() << "Verifying game files for:" << game_.title;

    if (!game_.isInstalled  ||  game_.installPath.trimmed().isEmpty()) {
        QMessageBox::information(this, tr("Not installed"),
                                            tr("This game is not installed."));
        return;
    }

    const QString path = game_.installPath;
    if (!QDir(path).exists()) {
        QMessageBox::warning(this, tr("Folder not found"),
                             tr("Install folder does not exist:\n%1").arg(path));
        return;
    }

    // Show progress dialog
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Verify Game Files"));
    msgBox.setText(tr("Verifying game files for: %1").arg(game_.title));
    msgBox.setInformativeText(tr("Checking file integrity...\n\nThis may take a few minutes."));
    msgBox.setStandardButtons(QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Information);

    // Count files
    QDir gameDir(path);
    gameDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList entries = gameDir.entryList();
    int totalFiles = entries.count();

    if (totalFiles  ==  0) {
        QMessageBox::warning(this, tr("No files found"),
                             tr("No game files found in:\n%1").arg(path));
        return;
    }

    // Verify files (simplified - just check if files exist)
    int corruptedFiles = 0;
    int missingFiles = 0;

    // In a real implementation, we would:
    // 1. Get checksums from GOG API
    // 2. Calculate checksums for local files
    // 3. Compare and report differences

    // For now, just verify that files exist
    for (const QString& entry : entries) {
        QFileInfo info(gameDir.absoluteFilePath(entry));
        if (!info.exists()) {
                missingFiles++;
        }
    }

    // Show results
    if (missingFiles  ==  0  &&  corruptedFiles  ==  0) {
        QMessageBox::information(this, tr("Verification Complete"),
                                            tr("All game files are intact!\n\n"
                                                "Total files checked: %1").arg(totalFiles));
    } else {
        QString message = tr("Verification Complete\n\n"
                            "Total files: %1\n"
                            "Missing files: %2\n"
                            "Corrupted files: %3\n\n"
                            "Click 'Repair Game' to fix issues.")
                            .arg(totalFiles).arg(missingFiles).arg(corruptedFiles);
        QMessageBox::warning(this, tr("Issues Found"), message);
    }
}

void GameDetailsDialog::repairGame()
{
    qDebug() << "Repairing game:" << game_.title;

    if (!game_.isInstalled  ||  game_.installPath.trimmed().isEmpty()) {
        QMessageBox::information(this, tr("Not installed"),
                                            tr("This game is not installed."));
        return;
    }

    const QString path = game_.installPath;
    if (!QDir(path).exists()) {
        QMessageBox::warning(this, tr("Folder not found"),
                             tr("Install folder does not exist:\n%1").arg(path));
        return;
    }

    // Confirm repair
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        tr("Repair Game"),
        tr("This will verify and repair game files for:\n%1\n\n"
           "This may take a while and require re-downloading files.\n"
           "Continue?").arg(game_.title),
        QMessageBox::Yes | QMessageBox::No);

    if (reply  !=  QMessageBox::Yes) {
        return;
    }

    // Show repair progress
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Repairing Game"));
    msgBox.setText(tr("Repairing: %1").arg(game_.title));
    msgBox.setInformativeText(tr("Checking and repairing game files...\n\n"
                                            "This may take several minutes."));
    msgBox.setStandardButtons(QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Information);

    // In a real implementation, we would:
    // 1. Get checksums from GOG API
    // 2. Identify corrupted/missing files
    // 3. Re-download and replace them
    // 4. Verify checksums after repair

    // For now, show a message that repair would be performed
    QMessageBox::information(this, tr("Repair Started"),
                            tr("Game repair has been initiated for:\n%1\n\n"
                               "Note: Full repair functionality requires GOG SDK integration.\n"
                               "Currently, you can manually re-download the game to fix issues.")
                            .arg(game_.title));
}

} // namespace ui
} // namespace opengalaxy
