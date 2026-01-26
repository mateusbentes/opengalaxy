// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "opengalaxy/runners/runner.h"

namespace opengalaxy::runners {

class ProtonRunner : public Runner {
  public:
    ProtonRunner(QString name, QString protonDir);

    QString name() const override;
    QString version() const override;
    bool isAvailable() const override;
    RunnerCapabilities capabilities() const override;

    bool canRun(const LaunchConfig &config) const override;
    std::unique_ptr<QProcess> launch(const LaunchConfig &config) override;

  private:
    QString runnerName_;
    QString protonDir_;

    QString protonScriptPath_() const;
};

} // namespace opengalaxy::runners
