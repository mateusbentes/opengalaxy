#ifndef LIBRARY_PAGE_H
#define LIBRARY_PAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListView>

namespace opengalaxy {
namespace ui {

class LibraryPage : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryPage(QWidget *parent = nullptr);
    ~LibraryPage();

private:
    QListView *libraryView;
};

} // namespace ui
} // namespace opengalaxy

#endif // LIBRARY_PAGE_H
