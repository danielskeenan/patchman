/**
 * @file RackEditor.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include <QVBoxLayout>
#include <QPushButton>
#include "RackEditor.h"
#include "d192/D192RackEditor.h"
#include "AutoNumberDialog.h"

namespace patchman
{

RackEditor *RackEditor::create(Rack *rack, QWidget *parent)
{
    switch (rack->getRackType()) {
        case Rack::Type::D192Rack:
            return new D192RackEditor(dynamic_cast<D192Rack *>(rack), parent);
        case Rack::Type::Enr96:
        case Rack::Type::Enr48:
        case Rack::Type::Enr24:
        case Rack::Type::Enr12:
            return nullptr;
    }
    Q_UNREACHABLE();
}

RackEditorContainer::RackEditorContainer(Rack *rack, QWidget *parent)
    : QWidget(parent), rack_(rack), editor_(RackEditor::create(rack_, this))
{
    connect(editor_, &RackEditor::dataChanged, this, &RackEditorContainer::dataChanged);

    auto *layout = new QVBoxLayout(this);

    auto *buttonsLayout = new QHBoxLayout;
    layout->addLayout(buttonsLayout);

    auto *autonumber = new QPushButton(tr("Autonumber"), this);
    connect(autonumber, &QPushButton::clicked, this, &RackEditorContainer::autonumber);
    buttonsLayout->addWidget(autonumber);

    auto *unpatch = new QPushButton(tr("Unpatch"), this);
    connect(unpatch, &QPushButton::clicked, this, &RackEditorContainer::unpatch);
    buttonsLayout->addWidget(unpatch);
    buttonsLayout->addStretch();

    layout->addWidget(editor_);
}

void RackEditorContainer::autonumber()
{
    const auto circuits = editor_->getSelectedCircuits();
    AutoNumberDialog dialog(this);
    if (dialog.exec() != AutoNumberDialog::Accepted) {
        return;
    }
    const auto options = dialog.getOptions();
    unsigned int nextAddress = options.start;
    for (auto circuitIt = circuits.begin(); circuitIt != circuits.end(); circuitIt += options.offset) {
        const auto circuit = *circuitIt;
        const auto lug = rack_->getLugForCircuit(circuit);
        rack_->setLugAddress(lug, nextAddress);
        nextAddress += options.increment;
    }
}

void RackEditorContainer::unpatch()
{
    const auto circuits = editor_->getSelectedCircuits();
    for (const auto circuit: circuits) {
        const auto lug = rack_->getLugForCircuit(circuit);
        rack_->setLugAddress(lug, 0);
    }
}

} // patchman
