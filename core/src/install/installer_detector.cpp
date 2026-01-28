// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/install/installer_detector.h"
#include "opengalaxy/util/dos_detector.h"
#include "opengalaxy/util/log.h"

#include <QFile>
#include <QFileInfo>
#include <QSysInfo>

namespace opengalaxy::install {

InstallerType InstallerDetector::detectType(const QString &filePath) {
    const QString ext = QFileInfo(filePath).suffix().toLower();

    // Shell scripts
    if (ext == "sh" || ext == "bash") return InstallerType::ShellScript;

    // Windows batch/command
    if (ext == "bat") return InstallerType::ScriptBat;
    if (ext == "cmd") return InstallerType::ScriptCmd;
    if (ext == "ps1") return InstallerType::ScriptPowershell;

    // Scripting languages
    if (ext == "py") return InstallerType::PythonScript;
    if (ext == "rb") return InstallerType::RubyScript;
    if (ext == "pl") return InstallerType::PerlScript;
    if (ext == "js") return InstallerType::NodeScript;
    if (ext == "jar") return InstallerType::JavaJar;

    // Executables
    if (ext == "exe") {
        // Check if it's DOS or Windows
        if (util::DOSDetector::isDOSExecutable(filePath)) {
            return InstallerType::DOSExecutable;
        }
        return InstallerType::WindowsExe;
    }

    // macOS
    if (ext == "pkg") return InstallerType::MacPkg;
    if (ext == "dmg") return InstallerType::MacDmg;
    if (ext == "app") return InstallerType::MacApp;

    // Linux packages
    if (ext == "deb") return InstallerType::LinuxDeb;
    if (ext == "rpm") return InstallerType::LinuxRpm;
    if (ext == "appimage") return InstallerType::LinuxAppImage;

    // Windows packages
    if (ext == "msi") return InstallerType::WindowsMsi;

    // Universal archives
    if (ext == "zip") return InstallerType::UniversalZip;
    if (ext == "tar") return InstallerType::UniversalTar;
    if (ext == "gz" || ext == "tgz") return InstallerType::UniversalTarGz;
    if (ext == "bz2" || ext == "tbz2") return InstallerType::UniversalTarBz2;
    if (ext == "7z") return InstallerType::Universal7z;

    // CD/DVD images
    if (ext == "iso") return InstallerType::ArchiveIso;
    if (ext == "bin") return InstallerType::ArchiveBin;
    if (ext == "cue") return InstallerType::ArchiveCue;
    if (ext == "nrg") return InstallerType::ArchiveNrg;
    if (ext == "mdf") return InstallerType::ArchiveMdf;

    // Other archives
    if (ext == "rar") return InstallerType::ArchiveRar;

    return InstallerType::Unknown;
}

QString InstallerDetector::typeName(InstallerType type) {
    switch (type) {
    case InstallerType::ShellScript:
        return "Shell Script (.sh)";
    case InstallerType::WindowsExe:
        return "Windows Executable (.exe)";
    case InstallerType::DOSExecutable:
        return "DOS Executable (.exe)";
    case InstallerType::UniversalZip:
        return "ZIP Archive (.zip)";
    case InstallerType::UniversalTar:
        return "TAR Archive (.tar)";
    case InstallerType::UniversalTarGz:
        return "Gzipped TAR (.tar.gz)";
    case InstallerType::UniversalTarBz2:
        return "Bzip2 TAR (.tar.bz2)";
    case InstallerType::Universal7z:
        return "7-Zip Archive (.7z)";
    case InstallerType::MacPkg:
        return "macOS Package (.pkg)";
    case InstallerType::MacDmg:
        return "macOS Disk Image (.dmg)";
    case InstallerType::MacApp:
        return "macOS Application (.app)";
    case InstallerType::LinuxDeb:
        return "Debian Package (.deb)";
    case InstallerType::LinuxRpm:
        return "RPM Package (.rpm)";
    case InstallerType::LinuxAppImage:
        return "AppImage (.AppImage)";
    case InstallerType::WindowsMsi:
        return "Windows Installer (.msi)";
    case InstallerType::WindowsZip:
        return "Windows ZIP (.zip)";
    case InstallerType::ArchiveRar:
        return "RAR Archive (.rar)";
    case InstallerType::ArchiveIso:
        return "ISO Image (.iso)";
    case InstallerType::ArchiveBin:
        return "Binary Image (.bin)";
    case InstallerType::ArchiveCue:
        return "CUE Sheet (.cue)";
    case InstallerType::ArchiveNrg:
        return "Nero Image (.nrg)";
    case InstallerType::ArchiveMdf:
        return "Alcohol Image (.mdf)";
    case InstallerType::ScriptBat:
        return "Batch Script (.bat)";
    case InstallerType::ScriptCmd:
        return "Command Script (.cmd)";
    case InstallerType::ScriptPowershell:
        return "PowerShell Script (.ps1)";
    case InstallerType::JavaJar:
        return "Java Archive (.jar)";
    case InstallerType::PythonScript:
        return "Python Script (.py)";
    case InstallerType::RubyScript:
        return "Ruby Script (.rb)";
    case InstallerType::PerlScript:
        return "Perl Script (.pl)";
    case InstallerType::NodeScript:
        return "Node.js Script (.js)";
    case InstallerType::GoExecutable:
        return "Go Executable";
    case InstallerType::RustExecutable:
        return "Rust Executable";
    default:
        return "Unknown";
    }
}

bool InstallerDetector::isSupportedOnCurrentPlatform(InstallerType type) {
#ifdef Q_OS_LINUX
    switch (type) {
    case InstallerType::ShellScript:
    case InstallerType::UniversalZip:
    case InstallerType::UniversalTar:
    case InstallerType::UniversalTarGz:
    case InstallerType::UniversalTarBz2:
    case InstallerType::Universal7z:
    case InstallerType::LinuxDeb:
    case InstallerType::LinuxRpm:
    case InstallerType::LinuxAppImage:
    case InstallerType::ArchiveIso:
    case InstallerType::ArchiveRar:
    case InstallerType::PythonScript:
    case InstallerType::JavaJar:
        return true;
    default:
        return false;
    }
#elif defined(Q_OS_MACOS)
    switch (type) {
    case InstallerType::ShellScript:
    case InstallerType::UniversalZip:
    case InstallerType::UniversalTar:
    case InstallerType::UniversalTarGz:
    case InstallerType::UniversalTarBz2:
    case InstallerType::Universal7z:
    case InstallerType::MacPkg:
    case InstallerType::MacDmg:
    case InstallerType::MacApp:
    case InstallerType::ArchiveIso:
    case InstallerType::ArchiveRar:
    case InstallerType::PythonScript:
    case InstallerType::JavaJar:
        return true;
    default:
        return false;
    }
#elif defined(Q_OS_WIN)
    switch (type) {
    case InstallerType::WindowsExe:
    case InstallerType::WindowsMsi:
    case InstallerType::WindowsZip:
    case InstallerType::UniversalZip:
    case InstallerType::UniversalTar:
    case InstallerType::UniversalTarGz:
    case InstallerType::UniversalTarBz2:
    case InstallerType::Universal7z:
    case InstallerType::ScriptBat:
    case InstallerType::ScriptCmd:
    case InstallerType::ScriptPowershell:
    case InstallerType::ArchiveRar:
    case InstallerType::PythonScript:
    case InstallerType::JavaJar:
        return true;
    default:
        return false;
    }
#else
    return false;
#endif
}

QPair<QString, QStringList> InstallerDetector::getExtractionCommand(InstallerType type,
                                                                    const QString &filePath,
                                                                    const QString &outputPath) {
    switch (type) {
    case InstallerType::UniversalZip:
        return {"unzip", {"-q", filePath, "-d", outputPath}};

    case InstallerType::UniversalTar:
        return {"tar", {"xf", filePath, "-C", outputPath}};

    case InstallerType::UniversalTarGz:
        return {"tar", {"xzf", filePath, "-C", outputPath}};

    case InstallerType::UniversalTarBz2:
        return {"tar", {"xjf", filePath, "-C", outputPath}};

    case InstallerType::Universal7z:
        return {"7z", {"x", filePath, QString("-o%1").arg(outputPath)}};

    case InstallerType::ArchiveRar:
        return {"unrar", {"x", filePath, outputPath}};

    case InstallerType::ArchiveIso:
        return {"7z", {"x", filePath, QString("-o%1").arg(outputPath)}};

    default:
        return {"", {}};
    }
}

QPair<QString, QStringList> InstallerDetector::getInstallationCommand(InstallerType type,
                                                                      const QString &filePath,
                                                                      const QString &installPath) {
    switch (type) {
    case InstallerType::ShellScript:
        return {"/bin/bash", {filePath}};

    case InstallerType::ScriptBat:
        return {"cmd.exe", {"/c", filePath}};

    case InstallerType::ScriptCmd:
        return {"cmd.exe", {"/c", filePath}};

    case InstallerType::ScriptPowershell:
        return {"powershell.exe", {"-ExecutionPolicy", "Bypass", "-File", filePath}};

    case InstallerType::PythonScript:
        return {"python3", {filePath}};

    case InstallerType::JavaJar:
        return {"java", {"-jar", filePath}};

    case InstallerType::LinuxDeb:
        return {"sudo", {"dpkg", "-i", filePath}};

    case InstallerType::LinuxRpm:
        return {"sudo", {"rpm", "-i", filePath}};

    case InstallerType::LinuxAppImage:
        return {filePath, {}};

    case InstallerType::MacPkg:
        return {"sudo", {"installer", "-pkg", filePath, "-target", QDir::homePath()}};

    case InstallerType::MacDmg:
        return {"hdiutil", {"attach", filePath, "-mountpoint", installPath}};

    case InstallerType::WindowsMsi:
        return {"msiexec.exe", {"/i", filePath}};

    default:
        return {"", {}};
    }
}

bool InstallerDetector::requiresExtraction(InstallerType type) {
    switch (type) {
    case InstallerType::UniversalZip:
    case InstallerType::UniversalTar:
    case InstallerType::UniversalTarGz:
    case InstallerType::UniversalTarBz2:
    case InstallerType::Universal7z:
    case InstallerType::ArchiveRar:
    case InstallerType::ArchiveIso:
        return true;
    default:
        return false;
    }
}

bool InstallerDetector::requiresInstallation(InstallerType type) {
    switch (type) {
    case InstallerType::ShellScript:
    case InstallerType::WindowsExe:
    case InstallerType::DOSExecutable:
    case InstallerType::ScriptBat:
    case InstallerType::ScriptCmd:
    case InstallerType::ScriptPowershell:
    case InstallerType::PythonScript:
    case InstallerType::JavaJar:
    case InstallerType::LinuxDeb:
    case InstallerType::LinuxRpm:
    case InstallerType::LinuxAppImage:
    case InstallerType::MacPkg:
    case InstallerType::MacDmg:
    case InstallerType::WindowsMsi:
        return true;
    default:
        return false;
    }
}

QStringList InstallerDetector::supportedExtensions() {
    return {"sh",    "bash",  "exe",  "zip",  "tar",  "gz",   "tgz",  "bz2",  "tbz2",
            "7z",    "pkg",   "dmg",  "app",  "deb",  "rpm",  "msi",  "rar",  "iso",
            "bin",   "cue",   "nrg",  "mdf",  "bat",  "cmd",  "ps1",  "py",   "rb",
            "pl",    "js",    "jar",  "AppImage"};
}

} // namespace opengalaxy::install
