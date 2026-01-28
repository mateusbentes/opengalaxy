// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QString>
#include <QStringList>

namespace opengalaxy::install {

/**
 * @brief Installer type enumeration
 */
enum class InstallerType {
    Unknown,
    ShellScript,      // .sh, .bash
    WindowsExe,       // .exe
    DOSExecutable,    // Pure DOS .exe
    UniversalZip,     // .zip
    UniversalTar,     // .tar
    UniversalTarGz,   // .tar.gz
    UniversalTarBz2,  // .tar.bz2
    Universal7z,      // .7z
    MacPkg,           // .pkg
    MacDmg,           // .dmg
    MacApp,           // .app (bundled app)
    LinuxDeb,         // .deb
    LinuxRpm,         // .rpm
    LinuxAppImage,    // .AppImage
    WindowsMsi,       // .msi
    WindowsZip,       // .zip (Windows specific)
    ArchiveRar,       // .rar
    ArchiveIso,       // .iso
    ArchiveBin,       // .bin
    ArchiveCue,       // .cue (CD image)
    ArchiveNrg,       // .nrg (Nero image)
    ArchiveMdf,       // .mdf (Alcohol image)
    ScriptBat,        // .bat (Windows batch)
    ScriptCmd,        // .cmd (Windows command)
    ScriptPowershell, // .ps1 (PowerShell)
    JavaJar,          // .jar
    PythonScript,     // .py
    RubyScript,       // .rb
    PerlScript,       // .pl
    NodeScript,       // .js
    GoExecutable,     // Go binary
    RustExecutable,   // Rust binary
};

/**
 * @brief Installer detector utility
 *
 * Detects installer type from file extension and content
 */
class InstallerDetector {
  public:
    /**
     * @brief Detect installer type from file path
     *
     * @param filePath Path to the installer file
     * @return Detected installer type
     */
    static InstallerType detectType(const QString &filePath);

    /**
     * @brief Get human-readable name for installer type
     *
     * @param type Installer type
     * @return Human-readable name
     */
    static QString typeName(InstallerType type);

    /**
     * @brief Check if installer type is supported on current platform
     *
     * @param type Installer type
     * @return true if supported
     */
    static bool isSupportedOnCurrentPlatform(InstallerType type);

    /**
     * @brief Get extraction command for installer type
     *
     * @param type Installer type
     * @param filePath Path to installer
     * @param outputPath Output directory
     * @return Command to execute (program and arguments)
     */
    static QPair<QString, QStringList> getExtractionCommand(InstallerType type,
                                                            const QString &filePath,
                                                            const QString &outputPath);

    /**
     * @brief Get installation command for installer type
     *
     * @param type Installer type
     * @param filePath Path to installer
     * @param installPath Installation directory
     * @return Command to execute (program and arguments)
     */
    static QPair<QString, QStringList> getInstallationCommand(InstallerType type,
                                                              const QString &filePath,
                                                              const QString &installPath);

    /**
     * @brief Check if type requires extraction
     *
     * @param type Installer type
     * @return true if extraction is needed
     */
    static bool requiresExtraction(InstallerType type);

    /**
     * @brief Check if type requires installation
     *
     * @param type Installer type
     * @return true if installation is needed
     */
    static bool requiresInstallation(InstallerType type);

    /**
     * @brief Get list of supported file extensions
     *
     * @return List of supported extensions
     */
    static QStringList supportedExtensions();

    /**
     * @brief Check if a Windows executable is actually a legacy DOS game
     *
     * Some old DOS games are packaged as Windows executables by GOG.
     * This checks if the game metadata indicates it's a DOS game.
     *
     * @param title Game title
     * @param genres Game genres
     * @return true if it's likely a legacy DOS game
     */
    static bool isLegacyDOSGame(const QString &title, const QStringList &genres);
};

} // namespace opengalaxy::install
