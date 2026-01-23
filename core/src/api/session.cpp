// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/api/session.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QTimer>
#include "opengalaxy/net/http_client.h"
#include "opengalaxy/util/result.h"

namespace opengalaxy::api {

Session::Session(QObject* parent) : QObject(parent), authenticated_(false) {
    loadSession();
}

Session::~Session() = default;

void Session::loginWithPassword(const QString& username, const QString& password, AuthCallback callback)
{
    QTimer::singleShot(100, this, [this, username, password, callback = std::move(callback)]() mutable {
        // NOTE: These are GOG's public OAuth client credentials for desktop applications.
        // They are not secret and are meant to be embedded in client applications.
        // See: https://gogapidocs.readthedocs.io/en/latest/auth.html
        const QString CLIENT_ID = qEnvironmentVariable("GOG_CLIENT_ID", "468999770165");
        const QString CLIENT_SECRET = qEnvironmentVariable("GOG_CLIENT_SECRET", "9d85bc8330b3df6d97c98e309705a47ddbd299ca");

        // GOG API uses GET with URL parameters, not POST with JSON
        QString url = QString("https://auth.gog.com/token?client_id=%1&client_secret=%2&grant_type=password&username=%3&password=%4")
                          .arg(QString(QUrl::toPercentEncoding(CLIENT_ID)),
                               QString(QUrl::toPercentEncoding(CLIENT_SECRET)),
                               QString(QUrl::toPercentEncoding(username)),
                               QString(QUrl::toPercentEncoding(password)));

        net::HttpClient* client = new net::HttpClient(this);
        client->get(url,
                     [this, callback = std::move(callback)](util::Result<net::HttpClient::Response> result) mutable {
                         if (!result.isOk()) {
                             // Network-level error (connection failed, timeout, etc.)
                             QString errorMsg = result.errorMessage();
                             
                             if (errorMsg.contains("timeout") || errorMsg.toLower().contains("timed out")) {
                                 callback(util::Result<AuthTokens>::error("Connection timeout. Please try again."));
                             } else if (errorMsg.contains("network") || errorMsg.contains("connection") || 
                                        errorMsg.contains("host") || errorMsg.contains("resolve")) {
                                 callback(util::Result<AuthTokens>::error("Network error. Please check your internet connection."));
                             } else {
                                 callback(util::Result<AuthTokens>::error("Login failed. Please try again."));
                             }
                             return;
                         }
                         
                         // Parse response body (works for both success and error responses)
                         const auto& response = result.value();
                         auto json = QJsonDocument::fromJson(response.body).object();
                         
                         // Check for GOG API error response
                         if (json.contains("error")) {
                             QString errorCode = json["error"].toString();
                             QString errorDesc = json["error_description"].toString();
                             
                             // Map GOG error codes to user-friendly messages
                             if (errorCode == "invalid_grant" || errorCode == "invalid_client" || 
                                 errorCode == "unauthorized_client") {
                                 callback(util::Result<AuthTokens>::error("Login or password are wrong"));
                             } else if (errorCode == "invalid_request") {
                                 callback(util::Result<AuthTokens>::error("Invalid request. Please try again."));
                             } else if (!errorDesc.isEmpty()) {
                                 callback(util::Result<AuthTokens>::error(errorDesc));
                             } else {
                                 callback(util::Result<AuthTokens>::error("Login or password are wrong"));
                             }
                             return;
                         }
                         
                         // Check HTTP status code
                         if (response.statusCode >= 400) {
                             if (response.statusCode == 401 || response.statusCode == 403) {
                                 callback(util::Result<AuthTokens>::error("Login or password are wrong"));
                             } else if (response.statusCode >= 500) {
                                 callback(util::Result<AuthTokens>::error("Server error. Please try again later."));
                             } else {
                                 callback(util::Result<AuthTokens>::error("Login failed. Please try again."));
                             }
                             return;
                         }
                         
                         // Success - extract tokens
                         AuthTokens tokens;
                         tokens.accessToken = json["access_token"].toString();
                         tokens.refreshToken = json["refresh_token"].toString();
                         tokens.expiresAt = QDateTime::currentDateTime().addSecs(json["expires_in"].toInt());
                         
                         if (tokens.accessToken.isEmpty()) {
                             callback(util::Result<AuthTokens>::error("Invalid response from server"));
                             return;
                         }
                         
                         setTokens(tokens);
                         callback(util::Result<AuthTokens>::success(tokens));
                     });
    });
}

void Session::loginWithOAuth(AuthCallback callback)
{
    // This is called from UI - the UI will show OAuth dialog and call loginWithAuthCode
    QTimer::singleShot(0, this, [callback = std::move(callback)]() mutable {
        callback(util::Result<AuthTokens>::error("OAuth dialog should be shown by UI layer"));
    });
}

void Session::loginWithAuthCode(const QString& authCode, AuthCallback callback)
{
    QTimer::singleShot(100, this, [this, authCode, callback = std::move(callback)]() mutable {
        const QString CLIENT_ID = qEnvironmentVariable("GOG_CLIENT_ID", "46899977096215655");
        const QString CLIENT_SECRET = qEnvironmentVariable("GOG_CLIENT_SECRET", "9d85c43b1482497dbbce61f6e4aa173a433796eeae2ca8c5f6129f2dc4de46d9");
        const QString REDIRECT_URI = "https://embed.gog.com/on_login_success?origin=client";

        // Exchange authorization code for tokens
        QString url = QString("https://auth.gog.com/token?client_id=%1&client_secret=%2&grant_type=authorization_code&code=%3&redirect_uri=%4")
                          .arg(QString(QUrl::toPercentEncoding(CLIENT_ID)),
                               QString(QUrl::toPercentEncoding(CLIENT_SECRET)),
                               QString(QUrl::toPercentEncoding(authCode)),
                               QString(QUrl::toPercentEncoding(REDIRECT_URI)));

        net::HttpClient* client = new net::HttpClient(this);
        client->get(url,
                     [this, callback = std::move(callback)](util::Result<net::HttpClient::Response> result) mutable {
                         if (!result.isOk()) {
                             QString errorMsg = result.errorMessage();
                             
                             if (errorMsg.contains("timeout") || errorMsg.toLower().contains("timed out")) {
                                 callback(util::Result<AuthTokens>::error("Connection timeout. Please try again."));
                             } else if (errorMsg.contains("network") || errorMsg.contains("connection") || 
                                        errorMsg.contains("host") || errorMsg.contains("resolve")) {
                                 callback(util::Result<AuthTokens>::error("Network error. Please check your internet connection."));
                             } else {
                                 callback(util::Result<AuthTokens>::error("Login failed. Please try again."));
                             }
                             return;
                         }
                         
                         const auto& response = result.value();
                         auto json = QJsonDocument::fromJson(response.body).object();
                         
                         // Check for GOG API error response
                         if (json.contains("error")) {
                             QString errorCode = json["error"].toString();
                             QString errorDesc = json["error_description"].toString();
                             
                             if (errorCode == "invalid_grant" || errorCode == "invalid_client") {
                                 callback(util::Result<AuthTokens>::error("Authorization failed. Please try logging in again."));
                             } else if (!errorDesc.isEmpty()) {
                                 callback(util::Result<AuthTokens>::error(errorDesc));
                             } else {
                                 callback(util::Result<AuthTokens>::error("Authorization failed"));
                             }
                             return;
                         }
                         
                         if (response.statusCode >= 400) {
                             callback(util::Result<AuthTokens>::error("Authorization failed. Please try again."));
                             return;
                         }
                         
                         // Success - extract tokens
                         AuthTokens tokens;
                         tokens.accessToken = json["access_token"].toString();
                         tokens.refreshToken = json["refresh_token"].toString();
                         tokens.expiresAt = QDateTime::currentDateTime().addSecs(json["expires_in"].toInt());
                         
                         if (tokens.accessToken.isEmpty()) {
                             callback(util::Result<AuthTokens>::error("Invalid response from server"));
                             return;
                         }
                         
                         setTokens(tokens);
                         callback(util::Result<AuthTokens>::success(tokens));
                     });
    });
}

void Session::logout() {
    tokens_ = {};
    user_ = {};
    authenticated_ = false;
    emit loggedOut();
}

void Session::refreshToken(AuthCallback callback) {
    QTimer::singleShot(100, this, [this, callback = std::move(callback)]() mutable {
        if (tokens_.refreshToken.isEmpty()) {
            callback(util::Result<AuthTokens>::error("No refresh token"));
            return;
        }
        
        const QString CLIENT_ID = qEnvironmentVariable("GOG_CLIENT_ID", "468999770165");
        const QString CLIENT_SECRET = qEnvironmentVariable("GOG_CLIENT_SECRET", "9d85bc8330b3df6d97c98e309705a47ddbd299ca");
        
        // GOG API uses GET with URL parameters, not POST with JSON
        QString url = QString("https://auth.gog.com/token?client_id=%1&client_secret=%2&grant_type=refresh_token&refresh_token=%3")
                          .arg(QString(QUrl::toPercentEncoding(CLIENT_ID)),
                               QString(QUrl::toPercentEncoding(CLIENT_SECRET)),
                               QString(QUrl::toPercentEncoding(tokens_.refreshToken)));

        net::HttpClient* client = new net::HttpClient(this);
        connect(client, &net::HttpClient::destroyed, this, &QObject::deleteLater);
        client->get(url, 
                     [this, callback = std::move(callback)](util::Result<net::HttpClient::Response> result) mutable {
            if (result) {
                auto json = QJsonDocument::fromJson(result.value().body).object();
                AuthTokens newTokens;
                newTokens.accessToken = json["access_token"].toString();
                newTokens.refreshToken = json["refresh_token"].toString();
                newTokens.expiresAt = QDateTime::currentDateTime().addSecs(json["expires_in"].toInt());
                setTokens(newTokens);
                callback(util::Result<AuthTokens>::success(newTokens));
            } else {
                logout();
                callback(util::Result<AuthTokens>::error(result.errorMessage()));
            }
        });
    });
}

bool Session::isAuthenticated() const {
    return authenticated_;
}

void Session::setTokens(const AuthTokens& tokens) {
    tokens_ = tokens;
    authenticated_ = tokens.isValid();
    if (authenticated_) {
        fetchUserInfo([this](util::Result<UserInfo> result) {
            if (result) {
                user_ = result.value();
                emit authenticated(user_);
                saveSession();
            } else {
                emit authenticationFailed(result.errorMessage());
            }
        });
    }
}

void Session::loadSession() {
    QString path = getSecureStoragePath();
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        auto doc = QJsonDocument::fromJson(file.readAll());
        auto obj = doc.object();
        tokens_.accessToken = obj["accessToken"].toString();
        tokens_.refreshToken = obj["refreshToken"].toString();
        tokens_.expiresAt = QDateTime::fromString(obj["expiresAt"].toString(), Qt::ISODate);
        if (tokens_.isValid()) {
            authenticated_ = true;
            refreshToken([this](util::Result<AuthTokens>){});
        }
    }
}

