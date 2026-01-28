// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "runner.h"
#include <QString>

namespace opengalaxy::runners {

/**
 * @brief DOSBox runner for MS-DOS games
 *
 * Handles launching MS-DOS games using DOSBox emulator.
 * Automatically configures DOSBox for optimal game compatibility.
 */
class DOSBoxRunner : public Runner {
  public:
    explicit DOSBoxRunner(const QString &dosboxPath);
    ~DOSBoxRunner() override = default;

    // Runner information
    QString name() const override;
    QString version() const override;
    bool isAvailable() const override;
    RunnerCapabilities capabilities() const override;

    // Check if this runner can run the given game
    bool canRun(const LaunchConfig &config) const override;

    // Launch game
    std::unique_ptr<QProcess> launch(const LaunchConfig &config) override;

    // Configuration
    QStringList configOptions() const override;
    void setConfigOption(const QString &key, const QString &value) override;

  private:
    QString dosboxPath_;
    QString cpuCycles_ = "max";         // CPU cycles: max, auto, or specific number
    QString renderScaler_ = "normal2x"; // Scaler: normal2x, normal3x, etc.
    bool fullscreen_ = false;

    // Helper to create DOSBox configuration
    QString createDOSBoxConfig(const LaunchConfig &config);

    // Helper to detect if a game is DOS-based
    static bool isDOSGame(const QString &executablePath);
};

} // namespace opengalaxy::runners
