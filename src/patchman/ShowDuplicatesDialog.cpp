/**
 * @file ShowDuplicatesDialog.cpp
 *
 * @author Dan Keenan
 * @date 1/7/24
 * @copyright GNU GPLv3
 */

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include "ShowDuplicatesDialog.h"

#include "patchlib/library/RomLibrary.h"
#include "qiconFromTheme.h"
#include "showPathInFileBrowser.h"

namespace patchman
{

ShowDuplicatesDialog::ShowDuplicatesDialog(const RomInfo &romInfo, QWidget *parent)
    : QDialog(parent), romInfo_(romInfo)
{
    actions_.showInFileBrowser = new QAction(QIcon::fromTheme("system-file-manager"), tr("&Show in Explorer"), this);
    connect(actions_.showInFileBrowser, &QAction::triggered, this, &ShowDuplicatesDialog::showInFileBrowser);

    resize(600, 320);

    auto *layout = new QVBoxLayout(this);

    // Title
    auto *titleLabel = new QLabel(tr("Duplicates of %1:").arg(romInfo.getFilePath()));
    titleLabel->setWordWrap(true);
    setWindowTitle(tr("Duplicates of %1").arg(romInfo.getFilePath()));

    // List
    fileList_ = new QListWidget(this);
    fileList_->setContextMenuPolicy(Qt::ActionsContextMenu);
    fileList_->addAction(actions_.showInFileBrowser);
    connect(fileList_, &QListWidget::itemSelectionChanged, this, &ShowDuplicatesDialog::updateActionsFromSelection);
    layout->addWidget(fileList_);

    // Buttons
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    auto refreshButton = new QPushButton(qiconFromTheme(ThemeIcon::ViewRefresh), tr("Refresh"), this);
    connect(refreshButton, &QPushButton::clicked, this, &ShowDuplicatesDialog::refreshDuplicates);
    buttonBox->addButton(refreshButton, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ShowDuplicatesDialog::close);
    layout->addWidget(buttonBox);

    refreshDuplicates();
    updateActionsFromSelection();
}

QString ShowDuplicatesDialog::getSelectedPath() const
{
    const auto selection = fileList_->selectedItems();
    if (selection.empty()) {
        qFatal("Tried to get selected duplicate when nothing is selected.");
    }
    return selection.first()->text();
}

void ShowDuplicatesDialog::refreshDuplicates()
{
    RomLibrary::get()->getDuplicates(romInfo_)
        .then(
            [this](const QList<RomInfo> &duplicates)
            {
                fileList_->clear();
                for (const auto &duplicate : duplicates) {
                    if (duplicate == romInfo_) {
                        continue;
                    }
                    fileList_->addItem(duplicate.getFilePath());
                }
            }
        );
}

void ShowDuplicatesDialog::showInFileBrowser()
{
    const auto path = getSelectedPath();
    showPathInFileBrowser(path);
}

void ShowDuplicatesDialog::updateActionsFromSelection()
{
    const bool hasSelection = fileList_->selectionModel()->hasSelection();
    actions_.showInFileBrowser->setEnabled(hasSelection);
}

} // patchman
