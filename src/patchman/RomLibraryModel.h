/**
 * @file RomLibraryModel.h
 *
 * @author Dan Keenan
 * @date 1/7/24
 * @copyright GNU GPLv3
 */

#ifndef ROMLIBRARYMODEL_H
#define ROMLIBRARYMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include "patchlib/library/RomInfo.h"

namespace patchman
{

/**
 * Model class for ROM Library data.
 */
class RomLibraryModel: public QAbstractTableModel
{
Q_OBJECT
public:
    enum class Column
    {
        Type,
        Name,
        Modified,
        RackCount,
        Checksum,
        PatchHash,
    };
    static const auto kColumnCount = static_cast<std::underlying_type_t<Column>>(Column::PatchHash) + 1;

    using QAbstractTableModel::QAbstractTableModel;

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] const RomInfo &getRomInfoForRow(int row) const;

public Q_SLOTS:
    void checkForFilesystemChanges();

private:
    std::mutex romInfoMutex_;
    QList<RomInfo> romInfo_;
    std::mutex patchTableCountsMutex_;
    /** How many patch tables have the same hash. */
    QHash<QByteArray, unsigned int> patchTableCounts_;
};

/**
 * Sort/Filter implementations for RomLibraryModel.
 */
class RomLibrarySortFilterModel: public QSortFilterProxyModel
{
Q_OBJECT
public:
    explicit RomLibrarySortFilterModel(RomLibraryModel *sourceModel, QObject *parent = nullptr);
};

} // patchman

#endif //ROMLIBRARYMODEL_H
