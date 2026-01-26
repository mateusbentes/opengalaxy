#include "login_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include "opengalaxy/api/session.h"

namespace opengalaxy {
namespace ui {

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
{
    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 60);  // Add bottom margin to prevent cutoff
    mainLayout->setSpacing(0);

    // Create centered container
    QWidget* container = new QWidget(this);
    container->setMaximumWidth(450);

    QVBoxLayout* containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(40, 40, 40, 60);  // Increased bottom margin
    containerLayout->setSpacing(20);

    // Title
    QLabel* titleLabel = new QLabel(tr("Welcome to OpenGalaxy"), container);
    titleLabel->setStyleSheet(R"(
        QLabel {
                color: #3c3a37;
                font-size: 32px;
                font-weight: 700;
                margin-bottom: 20px;
        }
    )");
    titleLabel->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(titleLabel);

    // Subtitle
    QLabel* subtitleLabel = new QLabel(tr("Sign in with your GOG account"), container);
    subtitleLabel->setStyleSheet(R"(
        QLabel {
                color: #5a5855;
                font-size: 16px;
                margin-bottom: 30px;
        }
    )");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(subtitleLabel);

    // Info text
    QLabel* infoLabel = new QLabel(tr("You will be redirected to GOG's secure login page"), container);
    infoLabel->setStyleSheet(R"(
        QLabel {
                color: #8a8884;
                font-size: 14px;
                margin-bottom: 20px;
        }
    )");
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setWordWrap(true);
    containerLayout->addWidget(infoLabel);

    // Add spacing before button
    containerLayout->addSpacing(20);

    // Login button with GOG branding
    QPushButton* loginButton = new QPushButton(tr("🔐 Sign In with GOG"), container);
    loginButton->setMinimumHeight(56);
    loginButton->setStyleSheet(R"(
        QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #9b4dca, stop:1 #8b3dba);
                border: none;
                border-radius: 12px;
                padding: 16px 40px;
                color: white;
                font-size: 16px;
                font-weight: 600;
                margin-top: 10px;
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
    containerLayout->addWidget(loginButton);

    // Add spacing after button
    containerLayout->addSpacing(20);

    // Add container to main layout with centering
    mainLayout->addStretch(1);
    QHBoxLayout* centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addWidget(container);
    centerLayout->addStretch();
    mainLayout->addLayout(centerLayout);
    mainLayout->addStretch(1);

    // Set page background - GOG Galaxy beige/cream color
    setStyleSheet(R"(
        LoginPage {
                background: #f5f3f0;
        }
    )");

    // Connect signals
    connect(loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
}

LoginPage::~LoginPage()
{
}

void LoginPage::onLoginClicked()
{
    // Emit with empty credentials - OAuth dialog will handle authentication
    emit loginRequested("", "");
}

} // namespace ui
} // namespace opengalaxy
