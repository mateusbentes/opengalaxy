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
    void loginRequested(const QString& username, const QString& password);

private slots:
    void onLoginClicked();
};

} // namespace ui
} // namespace opengalaxy

#endif // LOGIN_PAGE_H
