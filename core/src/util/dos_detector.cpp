// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/util/dos_detector.h"

#include <QFile>
#include <QFileInfo>

namespace opengalaxy::util {

QStringList DOSDetector::dosGameKeywords() {
    return {
        "DOS",
        "MS-DOS",
        "Retro",
        "Classic",
        "Vintage",
        "Point and Click",
        "Adventure",
        "Text Adventure",
        "Interactive Fiction",
    };
}

QStringList DOSDetector::knownDOSGames() {
    return {
        "Battle Chess",
        "Monkey Island",
        "Day of the Tentacle",
        "Sam & Max",
        "Maniac Mansion",
        "King's Quest",
        "Space Quest",
        "Police Quest",
        "Leisure Suit Larry",
        "Doom",
        "Doom II",
        "Heretic",
        "Hexen",
        "Wolfenstein 3D",
        "Commander Keen",
        "Duke Nukem",
        "Lemmings",
        "Civilization",
        "SimCity",
        "Dune",
        "Dune II",
        "StarCraft",
        "Warcraft",
        "Diablo",
        "Baldur's Gate",
        "Planescape Torment",
        "Icewind Dale",
        "Fallout",
        "Fallout 2",
        "The Elder Scrolls",
        "Daggerfall",
        "Morrowind",
        "Quake",
        "Half-Life",
        "System Shock",
        "Ultima",
        "Wizardry",
        "Might and Magic",
        "Eye of the Beholder",
        "Dungeon Master",
        "Wizards & Warriors",
        "Prince of Persia",
        "Another World",
        "Flashback",
        "Ghouls 'n Ghosts",
        "Castlevania",
        "Mega Man",
        "Sonic",
        "Pac-Man",
        "Tetris",
        "Arkanoid",
        "Breakout",
        "Asteroids",
        "Centipede",
        "Galaga",
        "Defender",
        "Robotron",
        "Joust",
        "Dig Dug",
        "Donkey Kong",
        "Mario",
        "Kirby",
        "Metroid",
        "Contra",
        "Gradius",
        "R-Type",
        "Shmup",
        "Shoot 'em up",
    };
}

bool DOSDetector::isDOSGameByMetadata(const QString &title, const QStringList &genres,
                                      int releaseYear) {
    // Check if "DOS" is explicitly in genres
    for (const auto &genre : genres) {
        if (genre.contains("DOS", Qt::CaseInsensitive)) {
            return true;
        }
    }

    // Check for retro/classic indicators
    const QStringList keywords = dosGameKeywords();
    for (const auto &keyword : keywords) {
        for (const auto &genre : genres) {
            if (genre.contains(keyword, Qt::CaseInsensitive)) {
                return true;
            }
        }
    }

    // Check known DOS game titles
    const QStringList knownGames = knownDOSGames();
    const QString titleLower = title.toLower();
    for (const auto &game : knownGames) {
        if (titleLower.contains(game.toLower())) {
            return true;
        }
    }

    // Heuristic: Games released before 1995 are often DOS games
    // (but this is not reliable, so we use it as a weak indicator)
    if (releaseYear > 0 && releaseYear < 1995) {
        // Only return true if there are other indicators
        if (!genres.isEmpty()) {
            for (const auto &genre : genres) {
                if (genre.contains("Adventure", Qt::CaseInsensitive) ||
                    genre.contains("RPG", Qt::CaseInsensitive) ||
                    genre.contains("Strategy", Qt::CaseInsensitive)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool DOSDetector::isDOSExecutable(const QString &executablePath) {
    QFile file(executablePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Read first 2 bytes (MZ header for DOS/Windows executables)
    const QByteArray header = file.read(2);
    if (header.size() < 2 || header[0] != 'M' || header[1] != 'Z') {
        file.close();
        return false;
    }

    // Check if it's a pure DOS executable (not Windows PE)
    // Windows PE has additional headers at offset 0x3C
    file.seek(0x3C);
    const QByteArray peOffsetBytes = file.read(4);
    file.close();

    if (peOffsetBytes.size() < 4) {
        // If we can't read PE offset, assume it's DOS
        return true;
    }

    // Convert bytes to uint32_t (little-endian)
    const uint32_t peOffset = static_cast<uint8_t>(peOffsetBytes[0]) |
                              (static_cast<uint8_t>(peOffsetBytes[1]) << 8) |
                              (static_cast<uint8_t>(peOffsetBytes[2]) << 16) |
                              (static_cast<uint8_t>(peOffsetBytes[3]) << 24);

    // If PE offset is 0 or very small (< 64), it's likely a pure DOS executable
    // Windows PE files have PE offset > 64 (typically 0x80 or higher)
    // If PE offset is valid (> 64), it's a Windows PE executable, not pure DOS
    if (peOffset > 64 && peOffset < 0x10000) {
        // This is a Windows PE executable (Win32), not pure DOS
        return false;
    }

    // Pure DOS executable
    return true;
}

} // namespace opengalaxy::util
