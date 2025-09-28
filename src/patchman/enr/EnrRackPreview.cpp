/**
 * @file EnrRackPreview.cpp
 *
 * @author Dan Keenan
 * @date 9/27/25
 * @copyright GNU GPLv3
 */

#include "EnrRackPreview.h"
#include <QItemSelectionModel>

#include "QVBoxLayout"

namespace patchman {

namespace detail {
EnrRackPreviewModel::EnrRackPreviewModel(EnrRack *rack, QObject *parent)
    : QAbstractListModel(parent)
    , rack_(rack)
{
    connect(rack_, &EnrRack::lugChanged, this, &EnrRackPreviewModel::lugChanged);
}

int EnrRackPreviewModel::rowCount(const QModelIndex &parent) const
{
    return static_cast<int>(rack_->getLugCount() / rack_->getLugsPerModule());
}

QVariant EnrRackPreviewModel::data(const QModelIndex &index, int role) const
{
    const auto slot = index.row();
    const auto lugA = slot * 2;
    const auto lugB = lugA + 1;

    if (role == Qt::DisplayRole) {
        const auto addrA = rack_->getLugAddress(lugA);
        const auto addrB = rack_->getLugAddress(lugB);
        if (addrA != 0 && addrB != 0) {
            return tr("%1 / %2").arg(addrA).arg(addrB);
        } else if (addrA != 0 && addrB == 0) {
            return addrA;
        } else if (addrA == 0 && addrB != 0) {
            return addrB;
        }
        return tr(" - ");
    } else if (role == Qt::ForegroundRole) {
        const auto phase = rack_->getPhaseForLug(lugA);
        switch (phase) {
        case Phase::A:
            return QBrush(QColorConstants::Black);
        case Phase::B:
            return QBrush(QColorConstants::Red);
        case Phase::C:
            return QBrush(QColorConstants::Blue);
        }
    } else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    return {};
}

void EnrRackPreviewModel::lugChanged(unsigned int lug)
{
    const auto ix = index(static_cast<int>(lug) / 2, 0);
    Q_EMIT(dataChanged(ix, ix));
}

Qt::ItemFlags EnrRackPreviewModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
} // namespace detail

EnrRackPreview::EnrRackPreview(EnrRack *rack, QWidget *parent)
    : RackPreview(parent)
    , rack_(rack)
    , list_(new QListView(this))
    , model_(new detail::EnrRackPreviewModel(rack, this))
{
    setFixedWidth(100);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(list_);
    list_->setModel(model_);
    list_->setFrameStyle(QFrame::Panel | QFrame::Raised);
    list_->setLineWidth(1);
    list_->setSelectionMode(QListView::ExtendedSelection);
    list_->setStyleSheet(R"(
QListView::item {
  background: white;
  border-top: 1px solid black;
  border-bottom: 1px solid black;
}
QListView::item:selected {
  background-color: palette(highlight);
}
)");
    connect(
        list_->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &EnrRackPreview::selectionChanged);
}

QList<unsigned int> EnrRackPreview::getSelectedCircuits() const
{
    QList<unsigned int> circuits;
    const auto selection = list_->selectionModel()->selection();
    for (const auto &range : selection) {
        for (int slot = range.top(); slot <= range.bottom(); ++slot) {
            const auto circuitA = rack_->getCircuitForLug(static_cast<unsigned int>(slot * 2));
            circuits.append(circuitA);
            const auto circuitB = circuitA + 1;
            circuits.append(circuitB);
        }
    }
    return circuits;
}

void EnrRackPreview::selectCircuits(const QList<unsigned int> &circuits)
{
    QItemSelection s;
    for (const auto circuit : circuits) {
        const auto lug = rack_->getLugForCircuit(circuit);
        const auto slot = lug / 2;
        const auto ix = model_->index(static_cast<int>(slot));
        s.select(ix, ix);
    }
    QSignalBlocker blocker(this);
    list_->selectionModel()->select(s, QItemSelectionModel::ClearAndSelect);
    if (!s.empty()) {
        list_->selectionModel()->setCurrentIndex(s.first().topLeft(), QItemSelectionModel::Current);
    }
}

} // namespace patchman
