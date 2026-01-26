#ifndef OAUTH_LOGIN_DIALOG_H
#define OAUTH_LOGIN_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QString>
#include <QTcpServer>

namespace opengalaxy {
namespace ui {

class OAuthLoginDialog : public QDialog {
    Q_OBJECT

  public:
    explicit OAuthLoginDialog(QWidget *parent = nullptr);
    explicit OAuthLoginDialog(const QString &username, const QString &password,
                              QWidget *parent = nullptr);
    ~OAuthLoginDialog() override;

    QString authorizationCode() const { return authCode_; }
    bool wasSuccessful() const { return success_; }

  signals:
    void authorizationReceived(const QString &code);

  private:
    void setupUi();
    void startOAuthFlow();
    void showCodeInputDialog();
    QString extractCodeFromUrl(const QUrl &url);

    QTcpServer *localServer_ = nullptr;
    QLabel *statusLabel_ = nullptr;
    QString authCode_;
    bool success_ = false;
    QString username_;
    QString password_;
    int localPort_ = 0;

    // GOG OAuth parameters
    static constexpr const char *CLIENT_ID = "46899977096215655";
    // Use GOG's registered redirect URI for embed clients
    static constexpr const char *REDIRECT_URI =
        "https://embed.gog.com/on_login_success?origin=client";
};

} // namespace ui
} // namespace opengalaxy

#endif // OAUTH_LOGIN_DIALOG_H
