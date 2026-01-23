// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/runners/runner.h"
#include <QFile>
#include <QFileInfo>

namespace opengalaxy::runners {

Architecture Runner::detectArchitecture(const QString& executablePath)
{
    QFile file(executablePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return Architecture::Unknown;
    }

    // Read first 20 bytes to check magic and architecture
    QByteArray header = file.read(20);
    file.close();

    if (header.size() < 20) {
        return Architecture::Unknown;
    }

    // ELF magic: 0x7F 'E' 'L' 'F'
    if (header[0] == 0x7F && header[1] == 'E' && header[2] == 'L' && header[3] == 'F') {
        // ELF class: header[4]
        // 1 = 32-bit, 2 = 64-bit
        unsigned char elfClass = static_cast<unsigned char>(header[4]);
        
        // Machine type: header[18-19] (little-endian)
        unsigned short machine = (static_cast<unsigned char>(header[19]) << 8) | 
                                  static_cast<unsigned char>(header[18]);

        // Common machine types:
        // 0x03 = x86 (32-bit)
        // 0x3E = x86-64 (64-bit)
        // 0x28 = ARM (32-bit)
        // 0xB7 = AArch64 (ARM 64-bit)

        if (machine == 0x3E) {
            return Architecture::X86_64;
        } else if (machine == 0x03) {
            return Architecture::X86;
        } else if (machine == 0xB7) {
            return Architecture::ARM64;
        } else if (machine == 0x28) {
            return Architecture::ARM;
        }
    }

    // PE/COFF magic: 'M' 'Z' (Windows executables)
    if (header[0] == 'M' && header[1] == 'Z') {
        // For PE files, we'd need to read more to determine architecture
        // For now, assume x86_64 for .exe files (most common)
        // A full implementation would parse the PE header
        if (executablePath.endsWith(".exe", Qt::CaseInsensitive)) {
            // Simple heuristic: check file size
            QFileInfo info(executablePath);
            // Most modern games are 64-bit
            return Architecture::X86_64;
        }
    }

    // Mach-O magic (macOS)
    // 0xFEEDFACE = 32-bit
    // 0xFEEDFACF = 64-bit
    // 0xCAFEBABE = Universal binary
    unsigned int magic = (static_cast<unsigned char>(header[0]) << 24) |
                         (static_cast<unsigned char>(header[1]) << 16) |
                         (static_cast<unsigned char>(header[2]) << 8) |
                         static_cast<unsigned char>(header[3]);

    if (magic == 0xFEEDFACF) {
        // 64-bit Mach-O, check CPU type
        // Bytes 4-7 contain CPU type
        unsigned int cpuType = (static_cast<unsigned char>(header[4]) << 24) |
                               (static_cast<unsigned char>(header[5]) << 16) |
                               (static_cast<unsigned char>(header[6]) << 8) |
                               static_cast<unsigned char>(header[7]);
        
        if (cpuType == 0x01000007) { // CPU_TYPE_X86_64
            return Architecture::X86_64;
        } else if (cpuType == 0x0100000C) { // CPU_TYPE_ARM64
            return Architecture::ARM64;
        }
    } else if (magic == 0xFEEDFACE) {
        return Architecture::X86; // 32-bit Mach-O
    }

    return Architecture::Unknown;
}

Platform Runner::detectPlatform(const QString& executablePath)
{
    QFile file(executablePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return Platform::Unknown;
    }

    QByteArray header = file.read(4);
    file.close();

    if (header.size() < 4) {
        return Platform::Unknown;
    }

    // ELF magic
    if (header[0] == 0x7F && header[1] == 'E' && header[2] == 'L' && header[3] == 'F') {
        return Platform::Linux;
    }

    // PE magic
    if (header[0] == 'M' && header[1] == 'Z') {
        return Platform::Windows;
    }

    // Mach-O magic
    unsigned int magic = (static_cast<unsigned char>(header[0]) << 24) |
                         (static_cast<unsigned char>(header[1]) << 16) |
                         (static_cast<unsigned char>(header[2]) << 8) |
                         static_cast<unsigned char>(header[3]);

    if (magic == 0xFEEDFACE || magic == 0xFEEDFACF || magic == 0xCAFEBABE) {
        return Platform::MacOS;
    }

    // Check by extension as fallback
    if (executablePath.endsWith(".exe", Qt::CaseInsensitive)) {
        return Platform::Windows;
    }
    if (executablePath.endsWith(".app", Qt::CaseInsensitive)) {
        return Platform::MacOS;
    }

    return Platform::Unknown;
}

} // namespace opengalaxy::runners
