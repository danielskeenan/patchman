/**
 * @file D192RackEditor.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include "D192RackEditor.h"
#include <QHBoxLayout>
#include <QPushButton>

namespace patchman {

D192RackEditor::D192RackEditor(D192Rack *rack, QWidget *parent)
    : RackEditor(rack, parent), rack_(rack), table_(new QTableView(this)),
      model_(new D192RackModel(rack_, this)),
      preview_(new D192RackPreview(rack_, this)) {
  auto *layout = new QHBoxLayout(this);

  // Table
  layout->addWidget(table_);
  table_->setModel(model_);
  connect(model_, &D192RackModel::dataChanged,
          [this]() { Q_EMIT(dataChanged()); });
  setModuleRowSpans();

  // Preview
  layout->addWidget(preview_);
  connect(preview_->selectionModel(), &QItemSelectionModel::currentChanged,
          this, &D192RackEditor::previewSelectionChanged);
  connect(table_->selectionModel(), &QItemSelectionModel::currentChanged, this,
          &D192RackEditor::tableSelectionChanged);
}

QList<unsigned int> D192RackEditor::getSelectedCircuits() const {
  QList<unsigned int> circuits;
  if (table_->selectionModel()->hasSelection()) {
    // The selection is not sorted, so need to ensure this is in circuit order.
    const auto indexes = table_->selectionModel()->selectedIndexes();
    for (const auto index : indexes) {
      circuits.push_back(index.row());
    }
    std::sort(circuits.begin(), circuits.end());
    circuits.erase(std::unique(circuits.begin(), circuits.end()),
                   circuits.end());
  } else {
    // Renumber all circuits.
    circuits.resize(model_->rowCount(QModelIndex()));
    std::iota(circuits.begin(), circuits.end(), 0);
  }

  return circuits;
}

void D192RackEditor::setModuleRowSpans() {
  const auto rowCount = model_->rowCount(QModelIndex());
  const auto lugsPerModule = rack_->getLugsPerModule();
  for (int row = 0; row < rowCount; row += lugsPerModule) {
    table_->setSpan(row,
                    static_cast<std::underlying_type_t<D192RackModel::Column>>(
                        D192RackModel::Column::Module),
                    lugsPerModule, 1);
  }
}

void D192RackEditor::tableSelectionChanged() {
  const auto current = table_->selectionModel()->currentIndex();
  if (current.isValid()) {
    const auto circuit = current.row();
    preview_->selectLug(rack_->getLugForCircuit(circuit));
  } else {
    preview_->clearSelection();
  }
}

void D192RackEditor::previewSelectionChanged() {
  const auto current = preview_->selectionModel()->currentIndex();
  if (current.isValid()) {
    const auto slot = (current.column() * 32) + current.row();
    const auto lug = slot * 2;
    const auto circuit = rack_->getCircuitForLug(lug);
    table_->selectionModel()->setCurrentIndex(
        model_->index(circuit,
                      static_cast<int>(D192RackModel::Column::Address)),
        QItemSelectionModel::Clear | QItemSelectionModel::Select |
            QItemSelectionModel::Current);
  } else {
    table_->clearSelection();
  }
}

} // namespace patchman
