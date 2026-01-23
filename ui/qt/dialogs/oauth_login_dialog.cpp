#include "oauth_login_dialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QUrlQuery>
#include <QDesktopServices>
#include <QTcpSocket>
#include <QTimer>
#include <QRegularExpression>

namespace opengalaxy {
namespace ui {

OAuthLoginDialog::OAuthLoginDialog(QWidget* parent)
    : QDialog(parent)
    , username_("")
    , password_("")
{
    setupUi();
}

OAuthLoginDialog::OAuthLoginDialog(const QString& username, const QString& password, QWidget* parent)
    : QDialog(parent)
    , username_(username)
    , password_(password)
{
    setupUi();
}

OAuthLoginDialog::~OAuthLoginDialog() = default;

void OAuthLoginDialog::setupUi()
{
    setWindowTitle(tr("GOG Login"));
    setMinimumSize(500, 300);
    resize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    // Title
    QLabel* titleLabel = new QLabel(tr("Sign in with GOG"), this);
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 24px;
            font-weight: bold;
            color: #3c3a37;
        }
    )");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // Info text
    QLabel* infoLabel = new QLabel(
        tr("Your default web browser will open with GOG's login page.\n\n"
           "After logging in, you'll be redirected back to OpenGalaxy automatically."),
        this
    );
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet(R"(
        QLabel {
            font-size: 14px;
            color: #5a5855;
            padding: 20px;
            background: #f5f3f0;
            border-radius: 8px;
        }
    )");
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoLabel);

    // Status label
    statusLabel_ = new QLabel(tr("Waiting for login..."), this);
    statusLabel_->setStyleSheet(R"(
        QLabel {
            font-size: 12px;
            color: #8a8884;
            padding: 10px;
        }
    )");
    statusLabel_->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel_);

    layout->addStretch();

    // Cancel button
    QPushButton* cancelBtn = new QPushButton(tr("Cancel"), this);
    cancelBtn->setStyleSheet(R"(
        QPushButton {
            background: #e0e0e0;
            border: none;
            border-radius: 8px;
            padding: 12px 30px;
            font-size: 14px;
            color: #3c3a37;
        }
        QPushButton:hover {
            background: #d0d0d0;
        }
    )");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    layout->addWidget(cancelBtn, 0, Qt::AlignCenter);

    // Start OAuth flow automatically
    QTimer::singleShot(500, this, &OAuthLoginDialog::startOAuthFlow);
}

void OAuthLoginDialog::startOAuthFlow()
{
    // Start local server to receive OAuth callback on fixed port
    localServer_ = new QTcpServer(this);
    
    // Try to listen on the fixed port that GOG expects
    if (!localServer_->listen(QHostAddress::LocalHost, LOCAL_PORT)) {
        QMessageBox::critical(this, tr("Error"), 
            tr("Failed to start local server on port %1.\n\n"
               "Make sure the port is not in use by another application.\n\n"
               "Error: %2")
            .arg(LOCAL_PORT)
            .arg(localServer_->errorString()));
        reject();
        return;
    }
    
    localPort_ = LOCAL_PORT;
    connect(localServer_, &QTcpServer::newConnection, this, &OAuthLoginDialog::onIncomingConnection);
    
    // Build OAuth URL with registered redirect URI
    QString authUrl = QString("https://auth.gog.com/auth?client_id=%1&redirect_uri=%2&response_type=code&layout=client2")
                          .arg(CLIENT_ID, QUrl::toPercentEncoding(REDIRECT_URI));
    
    // Open in default browser
    if (!QDesktopServices::openUrl(QUrl(authUrl))) {
        QMessageBox::critical(this, tr("Error"), 
            tr("Failed to open web browser.\n\nPlease open this URL manually:\n%1").arg(authUrl));
        reject();
        return;
    }
    
    statusLabel_->setText(tr("Browser opened. Please sign in with GOG..."));
}

void OAuthLoginDialog::onIncomingConnection()
{
    QTcpSocket* socket = localServer_->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &OAuthLoginDialog::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void OAuthLoginDialog::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    QString request = QString::fromUtf8(socket->readAll());
    
    // Extract the URL from the HTTP request
    QRegularExpression re("GET\\s+([^\\s]+)");
    QRegularExpressionMatch match = re.match(request);
    
    if (match.hasMatch()) {
        QString path = match.captured(1);
        QUrl url("http://localhost" + path);
        
        authCode_ = extractCodeFromUrl(url);
        
        // Send response to browser
        QString response;
        if (!authCode_.isEmpty()) {
            response = R"(HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8

<!DOCTYPE html>
<html>
<head>
    <title>Login Successful</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; padding: 50px; background: #f5f3f0; }
        .success { color: #4caf50; font-size: 24px; margin-bottom: 20px; }
        .message { color: #5a5855; font-size: 16px; }
    </style>
</head>
<body>
    <div class="success">✓ Login Successful!</div>
    <div class="message">You can close this window and return to OpenGalaxy.</div>
    <script>setTimeout(function(){ window.close(); }, 2000);</script>
</body>
</html>)";
            success_ = true;
            statusLabel_->setText(tr("Login successful! Closing..."));
        } else {
            response = R"(HTTP/1.1 400 Bad Request
Content-Type: text/html; charset=utf-8

<!DOCTYPE html>
<html>
<head>
    <title>Login Failed</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; padding: 50px; background: #f5f3f0; }
        .error { color: #f44336; font-size: 24px; margin-bottom: 20px; }
        .message { color: #5a5855; font-size: 16px; }
    </style>
</head>
<body>
    <div class="error">✗ Login Failed</div>
    <div class="message">No authorization code received. Please try again.</div>
</body>
</html>)";
            statusLabel_->setText(tr("Login failed. Please try again."));
        }
        
        socket->write(response.toUtf8());
        socket->flush();
        socket->disconnectFromHost();
        
        // Close dialog after a short delay
        QTimer::singleShot(1000, this, [this]() {
            if (success_) {
                emit authorizationReceived(authCode_);
                accept();
            } else {
                reject();
            }
        });
    }
}

QString OAuthLoginDialog::extractCodeFromUrl(const QUrl& url)
{
    QUrlQuery query(url);
    return query.queryItemValue("code");
}

} // namespace ui
} // namespace opengalaxy
