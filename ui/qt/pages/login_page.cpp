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
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create centered container
    QWidget* container = new QWidget(this);
    container->setMaximumWidth(450);
    
    QVBoxLayout* containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(40, 40, 40, 40);
    containerLayout->setSpacing(20);
    
    // Title
    QLabel* titleLabel = new QLabel("Welcome to OpenGalaxy", container);
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #ffffff;
            font-size: 32px;
            font-weight: 700;
            margin-bottom: 20px;
        }
    )");
    titleLabel->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(titleLabel);
    
    // Subtitle
    QLabel* subtitleLabel = new QLabel("Sign in to access your library", container);
    subtitleLabel->setStyleSheet(R"(
        QLabel {
            color: #b8b8d1;
            font-size: 16px;
            margin-bottom: 30px;
        }
    )");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(subtitleLabel);
    
    // Username field
    usernameEdit = new QLineEdit(container);
    usernameEdit->setPlaceholderText("Email or Username");
    usernameEdit->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 0.08);
            border: 2px solid rgba(124, 77, 255, 0.3);
            border-radius: 12px;
            padding: 16px 20px;
            color: #ffffff;
            font-size: 16px;
        }
        QLineEdit:focus {
            border-color: #7c4dff;
            background: rgba(255, 255, 255, 0.12);
        }
        QLineEdit::placeholder {
            color: rgba(255, 255, 255, 0.4);
        }
    )");
    containerLayout->addWidget(usernameEdit);
    
    // Password field
    passwordEdit = new QLineEdit(container);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet(usernameEdit->styleSheet());
    containerLayout->addWidget(passwordEdit);
    
    // Login button
    QPushButton* loginButton = new QPushButton("Sign In", container);
    loginButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #7c4dff, stop:1 #5a3aff);
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
                stop:0 #8c5dff, stop:1 #6a4aff);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #6c3def, stop:1 #4a2aef);
        }
    )");
    containerLayout->addWidget(loginButton);
    
    // (OAuth removed; password login only)

    containerLayout->addStretch();

    // Add container to main layout with centering
    mainLayout->addStretch();
    QHBoxLayout* centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addWidget(container);
    centerLayout->addStretch();
    mainLayout->addLayout(centerLayout);
    mainLayout->addStretch();

    // Set page background
    setStyleSheet(R"(
        LoginPage {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a0f2e, stop:1 #2d1b4e);
        }
    )");

    // Connect signals
    connect(loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);
}

LoginPage::~LoginPage()
{
}

void LoginPage::onLoginClicked()
{
    const QString username = usernameEdit->text().trimmed();
    const QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Error", "Please enter both username and password.");
        return;
    }

    emit loginRequested(username, password);
}

void LoginPage::onOAuthClicked()
{
    // Removed (password login only)
}

} // namespace ui
} // namespace opengalaxy
