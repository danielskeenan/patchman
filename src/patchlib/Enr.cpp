/**
 * @file Enr.cpp
 *
 * @author Dan Keenan
 * @date 12/12/23
 * @copyright GNU GPLv3
 */

#include "patchlib/Enr.h"
#include "patchlib/Exceptions.h"
#include <frozen/unordered_map.h>
#include <frozen/string.h>
#include <QtEndian>
#include <source_location>
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <iostream>

namespace patchman
{

constexpr auto kPatchTableCount = 16;

constexpr auto kPatchTableStart = 0x3000;

constexpr auto kLugCounts = frozen::make_unordered_map<Rack::Type, unsigned int>(
    {
        {Rack::Type::Enr96, 96},
    }
);

constexpr auto kRomSizes = frozen::make_unordered_map<Rack::Type, unsigned int>(
    {
        {Rack::Type::Enr96, 16384},
    }
);

static const auto kSoftwareChecksums = frozen::make_unordered_map<EnrRom::Version, QByteArray>(
    {
        {EnrRom::Version::EnrRack220,
         QByteArray::fromHex("b4bb92dd71b85ffb4f1f6beda724162c2f69d7ed9532c477e949a1ffbc46d509")},
        {EnrRom::Version::EnrRack230,
         QByteArray::fromHex("3c56a7ca14ee3141428c410ed54557e01f403c45025982dd43d18c5b33792a1c")},
        {EnrRom::Version::EnrRack254,
         QByteArray::fromHex("914890dec4168cd2aa1c7871f86321c1408893dcf33d7c5efd044f30690e06cd")},
        {EnrRom::Version::EnrRack260,
         QByteArray::fromHex("ea64a685aa268d1ecb3f9fa0d6bab846aa34530c4fbf5528e5617b4f2526a7bf")},
        {EnrRom::Version::EnrRack272,
         QByteArray::fromHex("9762b5402fa1c5b928190af76433be67bf3500e9f700127f31560008ab272485")},
        {EnrRom::Version::EnrRack274,
         QByteArray::fromHex("1c5e4a877aa0448fe556d525499fc85e2e5a37c3361ff745a9d200e3debb05e0")},
        {EnrRom::Version::EnrRack294,
         QByteArray::fromHex("1c55981980eeb717264713ce336169dd3fa79d0716374719cc49351cd435ca5d")},

    }
);

static const QHash<EnrRom::Version, QString> kSoftwareFilepaths{
        {EnrRom::Version::EnrRack220, ":/bin/enr/enr_rack_220.bin"},
        {EnrRom::Version::EnrRack230, ":/bin/enr/enr_rack_230.bin"},
        {EnrRom::Version::EnrRack254, ":/bin/enr/enr_rack_254.bin"},
        {EnrRom::Version::EnrRack260, ":/bin/enr/enr_rack_260.bin"},
        {EnrRom::Version::EnrRack272, ":/bin/enr/enr_rack_272.bin"},
        {EnrRom::Version::EnrRack274, ":/bin/enr/enr_rack_274.bin"},
        {EnrRom::Version::EnrRack294, ":/bin/enr/enr_rack_294.bin"},
};

unsigned int EnrRack::getLugCount() const
{
    switch (getRackType()) {
        case Type::Enr96:
            return 96;
    }
    Q_UNREACHABLE();
}

unsigned int EnrRack::getLugsPerModule() const
{
    return 2;
}

unsigned int EnrRack::getModuleDensityForLug(unsigned int lug) const
{
    // TODO: Confirm this is correct. This is copied from D192, where the first lug in the slot is the control lug for single-density modules.
    // Get the lugs in this slot.
    unsigned int lugA, lugB;
    if (lug % 2 == 0) {
        lugA = lug;
        lugB = lug + 1;
    }
    else {
        lugA = lug - 1;
        lugB = lug;
    }

    const auto lugAAddr = getLugAddress(lugA);
    const auto lugBAddr = getLugAddress(lugB);

    if (lugAAddr == 0 && lugBAddr == 0) {
        // Blank module
        return 0;
    }
    else if (lugAAddr != 0 && lugBAddr != 0) {
        return 2;
    }
    else {
        return 1;
    }
}

QString EnrRack::getModuleNameForLug(unsigned int lug) const
{
    const auto density = getModuleDensityForLug(lug);

    switch (density) {
        case 0:return tr("Blank");
        case 1:return tr("6 kW");
        case 2:return tr("1.8/2.4 kW");
        default:return Rack::getModuleNameForLug(lug);
    }
    Q_UNREACHABLE();
}

Phase EnrRack::getPhaseForLug(unsigned int lug) const
{
    const auto phase = lug / (getLugCount() / 3);
    return static_cast<Phase>(phase + 1);
}

void EnrRack::initLugAddressMap()
{
    lugAddresses_.fill(0, kLugCounts.at(getRackType()));
    lugFlags_.fill({}, kLugCounts.at(getRackType()));
}

void EnrRack::fromByteArray(QByteArrayView data)
{
    Q_ASSERT_X(lugAddresses_.size() == getLugCount(),
               std::source_location::current().function_name(),
               "Lug addresses size mismatch.");
    Q_ASSERT_X(lugFlags_.size() == getLugCount(),
               std::source_location::current().function_name(),
               "Lug flags size mismatch.");
    // Different from D192. Each table is a list of addresses, stored in lug order.
    for (unsigned int lug = 0; lug < getLugCount(); ++lug) {
        const auto dataOffset = lug * 2;
        Q_ASSERT_X(dataOffset < data.size(),
                   std::source_location::current().function_name(),
                   "Tried to load lug patch beyond end of rack");
        const auto lugData = qFromLittleEndian<uint16_t>(data.data() + dataOffset);
        const auto address = lugData & 0x01FFu;
        // 9-bit unsigned int maxes out at 511.
        const auto flags = (lugData & 0xFE00u) >> 1;
        lugAddresses_[lug] = address;
        lugFlags_[lug] = flags;
    }
}

QByteArray EnrRack::toByteArray() const
{
    Q_ASSERT_X(lugAddresses_.size() == getLugCount(),
               std::source_location::current().function_name(),
               "Lug addresses size mismatch.");
    Q_ASSERT_X(lugFlags_.size() == getLugCount(),
               std::source_location::current().function_name(),
               "Lug flags size mismatch.");
    QByteArray data(getLugCount() * 2, 0);
    for (unsigned int lug = 0; lug < getLugCount(); ++lug) {
        const auto address = lugAddresses_.at(lug);
        const auto flags = lugFlags_.at(lug);
        const auto lugData = qToLittleEndian<uint16_t>(static_cast<uint16_t>(address) | (flags.to_ulong() << 9));

        const auto dataOffset = lug * 2;
        std::memcpy(data.data() + dataOffset, &lugData, 2);
    }

    return data;
}

const QMap<EnrRom::Version, QString> &EnrRom::getVersionNames()
{
    static const QMap<Version, QString> versions{
        {Version::Unknown, tr("Unknown")},
        {Version::EnrRack220, tr("2.20")},
        {Version::EnrRack230, tr("2.30")},
        {Version::EnrRack254, tr("2.54")},
        {Version::EnrRack260, tr("2.60")},
        {Version::EnrRack272, tr("2.72")},
        {Version::EnrRack274, tr("2.74")},
        {Version::EnrRack294, tr("2.94")},
    };

    return versions;
}

EnrRom::EnrRom(QObject *parent)
    : Rom(parent)
{
    // Load default software.
    setVersion(Version::EnrRack294);

    for (unsigned int rackNum = 0; rackNum < kPatchTableCount; ++rackNum) {
        auto rack = new EnrRack(rackNum, Rack::Type::Enr96, this);
        rack->initLugAddressMap();
        racks_.push_back(rack);
    }
}

bool operator==(const EnrRom &lhs, const EnrRom &rhs)
{
    if (lhs.version_ != rhs.version_) {
        return false;
    }

    return static_cast<const Rom &>(lhs) == static_cast<const Rom &>(rhs);
}

void EnrRom::setVersion(EnrRom::Version version)
{
    if (version == Version::Unknown) {
        qWarning() << "Cannot set ENR software version to Unknown.";
    }
    version_ = version;

    const auto path = kSoftwareFilepaths.value(version);
    QFile file(path);
    file.open(QFile::ReadOnly);
    Q_ASSERT_X(file.isOpen(),
               std::source_location::current().function_name(),
               "Failed to open software version from internal resource.");
    software_ = file.readAll();
    software_hash_ = QCryptographicHash::hash(software_, QCryptographicHash::Algorithm::Sha256);
    Q_EMIT(versionChanged(version_));
    Q_EMIT(titleChanged());
}

void EnrRom::loadFromData(QByteArrayView data)
{
    if (!isEnrRom(data)) {
        throw InvalidRomException(tr("This is not an ENR ROM."));
    }

    // Guess software version
    const auto software = data.sliced(0, kPatchTableStart).toByteArray();
    const auto swHash = QCryptographicHash::hash(software, getHashAlgorithm());
    version_ = Version::Unknown;
    for (const auto &[version, checksum]: kSoftwareChecksums) {
        if (checksum == swHash) {
            version_ = version;
            break;
        }
    }
    // Hold on to the rom file, the patch table will be spliced in at the end.
    software_ = software;
    software_hash_ = swHash;

    // Patch tables starts as 0x0300. 16 tables total, 192 bytes per table. Each circuit is 2 bytes little-endian.
    // Address is 9 bits, flags remaining 7 bits.
    const auto allRackPatchTables = data.sliced(kPatchTableStart);
    for (unsigned int rackNum = 0; rackNum < kPatchTableCount; ++rackNum) {
        auto rack = dynamic_cast<EnrRack *>(getRack(rackNum));
        if (rack == nullptr) {
            rack = dynamic_cast<EnrRack *>(addRack(rackNum, Rack::Type::Enr96));
        }
        Q_ASSERT_X(rack != nullptr, std::source_location::current().function_name(), "Failed to create new ENR rack.");
        const auto rackPatchTable = allRackPatchTables.sliced(rackNum * (kLugCounts.at(Rack::Type::Enr96) * 2));
        rack->fromByteArray(rackPatchTable);
    }

    // Sanity check the racks. If all racks have all lugs set to address 511, then this is a stock 1-1 patch and
    // should not be edited.
    const auto is1to1 = [](const Rack *rack)
    {
        for (const auto lugAddress : rack->getLugAddressesView()) {
            if (lugAddress.second != 511) {
                return false;
            }
        }
        return true;
    };
    if (std::all_of(racks_.cbegin(), racks_.cend(), is1to1)) {
        throw InvalidRomException(tr("ENR 1-1 Patch detected."),
                                  tr("ENR 1-1 patch ROMs are special and are not editable. To modify an ENR 1-1 patch, create a new patch ROM and autonumber each rack."));
    }
}

QByteArray EnrRom::toByteArray() const
{
    QByteArray data = software_;
    for (const auto *rack: racks_) {
        const auto *enrRack = dynamic_cast<const EnrRack *>(rack);
        Q_ASSERT_X(enrRack != nullptr,
                   std::source_location::current().function_name(),
                   "Non ENR Rack stored in ENR patch rom.");
        const auto patchTableLength = kLugCounts.at(enrRack->getRackType()) * 2;
        const unsigned int dataOffset = kPatchTableStart + (enrRack->getRackNum() * patchTableLength);
        data.replace(dataOffset, patchTableLength, enrRack->toByteArray());
    }

    return data;
}

Rack *EnrRom::addRack(unsigned int rackNum, Rack::Type rackType)
{
    auto *rack = dynamic_cast<EnrRack *>(getRack(rackNum));
    if (rack == nullptr) {
        rack = new EnrRack(rackNum, rackType, this);
        racks_.push_back(rack);
        sortRacks();
    }
    rack->initLugAddressMap();
    return rack;
}

bool EnrRom::isEnrRom(QByteArrayView data)
{
    const auto dataSize = data.size();
    if (!std::ranges::any_of(std::views::values(kRomSizes), [dataSize](auto check)
    { return check == dataSize; })) {
        // Wrong size.
        return false;
    }

    return true;
}

QString EnrRom::getTitle() const
{
    return tr("ENR Rack v%1").arg(getVersionNames().value(version_));
}

QByteArray EnrRom::getSoftwareHash() const
{
    return software_hash_;
}

QByteArray EnrRom::getPatchHash() const
{
    return QCryptographicHash::hash(toByteArray().sliced(kPatchTableStart), getHashAlgorithm());
}

};
