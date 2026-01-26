#include "game_information_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

namespace opengalaxy {
namespace ui {

GameInformationDialog::GameInformationDialog(const api::GameInfo& game,
                                             QWidget* parent)
    : QDialog(parent)
    , game_(game)
{
    setWindowTitle(tr("Information - %1").arg(game_.title));
    setMinimumSize(600, 500);
    setStyleSheet(R"(
        QDialog {
            background: #f8f7f5;
        }
        QLabel {
            color: #3c3a37;
        }
        QLabel#title {
            font-size: 24px;
            font-weight: bold;
            color: #2c2a27;
        }
        QLabel#section {
            font-size: 14px;
            font-weight: 600;
            color: #3c3a37;
            margin-top: 15px;
            margin-bottom: 8px;
        }
        QLabel#content {
            font-size: 13px;
            color: #5a5855;
            line-height: 1.5;
        }
        QPushButton {
            background: #6c5ce7;
            border: none;
            border-radius: 6px;
            padding: 10px 18px;
            color: white;
            font-weight: 600;
            font-size: 13px;
        }
        QPushButton:hover {
            background: #5f3dc4;
        }
        QPushButton:pressed {
            background: #4c2d9f;
        }
    )");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(15);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border: none;");

    QWidget* content = new QWidget(scrollArea);
    QVBoxLayout* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(10);

    // Title
    QLabel* titleLabel = new QLabel(game_.title, content);
    titleLabel->setObjectName("title");
    titleLabel->setWordWrap(true);
    contentLayout->addWidget(titleLabel);

    // Description
    if (!game_.description.isEmpty()) {
        QLabel* descLabel = new QLabel(game_.description, content);
        descLabel->setObjectName("content");
        descLabel->setWordWrap(true);
        contentLayout->addWidget(descLabel);
    }

    // Links section
    QLabel* linksTitle = new QLabel(tr("Links"), content);
    linksTitle->setObjectName("section");
    contentLayout->addWidget(linksTitle);

    QVBoxLayout* linksLayout = new QVBoxLayout();
    linksLayout->setContentsMargins(0, 0, 0, 0);
    linksLayout->setSpacing(8);

    // GOG Store link
    QPushButton* gogBtn = new QPushButton(tr("🔗 GOG Store Page"), content);
    connect(gogBtn, &QPushButton::clicked, this, [this]() {
        QString url;
        if (!game_.slug.isEmpty()) {
            url = QString("https://www.gog.com/en/game/%1").arg(game_.slug);
        } else {
            // Fallback: search for game by title
            QString searchQuery = game_.title;
            searchQuery = searchQuery.replace(" ", "%20");
            url = QString("https://www.gog.com/en/games?search=%1").arg(searchQuery);
        }
        qDebug() << "Opening GOG Store:" << url << "Slug:" << game_.slug;
        openLink(url);
    });
    linksLayout->addWidget(gogBtn);

    // Support link - GOG uses support.gog.com help center
    QPushButton* supportBtn = new QPushButton(tr("❓ Support"), content);
    connect(supportBtn, &QPushButton::clicked, this, [this]() {
        QString url;
        if (!game_.slug.isEmpty()) {
            // Try to open game-specific support article
            url = QString(
                "https://support.gog.com/hc/en-us/search?query=%1&product=gog")
                .arg(game_.title.replace(" ", "%20"));
        } else {
            // Fallback: GOG support main page
            url = "https://support.gog.com/hc/en-us";
        }
        qDebug() << "Opening Support:" << url << "Slug:" << game_.slug;
        openLink(url);
    });
    linksLayout->addWidget(supportBtn);

    // Forum link
    QPushButton* forumBtn = new QPushButton(tr("💬 Forum"), content);
    connect(forumBtn, &QPushButton::clicked, this, [this]() {
        QString url;
        if (!game_.slug.isEmpty()) {
            url = QString("https://www.gog.com/forum/%1").arg(game_.slug);
        } else {
            // Fallback: GOG forum main page
            url = "https://www.gog.com/forum";
        }
        qDebug() << "Opening Forum:" << url << "Slug:" << game_.slug;
        openLink(url);
    });
    linksLayout->addWidget(forumBtn);

    // PCGamingWiki link
    QPushButton* wikiBtn = new QPushButton(tr("📖 PCGamingWiki"), content);
    connect(wikiBtn, &QPushButton::clicked, this, [this]() {
        openLink(QString("https://www.pcgamingwiki.com/wiki/%1").arg(game_.title));
    });
    linksLayout->addWidget(wikiBtn);

    contentLayout->addLayout(linksLayout);

    // Game info section
    QLabel* infoTitle = new QLabel(tr("Game Information"), content);
    infoTitle->setObjectName("section");
    contentLayout->addWidget(infoTitle);

    QString releaseDateStr = game_.releaseDate.isValid()
        ? game_.releaseDate.toString("yyyy-MM-dd")
        : tr("Unknown");

    QString infoText = QString(
        "<b>%1:</b> %2<br>"
        "<b>%3:</b> %4"
    ).arg(
        tr("Platform"),
        game_.platform,
        tr("Release Date"),
        releaseDateStr
    );

    QLabel* infoLabel = new QLabel(infoText, content);
    infoLabel->setObjectName("content");
    infoLabel->setTextFormat(Qt::RichText);
    infoLabel->setWordWrap(true);
    contentLayout->addWidget(infoLabel);

    contentLayout->addStretch();

    scrollArea->setWidget(content);
    mainLayout->addWidget(scrollArea);

    // Close button
    QPushButton* closeBtn = new QPushButton(tr("Close"), this);
    closeBtn->setMinimumWidth(100);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(closeBtn);
}

GameInformationDialog::~GameInformationDialog() = default;

void GameInformationDialog::openLink(const QString& url)
{
    QDesktopServices::openUrl(QUrl(url));
}

} // namespace ui
} // namespace opengalaxy
