#ifndef CLOUD_SAVES_DIALOG_H
#define CLOUD_SAVES_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>

namespace opengalaxy {
namespace ui {

class CloudSavesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CloudSavesDialog(QWidget *parent = nullptr);
    ~CloudSavesDialog();

private slots:
    void onDownload();

private:
    QListWidget *savesList;
    QPushButton *downloadButton;
};

} // namespace ui
} // namespace opengalaxy

#endif // CLOUD_SAVES_DIALOG_H
