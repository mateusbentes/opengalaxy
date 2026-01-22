#ifndef GAME_DETAILS_DIALOG_H
#define GAME_DETAILS_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>

#include "opengalaxy/api/models.h"
#include "opengalaxy/library/library_service.h"
#include "opengalaxy/runners/runner_manager.h"

namespace opengalaxy {
namespace ui {

class GameDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    GameDetailsDialog(const api::GameInfo& game,
                      library::LibraryService* libraryService,
                      runners::RunnerManager* runnerManager,
                      QWidget* parent = nullptr);
    ~GameDetailsDialog();

private slots:
    void onSave();
    void onClose();

private:
    void populateRunners();

    api::GameInfo game_;
    library::LibraryService* libraryService_ = nullptr;
    runners::RunnerManager* runnerManager_ = nullptr;

    QLabel* titleLabel_ = nullptr;
    QComboBox* runnerCombo_ = nullptr;
    QLineEdit* runnerExecutableEdit_ = nullptr;
    QPlainTextEdit* runnerArgsEdit_ = nullptr;
    QPlainTextEdit* envEdit_ = nullptr;

    QPushButton* saveButton_ = nullptr;
    QPushButton* closeButton_ = nullptr;
};

} // namespace ui
} // namespace opengalaxy

#endif // GAME_DETAILS_DIALOG_H
