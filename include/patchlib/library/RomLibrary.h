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

namespace Orm
{
// Declare the database manager so we can hold onto an instance without consumers needing to link with the ORM library.
class DatabaseManager;
}

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
     * Update library with contents from paths.
     *
     * @param searchPaths
     */
    QFuture<QList<RomInfo>> getAllRoms(const QStringList &searchPaths);

private:
    QThreadPool pool_;
    std::shared_ptr<Orm::DatabaseManager> db_;

    explicit RomLibrary(QObject *parent = nullptr);

    QFuture<void> open();
};

} // patchman

#endif //ROMLIBRARY_H
