/**
 * @file RomLibrary.cpp
 *
 * @author Dan Keenan
 * @date 1/1/24
 * @copyright GNU GPLv3
 */

#include "patchlib/library/RomLibrary.h"
#include "patchlib/library/RomInfo.h"
#include <QtConcurrent>
#include <orm/db.hpp>
#include <QStandardPaths>

namespace patchman
{

RomLibrary::RomLibrary(QObject *parent)
    : QObject(parent), pool_(new QThreadPool(this))
{
    // Ensure that database work allways happens on the same thread, without having to manually manage that thread.
    pool_->setMaxThreadCount(1);
    pool_->setExpiryTimeout(-1);
}

RomLibrary *RomLibrary::get()
{
    static RomLibrary *instance = nullptr;
    if (instance == nullptr) {
        instance = new RomLibrary();
        auto result = instance->open();
        result.waitForFinished();
    }
    return instance;
}

QFuture<void> RomLibrary::open()
{

    return QtConcurrent::run(pool_, [this]()
    {
        // Use an env var for this path to facilitate testing.
        const auto
            dbPath = qEnvironmentVariable("PATCHMAN_DB_PATH",
                                          QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
                                              .filePath("patchman.db"));
        db_ = Orm::DB::create(
            {
                {"driver", "QSQLITE"},
                {"database", dbPath},
                {"foreign_key_constraints", true},
                {"check_database_exists", false},
                {"qt_timezone", Qt::UTC},
                {"return_qdatetime", true},
                {"prefix", ""},
                {"prefix_indexes", false},
            }
        );

        // Create tables.
        const QStringList ddlSpecs{
            RomInfo::getDDL(),
        };
        for (const auto &ddl : ddlSpecs) {
            db_->unprepared(ddl);
        }
    });
}

} // patchman
