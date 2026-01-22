#ifndef LIBRARY_PAGE_H
#define LIBRARY_PAGE_H

#include <QWidget>
#include <QGridLayout>

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
    
    QGridLayout* gameGrid;
};

} // namespace ui
} // namespace opengalaxy

#endif // LIBRARY_PAGE_H
