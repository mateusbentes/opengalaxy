#include "game_details_dialog.h"

#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>

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
    setWindowTitle("Game Properties");
    setModal(true);
    resize(520, 520);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(12);

    titleLabel_ = new QLabel(QString("<b>%1</b><br/><span style='color:#888'>%2</span>")
                                 .arg(game_.title, game_.platform),
                             this);
    root->addWidget(titleLabel_);

    auto* compatBox = new QGroupBox("Compatibility", this);
    auto* form = new QFormLayout(compatBox);

    runnerCombo_ = new QComboBox(compatBox);
    runnerCombo_->setToolTip("Choose how this game should be launched");

    runnerExecutableEdit_ = new QLineEdit(compatBox);
    runnerExecutableEdit_->setPlaceholderText("Optional: full path to wrapper (overrides auto-detected path)");

    runnerArgsEdit_ = new QPlainTextEdit(compatBox);
    runnerArgsEdit_->setPlaceholderText("Runner arguments (one per line)\nExample: --some-flag");
    runnerArgsEdit_->setFixedHeight(90);

    envEdit_ = new QPlainTextEdit(compatBox);
    envEdit_->setPlaceholderText("Environment variables (KEY=VALUE, one per line)\nExample: BOX64_LOG=1");
    envEdit_->setFixedHeight(140);

    form->addRow("Translator / Runner", runnerCombo_);
    form->addRow("Runner executable", runnerExecutableEdit_);
    form->addRow("Runner arguments", runnerArgsEdit_);
    form->addRow("Environment", envEdit_);

    root->addWidget(compatBox);

    auto* buttons = new QHBoxLayout();
    buttons->addStretch();
    saveButton_ = new QPushButton("Save", this);
    closeButton_ = new QPushButton("Close", this);
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
        QMessageBox::warning(this, "Error", "Library service not available");
        return;
    }

    libraryService_->updateGameProperties(game_);
    QMessageBox::information(this, "Saved", "Game properties saved.");
}

void GameDetailsDialog::onClose()
{
    accept();
}

} // namespace ui
} // namespace opengalaxy
