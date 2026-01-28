// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QString>
#include <QStringList>

namespace opengalaxy::util {

/**
 * @brief Utility class for detecting DOS games
 *
 * Provides heuristics to identify MS-DOS games from various sources:
 * - Game metadata (genres, title keywords)
 * - Executable file headers
 * - GOG API information
 */
class DOSDetector {
  public:
    /**
     * @brief Check if a game is likely a DOS game based on metadata
     *
     * Uses heuristics like:
     * - "DOS" in genres
     * - "Retro" or "Classic" in genres
     * - Known DOS game titles
     * - Release date (pre-1995 games are often DOS)
     *
     * @param title Game title
     * @param genres Game genres
     * @param releaseYear Release year (0 if unknown)
     * @return true if game is likely DOS
     */
    static bool isDOSGameByMetadata(const QString &title, const QStringList &genres,
                                    int releaseYear = 0);

    /**
     * @brief Check if an executable is a DOS executable
     *
     * Checks the MZ header and PE offset to determine if it's a DOS executable.
     *
     * @param executablePath Path to the executable
     * @return true if it's a DOS executable
     */
    static bool isDOSExecutable(const QString &executablePath);

    /**
     * @brief Get a list of known DOS game keywords
     *
     * @return List of keywords that indicate a DOS game
     */
    static QStringList dosGameKeywords();

    /**
     * @brief Get a list of known DOS game titles (partial matches)
     *
     * @return List of known DOS game titles
     */
    static QStringList knownDOSGames();
};

} // namespace opengalaxy::util
