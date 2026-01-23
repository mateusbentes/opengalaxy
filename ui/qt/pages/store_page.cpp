#include "store_page.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

namespace opengalaxy {
namespace ui {

StorePage::StorePage(TranslationManager* translationManager, api::Session* session, QWidget* parent)
    : QWidget(parent)
    , session_(session)
    , gogClient_(session_, this)
    , translationManager_(translationManager)
{
    // Set GOG API locale based on current UI language
    if (translationManager_) {
        gogClient_.setLocale(translationManager_->gogApiLocale());
    }
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 30, 40, 40);  // Increased bottom margin to prevent cutoff
    layout->setSpacing(16);

    auto* header = new QHBoxLayout();
    auto* title = new QLabel(tr("Store"), this);
    title->setStyleSheet(R"(
        QLabel { color: #3c3a37; font-size: 28px; font-weight: 700; }
    )");
    header->addWidget(title);
    header->addStretch();

    searchEdit_ = new QLineEdit(this);
    searchEdit_->setPlaceholderText(tr("Search on GOG..."));
    searchEdit_->setFixedWidth(360);
    searchEdit_->setStyleSheet(R"(
        QLineEdit {
            background: #ffffff;
            border: 2px solid #d0cec9;
            border-radius: 8px;
            padding: 10px 16px;
            color: #3c3a37;
            font-size: 14px;
        }
        QLineEdit:focus { border-color: #9b4dca; }
        QLineEdit::placeholder { color: #8a8884; }
    )");
    header->addWidget(searchEdit_);

    layout->addLayout(header);

    storeView_ = new QListView(this);
    storeView_->setStyleSheet("QListView { background: transparent; border: none; color: #3c3a37; }");
    model_ = new QStandardItemModel(this);
    storeView_->setModel(model_);

    layout->addWidget(storeView_);

    setStyleSheet(R"(
        StorePage {
            background: #f5f3f0;
        }
    )");

    connect(searchEdit_, &QLineEdit::returnPressed, this, &StorePage::doSearch);
}

StorePage::~StorePage() = default;

void StorePage::doSearch()
{
    const QString query = searchEdit_->text().trimmed();
    if (query.isEmpty()) return;

    model_->clear();

    gogClient_.searchStore(query, [this](opengalaxy::util::Result<std::vector<api::StoreGameInfo>> result) {
        if (!result.isOk()) {
            QMessageBox::warning(this, tr("Store"), result.errorMessage());
            return;
        }

        for (const auto& g : result.value()) {
            QString line = g.title;
            if (!g.price.isEmpty()) {
                line += QString("  —  %1").arg(g.price);
            }
            auto* item = new QStandardItem(line);
            item->setEditable(false);
            model_->appendRow(item);
        }
    });
}

} // namespace ui
} // namespace opengalaxy
