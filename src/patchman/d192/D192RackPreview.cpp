/**
 * @file D192RackPreview.cpp
 *
 * @author Dan Keenan
 * @date 9/27/25
 * @copyright GNU GPLv3
 */

#include "D192RackPreview.h"
#include <QHeaderView>

namespace patchman {
namespace detail {
D192RackPreviewModel::D192RackPreviewModel(D192Rack *rack, QObject *parent)
    : QAbstractTableModel(parent), rack_(rack) {
  connect(rack_, &D192Rack::lugChanged, this,
          &D192RackPreviewModel::lugChanged);
}

int D192RackPreviewModel::rowCount(const QModelIndex &parent) const {
  return static_cast<int>(rack_->getLugCount() / rack_->getLugsPerModule() / 3);
}

int D192RackPreviewModel::columnCount(const QModelIndex &parent) const {
  return 3;
}

QVariant D192RackPreviewModel::data(const QModelIndex &index, int role) const {
  const auto slot = (index.column() * 32) + index.row();
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

Qt::ItemFlags D192RackPreviewModel::flags(const QModelIndex &index) const {
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex D192RackPreviewModel::lugIndex(int lug) const {
  const auto row = lug % 64 / 2;
  const auto col = [lug]() {
    if (lug < 64) {
      return 0;
    } else if (lug < 128) {
      return 1;
    } else {
      return 2;
    }
  }();
  return index(row, col);
}

void D192RackPreviewModel::lugChanged(unsigned int lug) {
  const auto ix = lugIndex(static_cast<int>(lug));
  Q_EMIT(dataChanged(ix, ix));
}
} // namespace detail

D192RackPreview::D192RackPreview(D192Rack *rack, QWidget *parent)
    : QTableView(parent), model_(new detail::D192RackPreviewModel(rack, this)) {
  QTableView::setModel(model_);
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  setLineWidth(1);
  setShowGrid(false);
  horizontalHeader()->setVisible(false);
  verticalHeader()->setVisible(false);
  for (int col = 0; col < 3; ++col) {
    setColumnWidth(col, 75);
  }
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  setStyleSheet(R"(
QTableView::item {
  background: white;
  border: 1px solid black;
}
QTableView::item:selected {
  background-color: lightgrey;
}
)");
}

void D192RackPreview::selectLug(const int lug) {
  selectionModel()->setCurrentIndex(model_->lugIndex(lug),
                                    QItemSelectionModel::Clear |
                                        QItemSelectionModel::Select |
                                        QItemSelectionModel::Current);
}
} // namespace patchman
