/**
 * @file D192RackEditor.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include <QHBoxLayout>
#include <QPushButton>
#include "D192RackEditor.h"

namespace patchman
{

D192RackEditor::D192RackEditor(D192Rack *rack, QWidget *parent)
    : RackEditor(rack, parent), rack_(rack), table_(new QTableView(this)), model_(new D192RackModel(rack_, this))
{
    auto *layout = new QHBoxLayout(this);

    layout->addWidget(table_);
    table_->setModel(model_);
    connect(model_, &D192RackModel::dataChanged, [this]()
    {
        Q_EMIT(dataChanged());
    });
    setModuleRowSpans();
}

QList<unsigned int> D192RackEditor::getSelectedCircuits() const
{
    QList<unsigned int> circuits;
    if (table_->selectionModel()->hasSelection()) {
        // The selection is not sorted, so need to ensure this is in circuit order.
        const auto indexes = table_->selectionModel()->selectedIndexes();
        for (const auto index: indexes) {
            circuits.push_back(index.row());
        }
        std::sort(circuits.begin(), circuits.end());
        circuits.erase(std::unique(circuits.begin(), circuits.end()), circuits.end());
    }
    else {
        // Renumber all circuits.
        circuits.resize(model_->rowCount(QModelIndex()));
        std::iota(circuits.begin(), circuits.end(), 0);
    }

    return circuits;
}

void D192RackEditor::setModuleRowSpans()
{
    const auto rowCount = model_->rowCount(QModelIndex());
    const auto lugsPerModule = rack_->getLugsPerModule();
    for (int row = 0; row < rowCount; row += lugsPerModule) {
        table_->setSpan(
            row,
            static_cast<std::underlying_type_t<D192RackModel::Column>>(D192RackModel::Column::Module),
            lugsPerModule,
            1
        );
    }
}

} // patchman
