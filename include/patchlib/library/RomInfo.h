/**
 * @file RomInfo.h
 *
 * @author Dan Keenan
 * @date 1/1/24
 * @copyright GNU GPLv3
 */

#ifndef ROMINFO_H
#define ROMINFO_H

#include <orm/tiny/model.hpp>
#include "OrmUtils.h"

using Orm::Tiny::Model;

namespace patchman
{

class RomInfo final: public Model<RomInfo>
{
    friend Model;
    using Model::Model;

    friend bool operator==(const RomInfo &lhs, const RomInfo &rhs)
    {
        return
            lhs.getFilePath() == rhs.getFilePath()
                && lhs.getFileMTime() == rhs.getFileMTime()
                && lhs.getHashAlgo() == rhs.getHashAlgo()
                && lhs.getSoftwareHash() == rhs.getSoftwareHash()
                && lhs.getPatchHash() == rhs.getPatchHash()
                && lhs.getRomType() == rhs.getRomType()
                && lhs.getRackCount() == rhs.getRackCount()
                && lhs.getRomChecksum() == rhs.getRomChecksum();
    }

    friend bool operator!=(const RomInfo &lhs, const RomInfo &rhs)
    {
        return !(lhs == rhs);
    }

public:
    [[nodiscard]] static QStringList getDDL();

    PATCHMAN_ORM_COL(QString, FilePath, "file_path")
    PATCHMAN_ORM_COL_DATETIME(QDateTime, FileMTime, "file_mtime")
    PATCHMAN_ORM_COL_INTEGRAL(int, HashAlgo, "hash_algo")
    PATCHMAN_ORM_COL(QByteArray, SoftwareHash, "sw_hash")
    PATCHMAN_ORM_COL(QByteArray, PatchHash, "patch_hash")
    PATCHMAN_ORM_COL_INTEGRAL(int, RomType, "rom_type")
    PATCHMAN_ORM_COL_INTEGRAL(unsigned int, RackCount, "rack_count")
    PATCHMAN_ORM_COL(QByteArray, RomChecksum, "rom_checksum")

private:
    QString u_table{"rom_info"};
    bool u_timestamps = false;
};

} // patchman

#endif //ROMINFO_H
