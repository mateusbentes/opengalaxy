#ifndef OAUTH_LOGIN_DIALOG_H
#define OAUTH_LOGIN_DIALOG_H

#include <QDialog>
#include <QString>
#include <QTcpServer>
#include <QLabel>

namespace opengalaxy {
namespace ui {

class OAuthLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OAuthLoginDialog(QWidget* parent = nullptr);
    explicit OAuthLoginDialog(const QString& username, const QString& password, QWidget* parent = nullptr);
    ~OAuthLoginDialog() override;

    QString authorizationCode() const { return authCode_; }
    bool wasSuccessful() const { return success_; }

signals:
    void authorizationReceived(const QString& code);

private slots:
    void onIncomingConnection();
    void onReadyRead();

private:
    void setupUi();
    void startOAuthFlow();
    QString extractCodeFromUrl(const QUrl& url);

    QTcpServer* localServer_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QString authCode_;
    bool success_ = false;
    QString username_;
    QString password_;
    int localPort_ = 0;

    // GOG OAuth parameters
    static constexpr const char* CLIENT_ID = "46899977096215655";
    static constexpr const char* REDIRECT_URI = "http://localhost:7127";
    static constexpr int LOCAL_PORT = 7127;
};

} // namespace ui
} // namespace opengalaxy

#endif // OAUTH_LOGIN_DIALOG_H
