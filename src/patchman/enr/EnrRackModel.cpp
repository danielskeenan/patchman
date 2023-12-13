/**
 * @file EnrRackModel.cpp
 *
 * @author Dan Keenan
 * @date 12/13/23
 * @copyright GNU GPLv3
 */

#include "EnrRackModel.h"

namespace patchman
{

QString EnrRackModel::getModuleNameForLug(unsigned int lug) const
{
    const auto density = rack_->getModuleDensityForLug(lug);

    switch (density) {
        case 0:
            return tr("Blank");
        case 1:
            return tr("6 kW Module");
        case 2:
            return tr("1.8/2.4 kW Module");
        default:
            return RackModel::getModuleNameForLug(lug);
    }
    Q_UNREACHABLE();
}

} // patchman
