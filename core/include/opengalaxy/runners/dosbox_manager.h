// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <QString>
#include <QStringList>

namespace opengalaxy::runners {

/**
 * @brief DOSBox process manager
 *
 * Manages DOSBox processes safely:
 * - Find running DOSBox instances
 * - Gracefully terminate with save prompts
 * - Force kill if necessary
 * - Detect stale processes
 */
class DOSBoxManager {
  public:
    /**
     * @brief Find all running DOSBox processes
     *
     * @return List of DOSBox process IDs
     */
    static QStringList findRunningDOSBoxProcesses();

    /**
     * @brief Gracefully terminate DOSBox process
     *
     * Sends SIGTERM to allow DOSBox to save state
     *
     * @param pid Process ID
     * @param timeoutMs Timeout in milliseconds to wait for graceful shutdown
     * @return true if successfully terminated
     */
    static bool gracefullyTerminate(const QString &pid, int timeoutMs = 5000);

    /**
     * @brief Force kill DOSBox process
     *
     * Sends SIGKILL to immediately terminate
     *
     * @param pid Process ID
     * @return true if successfully killed
     */
    static bool forceKill(const QString &pid);

    /**
     * @brief Terminate all DOSBox processes gracefully
     *
     * @param timeoutMs Timeout for graceful shutdown
     * @return Number of processes terminated
     */
    static int terminateAllDOSBox(int timeoutMs = 5000);

    /**
     * @brief Check if DOSBox process is still running
     *
     * @param pid Process ID
     * @return true if process is running
     */
    static bool isProcessRunning(const QString &pid);

    /**
     * @brief Get DOSBox process info
     *
     * @param pid Process ID
     * @return Process info (command line, memory usage, etc.)
     */
    static QString getProcessInfo(const QString &pid);

    /**
     * @brief Check if DOSBox has unsaved progress
     *
     * Heuristic: if process has been running for more than 30 seconds,
     * assume there might be unsaved progress
     *
     * @param pid Process ID
     * @return true if likely has unsaved progress
     */
    static bool likelyHasUnsavedProgress(const QString &pid);

    /**
     * @brief Get process uptime in seconds
     *
     * @param pid Process ID
     * @return Uptime in seconds, or -1 if process not found
     */
    static int getProcessUptime(const QString &pid);
};

} // namespace opengalaxy::runners
