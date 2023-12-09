/**
 * @file Settings.h
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <dragoonboots/qsettingscontainer/QSettingsContainer.h>
#include <QStandardPaths>
#include "RecentDocument.h"

namespace patchman
{

class Settings: public dragoonboots::qsettingscontainer::QSettingsContainer
{
public:
    /** MainWindow Geometry */
    DGSETTINGS_SETTING(QByteArray, MainWindowGeometry, {})

    /** Last path opened in a file dialog */
    DGSETTINGS_SETTING(QString, LastFileDialogPath, QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))

    /** Recent documents list */
    DGSETTINGS_SETTING(QList<RecentDocument>, RecentDocuments, {})

    /** Max number of recent documents to track */
    DGSETTINGS_SETTING(unsigned int, RecentDocumentsMax, 8)
};

} // patchman

#endif //SETTINGS_H
