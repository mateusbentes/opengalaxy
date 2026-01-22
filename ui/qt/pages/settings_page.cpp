#include "settings_page.h"

namespace opengalaxy {
namespace ui {

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent)
{
    autoLoginCheck = new QCheckBox("Auto-login", this);
    saveButton = new QPushButton("Save", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(autoLoginCheck);
    layout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &SettingsPage::onSaveSettings);
}

SettingsPage::~SettingsPage()
{
}

void SettingsPage::onSaveSettings()
{
}

} // namespace ui
} // namespace opengalaxy
