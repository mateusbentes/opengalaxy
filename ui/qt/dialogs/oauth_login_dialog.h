#ifndef OAUTH_LOGIN_DIALOG_H
#define OAUTH_LOGIN_DIALOG_H

#include <QDialog>
#include <QString>

#ifdef HAVE_WEBENGINE
#include <QWebEngineView>
#include <QWebEnginePage>
#endif

namespace opengalaxy {
namespace ui {

class OAuthLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OAuthLoginDialog(const QString& username, const QString& password, QWidget* parent = nullptr);
    ~OAuthLoginDialog() override;

    QString authorizationCode() const { return authCode_; }
    bool wasSuccessful() const { return success_; }

signals:
    void authorizationReceived(const QString& code);

private slots:
    void onUrlChanged(const QUrl& url);

private:
    void setupUi();
    void autoFillCredentials();
    QString extractCodeFromUrl(const QUrl& url);

#ifdef HAVE_WEBENGINE
    QWebEngineView* webView_ = nullptr;
#endif
    QString authCode_;
    bool success_ = false;
    QString username_;
    QString password_;

    // GOG OAuth parameters
    static constexpr const char* CLIENT_ID = "46899977096215655";
    static constexpr const char* REDIRECT_URI = "https://embed.gog.com/on_login_success?origin=client";
};

} // namespace ui
} // namespace opengalaxy

#endif // OAUTH_LOGIN_DIALOG_H
