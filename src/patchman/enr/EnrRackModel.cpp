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

int EnrRackModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<std::underlying_type_t<EnrColumn>>(EnrColumn::Analog) + 1;
}

QVariant EnrRackModel::data(const QModelIndex &index, int role) const
{
    const auto column = static_cast<EnrColumn>(index.column());
    if (column == EnrColumn::Analog) {
        const auto circuit = index.row();
        const auto lug = rack_->getLugForCircuit(circuit);
        if (role == Qt::DisplayRole) {
            const auto chan = rack_->getLugAnalogChan(lug);
            if (chan == 0) {
                return tr(" - ");
            }
            else {
                return chan;
            }
        }
        else if (role == Qt::EditRole) {
            return rack_->getLugAnalogChan(lug);
        }
    }

    return RackModel::data(index, role);
}

bool EnrRackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return RackModel::setData(index, value, role);
}

QVariant EnrRackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal) {
        const auto column = static_cast<EnrColumn>(section);
        if (role == Qt::ItemDataRole::DisplayRole) {
            if (column == EnrColumn::Analog) {
                return tr("Analog Chan");
            }
        }
    }

    return RackModel::headerData(section, orientation, role);
}

Qt::ItemFlags EnrRackModel::flags(const QModelIndex &index) const
{
    const auto column = static_cast<EnrColumn>(index.column());
    if (column == EnrColumn::Analog) {
        // Use the same flags as the corresponding DMX address column.
        return RackModel::flags(createIndex(index.row(), static_cast<int>(Column::Address)));
    }

    return RackModel::flags(index);
}

} // patchman
