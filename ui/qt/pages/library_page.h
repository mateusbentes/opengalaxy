#ifndef LIBRARY_PAGE_H
#define LIBRARY_PAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QMap>

#include "opengalaxy/api/session.h"
#include "opengalaxy/api/gog_client.h"
#include "opengalaxy/library/library_service.h"
#include "opengalaxy/runners/runner_manager.h"
#include "opengalaxy/install/install_service.h"

namespace opengalaxy {
namespace ui {

class GameCard;

class LibraryPage : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryPage(QWidget *parent = nullptr);
    ~LibraryPage();

private:
    void openGameDetails(const QString& gameId);
    void launchGame(const QString& gameId);
    void installGame(const QString& gameId);
    void cancelInstall(const QString& gameId);

    QGridLayout* gameGrid = nullptr;

    QMap<QString, GameCard*> cardsById_;

    // Core services used by UI
    api::Session session_;
    api::GOGClient gogClient_;
    library::LibraryService libraryService_;
    runners::RunnerManager runnerManager_;
    install::InstallService installService_;
};

} // namespace ui
} // namespace opengalaxy

#endif // LIBRARY_PAGE_H
