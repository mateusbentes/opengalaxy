#ifndef SETTINGS_PAGE_H
#define SETTINGS_PAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>

namespace opengalaxy {
namespace ui {

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);
    ~SettingsPage();

private slots:
    void onSaveSettings();

private:
    QCheckBox *autoLoginCheck;
    QPushButton *saveButton;
};

} // namespace ui
} // namespace opengalaxy

#endif // SETTINGS_PAGE_H
