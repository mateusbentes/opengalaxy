#ifndef STORE_PAGE_H
#define STORE_PAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListView>

namespace opengalaxy {
namespace ui {

class StorePage : public QWidget
{
    Q_OBJECT

public:
    explicit StorePage(QWidget *parent = nullptr);
    ~StorePage();

private:
    QListView *storeView;
};

} // namespace ui
} // namespace opengalaxy

#endif // STORE_PAGE_H
