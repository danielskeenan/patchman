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
#include <QDesktopServices>
#include "patchman_config.h"

namespace patchman
{

/**
 * Show the help browser.
 */
inline void showHelp()
{
    QDesktopServices::openUrl(
        QUrl(
            QString("file:///%1/../%2/index.html")
                .arg(qApp->applicationDirPath(), config::kHelpPath),
            QUrl::TolerantMode)
    );
}

} // patchman

#endif //HELP_H
