/**
 * @file RomLibraryModel.cpp
 *
 * @author Dan Keenan
 * @date 1/7/24
 * @copyright GNU GPLv3
 */

#include "RomLibraryModel.h"

#include <utility>
#include <QIcon>
#include "patchlib/library/RomLibrary.h"
#include "patchlib/Rom.h"
#include "Settings.h"

namespace patchman
{

RomLibraryModel::RomLibraryModel(QObject *parent)
    : QAbstractTableModel(parent), watcher_(new QFutureWatcher<QList<RomInfo>>(this))
{
    connect(watcher_, &RomInfoListWatcher::progressRangeChanged, this, &RomLibraryModel::progressRangeChanged);
    connect(watcher_, &RomInfoListWatcher::progressTextChanged, this, &RomLibraryModel::progressTextChanged);
    connect(watcher_, &RomInfoListWatcher::progressValueChanged, this, &RomLibraryModel::progressValueChanged);
}

int RomLibraryModel::rowCount(const QModelIndex &parent) const
{
    return static_cast<int>(romInfo_.size());
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
                case Column::PatchHash:return tr("Patch Hash");
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
            return romInfo.getFileMTime();
        }
        else if (column == Column::RackCount) {
            return romInfo.getRackCount();
        }
        else if (column == Column::Checksum) {
            return romInfo.getRomChecksum().toHex();
        }
        else if (column == Column::PatchHash) {
            return romInfo.getPatchHash().toHex();
        }
    }
    else if (role == Qt::DecorationRole) {
        if (column == Column::Name) {
            if (patchTableCounts_[romInfo.getPatchHash()] > 1) {
                return QIcon::fromTheme("document-duplicate");
            }
        }
    }
    else if (role == Qt::UserRole) {
        if (column == Column::Type) {
            return Rom::typeName(static_cast<Rom::Type>(romInfo.getRomType()));
        }
        else if (column == Column::Name) {
            return QFileInfo(romInfo.getFilePath()).fileName();
        }
        else if (column == Column::Modified) {
            return romInfo.getFileMTime();
        }
        else if (column == Column::RackCount) {
            return romInfo.getRackCount();
        }
        else if (column == Column::Checksum) {
            return romInfo.getRomChecksum();
        }
        else if (column == Column::PatchHash) {
            return romInfo.getPatchHash();
        }
    }

    return {};
}

const RomInfo &RomLibraryModel::getRomInfoForRow(int row) const
{
    return romInfo_.at(row);
}

void RomLibraryModel::checkForFilesystemChanges()
{
    Q_EMIT(progressTextChanged(tr("Searching for ROMs")));
    auto future = RomLibrary::get()->getAllRoms(Settings::GetRomSearchPaths());
    watcher_->setFuture(future);
    future
        .then(
            [this](const QList<RomInfo> &romInfoList)
            {
                std::scoped_lock romInfoGuard(romInfoMutex_);
                beginResetModel();
                romInfo_ = romInfoList;
                endResetModel();

                return romInfoList;
            })
        .then(
            [this](const QList<RomInfo> &romInfoList)
            {
                std::scoped_lock patchTableCountsGuard(patchTableCountsMutex_);
                beginResetModel();
                patchTableCounts_.clear();
                patchTableCounts_.reserve(romInfoList.size());
                Q_EMIT(progressRangeChanged(0, static_cast<int>(romInfoList.size())));
                Q_EMIT(progressTextChanged(tr("Finding duplicate ROMs")));
                for (int ix = 0; ix < romInfoList.size(); ++ix) {
                    const auto &romInfo = romInfoList.at(ix);
                    const auto duplicates = RomLibrary::get()->getDuplicates(romInfo).result();
                    patchTableCounts_[romInfo.getPatchHash()] = static_cast<unsigned int>(duplicates.size());
                    Q_EMIT(progressValueChanged(ix + 1));
                }
                endResetModel();
                Q_EMIT(progressTextChanged(""));
            }
        );
}

RomLibrarySortFilterModel::RomLibrarySortFilterModel(RomLibraryModel *sourceModel, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
    setFilterRole(Qt::UserRole);
    setSortRole(Qt::UserRole);
}

} // patchman
