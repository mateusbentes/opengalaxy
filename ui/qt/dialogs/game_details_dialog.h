#ifndef GAME_DETAILS_DIALOG_H
#define GAME_DETAILS_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace opengalaxy {
namespace ui {

class GameDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameDetailsDialog(QWidget *parent = nullptr);
    ~GameDetailsDialog();

private slots:
    void onClose();

private:
    QLabel *detailsLabel;
    QPushButton *closeButton;
};

} // namespace ui
} // namespace opengalaxy

#endif // GAME_DETAILS_DIALOG_H
