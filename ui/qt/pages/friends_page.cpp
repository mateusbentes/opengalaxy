#include "friends_page.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

namespace opengalaxy {
namespace ui {

FriendsPage::FriendsPage(api::Session* session, QWidget *parent)
    : QWidget(parent)
    , session_(session)
    , gogClient_(session_, this)
{
    setupUI();
}

FriendsPage::~FriendsPage()
{
}

void FriendsPage::setupUI()
{
    setStyleSheet(R"(
        FriendsPage {
            background: #f5f3f0;
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 30, 40, 40);
    layout->setSpacing(16);

    // Header
    QHBoxLayout* headerLayout = new QHBoxLayout();
    
    QLabel* titleLabel = new QLabel(tr("Friends"), this);
    titleLabel->setStyleSheet(R"(
        QLabel { color: #3c3a37; font-size: 28px; font-weight: 700; }
    )");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    
    QPushButton* refreshBtn = new QPushButton(tr("Refresh"), this);
    refreshBtn->setStyleSheet(R"(
        QPushButton {
            background: #9b4dca;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            color: white;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #8b3dba;
        }
    )");
    connect(refreshBtn, &QPushButton::clicked, this, &FriendsPage::refreshFriends);
    headerLayout->addWidget(refreshBtn);
    
    layout->addLayout(headerLayout);

    // Status label
    statusLabel = new QLabel(tr("Loading friends..."), this);
    statusLabel->setStyleSheet(R"(
        QLabel {
            color: #5a5855;
            font-size: 14px;
            padding: 10px;
        }
    )");
    layout->addWidget(statusLabel);

    // Friends list
    friendsView = new QListWidget(this);
    friendsView->setStyleSheet(R"(
        QListWidget {
            background: #ffffff;
            border: 2px solid #d0cec9;
            border-radius: 12px;
            color: #3c3a37;
            font-size: 14px;
            padding: 8px;
        }
        QListWidget::item {
            padding: 12px;
            border-bottom: 1px solid #e8e6e3;
        }
        QListWidget::item:hover {
            background: #f9f8f7;
        }
        QListWidget::item:selected {
            background: #e8e0f5;
            color: #3c3a37;
        }
    )");
    layout->addWidget(friendsView);

    // Auto-refresh on page show
    refreshFriends();
}

void FriendsPage::refreshFriends()
{
    if (!session_ || !session_->isAuthenticated()) {
        statusLabel->setText(tr("Please log in to view friends."));
        friendsView->clear();
        return;
    }

    statusLabel->setText(tr("Loading friends..."));
    friendsView->clear();

    // Note: GOG API doesn't have a public friends endpoint in the current implementation
    // This is a placeholder that shows the structure
    // You would need to implement gogClient_.getFriends() in the GOGClient class
    
    // For now, show a message
    statusLabel->setText(tr("Friends feature coming soon!"));
    
    QListWidgetItem* item = new QListWidgetItem(friendsView);
    item->setText(tr("🚧 The GOG API friends endpoint is not yet implemented.\n"
                     "This feature will be available in a future update."));
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    
    // TODO: Implement when GOG API client has friends support
    // gogClient_.getFriends([this](util::Result<std::vector<api::Friend>> result) {
    //     if (!result.isOk()) {
    //         statusLabel->setText(tr("Failed to load friends: %1").arg(result.errorMessage()));
    //         return;
    //     }
    //     
    //     const auto& friends = result.value();
    //     if (friends.empty()) {
    //         statusLabel->setText(tr("No friends found."));
    //         return;
    //     }
    //     
    //     statusLabel->setText(tr("%1 friends").arg(friends.size()));
    //     
    //     for (const auto& friendData : friends) {
    //         QListWidgetItem* item = new QListWidgetItem(friendsView);
    //         item->setText(QString("%1 - %2").arg(friendData.username, friendData.status));
    //     }
    // });
}

} // namespace ui
} // namespace opengalaxy
