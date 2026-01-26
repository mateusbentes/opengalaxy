#ifndef STORE_PAGE_H
#define STORE_PAGE_H

#include <QWidget>

namespace opengalaxy {
namespace ui {

class StorePage : public QWidget {
    Q_OBJECT

  public:
    explicit StorePage(QWidget *parent = nullptr);
    ~StorePage();
};

} // namespace ui
} // namespace opengalaxy

#endif // STORE_PAGE_H
