/**
 * @file Rack.cpp
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#include "patchlib/Rack.h"
#include <source_location>

namespace patchman
{

bool operator==(const Rack &lhs, const Rack &rhs)
{
    return lhs.rackNum_ == rhs.rackNum_ &&
        lhs.rackType_ == rhs.rackType_ &&
        lhs.lugAddresses_ == rhs.lugAddresses_;
}

std::strong_ordering operator<=>(const Rack &lhs, const Rack &rhs)
{
    return lhs.rackNum_ <=> rhs.rackNum_;
}

unsigned int Rack::getRackNum() const
{
    return rackNum_;
}

void Rack::setRackNum(unsigned int rackNum)
{
    rackNum_ = rackNum;
    Q_EMIT(rackNumChanged());
}

Rack::Type Rack::getRackType() const
{
    return rackType_;
}

void Rack::setRackType(Rack::Type rackType)
{
    rackType_ = rackType;
    Q_EMIT(rackTypeChanged());
}

QString Rack::getRackName() const
{
    return tr("Rack %1").arg(getRackNum());
}

QString Rack::getModuleNameForLug(unsigned int lug) const
{
    const auto density = getModuleDensityForLug(lug);
    return tr("%1 Ckt Module").arg(density);
}

unsigned int Rack::getLugAddress(unsigned int lug) const
{
    Q_ASSERT_X(lug < lugAddresses_.size(),
               std::source_location::current().function_name(),
               "Tried to get lug not in rack.");
    return lugAddresses_.at(lug);
}

void Rack::setLugAddress(unsigned int lug, unsigned int address)
{
    Q_ASSERT_X(lug < lugAddresses_.size(),
               std::source_location::current().function_name(),
               "Tried to set lug not in rack.");
    lugAddresses_[lug] = address;
    Q_EMIT(lugChanged(lug));
}

bool Rack::isPatched() const
{
    return std::any_of(lugAddresses_.cbegin(), lugAddresses_.cend(),
                       [](unsigned int addr)
                       { return addr > 0; });
}

} // patchlib
