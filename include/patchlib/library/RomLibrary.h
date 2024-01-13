/**
 * @file RomLibrary.h
 *
 * @author Dan Keenan
 * @date 1/1/24
 * @copyright GNU GPLv3
 */

#ifndef ROMLIBRARY_H
#define ROMLIBRARY_H

#include <QObject>
#include <QFuture>
#include "RomInfo.h"

namespace patchman
{

/**
 * Store information about ROMs in a database.
 *
 * The path to the stored library is in the user's AppData directory, or from the env var PATCHMAN_DB_PATH if set.
 */
class RomLibrary: public QObject
{
Q_OBJECT
public:
    static RomLibrary *get();

    /**
     * Perform database cleaning.
     * @return
     */
    QFuture<void> cleanup();

    /**
     * Update library with contents from paths.
     *
     * @param searchPaths
     */
    QFuture<QList<RomInfo>> getAllRoms(const QStringList &searchPaths);

    /**
     * Find ROMs with the same patch table as @p romInfo.
     *
     * @param romInfo
     * @return
     */
    QFuture<QList<RomInfo>> getDuplicates(const RomInfo &romInfo);

private:
    /** PTCH */
    static const int32_t kAppId = 0x50544348;
    static const int32_t kAppVersion = 0;
    QThreadPool pool_;

    explicit RomLibrary(QObject *parent = nullptr);

    QFuture<void> open();

    static QString getDbPath();
    static void deleteDbFile();
};

} // patchman

#endif //ROMLIBRARY_H
