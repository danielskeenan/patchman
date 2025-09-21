/**
 * @file RackModel.h
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#ifndef RACKMODEL_H
#define RACKMODEL_H

#include <QAbstractTableModel>
#include "patchlib/Rack.h"

namespace patchman
{

/**
 * Base class for rack patch model classes.
 */
class RackModel: public QAbstractTableModel
{
Q_OBJECT
public:
    enum class Column
    {
        Phase,
        Circuit,
        Module,
        Address,
    };

    explicit RackModel(Rack *rack, QObject *parent);
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

protected:
    /** Starting column id for child classes to add their own columns */
    static inline const std::underlying_type_t<Column>
        kUserColumn = static_cast<std::underlying_type_t<Column>>(Column::Address) + 1;

private:
    Rack *rack_;
    int lugCount_;

    void updateLugCount();

private Q_SLOTS:
    void rackTypeChanged();
    void lugChanged(unsigned int lug);
};

} // patchman

#endif //RACKMODEL_H
