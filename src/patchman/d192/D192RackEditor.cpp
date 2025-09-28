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
    : RackEditor(rack, parent)
    , rack_(rack)
    , table_(new QTableView(this))
    , model_(new D192RackModel(rack_, this))
{
    auto *layout = new QHBoxLayout(this);

    layout->addWidget(table_);
    table_->setModel(model_);
    connect(model_, &D192RackModel::dataChanged, [this]() { Q_EMIT(dataChanged()); });
    connect(
        table_->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &D192RackEditor::selectionChanged);
    setModuleRowSpans();
}

QList<unsigned int> D192RackEditor::getSelectedCircuits() const
{
    QList<unsigned int> circuits;
    // The selection is not sorted, so need to ensure this is in circuit order.
    const auto indexes = table_->selectionModel()->selectedIndexes();
    for (const auto index : indexes) {
        circuits.push_back(index.row());
    }
    std::sort(circuits.begin(), circuits.end());
    circuits.erase(std::unique(circuits.begin(), circuits.end()), circuits.end());

    return circuits;
}

void D192RackEditor::selectCircuits(const QList<unsigned int> &circuits)
{
    QItemSelection s;
    for (const auto circuit : circuits) {
        s.select(
            model_->index(static_cast<int>(circuit), 0),
            model_->index(static_cast<int>(circuit), model_->columnCount({}) - 1));
    }
    // Avoid signal loops.
    QSignalBlocker blocker(this);
    table_->selectionModel()->select(s, QItemSelectionModel::ClearAndSelect);
    if (!s.empty()) {
        table_->selectionModel()->setCurrentIndex(s.first().topLeft(), QItemSelectionModel::Current);
    }
}

void D192RackEditor::setModuleRowSpans()
{
    const auto rowCount = model_->rowCount(QModelIndex());
    const auto lugsPerModule = rack_->getLugsPerModule();
    for (int row = 0; row < rowCount; row += lugsPerModule) {
        table_->setSpan(
            row,
            static_cast<std::underlying_type_t<D192RackModel::Column>>(
                D192RackModel::Column::Module),
            lugsPerModule,
            1);
    }
}

} // namespace patchman
