/**
 * @file EnrRackPreview.cpp
 *
 * @author Dan Keenan
 * @date 9/27/25
 * @copyright GNU GPLv3
 */

#include "EnrRackPreview.h"
#include <QItemSelectionModel>

namespace patchman {

namespace detail {
EnrRackPreviewModel::EnrRackPreviewModel(EnrRack *rack, QObject *parent)
    : QAbstractListModel(parent), rack_(rack) {
  connect(rack_, &EnrRack::lugChanged, this, &EnrRackPreviewModel::lugChanged);
}

int EnrRackPreviewModel::rowCount(const QModelIndex &parent) const {
  return static_cast<int>(rack_->getLugCount() / rack_->getLugsPerModule());
}

QVariant EnrRackPreviewModel::data(const QModelIndex &index, int role) const {
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

void EnrRackPreviewModel::lugChanged(unsigned int lug) {
  const auto ix = index(static_cast<int>(lug) / 2, 0);
  Q_EMIT(dataChanged(ix, ix));
}

Qt::ItemFlags EnrRackPreviewModel::flags(const QModelIndex &index) const {
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
} // namespace detail

EnrRackPreview::EnrRackPreview(EnrRack *rack, QWidget *parent)
    : QListView(parent), model_(new detail::EnrRackPreviewModel(rack, this)) {
  QListView::setModel(model_);
  setFixedWidth(100);
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  setLineWidth(1);
  setSelectionMode(QListView::ExtendedSelection);
  setStyleSheet(R"(
QListView::item {
  background: white;
  border-top: 1px solid black;
  border-bottom: 1px solid black;
}
QListView::item:selected {
  background-color: lightgrey;
}
)");
}

void EnrRackPreview::selectLugs(const std::set<int> &lugs) {
  QItemSelection s;
  for (const int lug : lugs) {
    const auto slot = lug / 2;
    const auto ix = model_->index(slot);
    s.select(ix, ix);
  }
  selectionModel()->select(s, QItemSelectionModel::ClearAndSelect);
  if (!s.empty()) {
    selectionModel()->setCurrentIndex(s.first().topLeft(),
                                      QItemSelectionModel::Current);
  }
}

} // namespace patchman
