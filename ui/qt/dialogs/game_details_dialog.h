#ifndef GAME_DETAILS_DIALOG_H
#define GAME_DETAILS_DIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "opengalaxy/api/models.h"
#include "opengalaxy/library/library_service.h"
#include "opengalaxy/runners/runner_manager.h"

namespace opengalaxy {
namespace ui {

class GameDetailsDialog : public QDialog {
    Q_OBJECT

  public:
    explicit GameDetailsDialog(const api::GameInfo &game, library::LibraryService *libraryService,
                               runners::RunnerManager *runnerManager, QWidget *parent = nullptr);
    ~GameDetailsDialog();

  private slots:
    void onSave();
    void onClose();
    void launchWinecfg();
    void launchProtontricks();
    void launchWinetricks();
    void launchRegedit();
    void openInstallFolder();
    void updateGame();
    void verifyGameFiles();
    void repairGame();

  private:
    void populateRunners();

    api::GameInfo game_;
    library::LibraryService *libraryService_ = nullptr;
    runners::RunnerManager *runnerManager_ = nullptr;

    QLabel *titleLabel_ = nullptr;
    QComboBox *runnerCombo_ = nullptr;
    QLineEdit *runnerExecutableEdit_ = nullptr;
    QPlainTextEdit *runnerArgsEdit_ = nullptr;
    QPlainTextEdit *envEdit_ = nullptr;

    // Tweaks checkboxes
    QCheckBox *hideGameCheck_ = nullptr;
    QCheckBox *mangohudCheck_ = nullptr;
    QCheckBox *dxvkFpsCheck_ = nullptr;
    QCheckBox *gamemodeCheck_ = nullptr;
    QCheckBox *cloudSavesCheck_ = nullptr;

    // Tool buttons
    QPushButton *winecfgBtn_ = nullptr;
    QPushButton *protontricksBtn_ = nullptr;
    QPushButton *winetricksBtn_ = nullptr;
    QPushButton *regeditBtn_ = nullptr;
    QPushButton *openFolderBtn_ = nullptr;
    QPushButton *verifyFilesBtn_ = nullptr;
    QPushButton *repairGameBtn_ = nullptr;
    QPushButton *updateGameBtn_ = nullptr;

    QPushButton *saveButton_ = nullptr;
    QPushButton *closeButton_ = nullptr;
};

} // namespace ui
} // namespace opengalaxy

#endif // GAME_DETAILS_DIALOG_H
