#include "login_page.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(
        R"(
        LoginPage {
            background: qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1,
                stop:0 #1a0f2e, stop:0.3 #2a1b4a, stop:0.7 #3a2b5e, stop:1 #4a3b7e);
        }
        
        QLabel {
            color: #e0e0ff;
            font-size: 28px;
            font-weight: 700;
            margin-bottom: 40px;
        }
        
        QLineEdit {
            background: rgba(255,255,255,0.1);
            border: 2px solid transparent;
            border-radius: 12px;
            padding: 16px 20px;
            color: #ffffff;
            font-size: 16px;
            margin-bottom: 20px;
            backdrop-filter: blur(20px);
        }
        
        QLineEdit:focus {
            border-color: #7c4dff;
            background: rgba(255,255,255,0.15);
        }
        
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #7c4dff, stop:1 #5a3aff);
            border: none;
            border-radius: 12px;
            padding: 16px 40px;
            color: white;
            font-size: 16px;
            font-weight: 600;
            margin-top: 20px;
        }
        
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #9a6fff, stop:1 #7c4dff);
            transform: translateY(-2px);
        }
        )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(100, 200, 100, 200);
    layout->setSpacing(30);

    QLabel *title = new QLabel("Welcome to OpenGalaxy", this);
    title->setAlignment(Qt::AlignCenter);

    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Username or Email");
    
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Sign In", this);
    
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(10);
    shadow->setColor(QColor(0,0,0,0.3));
    loginButton->setGraphicsEffect(shadow);

    layout->addWidget(title);
    layout->addWidget(usernameEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
}

void LoginPage::onLoginClicked()
{
    // Simulate login success
    emit loginSuccess();
}
