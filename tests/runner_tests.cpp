// SPDX-License-Identifier: Apache-2.0
#include <QtTest/QtTest>
#include "opengalaxy/runners/runner_manager.h"
#include "opengalaxy/runners/runner.h"

class RunnerTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        manager_ = new opengalaxy::runners::RunnerManager(this);
    }

    void testRunnerDiscovery() {
        manager_->discoverRunners();
        auto runners = manager_->availableRunners();
        
        // Should find at least one runner (native)
        QVERIFY(runners.size() > 0);
    }

    void testNativeRunnerAvailable() {
        manager_->discoverRunners();
        auto* runner = manager_->getRunner("Native");
        
        QVERIFY(runner != nullptr);
        QVERIFY(runner->isAvailable());
    }

    void testArchitectureDetection() {
        // Test that we can get runner capabilities which includes architecture info
        manager_->discoverRunners();
        auto runners = manager_->availableRunners();
        
        QVERIFY(runners.size() > 0);
        
        // At least one runner should match host architecture
        bool foundHostArch = false;
#if defined(__x86_64__) || defined(_M_X64)
        for (const auto& r : runners) {
            if (r.hostArch == opengalaxy::runners::Architecture::X86_64) {
                foundHostArch = true;
                break;
            }
        }
#elif defined(__aarch64__) || defined(_M_ARM64)
        for (const auto& r : runners) {
            if (r.hostArch == opengalaxy::runners::Architecture::ARM64) {
                foundHostArch = true;
                break;
            }
        }
#endif
        QVERIFY(foundHostArch);
    }

    void testPlatformDetection() {
        manager_->discoverRunners();
        auto runners = manager_->availableRunners();
        
        QVERIFY(runners.size() > 0);
        
        // At least one runner should support host platform
        bool foundHostPlatform = false;
#ifdef Q_OS_LINUX
        for (const auto& r : runners) {
            if (r.supportedPlatform == opengalaxy::runners::Platform::Linux) {
                foundHostPlatform = true;
                break;
            }
        }
#elif defined(Q_OS_WIN)
        for (const auto& r : runners) {
            if (r.supportedPlatform == opengalaxy::runners::Platform::Windows) {
                foundHostPlatform = true;
                break;
            }
        }
#elif defined(Q_OS_MACOS)
        for (const auto& r : runners) {
            if (r.supportedPlatform == opengalaxy::runners::Platform::MacOS) {
                foundHostPlatform = true;
                break;
            }
        }
#endif
        QVERIFY(foundHostPlatform);
    }

    void testBestRunnerSelection() {
        manager_->discoverRunners();
        
        opengalaxy::runners::LaunchConfig config;
        config.gamePath = "/tmp/test_game";
        config.gamePlatform = opengalaxy::runners::Platform::Linux;
        config.gameArch = opengalaxy::runners::Architecture::X86_64;
        
        auto* runner = manager_->findBestRunner(config);
        QVERIFY(runner != nullptr);
    }

    void cleanupTestCase() {
        delete manager_;
    }

private:
    opengalaxy::runners::RunnerManager* manager_;
};

QTEST_MAIN(RunnerTests)
#include "runner_tests.moc"
