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

void Session::loginWithPassword(const QString& username, const QString& password, AuthCallback callback) {
    QTimer::singleShot(100, this, [this, username, password, callback = std::move(callback)]() mutable {
        QJsonObject body;
        body["login"] = username;
        body["password"] = password;
        body["client_id"] = "468999770165";
        body["client_secret"] = "9d85bc8330b3df6d97c98e309705a47ddbd299ca";
        body["grant_type"] = "password";

        net::HttpClient* client = new net::HttpClient(this);
        connect(client, &net::HttpClient::destroyed, this, &QObject::deleteLater);
        client->post("https://auth.gog.com/token", QJsonDocument(body).toJson(), 
                     [this, callback = std::move(callback)](util::Result<net::HttpClient::Response> result) mutable {
            if (result) {
                auto json = QJsonDocument::fromJson(result.value().body).object();
                AuthTokens tokens;
                tokens.accessToken = json["access_token"].toString();
                tokens.refreshToken = json["refresh_token"].toString();
                tokens.expiresAt = QDateTime::currentDateTime().addSecs(json["expires_in"].toInt());
                setTokens(tokens);
                fetchUserInfo([callback = std::move(callback)](util::Result<UserInfo> userResult) mutable {
                    if (userResult) {
                        callback(util::Result<AuthTokens>::success({}));
                    } else {
                        callback(util::Result<AuthTokens>::error(userResult.errorMessage()));
                    }
                });
            } else {
                callback(util::Result<AuthTokens>::error(result.errorMessage()));
            }
        });
    });
}

void Session::loginWithOAuth(AuthCallback callback) {
    QTimer::singleShot(100, this, [callback = std::move(callback)]() mutable {
        callback(util::Result<AuthTokens>::error("OAuth not implemented"));
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
        QJsonObject body;
        body["refresh_token"] = tokens_.refreshToken;
        body["client_id"] = "468999770165";
        body["client_secret"] = "9d85bc8330b3df6d97c98e309705a47ddbd299ca";
        body["grant_type"] = "refresh_token";

        net::HttpClient* client = new net::HttpClient(this);
        connect(client, &net::HttpClient::destroyed, this, &QObject::deleteLater);
        client->post("https://auth.gog.com/token", QJsonDocument(body).toJson(), 
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

void Session::fetchUserInfo(UserCallback callback) {
    QTimer::singleShot(100, this, [callback = std::move(callback)]() mutable {
        UserInfo user;
        user.username = "testuser";
        user.userId = "12345";
        callback(util::Result<UserInfo>::success(user));
    });
}

QString Session::getSecureStoragePath() const {
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/session.json";
}

} // namespace opengalaxy::api
