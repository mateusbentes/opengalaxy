// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "opengalaxy/runners/runner.h"

namespace opengalaxy::runners {

class WrapperRunner : public Runner {
public:
    WrapperRunner(QString runnerName,
                        QString wrapperExecutable,
                        Platform supportedPlatform,
                        Architecture hostArch,
                        Architecture targetArch,
                        bool requiresISATranslation);

    QString name() const override;
    QString version() const override;
    bool isAvailable() const override;
    RunnerCapabilities capabilities() const override;

    bool canRun(const LaunchConfig& config) const override;
    std::unique_ptr<QProcess> launch(const LaunchConfig& config) override;

private:
    QString runnerName_;
    QString wrapperExecutable_;
    Platform supportedPlatform_;
    Architecture hostArch_;
    Architecture targetArch_;
    bool requiresISATranslation_ = false;
};

} // namespace opengalaxy::runners
