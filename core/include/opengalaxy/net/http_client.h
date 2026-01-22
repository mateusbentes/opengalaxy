// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <functional>
#include "../util/result.h"

namespace opengalaxy::net {

/**
 * @brief Async HTTP client with timeout and retry support
 */
class HttpClient : public QObject {
    Q_OBJECT

public:
    explicit HttpClient(QObject* parent = nullptr);
    ~HttpClient() override;

    struct Request {
        QString url;
        QString method = "GET";
        QByteArray body;
        QMap<QString, QString> headers;
        int timeoutMs = 30000;
        int maxRetries = 3;
    };

    struct Response {
        int statusCode = 0;
        QByteArray body;
        QMap<QString, QString> headers;
        QString error;
    };

    using Callback = std::function<void(util::Result<Response>)>;

    // Async request
    void request(const Request& req, Callback callback);

    // Convenience methods
    void get(const QString& url, Callback callback);
    void post(const QString& url, const QByteArray& body, Callback callback);
    void postJson(const QString& url, const QJsonObject& json, Callback callback);

    // Download file with progress
    using ProgressCallback = std::function<void(qint64 received, qint64 total)>;
    void downloadFile(const QString& url, const QString& destPath,
                     Callback callback, ProgressCallback progressCallback = nullptr);

    // Set default headers (e.g., User-Agent, Authorization)
    void setDefaultHeader(const QString& name, const QString& value);
    void clearDefaultHeaders();

signals:
    void requestStarted(const QString& url);
    void requestFinished(const QString& url, int statusCode);

private:
    QNetworkAccessManager* manager_;
    QMap<QString, QString> defaultHeaders_;

    void executeRequest(const Request& req, Callback callback, int retryCount = 0);
    QNetworkRequest buildRequest(const Request& req);
};

} // namespace opengalaxy::net
