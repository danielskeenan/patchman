/**
 * @file SettingsDialog.cpp
 *
 * @author Dan Keenan
 * @date 1/7/24
 * @copyright GNU GPLv3
 */

#include "SettingsDialog.h"
#include "Settings.h"
#include "qiconFromTheme.h"
#include <ranges>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>

namespace patchman
{

SearchPathsWidget::SearchPathsWidget(QWidget *parent)
    : QWidget(parent), listWidget_(new QListWidget(this))
{
    auto *layout = new QVBoxLayout(this);

    // List
    layout->addWidget(listWidget_);
    listWidget_->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    listWidget_->setSelectionBehavior(QListWidget::SelectRows);
    listWidget_->setSelectionMode(QListWidget::ExtendedSelection);
    listWidget_->addItems(Settings::GetRomSearchPaths());
    connect(listWidget_, &QListWidget::itemSelectionChanged, this, &SearchPathsWidget::selectionChanged);

    // Add action
    actions_.addPath = new QAction(qiconFromTheme(ThemeIcon::ListAdd), tr("Add"), this);
    connect(actions_.addPath, &QAction::triggered, this, &SearchPathsWidget::addSearchPath);
    listWidget_->addAction(actions_.addPath);
    // Remove action
    actions_.removePath = new QAction(qiconFromTheme(ThemeIcon::ListRemove), tr("Remove"), this);
    connect(actions_.removePath, &QAction::triggered, this, &SearchPathsWidget::removeSearchPath);
    listWidget_->addAction(actions_.removePath);

    // Modification buttons (these are linked to the above actions, using their icons & labels, and tracking their
    // enabled state.)
    auto *buttonLayout = new QHBoxLayout;
    // Add button
    auto *addButton = new QPushButton(actions_.addPath->icon(), actions_.addPath->text(), this);
    connect(addButton, &QPushButton::clicked, actions_.addPath, &QAction::trigger);
    connect(actions_.addPath, &QAction::enabledChanged, addButton, &QPushButton::setEnabled);
    buttonLayout->addWidget(addButton);
    // Remove button
    auto *removeButton = new QPushButton(actions_.removePath->icon(), actions_.removePath->text(), this);
    connect(removeButton, &QPushButton::clicked, actions_.removePath, &QAction::trigger);
    connect(actions_.removePath, &QAction::enabledChanged, removeButton, &QPushButton::setEnabled);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);
}

QList<QString> SearchPathsWidget::getPaths() const
{
    QStringList paths;
    paths.reserve(listWidget_->count());
    for (int row = 0; row < listWidget_->count(); ++row) {
        const auto *item = listWidget_->item(row);
        paths.push_back(item->text());
    }

    return paths;
}

void SearchPathsWidget::selectionChanged()
{
    actions_.removePath->setEnabled(listWidget_->selectionModel()->hasSelection());
}

void SearchPathsWidget::addSearchPath()
{
    auto *fileDialog = new QFileDialog(this);
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->setDirectory(Settings::GetLastFileDialogPath());
    if (fileDialog->exec() == QFileDialog::Accepted) {
        listWidget_->addItems(fileDialog->selectedFiles());
    }
}

void SearchPathsWidget::removeSearchPath()
{
    if (!listWidget_->selectionModel()->hasSelection()) {
        return;
    }
    std::vector<int> selectedRows;
    for (const auto &index : listWidget_->selectionModel()->selectedIndexes()) {
        selectedRows.push_back(index.row());
    }
    std::sort(selectedRows.begin(), selectedRows.end());
    selectedRows.erase(std::unique(selectedRows.begin(), selectedRows.end()), selectedRows.end());
    for (const auto row : std::ranges::reverse_view(selectedRows)) {
        delete listWidget_->takeItem(row);
    }
}

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Settings"));
    resize(600, 320);

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout;
    layout->addLayout(form);

    // Search Paths
    widgets_.searchPaths = new SearchPathsWidget(this);
    form->addRow(tr("Search Paths"), widgets_.searchPaths);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
    layout->addWidget(buttonBox);
}

void SettingsDialog::accept()
{
    Settings::SetRomSearchPaths(widgets_.searchPaths->getPaths());

    QDialog::accept();
}

} // patchman
