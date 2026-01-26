// SPDX-License-Identifier: Apache-2.0
#include "proton_discovery.h"

#include <QDir>
#include <QFileInfo>

namespace opengalaxy::runners {

static void scanCompatToolsDir(const QString& dirPath, std::vector<ProtonInstall>& out)
{
    QDir dir(dirPath);
    if (!dir.exists()) return;

    const auto entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& fi : entries) {
        const QString base = fi.fileName();
        if (!base.startsWith("GE-Proton")) continue;

        const QString protonScript = QDir(fi.absoluteFilePath()).filePath("proton");
        if (!QFileInfo::exists(protonScript)) continue;

        ProtonInstall p;
        p.name = QString("Proton-GE (%1)").arg(base);
        p.protonDir = fi.absoluteFilePath();
        out.push_back(p);
    }
}

std::vector<ProtonInstall> discoverProtonGE()
{
    std::vector<ProtonInstall> installs;

    // Standard Steam locations
    scanCompatToolsDir(QDir::home().filePath(".steam/root/compatibilitytools.d"), installs);
    scanCompatToolsDir(QDir::home().filePath(".local/share/Steam/compatibilitytools.d"), installs);

    // Flatpak Steam
    scanCompatToolsDir(
        QDir::home().filePath(
            ".var/app/com.valvesoftware.Steam/data/Steam/compatibilitytools.d"),
        installs);

    return installs;
}

} // namespace opengalaxy::runners
