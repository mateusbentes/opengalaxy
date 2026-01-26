// SPDX-License-Identifier: Apache-2.0
#include <QtTest/QtTest>
#include "opengalaxy/api/session.h"
#include "opengalaxy/api/gog_client.h"
#include "opengalaxy/net/http_client.h"

class ApiTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Setup test environment
    }

    void testSessionCreation() {
        opengalaxy::api::Session session;
        QVERIFY(!session.isAuthenticated());
    }

    void testTokenExpiry() {
        opengalaxy::api::AuthTokens tokens;
        tokens.accessToken = "test_token";
        tokens.expiresAt = QDateTime::currentDateTime().addSecs(-3600);  // Expired

        QVERIFY(tokens.isExpired());
        QVERIFY(!tokens.isValid());
    }

    void testTokenValid() {
        opengalaxy::api::AuthTokens tokens;
        tokens.accessToken = "test_token";
        tokens.expiresAt = QDateTime::currentDateTime().addSecs(3600);  // Valid for 1 hour

        QVERIFY(!tokens.isExpired());
        QVERIFY(tokens.isValid());
    }

    void testHttpClientGet() {
        opengalaxy::net::HttpClient client;

        QSignalSpy spy(&client, &opengalaxy::net::HttpClient::requestFinished);

        bool callbackCalled = false;
        client.get("https://httpbin.org/get", [&callbackCalled](opengalaxy::util::Result<opengalaxy::net::HttpClient::Response> result) {
                callbackCalled = true;
                QVERIFY(result.isOk());
                QCOMPARE(result.value().statusCode, 200);
        });

        // Wait for async operation
        QTest::qWait(5000);
        QVERIFY(callbackCalled);
        QCOMPARE(spy.count(), 1);
    }

    void testHttpClientTimeout() {
        opengalaxy::net::HttpClient client;

        opengalaxy::net::HttpClient::Request req;
        req.url = "https://httpbin.org/delay/10";  // 10 second delay
        req.timeoutMs = 1000;  // 1 second timeout
        req.maxRetries = 0;

        bool callbackCalled = false;
        client.request(req, [&callbackCalled](opengalaxy::util::Result<opengalaxy::net::HttpClient::Response> result) {
                callbackCalled = true;
                QVERIFY(result.isError());
        });

        QTest::qWait(3000);
        QVERIFY(callbackCalled);
    }

    void cleanupTestCase() {
        // Cleanup
    }
};

QTEST_MAIN(ApiTests)
#include "api_tests.moc"