void Session::saveSession() {
    QString path = getSecureStoragePath();
    QDir().mkpath(QFileInfo(path).path());
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject obj;
        obj["accessToken"] = tokens_.accessToken;
        obj["refreshToken"] = tokens_.refreshToken;
        obj["expiresAt"] = tokens_.expiresAt.toString(Qt::ISODate);
        file.write(QJsonDocument(obj).toJson());
    }
}

void Session::fetchUserInfo(UserCallback callback)
{
    // Matches MiniGalaxy approach: embed endpoint gives username with Bearer token.
    net::HttpClient* client = new net::HttpClient(this);

    net::HttpClient::Request req;
    req.url = "https://embed.gog.com/userData.json";
    req.method = "GET";
    req.headers["Authorization"] = "Bearer " + tokens_.accessToken;

    client->request(req, [callback = std::move(callback)](util::Result<net::HttpClient::Response> result) mutable {
        if (!result.isOk()) {
            callback(util::Result<UserInfo>::error(result.errorMessage()));
            return;
        }

        const QJsonObject obj = QJsonDocument::fromJson(result.value().body).object();
        UserInfo user;
        user.username = obj.value("username").toString();
        user.userId = obj.value("userId").toVariant().toString();
        callback(util::Result<UserInfo>::success(user));
    });
}


QString Session::getSecureStoragePath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/session.json";
}

} // namespace opengalaxy::api
