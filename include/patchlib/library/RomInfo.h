/**
 * @file RomInfo.h
 *
 * @author Dan Keenan
 * @date 1/1/24
 * @copyright GNU GPLv3
 */

#ifndef ROMINFO_H
#define ROMINFO_H

#include <QStringList>
#include <QDateTime>
#include <QtSql/QSqlQuery>

namespace patchman
{

class RomInfo
{
public:
    friend bool operator==(const RomInfo &lhs, const RomInfo &rhs)
    {
        return lhs.filePath_ == rhs.filePath_ &&
            lhs.fileMTime_ == rhs.fileMTime_ &&
            lhs.hashAlgo_ == rhs.hashAlgo_ &&
            lhs.softwareHash_ == rhs.softwareHash_ &&
            lhs.patchHash_ == rhs.patchHash_ &&
            lhs.romType_ == rhs.romType_ &&
            lhs.rackCount_ == rhs.rackCount_ &&
            lhs.romChecksum_ == rhs.romChecksum_;
    }

    friend bool operator!=(const RomInfo &lhs, const RomInfo &rhs)
    {
        return !(rhs == lhs);
    }

public:
    constexpr static const auto kTable = "rom_info";

    [[nodiscard]] static QList<QSqlQuery> getDDL();
    void bind(QSqlQuery &q, int pos);
    [[nodiscard]] static RomInfo hydrate(QSqlQuery &q);

    constexpr static const auto kColFilePath = "file_path";

    [[nodiscard]] const QString &getFilePath() const
    {
        return filePath_;
    }

    void setFilePath(const QString &filePath)
    {
        filePath_ = filePath;
    }

    constexpr static const auto kColFileMTime = "file_mtime";

    [[nodiscard]] const QDateTime &getFileMTime() const
    {
        return fileMTime_;
    }

    void setFileMTime(const QDateTime &fileMTime)
    {
        fileMTime_ = fileMTime.toUTC();
    }

    constexpr static const auto kColHashAlgo = "hash_algo";

    [[nodiscard]] int getHashAlgo() const
    {
        return hashAlgo_;
    }

    void setHashAlgo(int hashAlgo)
    {
        hashAlgo_ = hashAlgo;
    }

    constexpr static const auto kColSoftwareHash = "sw_hash";

    [[nodiscard]] const QByteArray &getSoftwareHash() const
    {
        return softwareHash_;
    }

    void setSoftwareHash(const QByteArray &softwareHash)
    {
        softwareHash_ = softwareHash;
    }

    constexpr static const auto kColPatchHash = "patch_hash";

    [[nodiscard]] const QByteArray &getPatchHash() const
    {
        return patchHash_;
    }

    void setPatchHash(const QByteArray &patchHash)
    {
        patchHash_ = patchHash;
    }

    constexpr static const auto kColRomType = "rom_type";

    [[nodiscard]] int getRomType() const
    {
        return romType_;
    }

    void setRomType(int romType)
    {
        romType_ = romType;
    }

    constexpr static const auto kColRackCount = "rack_count";

    [[nodiscard]] unsigned int getRackCount() const
    {
        return rackCount_;
    }

    void setRackCount(unsigned int rackCount)
    {
        rackCount_ = rackCount;
    }

    constexpr static const auto kColRomChecksum = "rom_checksum";

    [[nodiscard]] const QByteArray &getRomChecksum() const
    {
        return romChecksum_;
    }

    void setRomChecksum(const QByteArray &romChecksum)
    {
        romChecksum_ = romChecksum;
    }

    inline static const QStringList kAllColumns{
        kColFilePath,
        kColFileMTime,
        kColHashAlgo,
        kColSoftwareHash,
        kColPatchHash,
        kColRomType,
        kColRackCount,
        kColRomChecksum,
    };

private:
    QString filePath_;
    QDateTime fileMTime_;
    int hashAlgo_;
    QByteArray softwareHash_;
    QByteArray patchHash_;
    int romType_;
    unsigned int rackCount_;
    QByteArray romChecksum_;
};

} // patchman

#endif //ROMINFO_H
