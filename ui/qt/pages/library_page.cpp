#include "library_page.h"
#include "../models/library_model.h"

namespace opengalaxy {
namespace ui {

LibraryPage::LibraryPage(QWidget *parent)
    : QWidget(parent)
{
    libraryView = new QListView(this);
    libraryView->setModel(new opengalaxy::ui::LibraryModel(this));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(libraryView);
}

LibraryPage::~LibraryPage()
{
}

} // namespace ui
} // namespace opengalaxy
