// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QString>
#include <QStringList>
#include <vector>

namespace opengalaxy::runners {

struct ProtonInstall {
    QString name;
    QString protonDir; // directory that contains the 'proton' script
};

std::vector<ProtonInstall> discoverProtonGE();

} // namespace opengalaxy::runners
