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
#include "../AutoNumberDialog.h"

namespace patchman
{

D192RackModuleMimic::D192RackModuleMimic(D192Rack *rack, unsigned int firstLug, QWidget *parent)
    : QWidget(parent), rack_(rack), firstLug_(firstLug), label_(new QLabel(this))
{
    for (unsigned int slotLug = 0; slotLug < addresses_.size(); ++slotLug) {
        addresses_[slotLug] = rack_->getLugAddress(firstLug_ + slotLug);
    }
    updateLabel();
}

void D192RackModuleMimic::setAddress(unsigned int lug, unsigned int address)
{
    if (lug < firstLug_ || lug >= firstLug_ + addresses_.size()) {
        qWarning() << "Tried to set lug address on widget that does not contain that lug.";
        return;
    }
    addresses_[firstLug_ - lug] = address;
    updateLabel();
}

void D192RackModuleMimic::updateLabel()
{
    const auto density = rack_->getModuleDensityForLug(firstLug_);
    if (density == 0) {
        label_->clear();
    }
    else if (density == 1) {
        label_->setText(tr("%1").arg(addresses_.front()));
    }
    else {
        label_->setText(tr("%1 / %2").arg(addresses_.front()).arg(addresses_.back()));
    }
}

D192RackMimic::D192RackMimic(D192Rack *rack, QWidget *parent)
    : QWidget(parent), rack_(rack)
{
    auto *layout = new QGridLayout(this);
    for (int lug = 0; lug < 192; lug += 2) {
        auto *widget = new D192RackModuleMimic(rack_, lug, this);
        slots_.push_back(widget);
        const auto slotNum = lug / 2;
        const auto [col, row] = std::div(slotNum, 32);
        layout->addWidget(widget, row, col, Qt::AlignCenter);
    }
    connect(rack_, &D192Rack::lugChanged, this, &D192RackMimic::lugChanged);
}

void D192RackMimic::lugChanged(unsigned int lug)
{
    const auto slotNum = lug / 2;
    slots_.at(slotNum)->setAddress(lug, rack_->getLugAddress(lug));
}

D192RackEditor::D192RackEditor(D192Rack *rack, QWidget *parent)
    : RackEditor(rack, parent), rack_(rack), table_(new QTableView(this)), model_(new D192RackModel(rack_, this)),
      mimic_(new D192RackMimic(rack_, this))
{
    auto *layout = new QHBoxLayout(this);

    layout->addWidget(table_);
    table_->setModel(model_);
    connect(model_, &D192RackModel::dataChanged, [this]()
    {
        Q_EMIT(dataChanged());
    });
    setModuleRowSpans();

//    layout->addWidget(mimic_, Qt::AlignCenter);
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
    for (unsigned int row = 0; row < rowCount; row += lugsPerModule) {
        table_->setSpan(
            row,
            static_cast<std::underlying_type_t<D192RackModel::Column>>(D192RackModel::Column::Module),
            lugsPerModule,
            1
        );
    }
}

} // patchman
