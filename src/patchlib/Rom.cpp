/**
 * @file Rom.cpp
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#include "patchlib/Rom.h"
#include "patchlib/D192.h"
#include "patchlib/BinLoader.h"
#include "patchlib/Enr.h"
#include "patchlib/Exceptions.h"
#include <frozen/map.h>
#include <QtEndian>
#include <QCryptographicHash>

namespace patchman
{

static const std::map<Rom::Type, std::function<bool(QByteArrayView)>> kRomTypeGuessers{
    {Rom::Type::D192, &D192Rom::isD192Rom},
    {Rom::Type::ENR, &EnrRom::isEnrRom},
};

bool operator==(const Rom &lhs, const Rom &rhs)
{
    if (lhs.racks_.size() != rhs.racks_.size()) {
        return false;
    }

    for (auto lhsRackIt = lhs.racks_.cbegin(), rhsRackIt = rhs.racks_.cbegin();
         lhsRackIt != lhs.racks_.cend() && rhsRackIt != rhs.racks_.cend(); ++lhsRackIt, ++rhsRackIt) {
        const auto lhsRack = *lhsRackIt;
        const auto rhsRack = *rhsRackIt;
        if (*lhsRack != *rhsRack) {
            return false;
        }
    }
    return true;
}

QString Rom::typeName(Rom::Type romType)
{
    switch (romType) {
        case Type::D192:
            return tr("D192");
        case Type::ENR:
            return tr("ENR");
    }
    Q_UNREACHABLE();
}

QString Rom::getTitle() const
{
    return tr("%1 ROM").arg(typeName(getType()));
}

Rom *Rom::create(Rom::Type romType, QObject *parent)
{
    Rom *rom;
    switch (romType) {
        case Rom::Type::D192:
            return new D192Rom(parent);
        case Rom::Type::ENR:
            return new EnrRom(parent);
    }
    Q_UNREACHABLE();
}

Rom::Type Rom::guessType(const QString &path)
{
    const auto data = BinLoader::loadFile(path);
    for (const auto &[romType, guesser]: kRomTypeGuessers) {
        if (guesser(data)) {
            return romType;
        }
    }
    throw InvalidRomException(tr("This is not a patch ROM file."));
}

void Rom::loadFromFile(const QString &path)
{
    const auto data = BinLoader::loadFile(path);
    loadFromData(data);
}

void Rom::saveToFile(const QString &path) const
{
    const auto data = toByteArray();
    QFile file(path);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        throw std::runtime_error("Could not open file for writing.");
    }
    file.write(data);
}

void Rom::removeRack(unsigned int rackNum)
{
    qsizetype removedCount = 0;
    for (qsizetype ix = 0; ix < racks_.size();) {
        auto &rack = racks_.at(ix);
        if (rack->getRackNum() == rackNum) {
            ++removedCount;
            racks_.removeAt(ix);
            rack->deleteLater();
        }
        else {
            ++ix;
        }
    }

    if (removedCount > 1) {
        qWarning("Removed more than one rack because multiple racks matched rack number.");
    }
    else if (removedCount == 0) {
        qWarning("Tried to remove rack but no racks matched.");
    }

    if (removedCount > 0) {
        Q_EMIT(rackRemoved(rackNum));
        sortRacks();
    }
}

Rack *Rom::getRack(unsigned int rackNum) const
{
    if (rackNum >= racks_.size()) {
        return nullptr;
    }
    else {
        return racks_.at(rackNum);
    }
}

void Rom::sortRacks()
{
    std::sort(racks_.begin(), racks_.end(), [](const Rack *lhs, const Rack *rhs)
    { return *lhs < *rhs; });
    for (unsigned int rackNum = 0; rackNum < racks_.size(); ++rackNum) {
        racks_.at(rackNum)->setRackNum(rackNum);
    }
}

unsigned int Rom::countPatchedRacks() const
{
    return std::count_if(racks_.cbegin(), racks_.cend(),
                         [](const Rack *rack)
                         { return rack->isPatched(); });
}

QByteArray Rom::getChecksum() const
{
    // This is a terrible checksum algorithm, but appears to be the one commonly in use.
    const auto data = toByteArray();
    uint32_t checksum = 0;
    for (const uint8_t byte : data) {
        checksum += byte;
    }

    // Need to swap endianness to display bytes in expected order.
    checksum = qToBigEndian(checksum);
    return QByteArray(std::bit_cast<const char *>(&checksum), sizeof(checksum));
}

void Rom::updateRomInfo(RomInfo &romInfo) const
{
    romInfo.setHashAlgo(getHashAlgorithm());
    romInfo.setSoftwareHash(getSoftwareHash());
    romInfo.setPatchHash(getPatchHash());
    romInfo.setRomType(static_cast<int>(getType()));
    romInfo.setRackCount(countPatchedRacks());
    romInfo.setRomChecksum(getChecksum());
}

QCryptographicHash::Algorithm Rom::getHashAlgorithm()
{
    return QCryptographicHash::Algorithm::Sha256;
}

} // patchlib
