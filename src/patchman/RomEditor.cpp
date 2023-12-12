/**
 * @file RomEditor.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include "RomEditor.h"
#include "RackEditor.h"
#include <QTabBar>
#include <source_location>
#include <QVBoxLayout>

namespace patchman
{

RomEditor::RomEditor(Rom *rom, QWidget *parent)
    : QWidget(parent), rom_(rom), tabs_(new QTabWidget(this))
{
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(tabs_);
    for (auto *rack: rom_->getRacksView()) {
        addTabForRack(rack);
    }
    connect(rom_, &Rom::rackAdded, this, &RomEditor::addTabForRack);
    connect(rom_, &Rom::rackRemoved, this, &RomEditor::rackRemoved);
}

void RomEditor::addTabForRack(Rack *rack)
{
    auto *page = new RackEditorContainer(rack, this);
    connect(page, &RackEditorContainer::dataChanged, this, &RomEditor::dataChanged);
    tabs_->addTab(page, rack->getRackName());
}

void RomEditor::rackRemoved(unsigned int rackNum)
{
    auto *page = tabs_->widget(rackNum);
    tabs_->removeTab(rackNum);
    page->deleteLater();
    for (int tabNum = 0; tabNum < tabs_->count(); ++tabNum) {
        const auto *rack = rom_->getRack(tabNum);
        Q_ASSERT_X(rack != nullptr,
                   std::source_location::current().function_name(),
                   "Rack out of bounds while updating tab bar.");
        tabs_->setTabText(tabNum, rack->getRackName());
    }
}

} // patchman
