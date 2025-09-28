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
#include <filesystem>
#include <QtConcurrent>
#include <QStandardPaths>
#include <QSqlError>

QDirIterator getRomDirIterator(const QString &path)
{
    return QDirIterator(path, QDir::Readable | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
}

namespace patchman
{

RomLibrary::RomLibrary(QObject *parent)
    : QObject(parent)
{
    // Ensure that database work always happens on the same thread, without having to manually manage that thread.
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
    return QtConcurrent::run(&pool_, []()
    {

        const auto dbPath = getDbPath();
        do {
            const bool initNewFile = (dbPath == ":memory:" || !QFileInfo(dbPath).isFile());
            if (dbPath != ":memory:") {
                // Doing this with QDir creates some heisenbugs that disappear when analyzed.
                auto dbDir = std::filesystem::path(dbPath.toStdString()).parent_path();
                std::filesystem::create_directories(dbDir);
            }
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(dbPath);
            if (!db.open()) {
                qCritical() << "Failed to open browser database:" << db.lastError();
                break;
            }

            // Integrity check
            if (!initNewFile) {
                // This should already be a database file, so make sure it's in good condition.

                // Verify app id.
                const auto dbAppId = []()
                {
                    QSqlQuery q("PRAGMA application_id;");
                    q.exec();
                    q.next();
                    return q.value(0).value<int32_t>();
                }();
                const auto dbAppVersion = []()
                {
                    QSqlQuery q("PRAGMA user_version;");
                    q.exec();
                    q.next();
                    return q.value(0).value<int32_t>();
                }();
                if (dbAppId != kAppId || dbAppVersion != kAppVersion) {
                    // Wrong database. Delete and recreate.
                    deleteDbFile();
                    continue;
                }

                // Verify integrity.
                const auto integrityCheckResults = []()
                {
                    QSqlQuery q("PRAGMA integrity_check(1);");
                    q.exec();
                    QStringList results;
                    while (q.next()) {
                        results.push_back(q.value(0).toString());
                    }
                    return results;
                }();
                if (integrityCheckResults.size() > 1 || integrityCheckResults.first() != "ok") {
                    // Bad database. Delete and recreate.
                    deleteDbFile();
                    continue;
                }
            }
        }
        while (!QSqlDatabase::database().isOpen());
        if (!QSqlDatabase::database().isOpen()) {
            qFatal("Failed to open browser database.");
        }

        // Create tables.
        QList<QSqlQuery> ddl{
            QSqlQuery(QString("PRAGMA application_id = %1;").arg(kAppId)),
            QSqlQuery(QString("PRAGMA user_version = %1").arg(kAppVersion)),
            QSqlQuery("PRAGMA foreign_keys = 1;")
        };
        ddl.append(std::move(RomInfo::getDDL()));

        for (auto &q : ddl) {
            q.exec();
        }
    });
}

QFuture<void> RomLibrary::cleanup()
{
    return QtConcurrent::run(&pool_, []()
    {
        if (!QSqlDatabase::database().isOpen()) {
            return;
        }

        static QList<QSqlQuery> ops{
            QSqlQuery("PRAGMA optimize;"),
            QSqlQuery("VACUUM;"),
        };
        for (auto &q : ops) {
            q.exec();
        }
    });
}

QFuture<QList<RomInfo>> RomLibrary::getAllRoms(const QStringList &searchPaths)
{
    return QtConcurrent::run(&pool_, [searchPaths](QPromise<QList<RomInfo>> &promise)
    {
        // Iterate through search dirs for ROMs.
        promise.setProgressRange(0, 0);
        promise.setProgressValue(0);
        int fileCount = 0;
        for (const auto &searchPath : searchPaths) {
            for (auto it = getRomDirIterator(searchPath); it.hasNext();) {
                const auto fileInfo = it.nextFileInfo();
                qDebug() << "Checking" << fileInfo.absoluteFilePath();
                if (fileInfo.isFile()) {
                    ++fileCount;
                }
            }
        }
        promise.setProgressRange(0, fileCount);

        // Count files before iterating for progress purposes.
        // Track file paths on disk so we can prune non-existent files from the database.
        QList<QVariant> foundOnDisk;
        int progressValue = 0;
        QSqlQuery filePathQ;
        filePathQ.prepare(
            QString("SELECT %1 FROM %2 WHERE %3 = ? LIMIT 1;")
                .arg(RomInfo::kAllColumns.join(", "))
                .arg(RomInfo::kTable)
                .arg(RomInfo::kColFilePath)
        );
        QSqlQuery saveRomInfoQ;
        saveRomInfoQ.prepare(
            QString("INSERT INTO %1(%2) VALUES(%3);")
                .arg(RomInfo::kTable)
                .arg(RomInfo::kAllColumns.join(", "))
                .arg(QStringList(RomInfo::kAllColumns.size(), "?").join(", "))
        );
        for (const auto &searchPath : searchPaths) {
            for (auto it = getRomDirIterator(searchPath); it.hasNext();) {
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

                // Has this file been modified?
                auto romInfo = [&filePathQ, &filePath]() -> std::optional<RomInfo>
                {
                    filePathQ.bindValue(0, filePath);
                    filePathQ.exec();
                    if (filePathQ.size() <= 0) {
                        return {};
                    }
                    filePathQ.next();
                    return RomInfo::hydrate(filePathQ);
                }();
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
                    romInfo->bind(saveRomInfoQ, 0);
                    saveRomInfoQ.exec();
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
        QSqlQuery pruneQ(
            QString("DELETE FROM %1 WHERE %2 NOT IN (%3)")
                .arg(RomInfo::kTable)
                .arg(RomInfo::kColFilePath)
                .arg(QStringList(foundOnDisk.size(), "?").join(", "))
        );
        for (const auto &path : foundOnDisk) {
            pruneQ.addBindValue(path);
        }
        pruneQ.exec();
        promise.setProgressValue(fileCount);

        // Return found ROMs.
        auto roms = []()
        {
            QList<RomInfo> result;
            QSqlQuery q(
                QString("SELECT %1 FROM %2;")
                    .arg(RomInfo::kAllColumns.join(", "))
                    .arg(RomInfo::kTable)
            );
            q.exec();
            result.reserve(q.size());
            while (q.next()) {
                result.push_back(RomInfo::hydrate(q));
            }

            return result;
        }();
        promise.addResult(roms);
    });
}

QFuture<QList<RomInfo>> RomLibrary::getDuplicates(const RomInfo &romInfo)
{
    const auto &patchHash = romInfo.getPatchHash();
    return QtConcurrent::run(&pool_, [patchHash](QPromise<QList<RomInfo>> &promise)
    {
        QList<RomInfo> duplicates;
        QSqlQuery q(
            QString("SELECT %1 FROM %2 WHERE %3 = ?")
                .arg(
                    RomInfo::kAllColumns.join(", "),
                    RomInfo::kTable,
                    RomInfo::kColPatchHash
                )
        );
        q.bindValue(0, patchHash);
        q.exec();
        duplicates.reserve(q.size());
        while (q.next()) {
            duplicates.push_back(RomInfo::hydrate(q));
        }
        promise.addResult(duplicates);
    });
}

QString RomLibrary::getDbPath()
{
    // Use an env var for this path to facilitate testing.
    static const auto dbPath =
        qEnvironmentVariable("PATCHMAN_DB_PATH",
                             QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
                                 .filePath("patchman.db"));

    return dbPath;
}

void RomLibrary::deleteDbFile()
{
    auto db = QSqlDatabase::database();
    const auto dbPath = getDbPath();

    if (db.isValid()) {
        const auto connName = db.connectionName();
        if (db.isOpen()) {
            db.close();
        }
        QSqlDatabase::removeDatabase(connName);
    }

    if (dbPath != ":memory:") {
        QFile::remove(dbPath);
    }
}

} // patchman
