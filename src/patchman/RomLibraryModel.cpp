/**
 * @file RomLibraryModel.cpp
 *
 * @author Dan Keenan
 * @date 1/7/24
 * @copyright GNU GPLv3
 */

#include "RomLibraryModel.h"

#include <utility>
#include "patchlib/library/RomLibrary.h"
#include "patchlib/Rom.h"
#include "Settings.h"

namespace patchman
{

void RomLibraryModel::checkForFilesystemChanges()
{
    RomLibrary::get()->getAllRoms(Settings::GetRomSearchPaths())
        .then(
            [this](QList<RomInfo> romInfo)
            {
                beginResetModel();
                romInfo_ = std::move(romInfo);
                endResetModel();
            });
}

int RomLibraryModel::rowCount(const QModelIndex &parent) const
{
    return romInfo_.size();
}

int RomLibraryModel::columnCount(const QModelIndex &parent) const
{
    return kColumnCount;
}

QVariant RomLibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        const auto column = static_cast<Column>(section);
        if (role == Qt::DisplayRole) {
            switch (column) {
                case Column::Type:return tr("Type");
                case Column::Name:return tr("Name");
                case Column::Modified:return tr("Modified");
                case Column::RackCount:return tr("Racks");
                case Column::Checksum:return tr("Checksum");
            }
        }
    }

    return {};
}

QVariant RomLibraryModel::data(const QModelIndex &index, int role) const
{
    const auto column = static_cast<Column>(index.column());
    const auto romInfo = romInfo_.at(index.row());

    if (role == Qt::DisplayRole) {
        if (column == Column::Type) {
            return Rom::typeName(static_cast<Rom::Type>(romInfo.getRomType()));
        }
        else if (column == Column::Name) {
            return QFileInfo(romInfo.getFilePath()).fileName();
        }
        else if (column == Column::Modified) {
            return QLocale().toString(romInfo.getFileMTime(), QLocale::ShortFormat);
        }
        else if (column == Column::RackCount) {
            return romInfo.getRackCount();
        }
        else if (column == Column::Checksum) {
            return romInfo.getRomChecksum().toHex();
        }
    }

    return {};
}

} // patchman
