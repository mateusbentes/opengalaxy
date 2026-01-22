#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

namespace opengalaxy {
namespace ui {

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

private slots:
    void onLoginClicked();

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
};

} // namespace ui
} // namespace opengalaxy

#endif // LOGIN_PAGE_H
