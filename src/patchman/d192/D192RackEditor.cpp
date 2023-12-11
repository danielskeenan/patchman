/**
 * @file D192RackEditor.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "D192RackEditor.h"
#include "../AutoNumberDialog.h"

namespace patchman
{
D192RackEditor::D192RackEditor(D192Rack *rack, QWidget *parent)
    : RackEditor(rack, parent), rack_(rack), table_(new QTableView(this)), model_(new D192RackModel(rack_, this))
{
    auto *layout = new QVBoxLayout(this);

    auto *buttonsLayout = new QHBoxLayout;
    layout->addLayout(buttonsLayout);

    auto *autonumber = new QPushButton(tr("Autonumber"), this);
    connect(autonumber, &QPushButton::clicked, this, &D192RackEditor::autonumber);
    buttonsLayout->addWidget(autonumber);

    auto *unpatch = new QPushButton(tr("Unpatch"), this);
    connect(unpatch, &QPushButton::clicked, this, &D192RackEditor::unpatch);
    buttonsLayout->addWidget(unpatch);
    buttonsLayout->addStretch();

    layout->addWidget(table_);
    table_->setSelectionBehavior(QTableView::SelectRows);
    table_->setModel(model_);
    connect(model_, &D192RackModel::dataChanged, [this]()
    {
        Q_EMIT(dataChanged());
    });
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

void D192RackEditor::autonumber()
{
    const auto circuits = getSelectedCircuits();
    AutoNumberDialog dialog(this);
    if (dialog.exec() != AutoNumberDialog::Accepted) {
        return;
    }
    const auto options = dialog.getOptions();
    unsigned int nextAddress = options.start;
    for (auto circuitIt = circuits.begin(); circuitIt != circuits.end(); circuitIt += options.offset) {
        const auto circuit = *circuitIt;
        const auto lug = rack_->lugForCircuit(circuit);
        rack_->setLugAddress(lug, nextAddress);
        nextAddress += options.increment;
    }
}

void D192RackEditor::unpatch()
{
    const auto circuits = getSelectedCircuits();
    for (const auto circuit: circuits) {
        const auto lug = rack_->lugForCircuit(circuit);
        rack_->setLugAddress(lug, 0);
    }
}

} // patchman
