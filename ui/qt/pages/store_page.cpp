#include "store_page.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>

namespace opengalaxy {
namespace ui {

StorePage::StorePage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Create centered content
    QWidget* contentWidget = new QWidget(this);
    contentWidget->setMaximumWidth(700);

    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(40, 60, 40, 40);
    contentLayout->setSpacing(30);

    // Icon/Emoji
    QLabel* iconLabel = new QLabel("🛒", contentWidget);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 80px;");
    contentLayout->addWidget(iconLabel);

    // Title
    QLabel* titleLabel = new QLabel(tr("GOG Store"), contentWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
        QLabel {
                color: #3c3a37;
                font-size: 32px;
                font-weight: 700;
                margin-bottom: 10px;
        }
    )");
    contentLayout->addWidget(titleLabel);

    // Description
    QLabel* descLabel = new QLabel(
        tr("Browse and purchase games directly on the GOG website.\n"
           "Your purchases will automatically appear in your library."),
        contentWidget
    );
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet(R"(
        QLabel {
                color: #5a5855;
                font-size: 16px;
                line-height: 1.6;
        }
    )");
    contentLayout->addWidget(descLabel);

    contentLayout->addSpacing(20);

    // Open GOG Store button
    QPushButton* openStoreBtn = new QPushButton(tr("Open GOG Store in Browser"), contentWidget);
    openStoreBtn->setMinimumHeight(56);
    openStoreBtn->setCursor(Qt::PointingHandCursor);
    openStoreBtn->setStyleSheet(R"(
        QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #9b4dca, stop:1 #8b3dba);
                border: none;
                border-radius: 12px;
                padding: 16px 40px;
                color: white;
                font-size: 16px;
                font-weight: 600;
        }
        QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #ab5dda, stop:1 #9b4dca);
        }
        QPushButton:pressed {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #8b3dba, stop:1 #7b2daa);
        }
    )");
    contentLayout->addWidget(openStoreBtn);

    // Info text
    QLabel* infoLabel = new QLabel(
        tr("💡 Tip: After purchasing games on GOG.com, restart OpenGalaxy to see them in your library."),
        contentWidget
    );
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet(R"(
        QLabel {
                color: #8a8884;
                font-size: 14px;
                margin-top: 20px;
                padding: 15px;
                background: rgba(155, 77, 202, 0.1);
                border-radius: 8px;
        }
    )");
    contentLayout->addWidget(infoLabel);

    contentLayout->addStretch();

    // Center the content widget
    QHBoxLayout* centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addWidget(contentWidget);
    centerLayout->addStretch();

    layout->addStretch();
    layout->addLayout(centerLayout);
    layout->addStretch();

    setStyleSheet(R"(
        StorePage {
                background: #f5f3f0;
        }
    )");

    // Connect button to open browser
    connect(openStoreBtn, &QPushButton::clicked, this, []() {
        QDesktopServices::openUrl(QUrl("https://www.gog.com/games"));
    });
}

StorePage::~StorePage() = default;

} // namespace ui
} // namespace opengalaxy
