#include "store_page.h"

namespace opengalaxy {
namespace ui {

StorePage::StorePage(QWidget *parent)
    : QWidget(parent)
{
    storeView = new QListView(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(storeView);
}

StorePage::~StorePage()
{
}

} // namespace ui
} // namespace opengalaxy
