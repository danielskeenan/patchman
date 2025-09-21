/**
 * @file RackPatchModel.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include <QColor>
#include "RackModel.h"

namespace patchman
{

RackModel::RackModel(Rack *rack, QObject *parent)
    : QAbstractTableModel(parent), rack_(rack)
{
    updateLugCount();
    connect(rack_, &Rack::rackTypeChanged, this, &RackModel::rackTypeChanged);
    connect(rack_, &Rack::lugChanged, this, &RackModel::lugChanged);
}

void RackModel::updateLugCount()
{
    beginResetModel();
    lugCount_ = static_cast<int>(std::size(rack_->getLugAddressesView()));
    endResetModel();
}

int RackModel::rowCount(const QModelIndex &) const
{
    return lugCount_;
}

int RackModel::columnCount(const QModelIndex &) const
{
    return static_cast<std::underlying_type_t<Column>>(Column::Address) + 1;
}

QVariant RackModel::data(const QModelIndex &index, int role) const
{
    const auto column = static_cast<Column>(index.column());
    const auto circuit = index.row();
    const auto lug = rack_->getLugForCircuit(circuit);

    if (role == Qt::DisplayRole) {
        if (column == Column::Phase) {
            const auto phase = rack_->getPhaseForLug(lug);
            switch (phase) {
                case Phase::A:
                    return tr("A");
                case Phase::B:
                    return tr("B");
                case Phase::C:
                    return tr("C");
            }
        }
        else if (column == Column::Circuit) {
            return circuit + 1;
        }
        else if (column == Column::Module) {
            return rack_->getModuleNameForLug(lug);
        }
        else if (column == Column::Address) {
            const auto address = rack_->getLugAddress(lug);
            if (address == 0) {
                return tr(" - ");
            }
            else {
                return address;
            }
        }
    }
    else if (role == Qt::DecorationRole) {
        if (column == Column::Phase) {
            const auto phase = rack_->getPhaseForLug(lug);
            switch (phase) {
                case Phase::A:
                    return QColorConstants::Black;
                case Phase::B:
                    return QColorConstants::Red;
                case Phase::C:
                    return QColorConstants::Blue;
            }
        }
    }
    else if (role == Qt::EditRole) {
        if (column == Column::Address) {
            return rack_->getLugAddress(lug);
        }
    }

    return {};
}

bool RackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const auto column = static_cast<Column>(index.column());
    const auto circuit = index.row();
    const auto lug = rack_->getLugForCircuit(circuit);

    bool success = false;
    if (role == Qt::EditRole) {
        if (column == Column::Address) {
            const auto newAddress = value.toInt();
            if (newAddress >= 0 && newAddress <= 512) {
                // Emits dataChanged internally.
                rack_->setLugAddress(lug, newAddress);
                success = true;
            }
        }
    }

    return success;
}

QVariant RackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal) {
        const auto column = static_cast<Column>(section);
        if (role == Qt::ItemDataRole::DisplayRole) {
            switch (column) {
                case Column::Phase:
                    return tr("Phase");
                case Column::Circuit:
                    return tr("Circuit");
                case Column::Module:
                    return tr("Module");
                case Column::Address:
                    return tr("Addr");
            }
        }
    }

    return {};
}

Qt::ItemFlags RackModel::flags(const QModelIndex &index) const
{
    const auto defaultFlags = QAbstractTableModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    const auto column = static_cast<Column>(index.column());
    switch (column) {
        case Column::Phase:
        case Column::Circuit:
        case Column::Module:
            return defaultFlags;
        case Column::Address:
            return defaultFlags | Qt::ItemIsEditable;
    }
    Q_UNREACHABLE();
}

void RackModel::rackTypeChanged()
{
    updateLugCount();
}

void RackModel::lugChanged(unsigned int lug)
{
    const auto circuit = rack_->getCircuitForLug(lug);
    Q_EMIT(dataChanged(
        createIndex(circuit, 0),
        createIndex(circuit, columnCount(QModelIndex()) - 1)
    ));
}

} // patchman
