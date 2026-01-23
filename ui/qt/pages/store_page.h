#ifndef STORE_PAGE_H
#define STORE_PAGE_H

#include <QWidget>
#include <QListView>
#include <QLineEdit>
#include <QStandardItemModel>

#include "opengalaxy/api/session.h"
#include "opengalaxy/api/gog_client.h"
#include "i18n/translation_manager.h"

namespace opengalaxy {
namespace ui {

class StorePage : public QWidget
{
    Q_OBJECT

public:
    explicit StorePage(TranslationManager* translationManager, api::Session* session, QWidget *parent = nullptr);
    ~StorePage();

private:
    void doSearch();

    QLineEdit* searchEdit_ = nullptr;
    QListView* storeView_ = nullptr;
    QStandardItemModel* model_ = nullptr;

    api::Session* session_;
    api::GOGClient gogClient_;
    TranslationManager* translationManager_ = nullptr;
};

} // namespace ui
} // namespace opengalaxy

#endif // STORE_PAGE_H
