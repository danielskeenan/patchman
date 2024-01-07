/**
 * @file RomLibrary.cpp
 *
 * @author Dan Keenan
 * @date 1/1/24
 * @copyright GNU GPLv3
 */

#include "patchlib/library/RomLibrary.h"
#include "patchlib/Rom.h"
#include "patchlib/Exceptions.h"
#include <QtConcurrent>
#include <orm/db.hpp>
#include <QStandardPaths>

namespace patchman
{

RomLibrary::RomLibrary(QObject *parent)
    : QObject(parent)
{
    // Ensure that database work allways happens on the same thread, without having to manually manage that thread.
    pool_.setMaxThreadCount(1);
    pool_.setExpiryTimeout(-1);
}

RomLibrary *RomLibrary::get()
{
    static RomLibrary *instance = nullptr;
    if (instance == nullptr) {
        instance = new RomLibrary();
        auto result = instance->open();
        QFutureWatcher<void> futureWatcher;
        futureWatcher.setFuture(result);
        futureWatcher.waitForFinished();
    }
    return instance;
}

QFuture<void> RomLibrary::open()
{
    return QtConcurrent::run(&pool_, [this]()
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
        QStringList ddlSpecs;
        ddlSpecs.append(RomInfo::getDDL());

        for (const auto &ddl : ddlSpecs) {
            db_->unprepared(ddl);
        }
    });
}

QFuture<QList<RomInfo>> RomLibrary::getAllRoms(const QStringList &searchPaths)
{
    return QtConcurrent::run(&pool_, [this, searchPaths](QPromise<QList<RomInfo>> &promise)
    {
        // Iterate through search dirs for ROMs.
        promise.setProgressRange(0, 0);
        promise.setProgressValue(0);
        int fileCount = 0;
        for (const auto &searchPath : searchPaths) {
            QDirIterator it(searchPath, QDirIterator::Subdirectories);
            while (!it.next().isEmpty()) {
                if (it.fileInfo().isFile()) {
                    ++fileCount;
                }
            }
        }
        promise.setProgressRange(0, fileCount);

        // Count files before iterating for progress purposes.
        // Track file paths on disk so we can prune non-existent files from the database.
        QList<QVariant> foundOnDisk;
        int progressValue = 0;
        for (const auto &searchPath : searchPaths) {
            for (QDirIterator it(searchPath, QDirIterator::Subdirectories); it.hasNext();) {
                if (promise.isCanceled()) {
                    return;
                }

                const auto fileInfo = it.nextFileInfo();
                if (!fileInfo.isFile()) {
                    continue;
                }
                promise.setProgressValue(progressValue++);
                const auto filePath = fileInfo.canonicalFilePath();
                const auto fileMTime = fileInfo.lastModified().toUTC();
                std::optional<RomInfo> romInfo;

                // Has this file been modified?
                romInfo = RomInfo::firstWhereEq(RomInfo::kColFilePath, filePath);
                if (romInfo.has_value() && romInfo->getFileMTime().toUTC() == fileMTime) {
                    // File is already in database and has not changed.
                    foundOnDisk.push_back(filePath);
                    continue;
                }

                // File is new or has been modified since last check.
                try {
                    const auto romType = Rom::guessType(filePath);
                    // Add rom info to database.
                    const auto rom = Rom::create(romType);
                    rom->loadFromFile(filePath);
                    if (!romInfo.has_value()) {
                        // New ROM.
                        romInfo.emplace();
                    }
                    rom->updateRomInfo(*romInfo);
                    romInfo->setFilePath(filePath);
                    romInfo->setFileMTime(fileMTime);
                    romInfo->save();
                    foundOnDisk.push_back(filePath);
                    rom->deleteLater();
                }
                catch (const InvalidRomException &) {
                    // Not a ROM file; move on.
                    continue;
                }
            }
        }
        if (promise.isCanceled()) {
            return;
        }

        // Prune deleted ROMs from database.
        RomInfo::whereNotIn(RomInfo::kColFilePath, foundOnDisk)->remove();
        promise.setProgressValue(fileCount);

        // Return found ROMs.
        auto roms = RomInfo::all();
        promise.addResult(roms.all());
    });
}

} // patchman
