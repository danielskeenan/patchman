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
  connect(preview_->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &D192RackEditor::previewSelectionChanged);
  connect(table_->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &D192RackEditor::tableSelectionChanged);
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
  const auto selection = table_->selectionModel()->selection();
  std::set<int> circuits;
  for (const auto &range : selection) {
    for (int circuit = range.top(); circuit <= range.bottom(); ++circuit) {
      circuits.insert(circuit);
    }
  }
  // Avoid signal loops.
  disconnect(preview_->selectionModel(), &QItemSelectionModel::selectionChanged,
             this, &D192RackEditor::previewSelectionChanged);
  preview_->selectCircuits(circuits);
  connect(preview_->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &D192RackEditor::previewSelectionChanged);
}

void D192RackEditor::previewSelectionChanged() {
  const auto selection = preview_->selectionModel()->selection();
  QItemSelection s;
  for (const auto &lugIndex : selection.indexes()) {
    const auto slot = (lugIndex.column() * 32) + lugIndex.row();
    const auto lug = slot * 2;
    const auto circuitA = rack_->getCircuitForLug(lug);
    const auto circuitB = circuitA + 1;
    s.select(model_->index(circuitA, 0),
             model_->index(circuitB, model_->columnCount({}) - 1));
  }
  disconnect(table_->selectionModel(), &QItemSelectionModel::selectionChanged,
             this, &D192RackEditor::tableSelectionChanged);
  table_->selectionModel()->select(s, QItemSelectionModel::ClearAndSelect);
  table_->selectionModel()->setCurrentIndex(s.first().topLeft(),
                                            QItemSelectionModel::Current);
  connect(table_->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &D192RackEditor::tableSelectionChanged);
}

} // namespace patchman
