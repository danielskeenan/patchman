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
  connect(preview_->selectionModel(), &QItemSelectionModel::currentChanged,
          this, &EnrRackEditor::previewSelectionChanged);
  connect(table_->selectionModel(), &QItemSelectionModel::currentChanged, this,
          &EnrRackEditor::tableSelectionChanged);
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
  const auto current = table_->selectionModel()->currentIndex();
  if (current.isValid()) {
    preview_->selectLug(current.row());
  } else {
    preview_->clearSelection();
  }
}

void EnrRackEditor::previewSelectionChanged() {
  const auto current = preview_->selectionModel()->currentIndex();
  if (current.isValid()) {
    table_->selectionModel()->setCurrentIndex(
        model_->index(current.row() * 2,
                      static_cast<int>(EnrRackModel::Column::Address)),
        QItemSelectionModel::Clear | QItemSelectionModel::Select |
            QItemSelectionModel::Current);
  } else {
    table_->clearSelection();
  }
}

} // namespace patchman
