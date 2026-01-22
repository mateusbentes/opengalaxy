#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QPropertyAnimation>

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    
signals:
    void loginSuccess();

private slots:
    void onLoginClicked();

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPropertyAnimation *fadeAnimation;
};

#endif // LOGIN_PAGE_H
