#ifndef SETTINGS_PAGE_H
#define SETTINGS_PAGE_H

#include "opengalaxy/api/session.h"
#include <QComboBox>
#include <QWidget>

namespace opengalaxy {
namespace ui {
class TranslationManager;
}
} // namespace opengalaxy

class SettingsPage : public QWidget {
    Q_OBJECT

  public:
    explicit SettingsPage(opengalaxy::ui::TranslationManager *translationManager,
                          opengalaxy::api::Session *session, QWidget *parent = nullptr);

  signals:
    void logoutRequested();

  private slots:
    void onLanguageChanged(int index);
    void onInstallationFoldersClicked();
    void onLogoutClicked();
    void onAboutClicked();
    void onCheckForUpdates();

  private:
    opengalaxy::ui::TranslationManager *translationManager_ = nullptr;
    opengalaxy::api::Session *session_ = nullptr;
    QComboBox *languageCombo_ = nullptr;
    class QCheckBox *showHiddenGamesCheckbox_ = nullptr;
};

#endif // SETTINGS_PAGE_H
