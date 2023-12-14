/**
 * @file D192.cpp
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#include "patchlib/D192.h"
#include "patchlib/Exceptions.h"
#include <source_location>
#include <frozen/unordered_map.h>

namespace patchman
{

/**
 * The number of lugs in each rack getType.
 */
constexpr auto kLugCounts = frozen::make_unordered_map<Rack::Type, unsigned int>(
    {
        {Rack::Type::D192Rack, 192},
    }
);

/**
 * Two different kinds of ROMs used for patch info.
 */
enum class RomChip
{
    Rom2716,
    Rom2732,
};

/**
 * The number of bytes stored on each ROM.
 */
constexpr auto kRomSizes = frozen::make_unordered_map<RomChip, unsigned int>(
    {
        {RomChip::Rom2716, 2048},
        {RomChip::Rom2732, 4096},
    }
);

constexpr auto kCircuitToLugMap = std::to_array(
    {
        0,
        1,
        64,
        65,
        128,
        129,
        2,
        3,
        66,
        67,
        130,
        131,
        4,
        5,
        68,
        69,
        132,
        133,
        6,
        7,
        70,
        71,
        134,
        135,
        8,
        9,
        72,
        73,
        136,
        137,
        10,
        11,
        74,
        75,
        138,
        139,
        12,
        13,
        76,
        77,
        140,
        141,
        14,
        15,
        78,
        79,
        142,
        143,
        16,
        17,
        80,
        81,
        144,
        145,
        18,
        19,
        82,
        83,
        146,
        147,
        20,
        21,
        84,
        85,
        148,
        149,
        22,
        23,
        86,
        87,
        150,
        151,
        24,
        25,
        88,
        89,
        152,
        153,
        26,
        27,
        90,
        91,
        154,
        155,
        28,
        29,
        92,
        93,
        156,
        157,
        30,
        31,
        94,
        95,
        158,
        159,
        32,
        33,
        96,
        97,
        160,
        161,
        34,
        35,
        98,
        99,
        162,
        163,
        36,
        37,
        100,
        101,
        164,
        165,
        38,
        39,
        102,
        103,
        166,
        167,
        40,
        41,
        104,
        105,
        168,
        169,
        42,
        43,
        106,
        107,
        170,
        171,
        44,
        45,
        108,
        109,
        172,
        173,
        46,
        47,
        110,
        111,
        174,
        175,
        48,
        49,
        112,
        113,
        176,
        177,
        50,
        51,
        114,
        115,
        178,
        179,
        52,
        53,
        116,
        117,
        180,
        181,
        54,
        55,
        118,
        119,
        182,
        183,
        56,
        57,
        120,
        121,
        184,
        185,
        58,
        59,
        122,
        123,
        186,
        187,
        60,
        61,
        124,
        125,
        188,
        189,
        62,
        63,
        126,
        127,
        190,
        191,
    }
);

constexpr auto kLugToCircuitMap = std::to_array(
    {
        0,
        1,
        6,
        7,
        12,
        13,
        18,
        19,
        24,
        25,
        30,
        31,
        36,
        37,
        42,
        43,
        48,
        49,
        54,
        55,
        60,
        61,
        66,
        67,
        72,
        73,
        78,
        79,
        84,
        85,
        90,
        91,
        96,
        97,
        102,
        103,
        108,
        109,
        114,
        115,
        120,
        121,
        126,
        127,
        132,
        133,
        138,
        139,
        144,
        145,
        150,
        151,
        156,
        157,
        162,
        163,
        168,
        169,
        174,
        175,
        180,
        181,
        186,
        187,
        2,
        3,
        8,
        9,
        14,
        15,
        20,
        21,
        26,
        27,
        32,
        33,
        38,
        39,
        44,
        45,
        50,
        51,
        56,
        57,
        62,
        63,
        68,
        69,
        74,
        75,
        80,
        81,
        86,
        87,
        92,
        93,
        98,
        99,
        104,
        105,
        110,
        111,
        116,
        117,
        122,
        123,
        128,
        129,
        134,
        135,
        140,
        141,
        146,
        147,
        152,
        153,
        158,
        159,
        164,
        165,
        170,
        171,
        176,
        177,
        182,
        183,
        188,
        189,
        4,
        5,
        10,
        11,
        16,
        17,
        22,
        23,
        28,
        29,
        34,
        35,
        40,
        41,
        46,
        47,
        52,
        53,
        58,
        59,
        64,
        65,
        70,
        71,
        76,
        77,
        82,
        83,
        88,
        89,
        94,
        95,
        100,
        101,
        106,
        107,
        112,
        113,
        118,
        119,
        124,
        125,
        130,
        131,
        136,
        137,
        142,
        143,
        148,
        149,
        154,
        155,
        160,
        161,
        166,
        167,
        172,
        173,
        178,
        179,
        184,
        185,
        190,
        191,
    }
);

D192Rom::D192Rom(QObject *parent)
    : Rom(parent)
{
    const auto racksToAdd = (*std::ranges::min_element(std::views::values(kRomSizes))) / 512;
    for (unsigned int rackNum = 0; rackNum < racksToAdd; ++rackNum) {
        auto rack = new D192Rack(rackNum, Rack::Type::D192Rack, this);
        rack->initLugAddressMap();
        racks_.push_back(rack);
    }
}

Phase patchman::D192Rack::getPhaseForLug(unsigned int lug) const
{
    if (getRackType() == Rack::Type::D192Rack) {
        if (lug < 64) {
            return Phase::A;
        }
        else if (lug < 128) {
            return Phase::B;
        }
        else {
            return Phase::C;
        }
    }
    Q_UNREACHABLE();
}

