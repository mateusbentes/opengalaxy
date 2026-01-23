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
    
    // Auto-fill credentials when page loads
    connect(webView_, &QWebEngineView::loadFinished, this, [this](bool ok) {
        if (ok) {
            autoFillCredentials();
        }
    });
    
    layout->addWidget(webView_);
    
    // Add info label
    QLabel* infoLabel = new QLabel(tr("Logging in to GOG..."), this);
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

void OAuthLoginDialog::autoFillCredentials()
{
#ifdef HAVE_WEBENGINE
    // JavaScript to auto-fill the login form
    QString js = QString(R"(
        (function() {
            // Wait for form to be ready
            var checkForm = setInterval(function() {
                var usernameField = document.getElementById('login_username') || 
                                  document.querySelector('input[name="login[username]"]') ||
                                  document.querySelector('input[type="email"]');
                var passwordField = document.getElementById('login_password') || 
                                  document.querySelector('input[name="login[password]"]') ||
                                  document.querySelector('input[type="password"]');
                var submitButton = document.getElementById('login_submit') ||
                                 document.querySelector('button[type="submit"]') ||
                                 document.querySelector('input[type="submit"]');
                
                if (usernameField && passwordField) {
                    clearInterval(checkForm);
                    
                    // Fill in credentials
                    usernameField.value = '%1';
                    passwordField.value = '%2';
                    
                    // Trigger input events (some forms need this)
                    usernameField.dispatchEvent(new Event('input', { bubbles: true }));
                    passwordField.dispatchEvent(new Event('input', { bubbles: true }));
                    usernameField.dispatchEvent(new Event('change', { bubbles: true }));
                    passwordField.dispatchEvent(new Event('change', { bubbles: true }));
                    
                    // Auto-submit after a short delay
                    setTimeout(function() {
                        if (submitButton) {
                            submitButton.click();
                        } else {
                            // Try form submit
                            var form = usernameField.closest('form');
                            if (form) {
                                form.submit();
                            }
                        }
                    }, 500);
                }
            }, 100);
            
            // Stop checking after 10 seconds
            setTimeout(function() {
                clearInterval(checkForm);
            }, 10000);
        })();
    )").arg(username_.replace("'", "\\'"), 
            password_.replace("'", "\\'"));
    
    webView_->page()->runJavaScript(js);
#endif
}

} // namespace ui
} // namespace opengalaxy
