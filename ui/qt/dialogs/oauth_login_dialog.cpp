#include "oauth_login_dialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QUrlQuery>
#include <QDesktopServices>
#include <QTimer>
#include <QInputDialog>

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
    // Build OAuth URL with GOG's registered redirect URI
    QString authUrl = QString("https://auth.gog.com/auth?client_id=%1&redirect_uri=%2&response_type=code&layout=client2")
                          .arg(CLIENT_ID, QUrl::toPercentEncoding(REDIRECT_URI));
    
    // Open in default browser
    if (!QDesktopServices::openUrl(QUrl(authUrl))) {
        QMessageBox::critical(this, tr("Error"), 
            tr("Failed to open web browser.\n\nPlease open this URL manually:\n%1").arg(authUrl));
        reject();
        return;
    }
    
    statusLabel_->setText(tr("After logging in, you'll see a code.\nPlease copy and paste it here."));
    
    // Show input dialog after a delay
    QTimer::singleShot(2000, this, &OAuthLoginDialog::showCodeInputDialog);
}

void OAuthLoginDialog::showCodeInputDialog()
{
    bool ok;
    QString code = QInputDialog::getText(this, 
        tr("Enter Authorization Code"),
        tr("After logging in, GOG will show you a code.\nPlease copy and paste it here:"),
        QLineEdit::Normal,
        "",
        &ok);
    
    if (ok && !code.isEmpty()) {
        authCode_ = code.trimmed();
        success_ = true;
        statusLabel_->setText(tr("Code received! Logging in..."));
        emit authorizationReceived(authCode_);
        accept();
    } else {
        reject();
    }
}

QString OAuthLoginDialog::extractCodeFromUrl(const QUrl& url)
{
    QUrlQuery query(url);
    return query.queryItemValue("code");
}

} // namespace ui
} // namespace opengalaxy
