/**
 * @file help.h
 *
 * @author Dan Keenan
 * @date 2/17/2024
 * @copyright GNU GPLv3
 */

#ifndef HELP_H
#define HELP_H

#include <QApplication>
#include <QStandardPaths>
#include <QProcess>
#include "patchman_config.h"

namespace patchman
{

/**
 * Show the help browser.
 */
inline void showHelp()
{
    const auto collectionFile = QString("%1/../%2/%3.qhc")
        .arg(qApp->applicationDirPath(), config::kHelpPath, config::kProjectName);
    const auto assistantPath = []()
    {
        for (const auto &assistantName : {"assistant", "Assistant"}) {
            auto foundPath = QStandardPaths::findExecutable(assistantName);
            if (!foundPath.isEmpty()) {
                return foundPath;
            }
#ifdef PLATFORM_MACOS
            const auto extraPath = QString("%1/../Resources/Assistant.app/Contents/MacOS").arg(qApp->applicationDirPath());
#else

            const auto extraPath = qApp->applicationDirPath();
#endif
            foundPath = QStandardPaths::findExecutable(assistantName, {extraPath});
            if (!foundPath.isEmpty()) {
                return foundPath;
            }
        }
        return QString();
    }();

    auto *process = new QProcess;
    QStringList args(
        {
            "-collectionFile",
            collectionFile,
        });
    process->start(assistantPath, args);
    process->waitForStarted();
}

} // patchman

#endif //HELP_H
