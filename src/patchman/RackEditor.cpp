/**
 * @file RackEditor.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include "RackEditor.h"
#include "d192/D192RackEditor.h"

namespace patchman
{

RackEditor *RackEditor::create(Rack *rack, QWidget *parent)
{
    switch (rack->getRackType()) {
        case Rack::Type::D192Rack:
            return new D192RackEditor(dynamic_cast<D192Rack *>(rack), parent);
        case Rack::Type::Enr96:
        case Rack::Type::Enr48:
        case Rack::Type::Enr24:
        case Rack::Type::Enr12:
            return nullptr;
    }
    Q_UNREACHABLE();
}

} // patchman
