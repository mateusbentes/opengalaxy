#include "game_details_dialog.h"

#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QDebug>

namespace opengalaxy {
namespace ui {

static QStringList envToLines(const QMap<QString, QString>& env)
{
    QStringList lines;
    for (auto it = env.begin(); it != env.end(); ++it) {
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
        if (eq <= 0) {
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

    toolsLayout->addWidget(winecfgBtn);
    toolsLayout->addWidget(protontricksBtn);
    toolsLayout->addWidget(winetricksBtn);
    toolsLayout->addWidget(regeditBtn);
    toolsLayout->addStretch();

    root->addWidget(toolsBox);

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

    // Select preferred runner
    const QString pref = game_.preferredRunner.trimmed();
    if (!pref.isEmpty()) {
        const int idx = runnerCombo_->findData(pref);
        if (idx >= 0) runnerCombo_->setCurrentIndex(idx);
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
    const QMap<QString, QString> env = parseEnvLines(envEdit_->toPlainText(), &err);
    if (!err.isEmpty()) {
        QMessageBox::warning(this, "Invalid environment", err);
        return;
    }

    game_.preferredRunner = runnerCombo_->currentData().toString();
    game_.runnerExecutable = runnerExecutableEdit_->text().trimmed();
    game_.runnerArguments = runnerArgsEdit_->toPlainText().split('\n', Qt::SkipEmptyParts);
    for (QString& s : game_.runnerArguments) s = s.trimmed();
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
        // Extract prefix from runner executable path
        QString prefix = game_.runnerExecutable;
        if (prefix.contains("wine")) {
            env << "WINEPREFIX=" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.wine";
        }
    }
    
    process.setEnvironment(env);
    process.startDetached("winecfg");
    
    if (!process.waitForStarted()) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to launch winecfg. Make sure Wine is installed."));
    }
}

void GameDetailsDialog::launchProtontricks()
{
    qDebug() << "Launching protontricks for:" << game_.title;
    
    QProcess process;
    QStringList env = QProcess::systemEnvironment();
    
    process.setEnvironment(env);
    process.startDetached("protontricks", QStringList() << "--gui");
    
    if (!process.waitForStarted()) {
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
    process.startDetached("winetricks", QStringList() << "--gui");
    
    if (!process.waitForStarted()) {
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
    process.startDetached("wine", QStringList() << "regedit");
    
    if (!process.waitForStarted()) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to launch regedit. Make sure Wine is installed."));
    }
}

} // namespace ui
} // namespace opengalaxy
