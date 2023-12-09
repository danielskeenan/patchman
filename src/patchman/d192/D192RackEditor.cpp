/**
 * @file D192RackEditor.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include <QHBoxLayout>
#include "D192RackEditor.h"

namespace patchman
{
D192RackEditor::D192RackEditor(D192Rack *rack, QWidget *parent)
    : RackEditor(rack, parent), rack_(rack), table_(new QTableView(this)), model_(new D192RackModel(rack_, this))
{
    auto *layout = new QHBoxLayout(this);
    layout->addWidget(table_);
    table_->setModel(model_);
}

} // patchman