unsigned int D192Rack::getLugForCircuit(unsigned int circuit) const
{
    return kCircuitToLugMap.at(circuit);
}

unsigned int D192Rack::getCircuitForLug(unsigned int lug) const
{
    return kLugToCircuitMap.at(lug);
}

void D192Rack::initLugAddressMap()
{
    lugAddresses_.fill(0, kLugCounts.at(getRackType()));
}

Rack *D192Rom::addRack(unsigned int rackNum, Rack::Type rackType)
{
    auto *rack = dynamic_cast<D192Rack *>(getRack(rackNum));
    if (rack == nullptr) {
        rack = new D192Rack(rackNum, rackType, this);
        racks_.push_back(rack);
        sortRacks();
    }
    rack->initLugAddressMap();
    return rack;
}

void D192Rack::fromByteArray(QByteArrayView data)
{
    // Each byte is a DMX address (i.e. 0x00 -> DMX 1, 0x01 -> DMX 2, etc. The value in each location is the lug in the
    // rack that address will control. Lugs are numbered top to bottom, left to right (i.e. First column lugs 0-63,
    // second column lugs 64-127, etc. Note that this is different from how D192 racks are labeled, where the circuit
    // numbers run from left to right, top to bottom. Unpatched addresses are mapped to lug 255.
    for (unsigned int address = 1; address <= data.size(); ++address) {
        const uint8_t lug = data.at(address - 1);
        if (lug != 0xFF) {
            if (lug >= kLugCounts.at(getRackType())) {
                throw InvalidRomException(tr("Lug number out of range."),
                                          tr("Either this is not a D192 ROM or the file is corrupt."));
            }
            lugAddresses_[lug] = address;
        }
    }
}

QByteArray D192Rack::toByteArray() const
{
    QByteArray data(512, 0xFF);
    for (auto lug = 0; lug < lugAddresses_.size(); ++lug) {
        const auto address = lugAddresses_.at(lug);
        if (address == 0) {
            continue;
        }

        data[address - 1] = lug;
    }

    return data;
}

unsigned int D192Rack::getLugCount() const
{
    return kLugToCircuitMap.size();
}

unsigned int D192Rack::getLugsPerModule() const
{
    return 2;
}

unsigned int D192Rack::getModuleDensityForLug(unsigned int lug) const
{
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
QString D192Rack::getModuleNameForLug(unsigned int lug) const
{
    const auto density = getModuleDensityForLug(lug);

    switch (density) {
        case 0:return tr("Blank");
        case 1:return tr("6 kW");
        case 2:return tr("1.2/2.4 kW");
        default:return Rack::getModuleNameForLug(lug);
    }
    Q_UNREACHABLE();
}

bool D192Rom::isD192Rom(QByteArrayView data)
{
    // Patch EPROMs are 2716 (usually) or 2732 (big systems).
    const auto dataSize = data.size();
    if (!std::ranges::any_of(std::views::values(kRomSizes), [dataSize](auto check)
    { return check == dataSize; })) {
        // Wrong size.
        return false;
    }

    return true;
}

void D192Rom::loadFromData(const QByteArrayView data)
{
    if (!isD192Rom(data)) {
        throw InvalidRomException(tr("This is not a D192 ROM."));
    }

    // Patch is stored as a list of 512 byte chunks, one chunk for each rack in the ROM.
    const auto storedRackCount = data.size() / 512;
    // Separate patch tables.
    QByteArrayView allRackPatchTables(data);
    QList<QByteArrayView> rackPatchTables;
    rackPatchTables.reserve(storedRackCount);
    for (unsigned int tableIx = 0; tableIx < storedRackCount; ++tableIx) {
        rackPatchTables.push_back(allRackPatchTables.sliced(tableIx * 512, 512));
    }
    // Need to reverse because tables are stored in reverse order.
    std::reverse(rackPatchTables.begin(), rackPatchTables.end());
    // Load racks. The *last* entry in the rom is Rack 0, the preceding entry is Rack 1, etc.
    for (unsigned int rackNum = 0; rackNum < storedRackCount; ++rackNum) {
        auto rack = dynamic_cast<D192Rack *>(getRack(rackNum));
        if (rack == nullptr) {
            rack = dynamic_cast<D192Rack *>(addRack(rackNum, Rack::Type::D192Rack));
        }
        Q_ASSERT_X(rack != nullptr, std::source_location::current().function_name(), "Failed to create new D192 rack.");
        rack->fromByteArray(rackPatchTables.at(rackNum));
    }
}

QByteArray D192Rom::toByteArray() const
{
    // Determine which ROM size to use.
    const auto neededSize = racks_.size() * 512;
    const auto useSize = [neededSize]()
    {
        for (const auto romSize: std::views::values(kRomSizes)) {
            if (neededSize <= romSize) {
                return romSize;
            }
        }
        throw UnrepresentableException(tr("Cannot store racks in available ROM sizes."), tr("Remove some racks and try again."));
    }();

    QByteArray data;
    data.reserve(useSize);
    for (const auto *rack: std::views::reverse(racks_)) {
        const auto *d192Rack = dynamic_cast<const D192Rack *>(rack);
        Q_ASSERT_X(d192Rack != nullptr,
                   std::source_location::current().function_name(),
                   "Non D192 rack stored in D192 patch rom.");
        data.push_back(d192Rack->toByteArray());
    }
    data.resize(useSize, 0xFF);

    return data;
}

} // patchlib
