/**
 * @file AutoNumberDialog.cpp
 *
 * @author Dan Keenan
 * @date 12/11/23
 * @copyright GNU GPLv3
 */

#include <QFormLayout>
#include <QDialogButtonBox>
#include "AutoNumberDialog.h"

namespace patchman
{

AutoNumberDialog::AutoNumberDialog(QWidget *parent)
    : AutoNumberDialog(Options(), parent)
{}

AutoNumberDialog::AutoNumberDialog(const AutoNumberDialog::Options &options, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Autonumber"));
    auto layout = new QVBoxLayout(this);

    auto form = new QFormLayout;
    layout->addLayout(form);

    widgets_.start = new QSpinBox(this);
    widgets_.start->setMinimum(1);
    widgets_.start->setMaximum(512);
    widgets_.start->setValue(options.start);
    form->addRow(tr("Start"), widgets_.start);

    widgets_.increment = new QSpinBox(this);
    widgets_.increment->setMinimum(-512);
    widgets_.increment->setMaximum(512);
    widgets_.increment->setValue(options.increment);
    form->addRow(tr("Increment"), widgets_.increment);

    widgets_.offset = new QSpinBox(this);
    widgets_.offset->setMinimum(1);
    widgets_.offset->setMaximum(512);
    widgets_.offset->setValue(options.offset);
    form->addRow(tr("Offset"), widgets_.offset);

    auto buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        this
    );
    connect(buttons, &QDialogButtonBox::accepted, this, &AutoNumberDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &AutoNumberDialog::reject);
    layout->addWidget(buttons);
}

AutoNumberDialog::Options AutoNumberDialog::getOptions() const
{
    Options options;
    options.start = widgets_.start->value();
    options.increment = widgets_.increment->value();
    options.offset = widgets_.offset->value();

    return options;
}

} // patchman
