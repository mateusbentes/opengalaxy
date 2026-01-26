// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../util/result.h"
#include "models.h"
#include <QObject>
#include <QString>
#include <functional>

namespace opengalaxy::api {

/**
 * @brief Session manager for authentication and token management
 */
class Session : public QObject {
    Q_OBJECT

  public:
    explicit Session(QObject *parent = nullptr);
    ~Session() override;

    using AuthCallback = std::function<void(util::Result<AuthTokens>)>;
    using UserCallback = std::function<void(util::Result<UserInfo>)>;

    // Authentication methods
    void loginWithPassword(const QString &username, const QString &password, AuthCallback callback);
    void loginWithOAuth(AuthCallback callback); // Opens browser for OAuth flow
    void loginWithAuthCode(const QString &authCode,
                           AuthCallback callback); // Exchange auth code for tokens
    void logout();
    void refreshToken(AuthCallback callback);

    // Session state
    bool isAuthenticated() const;
    const AuthTokens &tokens() const { return tokens_; }
    const UserInfo &user() const { return user_; }

    // Token management
    void setTokens(const AuthTokens &tokens);
    void loadSession(); // Load from secure storage
    void saveSession(); // Save to secure storage

  signals:
    void authenticated(const UserInfo &user);
    void authenticationFailed(const QString &error);
    void sessionExpired();
    void loggedOut();

  private:
    AuthTokens tokens_;
    UserInfo user_;
    bool authenticated_ = false;

    void fetchUserInfo(UserCallback callback);
    QString getSecureStoragePath() const;
};

} // namespace opengalaxy::api
