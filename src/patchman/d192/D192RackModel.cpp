/**
 * @file D192RackModel.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include "D192RackModel.h"

namespace patchman
{

QString D192RackModel::getModuleNameForLug(unsigned int lug) const
{
    const auto density = rack_->getModuleDensityForLug(lug);

    switch (density) {
        case 0:
            return tr("Blank");
        case 1:
            return tr("6 kW Module");
        case 2:
            return tr("1.2/2.4 kW Module");
        default:
            return RackModel::getModuleNameForLug(lug);
    }
    Q_UNREACHABLE();
}

} // patchman
