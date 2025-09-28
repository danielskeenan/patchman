/**
 * @file RackPreview.cpp
 *
 * @author Dan Keenan
 * @date 9/28/25
 * @copyright GNU GPLv3
 */

#include "RackPreview.h"

#include "d192/D192RackPreview.h"
#include "enr/EnrRackPreview.h"

namespace patchman {

RackPreview *RackPreview::create(Rack *rack, QWidget *parent)
{
    switch (rack->getRackType()) {
    case Rack::Type::D192Rack:
        return new D192RackPreview(dynamic_cast<D192Rack *>(rack), parent);
    case Rack::Type::Enr96:
        return new EnrRackPreview(dynamic_cast<EnrRack *>(rack), parent);
    default:
        return nullptr;
    }
    Q_UNREACHABLE();
}

} // namespace patchman
