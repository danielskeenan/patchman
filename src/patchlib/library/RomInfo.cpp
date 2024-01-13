/**
 * @file RomInfo.cpp
 *
 * @author Dan Keenan
 * @date 1/1/24
 * @copyright GNU GPLv3
 */

#include "patchlib/library/RomInfo.h"
#include <QDateTime>

namespace patchman
{

QList<QSqlQuery> RomInfo::getDDL()
{
    return {
        QSqlQuery(QString(R"(
create table if not exists rom_info
(
    %1  text primary key not null collate NOCASE,
    %2  text,
    %3  integer,
    %4  BLOB,
    %5  BLOB,
    %6  integer,
    %7  integer,
    %8  BLOB
);
)")
                      .arg(kColFilePath)
                      .arg(kColFileMTime)
                      .arg(kColHashAlgo)
                      .arg(kColSoftwareHash)
                      .arg(kColPatchHash)
                      .arg(kColRomType)
                      .arg(kColRackCount)
                      .arg(kColRomChecksum)),
        QSqlQuery(QString(R"(
create index if not exists rom_info_patch_hash_index
    on rom_info (%1);
)").arg(kColPatchHash))
    };
}

void RomInfo::bind(QSqlQuery &q, int pos)
{
    q.bindValue(pos++, filePath_);
    q.bindValue(pos++, fileMTime_);
    q.bindValue(pos++, hashAlgo_);
    q.bindValue(pos++, softwareHash_);
    q.bindValue(pos++, patchHash_);
    q.bindValue(pos++, romType_);
    q.bindValue(pos++, rackCount_);
    q.bindValue(pos++, romChecksum_);
}

RomInfo RomInfo::hydrate(QSqlQuery &q)
{
    Q_ASSERT(q.isActive());
    RomInfo o;
    o.filePath_ = q.value(kColFilePath).toString();
    o.fileMTime_ = q.value(kColFileMTime).toDateTime();
    o.hashAlgo_ = q.value(kColHashAlgo).toInt();
    o.softwareHash_ = q.value(kColSoftwareHash).toByteArray();
    o.patchHash_ = q.value(kColPatchHash).toByteArray();
    o.romType_ = q.value(kColRomType).toInt();
    o.rackCount_ = q.value(kColRackCount).toUInt();
    o.romChecksum_ = q.value(kColRomChecksum).toByteArray();

    return o;
}

} // patchman
