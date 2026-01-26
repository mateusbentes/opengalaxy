// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "runner.h"
#include <QObject>
#include <memory>
#include <vector>

namespace opengalaxy::runners {

/**
 * @brief Manages all available game runners
 */
class RunnerManager : public QObject {
    Q_OBJECT

  public:
    explicit RunnerManager(QObject *parent = nullptr);
    ~RunnerManager() override;

    // Discover and register all available runners
    void discoverRunners();

    // Get all available runners
    std::vector<RunnerCapabilities> availableRunners() const;

    // Find best runner for a game
    Runner *findBestRunner(const LaunchConfig &config);

    // Get specific runner by name
    Runner *getRunner(const QString &name);

    // Register custom runner
    void registerRunner(std::unique_ptr<Runner> runner);

  signals:
    void runnersDiscovered(int count);
    void runnerAdded(const QString &name);

  private:
    std::vector<std::unique_ptr<Runner>> runners_;

    void discoverNativeRunner();
    void discoverWineRunners();
    void discoverProtonRunners();
    void discoverDOSBoxRunner();
    void discoverISATranslators();

    // Platform-specific discovery
    void discoverLinuxRunners();
    void discoverWindowsRunners();
    void discoverMacOSRunners();
};

} // namespace opengalaxy::runners
