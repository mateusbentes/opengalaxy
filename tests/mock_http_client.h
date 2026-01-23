// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "opengalaxy/net/http_client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

namespace opengalaxy::test {

/**
 * @brief Mock HTTP client for testing without network calls
 */
class MockHttpClient : public net::HttpClient {
    Q_OBJECT

public:
    explicit MockHttpClient(QObject* parent = nullptr) : net::HttpClient(parent) {}

    // Set mock response for next request
    void setMockResponse(int statusCode, const QByteArray& body, const QMap<QString, QString>& headers = {}) {
        mockStatusCode_ = statusCode;
        mockBody_ = body;
        mockHeaders_ = headers;
        shouldFail_ = false;
    }

    // Set mock error for next request
    void setMockError(const QString& error) {
        mockError_ = error;
        shouldFail_ = true;
    }

    // Mock request method to return mock data
    void request(const Request& req, std::function<void(util::Result<Response>)> callback) {
        QTimer::singleShot(10, this, [this, callback]() {
            if (shouldFail_) {
                callback(util::Result<Response>::error(mockError_));
            } else {
                Response response;
                response.statusCode = mockStatusCode_;
                response.body = mockBody_;
                response.headers = mockHeaders_;
                callback(util::Result<Response>::success(response));
            }
        });
    }

private:
    int mockStatusCode_ = 200;
    QByteArray mockBody_;
    QMap<QString, QString> mockHeaders_;
    QString mockError_;
    bool shouldFail_ = false;
};

} // namespace opengalaxy::test
