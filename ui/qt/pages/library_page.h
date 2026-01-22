#ifndef LIBRARY_PAGE_H
#define LIBRARY_PAGE_H

#include <QWidget>
#include <QGridLayout>

#include "opengalaxy/api/session.h"
#include "opengalaxy/api/gog_client.h"
#include "opengalaxy/library/library_service.h"
#include "opengalaxy/runners/runner_manager.h"

namespace opengalaxy {
namespace ui {

class LibraryPage : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryPage(QWidget *parent = nullptr);
    ~LibraryPage();

private:
    void loadGames();
    void openGameDetails(const QString& gameId);
    void launchGame(const QString& gameId);

    QGridLayout* gameGrid;

    // Core services used by UI
    api::Session session_;
    api::GOGClient gogClient_;
    library::LibraryService libraryService_;
    runners::RunnerManager runnerManager_;
};

} // namespace ui
} // namespace opengalaxy

#endif // LIBRARY_PAGE_H

