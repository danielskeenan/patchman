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
 * The number of lugs in each rack type.
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

Phase patchman::D192Rack::phaseForLug(unsigned int lug) const
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

void D192Rack::initLugAddressMap()
{
    lugAddresses_.fill(0, kLugCounts.at(getRackType()));
}

Rack *D192Rom::addRack(unsigned int rackNum, Rack::Type rackType)
{
    auto *rack = new D192Rack(rackNum, rackType, this);
    rack->initLugAddressMap();
    racks_.push_back(rack);
    sortRacks();
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
                throw InvalidRomException("Lug number out of range.");
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
        throw InvalidRomException("Not a D192 ROM");
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
        auto rack = dynamic_cast<D192Rack *>(addRack(rackNum, Rack::Type::D192Rack));
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
        throw patchman::UnrepresentableException("Cannot store racks in available rom sizes.");
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
