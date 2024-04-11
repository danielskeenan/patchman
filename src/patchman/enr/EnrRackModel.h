/**
 * @file EnrRackModel.h
 *
 * @author Dan Keenan
 * @date 12/13/23
 * @copyright GNU GPLv3
 */

#ifndef ENRRACKMODEL_H
#define ENRRACKMODEL_H

#include "../RackModel.h"
#include "patchlib/Enr.h"

namespace patchman
{

/**
 * Model class for ENR Racks.
 */
class EnrRackModel: public RackModel
{
Q_OBJECT
public:
    explicit EnrRackModel(EnrRack *rack, QObject *parent = nullptr)
        : RackModel(rack, parent), rack_(rack)
    {}

    enum class EnrColumn
    {
        Analog = RackModel::kUserColumn,
    };

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    EnrRack *rack_;
};

} // patchman

#endif //ENRRACKMODEL_H
