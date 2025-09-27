/**
 * @file EnrRackPreview.cpp
 *
 * @author Dan Keenan
 * @date 9/27/25
 * @copyright GNU GPLv3
 */

#include "EnrRackPreview.h"

namespace patchman {

detail::EnrRackPreviewModel::EnrRackPreviewModel(EnrRack *rack, QObject *parent)
    : QAbstractListModel(parent), rack_(rack) {}

int detail::EnrRackPreviewModel::rowCount(const QModelIndex &parent) const {
  return static_cast<int>(rack_->getLugCount() / rack_->getLugsPerModule());
}

QVariant detail::EnrRackPreviewModel::data(const QModelIndex &index,
                                           int role) const {
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
  }

  return {};
}

void detail::EnrRackPreviewModel::lugChanged(unsigned int lug) {
  const int slot = static_cast<int>(lug) / 2;
  Q_EMIT(dataChanged(createIndex(slot, 0), createIndex(slot, 0)));
}

EnrRackPreview::EnrRackPreview(EnrRack *rack, QWidget *parent)
    : QListView(parent), model_(new detail::EnrRackPreviewModel(rack, this)) {
  setFixedWidth(100);
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  setLineWidth(1);
  setStyleSheet(R"(
QListView {
  background: white;
}
QListView::item {
  border-top: 1px solid black;
  border-bottom: 1px solid black;
  color: black;
  text-align: center;
};
)");
  QListView::setModel(model_);
}

} // namespace patchman
