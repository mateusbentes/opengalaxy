// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/net/http_client.h"
#include "opengalaxy/util/log.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>

namespace opengalaxy::net {

HttpClient::HttpClient(QObject* parent)
    : QObject(parent)
    , manager_(new QNetworkAccessManager(this))
{
    // Set default User-Agent
    setDefaultHeader("User-Agent", "OpenGalaxy/1.0");

    // Enable connection keep-alive to prevent "Connection closed" errors
    setDefaultHeader("Connection", "keep-alive");
    setDefaultHeader("Accept", "*/*");
}

HttpClient::~HttpClient() = default;

void HttpClient::request(const Request& req, Callback callback) {
    executeRequest(req, callback, 0);
}

void HttpClient::get(const QString& url, Callback callback) {
    Request req;
    req.url = url;
    req.method = "GET";
    request(req, callback);
}

void HttpClient::post(const QString& url, const QByteArray& body, Callback callback) {
    Request req;
    req.url = url;
    req.method = "POST";
    req.body = body;
    request(req, callback);
}

void HttpClient::postJson(const QString& url, const QJsonObject& json, Callback callback) {
    Request req;
    req.url = url;
    req.method = "POST";
    req.body = QJsonDocument(json).toJson(QJsonDocument::Compact);
    req.headers["Content-Type"] = "application/json";
    request(req, callback);
}

void HttpClient::downloadFile(const QString& url, const QString& destPath,
                                        Callback callback, ProgressCallback progressCallback) {
    emit requestStarted(url);
    LOG_INFO(QString("Downloading file: %1 -> %2").arg(url, destPath));

    QNetworkRequest request(url);
    for (auto it = defaultHeaders_.begin(); it  !=  defaultHeaders_.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    QNetworkReply* reply = manager_->get(request);

    // Progress tracking
    if (progressCallback) {
        connect(reply, &QNetworkReply::downloadProgress,
                [progressCallback](qint64 received, qint64 total) {
                progressCallback(received, total);
        });
    }

    // Handle completion
    connect(reply, &QNetworkReply::finished, [this, reply, destPath, callback, url]() {
        reply->deleteLater();

        if (reply->error()  !=  QNetworkReply::NoError) {
                LOG_ERROR(QString("Download failed: %1 - %2").arg(url, reply->errorString()));
                callback(util::Result<Response>::error(reply->errorString()));
                emit requestFinished(url, 0);
                return;
        }

        // Write to file
        QFile file(destPath);
        if (!file.open(QIODevice::WriteOnly)) {
                LOG_ERROR(QString("Failed to open file for writing: %1").arg(destPath));
                callback(util::Result<Response>::error("Failed to open file for writing"));
                emit requestFinished(url, reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
                return;
        }

        file.write(reply->readAll());
        file.close();

        Response response;
        response.statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        LOG_INFO(QString("Download completed: %1").arg(destPath));
        callback(util::Result<Response>::success(response));
        emit requestFinished(url, response.statusCode);
    });
}

void HttpClient::setDefaultHeader(const QString& name, const QString& value) {
    defaultHeaders_[name] = value;
}

void HttpClient::clearDefaultHeaders() {
    defaultHeaders_.clear();
}

void HttpClient::executeRequest(const Request& req, Callback callback, int retryCount) {
    emit requestStarted(req.url);
    LOG_DEBUG(QString("HTTP %1 %2 (attempt %3)").arg(req.method, req.url).arg(retryCount + 1));

    QNetworkRequest request = buildRequest(req);
    QNetworkReply* reply = nullptr;

    if (req.method  ==  "GET") {
        reply = manager_->get(request);
    } else if (req.method  ==  "POST") {
        reply = manager_->post(request, req.body);
    } else if (req.method  ==  "PUT") {
        reply = manager_->put(request, req.body);
    } else if (req.method  ==  "DELETE") {
        reply = manager_->deleteResource(request);
    } else {
        callback(util::Result<Response>::error("Unsupported HTTP method: " + req.method));
        return;
    }

    // Timeout handling
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [reply, timer]() {
        reply->abort();
        timer->deleteLater();
    });
    timer->start(req.timeoutMs);

    // Handle completion
    connect(reply, &QNetworkReply::finished, [this, reply, req, callback, retryCount, timer]() {
        timer->stop();
        timer->deleteLater();
        reply->deleteLater();

        Response response;
        response.statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        response.body = reply->readAll();
        response.error = reply->errorString();

        // Extract headers
        for (const auto& header : reply->rawHeaderPairs()) {
                response.headers[QString::fromUtf8(header.first)] = QString::fromUtf8(header.second);
        }

        // Check for network-level errors (not HTTP errors)
        bool hasNetworkError = (reply->error()  !=  QNetworkReply::NoError  && 
                reply->error()  !=  QNetworkReply::ProtocolFailure  && 
                reply->error()  !=  QNetworkReply::ContentNotFoundError  && 
                reply->error()  !=  QNetworkReply::AuthenticationRequiredError  && 
                reply->error()  !=  QNetworkReply::ContentAccessDenied);

        // Retry on transient errors
        bool shouldRetry = (reply->error()  ==  QNetworkReply::TimeoutError  || 
                reply->error()  ==  QNetworkReply::TemporaryNetworkFailureError  || 
                reply->error()  ==  QNetworkReply::RemoteHostClosedError  ||   // Connection closed
                reply->error()  ==  QNetworkReply::ConnectionRefusedError  || 
                response.statusCode  ==  429  ||   // Rate limit
                response.statusCode  ==  503);   // Service unavailable

        if (shouldRetry  &&  retryCount < req.maxRetries) {
                LOG_WARNING(QString("Request failed, retrying: %1").arg(req.url));
                // Exponential backoff
                int delayMs = 1000 * (1 << retryCount);
                QTimer::singleShot(delayMs, [this, req, callback, retryCount]() {
                executeRequest(req, callback, retryCount + 1);
                });
                return;
        }

        // For HTTP 4xx/5xx errors with response body, treat as success so caller can
        // parse error. This is important for APIs that return error details in JSON
        if (response.statusCode  >=  400  &&  !response.body.isEmpty()) {
                LOG_DEBUG(QString("HTTP %1 %2 -> %3 (with error body)")
                          .arg(req.method, req.url)
                          .arg(response.statusCode));
                callback(util::Result<Response>::success(response));
                emit requestFinished(req.url, response.statusCode);
                return;
        }

        // Network-level errors (connection failed, timeout, etc.)
        if (hasNetworkError) {
                LOG_ERROR(QString("HTTP request failed: %1 - %2").arg(req.url, response.error));
                callback(util::Result<Response>::error(response.error, response.statusCode));
                emit requestFinished(req.url, response.statusCode);
                return;
        }

        // Success
        LOG_DEBUG(QString("HTTP %1 %2 -> %3").arg(req.method, req.url).arg(response.statusCode));
        callback(util::Result<Response>::success(response));
        emit requestFinished(req.url, response.statusCode);
    });
}

QNetworkRequest HttpClient::buildRequest(const Request& req) {
    QNetworkRequest request(req.url);

    // Apply default headers
    for (auto it = defaultHeaders_.begin(); it  !=  defaultHeaders_.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    // Apply request-specific headers
    for (auto it = req.headers.begin(); it  !=  req.headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    return request;
}

} // namespace opengalaxy::net
