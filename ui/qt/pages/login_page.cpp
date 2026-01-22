#include "login_page.h"

namespace opengalaxy {
namespace ui {

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
{
    usernameEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("Login", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Username:", this));
    layout->addWidget(usernameEdit);
    layout->addWidget(new QLabel("Password:", this));
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
}

LoginPage::~LoginPage()
{
}

void LoginPage::onLoginClicked()
{
}

} // namespace ui
} // namespace opengalaxy
