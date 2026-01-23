#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include <QWidget>
#include <QLineEdit>

namespace opengalaxy {
namespace ui {

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

signals:
    void oauthLoginRequested();

private slots:
    void onLoginClicked();

private:
    // Password login removed - OAuth only
};

} // namespace ui
} // namespace opengalaxy

#endif // LOGIN_PAGE_H
