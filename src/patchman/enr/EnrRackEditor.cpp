/**
 * @file EnrRackEditor.cpp
 *
 * @author Dan Keenan
 * @date 12/13/23
 * @copyright GNU GPLv3
 */

#include "EnrRackEditor.h"
#include <QHBoxLayout>
#include <QItemSelectionModel>

namespace patchman {

EnrRackEditor::EnrRackEditor(EnrRack *rack, QWidget *parent)
    : RackEditor(rack, parent), rack_(rack), table_(new QTableView(this)),
      model_(new EnrRackModel(rack_, this)),
      preview_(new EnrRackPreview(rack_, this)) {
  auto *layout = new QHBoxLayout(this);

  // Table
  layout->addWidget(table_);
  table_->setModel(model_);
  connect(model_, &EnrRackModel::dataChanged,
          [this]() { Q_EMIT(dataChanged()); });
  setModuleRowSpans();

  // Preview
  layout->addWidget(preview_);
  connect(preview_->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &EnrRackEditor::previewSelectionChanged);
  connect(table_->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &EnrRackEditor::tableSelectionChanged);
}

QList<unsigned int> EnrRackEditor::getSelectedCircuits() const {
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

void EnrRackEditor::setModuleRowSpans() {
  const auto rowCount = model_->rowCount(QModelIndex());
  const auto lugsPerModule = rack_->getLugsPerModule();
  for (int row = 0; row < rowCount; row += lugsPerModule) {
    table_->setSpan(row,
                    static_cast<std::underlying_type_t<EnrRackModel::Column>>(
                        EnrRackModel::Column::Module),
                    lugsPerModule, 1);
  }
}

void EnrRackEditor::tableSelectionChanged() {
  const auto selection = table_->selectionModel()->selection();
  std::set<int> lugs;
  for (const auto &range : selection) {
    for (int lug = range.top(); lug <= range.bottom(); ++lug) {
      lugs.insert(lug);
    }
  }
  // Avoid signal loops.
  disconnect(preview_->selectionModel(), &QItemSelectionModel::selectionChanged,
             this, &EnrRackEditor::previewSelectionChanged);
  preview_->selectLugs(lugs);
  connect(preview_->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &EnrRackEditor::previewSelectionChanged);
}

void EnrRackEditor::previewSelectionChanged() {
  const auto selection = preview_->selectionModel()->selection();
  QItemSelection s;
  for (const auto &range : selection) {
    for (int slot = range.top(); slot <= range.bottom(); ++slot) {
      const auto lugA = slot * 2;
      const auto lugB = lugA + 1;
      s.select(model_->index(lugA, 0),
               model_->index(lugB, model_->columnCount({}) - 1));
    }
  }
  disconnect(table_->selectionModel(), &QItemSelectionModel::selectionChanged,
             this, &EnrRackEditor::tableSelectionChanged);
  table_->selectionModel()->select(s, QItemSelectionModel::ClearAndSelect);
  table_->selectionModel()->setCurrentIndex(s.first().topLeft(),
                                            QItemSelectionModel::Current);
  connect(table_->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &EnrRackEditor::tableSelectionChanged);
}

} // namespace patchman
