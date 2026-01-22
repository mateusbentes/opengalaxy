#include "store_page.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

namespace opengalaxy {
namespace ui {

StorePage::StorePage(QWidget* parent)
    : QWidget(parent)
    , session_(this)
    , gogClient_(&session_, this)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 30, 40, 30);
    layout->setSpacing(16);

    auto* header = new QHBoxLayout();
    auto* title = new QLabel("Store", this);
    title->setStyleSheet(R"(
        QLabel { color: #ffffff; font-size: 28px; font-weight: 700; }
    )");
    header->addWidget(title);
    header->addStretch();

    searchEdit_ = new QLineEdit(this);
    searchEdit_->setPlaceholderText("Search on GOG...");
    searchEdit_->setFixedWidth(360);
    searchEdit_->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.08);
            border: 2px solid rgba(124, 77, 255, 0.3);
            border-radius: 8px;
            padding: 10px 16px;
            color: #ffffff;
            font-size: 14px;
        }
        QLineEdit:focus { border-color: #7c4dff; }
        QLineEdit::placeholder { color: rgba(255, 255, 255, 0.4); }
    )");
    header->addWidget(searchEdit_);

    layout->addLayout(header);

    storeView_ = new QListView(this);
    storeView_->setStyleSheet("QListView { background: transparent; border: none; color: white; }");
    model_ = new QStandardItemModel(this);
    storeView_->setModel(model_);

    layout->addWidget(storeView_);

    setStyleSheet(R"(
        StorePage {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a0f2e, stop:1 #2d1b4e);
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
            QMessageBox::warning(this, "Store", result.errorMessage());
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
