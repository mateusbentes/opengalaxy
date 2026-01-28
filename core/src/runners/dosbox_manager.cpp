// SPDX-License-Identifier: Apache-2.0
#include "opengalaxy/runners/dosbox_manager.h"
#include "opengalaxy/util/log.h"

#include <QProcess>
#include <QStandardPaths>
#include <QThread>
#include <signal.h>
#include <sys/types.h>

namespace opengalaxy::runners {

QStringList DOSBoxManager::findRunningDOSBoxProcesses() {
    QStringList pids;

    auto *proc = new QProcess();
    proc->setProgram("pgrep");
    proc->setArguments({"-f", "dosbox"});
    proc->start();

    if (proc->waitForFinished(5000)) {
        const QString output = QString::fromUtf8(proc->readAllStandardOutput());
        pids = output.split('\n', Qt::SkipEmptyParts);
        LOG_INFO(QString("Found %1 DOSBox processes").arg(pids.size()));
    }

    proc->deleteLater();
    return pids;
}

bool DOSBoxManager::gracefullyTerminate(const QString &pid, int timeoutMs) {
    if (pid.isEmpty()) return false;

    bool ok = false;
    int pidInt = pid.toInt(&ok);
    if (!ok) return false;

    LOG_INFO(QString("Gracefully terminating DOSBox process: %1").arg(pid));

    // Send SIGTERM (allows process to save state)
    if (kill(pidInt, SIGTERM) != 0) {
        LOG_ERROR(QString("Failed to send SIGTERM to process %1").arg(pid));
        return false;
    }

    // Wait for process to terminate
    int elapsed = 0;
    int checkInterval = 100; // Check every 100ms

    while (elapsed < timeoutMs) {
        if (!isProcessRunning(pid)) {
            LOG_INFO(QString("Process %1 terminated gracefully").arg(pid));
            return true;
        }

        QThread::msleep(checkInterval);
        elapsed += checkInterval;
    }

    LOG_WARNING(QString("Process %1 did not terminate within %2ms, forcing kill")
                    .arg(pid)
                    .arg(timeoutMs));
    return false;
}

bool DOSBoxManager::forceKill(const QString &pid) {
    if (pid.isEmpty()) return false;

    bool ok = false;
    int pidInt = pid.toInt(&ok);
    if (!ok) return false;

    LOG_WARNING(QString("Force killing DOSBox process: %1").arg(pid));

    // Send SIGKILL (immediate termination)
    if (kill(pidInt, SIGKILL) != 0) {
        LOG_ERROR(QString("Failed to send SIGKILL to process %1").arg(pid));
        return false;
    }

    // Wait a moment for process to die
    QThread::msleep(500);

    if (!isProcessRunning(pid)) {
        LOG_INFO(QString("Process %1 force killed").arg(pid));
        return true;
    }

    return false;
}

int DOSBoxManager::terminateAllDOSBox(int timeoutMs) {
    const QStringList pids = findRunningDOSBoxProcesses();
    int terminated = 0;

    LOG_INFO(QString("Terminating %1 DOSBox processes").arg(pids.size()));

    // First, try graceful termination for all
    for (const auto &pid : pids) {
        if (gracefullyTerminate(pid, timeoutMs)) {
            terminated++;
        }
    }

    // Force kill any remaining processes
    for (const auto &pid : pids) {
        if (isProcessRunning(pid)) {
            if (forceKill(pid)) {
                terminated++;
            }
        }
    }

    LOG_INFO(QString("Terminated %1 DOSBox processes").arg(terminated));
    return terminated;
}

bool DOSBoxManager::isProcessRunning(const QString &pid) {
    if (pid.isEmpty()) return false;

    auto *proc = new QProcess();
    proc->setProgram("kill");
    proc->setArguments({"-0", pid});
    proc->start();

    bool running = proc->waitForFinished(1000) && proc->exitCode() == 0;
    proc->deleteLater();

    return running;
}

QString DOSBoxManager::getProcessInfo(const QString &pid) {
    if (pid.isEmpty()) return "Invalid PID";

    auto *proc = new QProcess();
    proc->setProgram("ps");
    proc->setArguments({"-p", pid, "-o", "pid,cmd,etime,rss"});
    proc->start();

    QString info;
    if (proc->waitForFinished(2000)) {
        info = QString::fromUtf8(proc->readAllStandardOutput());
    }

    proc->deleteLater();
    return info;
}

bool DOSBoxManager::likelyHasUnsavedProgress(const QString &pid) {
    // If process has been running for more than 30 seconds, assume unsaved progress
    const int uptime = getProcessUptime(pid);
    return uptime > 30;
}

int DOSBoxManager::getProcessUptime(const QString &pid) {
    if (pid.isEmpty()) return -1;

    auto *proc = new QProcess();
    proc->setProgram("ps");
    proc->setArguments({"-p", pid, "-o", "etime="});
    proc->start();

    int uptime = -1;

    if (proc->waitForFinished(2000)) {
        const QString output = QString::fromUtf8(proc->readAllStandardOutput()).trimmed();

        // Parse etime format: [[DD-]HH:]MM:SS
        const QStringList parts = output.split(':');

        if (parts.size() >= 2) {
            bool ok = false;
            int seconds = parts.last().toInt(&ok);
            if (ok && parts.size() >= 2) {
                int minutes = parts[parts.size() - 2].toInt(&ok);
                if (ok) {
                    uptime = minutes * 60 + seconds;

                    // Handle hours and days if present
                    if (parts.size() >= 3) {
                        int hours = parts[parts.size() - 3].toInt(&ok);
                        if (ok) uptime += hours * 3600;
                    }
                    if (parts.size() >= 4) {
                        int days = parts[0].toInt(&ok);
                        if (ok) uptime += days * 86400;
                    }
                }
            }
        }
    }

    proc->deleteLater();
    return uptime;
}

} // namespace opengalaxy::runners
