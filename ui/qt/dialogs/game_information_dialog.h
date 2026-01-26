#ifndef GAME_INFORMATION_DIALOG_H
#define GAME_INFORMATION_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

#include "opengalaxy/api/models.h"

namespace opengalaxy {
namespace ui {

class GameInformationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameInformationDialog(const api::GameInfo& game,
                                   QWidget* parent = nullptr);
    ~GameInformationDialog();

private:
    void openLink(const QString& url);

    api::GameInfo game_;
};

} // namespace ui
} // namespace opengalaxy

#endif // GAME_INFORMATION_DIALOG_H
