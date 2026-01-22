// SPDX-License-Identifier: Apache-2.0
#include <QtTest/QtTest>
#include "opengalaxy/util/result.h"
#include "opengalaxy/util/log.h"

class CoreTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        opengalaxy::util::Logger::instance().setLevel(opengalaxy::util::LogLevel::Debug);
    }

    void testResultSuccess() {
        auto result = opengalaxy::util::Result<int>::success(42);
        
        QVERIFY(result.isOk());
        QVERIFY(!result.isError());
        QCOMPARE(result.value(), 42);
    }

    void testResultError() {
        auto result = opengalaxy::util::Result<int>::error("Test error", 404);
        
        QVERIFY(!result.isOk());
        QVERIFY(result.isError());
        QCOMPARE(result.errorMessage(), QString("Test error"));
        QCOMPARE(result.errorCode(), 404);
    }

    void testResultValueOr() {
        auto success = opengalaxy::util::Result<int>::success(42);
        auto error = opengalaxy::util::Result<int>::error("Error");
        
        QCOMPARE(success.valueOr(0), 42);
        QCOMPARE(error.valueOr(99), 99);
    }

    void testResultVoid() {
        auto success = opengalaxy::util::Result<void>::success();
        auto error = opengalaxy::util::Result<void>::error("Test error");
        
        QVERIFY(success.isOk());
        QVERIFY(error.isError());
        QCOMPARE(error.errorMessage(), QString("Test error"));
    }

    void testLogger() {
        opengalaxy::util::Logger::instance().info("Test info message");
        opengalaxy::util::Logger::instance().warning("Test warning message");
        opengalaxy::util::Logger::instance().error("Test error message");
        
        // Just verify it doesn't crash
        QVERIFY(true);
    }

    void testLoggerLevels() {
        opengalaxy::util::Logger::instance().setLevel(opengalaxy::util::LogLevel::Error);
        QCOMPARE(opengalaxy::util::Logger::instance().level(), opengalaxy::util::LogLevel::Error);
        
        // Debug and info should be filtered
        opengalaxy::util::Logger::instance().debug("Should not appear");
        opengalaxy::util::Logger::instance().info("Should not appear");
        
        // Error should appear
        opengalaxy::util::Logger::instance().error("Should appear");
        
        QVERIFY(true);
    }

    void cleanupTestCase() {
        // Cleanup
    }
};

QTEST_MAIN(CoreTests)
#include "core_tests.moc"
