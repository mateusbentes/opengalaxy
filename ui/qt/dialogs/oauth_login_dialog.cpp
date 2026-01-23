#include "oauth_login_dialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QUrlQuery>

#ifdef HAVE_WEBENGINE
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#endif

namespace opengalaxy {
namespace ui {

OAuthLoginDialog::OAuthLoginDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
}

OAuthLoginDialog::~OAuthLoginDialog() = default;

void OAuthLoginDialog::setupUi()
{
    setWindowTitle(tr("GOG Login"));
    setMinimumSize(800, 600);
    resize(900, 700);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

#ifdef HAVE_WEBENGINE
    // Create web view
    webView_ = new QWebEngineView(this);
    
    // Configure web engine settings
    QWebEngineSettings* settings = webView_->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    
    // Clear cookies for fresh login
    QWebEngineProfile::defaultProfile()->cookieStore()->deleteAllCookies();
    
    // Build OAuth URL
    QString authUrl = QString("https://auth.gog.com/auth?client_id=%1&redirect_uri=%2&response_type=code&layout=client2")
                          .arg(CLIENT_ID, REDIRECT_URI);
    
    // Load the login page
    webView_->load(QUrl(authUrl));
    
    // Monitor URL changes to catch the redirect
    connect(webView_, &QWebEngineView::urlChanged, this, &OAuthLoginDialog::onUrlChanged);
    
    layout->addWidget(webView_);
    
    // Add info label
    QLabel* infoLabel = new QLabel(tr("Please log in with your GOG account"), this);
    infoLabel->setStyleSheet("padding: 10px; background: #f0f0f0; color: #333;");
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->insertWidget(0, infoLabel);
    
#else
    // WebEngine not available - show error
    QLabel* errorLabel = new QLabel(
        tr("OAuth login requires Qt WebEngine which is not available.\n\n"
           "Please install qt6-webengine package:\n"
           "  sudo apt install qt6-webengine\n\n"
           "Or use the command-line tool for authentication."),
        this
    );
    errorLabel->setWordWrap(true);
    errorLabel->setStyleSheet("padding: 20px; color: #d32f2f;");
    layout->addWidget(errorLabel);
    
    QPushButton* closeBtn = new QPushButton(tr("Close"), this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    layout->addWidget(closeBtn);
#endif
}

void OAuthLoginDialog::onUrlChanged(const QUrl& url)
{
    // Check if we've been redirected to the success URL
    if (url.toString().startsWith(REDIRECT_URI)) {
        authCode_ = extractCodeFromUrl(url);
        
        if (!authCode_.isEmpty()) {
            success_ = true;
            emit authorizationReceived(authCode_);
            accept();
        } else {
            QMessageBox::warning(this, tr("Login Error"), 
                               tr("Failed to extract authorization code from redirect URL."));
            reject();
        }
    }
}

QString OAuthLoginDialog::extractCodeFromUrl(const QUrl& url)
{
    QUrlQuery query(url);
    return query.queryItemValue("code");
}

} // namespace ui
} // namespace opengalaxy
