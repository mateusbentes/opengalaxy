#ifndef SETTINGS_PAGE_H
#define SETTINGS_PAGE_H

#include <QWidget>
#include <QComboBox>

namespace opengalaxy {
namespace ui {
class TranslationManager;
}
}

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(opengalaxy::ui::TranslationManager* translationManager, QWidget *parent = nullptr);

private slots:
    void onLanguageChanged(int index);

private:
    opengalaxy::ui::TranslationManager* translationManager_ = nullptr;
    QComboBox* languageCombo_ = nullptr;
};

#endif // SETTINGS_PAGE_H
