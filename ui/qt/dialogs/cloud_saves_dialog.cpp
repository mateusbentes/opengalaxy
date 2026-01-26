#include "cloud_saves_dialog.h"

namespace opengalaxy {
namespace ui {

CloudSavesDialog::CloudSavesDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Cloud Saves");
    savesList = new QListWidget(this);
    downloadButton = new QPushButton("Download", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(savesList);
    layout->addWidget(downloadButton);

    connect(downloadButton, &QPushButton::clicked, this, &CloudSavesDialog::onDownload);
}

CloudSavesDialog::~CloudSavesDialog() {}

void CloudSavesDialog::onDownload() {}

} // namespace ui
} // namespace opengalaxy
