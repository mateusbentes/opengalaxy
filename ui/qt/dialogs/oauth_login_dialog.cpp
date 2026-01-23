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

    // Info text with instructions
    QLabel* infoLabel = new QLabel(
        tr("Your browser will open with GOG's login page.\n\n"
           "After logging in:\n"
           "1. You'll see a success page\n"
           "2. Copy the ENTIRE URL from your browser's address bar\n"
           "3. Paste it here\n\n"
           "The URL will look like:\n"
           "https://embed.gog.com/on_login_success?code=..."),
        this
    );
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet(R"(
        QLabel {
            font-size: 13px;
            color: #5a5855;
            padding: 20px;
            background: #f5f3f0;
            border-radius: 8px;
            line-height: 1.5;
        }
    )");
    infoLabel->setAlignment(Qt::AlignLeft);
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
    
    statusLabel_->setText(tr("Browser opened. After logging in, copy the URL and paste it here."));
    
    // Show input dialog after a delay to give user time to see the instructions
    QTimer::singleShot(3000, this, &OAuthLoginDialog::showCodeInputDialog);
}

void OAuthLoginDialog::showCodeInputDialog()
{
    bool ok;
    QString input = QInputDialog::getText(this, 
        tr("Paste Login URL"),
        tr("After logging in to GOG, copy the ENTIRE URL from your browser's address bar and paste it here.\n\n"
           "It should look like:\n"
           "https://embed.gog.com/on_login_success?code=XXXXX\n\n"
           "Paste the URL:"),
        QLineEdit::Normal,
        "",
        &ok);
    
    if (ok && !input.isEmpty()) {
        // Try to extract code from URL
        QString trimmedInput = input.trimmed();
        
        // If it's a full URL, extract the code
        if (trimmedInput.contains("code=")) {
            QUrl url(trimmedInput);
            authCode_ = extractCodeFromUrl(url);
        } else {
            // Maybe they just pasted the code directly
            authCode_ = trimmedInput;
        }
        
        if (!authCode_.isEmpty()) {
            success_ = true;
            statusLabel_->setText(tr("Code received! Logging in..."));
            emit authorizationReceived(authCode_);
            accept();
        } else {
            QMessageBox::warning(this, tr("Invalid Input"),
                tr("Could not find authorization code in the URL.\n\n"
                   "Please make sure you copied the entire URL from the browser's address bar."));
            // Try again
            QTimer::singleShot(500, this, &OAuthLoginDialog::showCodeInputDialog);
        }
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
