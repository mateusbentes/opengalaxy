#ifndef LIBRARY_PAGE_H
#define LIBRARY_PAGE_H

#include <QGridLayout>
#include <QLineEdit>
#include <QMap>
#include <QSet>
#include <QVector>
#include <QWidget>

#include "opengalaxy/api/gog_client.h"
#include "opengalaxy/api/session.h"
#include "opengalaxy/install/install_service.h"
#include "opengalaxy/library/library_service.h"
#include "opengalaxy/runners/runner_manager.h"

namespace opengalaxy {
namespace ui {

class GameCard;

class LibraryPage : public QWidget {
    Q_OBJECT

  public:
    explicit LibraryPage(api::Session *session, QWidget *parent = nullptr);
    ~LibraryPage();

    void refreshLibrary(bool forceRefresh = false);

  private slots:
    void filterGames(const QString &searchText);

  private:
    void openGameDetails(const QString &gameId);
    void openGameProperties(const QString &gameId);
    void showGameInformation(const QString &gameId);
    void launchGame(const QString &gameId);
    void installGame(const QString &gameId);
    void cancelInstall(const QString &gameId);
    void updateGame(const QString &gameId);
    void checkForUpdate(const QString &gameId);
    void updateGridLayout();

    QGridLayout *gameGrid = nullptr;
    QLineEdit *searchBox_ = nullptr;

    QMap<QString, GameCard *> cardsById_;
    QVector<api::GameInfo> allGames_; // Store all games for filtering
    bool isLoading_ = false;          // Prevent double-loading
    bool showHiddenGames_ = false;    // Whether to show hidden games

    // Core services used by UI (session is passed from AppWindow)
    api::Session *session_;
    api::GOGClient gogClient_;
    library::LibraryService libraryService_;
    runners::RunnerManager runnerManager_;
    install::InstallService installService_;
};

} // namespace ui
} // namespace opengalaxy

#endif // LIBRARY_PAGE_H
