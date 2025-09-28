/**
 * @file RackEditor.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include "RackEditor.h"
#include "AutoNumberDialog.h"
#include "d192/D192RackEditor.h"
#include "enr/EnrRackEditor.h"
#include <QPushButton>
#include <QVBoxLayout>

namespace patchman {

RackEditor *RackEditor::create(Rack *rack, QWidget *parent)
{
    switch (rack->getRackType()) {
    case Rack::Type::D192Rack:
        return new D192RackEditor(dynamic_cast<D192Rack *>(rack), parent);
    case Rack::Type::Enr96:
        return new EnrRackEditor(dynamic_cast<EnrRack *>(rack), parent);
    }
    Q_UNREACHABLE();
}

RackEditor::RackEditor(Rack *rack, QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

RackEditorContainer::RackEditorContainer(Rack *rack, QWidget *parent)
    : QWidget(parent)
    , rack_(rack)
    , editor_(RackEditor::create(rack_, this))
    , preview_(RackPreview::create(rack, this))
{
    connect(editor_, &RackEditor::dataChanged, this, &RackEditorContainer::dataChanged);

    auto *layout = new QHBoxLayout(this);

    auto *editorLayout = new QVBoxLayout;
    layout->addLayout(editorLayout);

    auto *buttonsLayout = new QHBoxLayout;
    editorLayout->addLayout(buttonsLayout);

    auto *autonumber = new QPushButton(tr("Autonumber"), this);
    connect(autonumber, &QPushButton::clicked, this, &RackEditorContainer::autonumber);
    buttonsLayout->addWidget(autonumber);

    auto *unpatch = new QPushButton(tr("Unpatch"), this);
    connect(unpatch, &QPushButton::clicked, this, &RackEditorContainer::unpatch);
    buttonsLayout->addWidget(unpatch);
    buttonsLayout->addStretch();

    // Editor
    editorLayout->addWidget(editor_);

    // Preview
    if (preview_ != nullptr) {
        layout->addWidget(preview_);
        connect(
            editor_,
            &RackEditor::selectionChanged,
            this,
            &RackEditorContainer::tableSelectionChanged);
        connect(
            preview_,
            &RackPreview::selectionChanged,
            this,
            &RackEditorContainer::previewSelectionChanged);
    }
}

void RackEditorContainer::autonumber()
{
    auto circuits = editor_->getSelectedCircuits();
    if (circuits.empty()) {
        circuits.resize(rack_->getLugCount());
        std::iota(circuits.begin(), circuits.end(), 0);
    }
    AutoNumberDialog dialog(this);
    if (dialog.exec() != AutoNumberDialog::Accepted) {
        return;
    }
    const auto options = dialog.getOptions();
    unsigned int nextAddress = options.start;
    for (auto circuitIt = circuits.begin(); circuitIt != circuits.end();
         circuitIt += options.offset) {
        const auto circuit = *circuitIt;
        const auto lug = rack_->getLugForCircuit(circuit);
        rack_->setLugAddress(lug, nextAddress);
        nextAddress += options.increment;
    }
}

void RackEditorContainer::unpatch()
{
    auto circuits = editor_->getSelectedCircuits();
    if (circuits.empty()) {
        circuits.resize(rack_->getLugCount());
        std::iota(circuits.begin(), circuits.end(), 0);
    }
    for (const auto circuit : circuits) {
        const auto lug = rack_->getLugForCircuit(circuit);
        rack_->setLugAddress(lug, 0);
    }
}

void RackEditorContainer::tableSelectionChanged()
{
    preview_->selectCircuits(editor_->getSelectedCircuits());
}

void RackEditorContainer::previewSelectionChanged()
{
    editor_->selectCircuits(preview_->getSelectedCircuits());
}

} // namespace patchman
