/**
 * @file MainWindow.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include "MainWindow.h"
#include "Settings.h"
#include "patchlib/Exceptions.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>

namespace patchman
{

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initMenus();
    initWidgets();
}

void MainWindow::initMenus()
{
    // File menu
    QMenu *menuFile = menuBar()->addMenu(tr("&File"));
    // Open
    actions_.fileOpen = new QMenu(tr("&Open"), this);
    actions_.fileOpen->setIcon(QIcon::fromTheme("document-open"));
    for (const auto romType: Rom::allTypes()) {
        auto openAction = new QAction(Rom::typeName(romType), actions_.fileOpen);
        connect(openAction, &QAction::triggered, [romType, this]()
        {
            open(romType);
        });
        actions_.fileOpen->addAction(openAction);
    }
    menuFile->addMenu(actions_.fileOpen);
    // Recent
    actions_.fileRecent = new QMenu(tr("&Recent"), this);
    actions_.fileRecent->setIcon(QIcon::fromTheme("folder-recent"));
    menuFile->addMenu(actions_.fileRecent);
    // Save
    actions_.fileSave = new QAction(tr("&Save"), this);
    actions_.fileSave->setIcon(QIcon::fromTheme("document-save"));
    actions_.fileSave->setShortcut(QKeySequence::StandardKey::Save);
    connect(actions_.fileSave, &QAction::triggered, this, &MainWindow::save);
    menuFile->addAction(actions_.fileSave);
    // Save As
    actions_.fileSaveAs = new QAction(tr("Save &As"), this);
    actions_.fileSaveAs->setIcon(QIcon::fromTheme("document-save-as"));
    actions_.fileSaveAs->setShortcut(QKeySequence::StandardKey::SaveAs);
    connect(actions_.fileSaveAs, &QAction::triggered, this, &MainWindow::saveAs);
    menuFile->addAction(actions_.fileSaveAs);
    // Exit
    actions_.fileExit = new QAction(tr("E&xit"), this);
    actions_.fileExit->setIcon(QIcon::fromTheme("application-exit"));
    actions_.fileExit->setShortcut(QKeySequence::StandardKey::Quit);
    connect(actions_.fileExit, &QAction::triggered, this, &MainWindow::close);
    menuFile->addAction(actions_.fileExit);

    setSaveEnabled();
    updateRecentDocuments();
}

void MainWindow::initWidgets()
{
    if (!restoreGeometry(Settings::GetMainWindowGeometry())) {
        resize(1024, 768);
        Settings::SetMainWindowGeometry(saveGeometry());
    }
}

void MainWindow::saveTo(const QString &path)
{
    if (rom_ == nullptr) {
        return;
    }

    try {
        rom_->saveToFile(path);
        setWindowModified(false);
        setWindowFilePath(path);
    }
    catch (const UnrepresentableException &e) {
        QMessageBox::critical(this,
                              tr("Error saving ROM"),
                              tr("This patch cannot be saved in this format. Correct errors and try again."));
    }
    catch (const std::runtime_error &e) {
        QMessageBox::critical(this, tr("Error saving ROM"), tr("An unknown error occurred saving the file."));
    }
}

void MainWindow::openFrom(const QString &path, Rom::Type romType)
{
    try {
        auto newRom = Rom::create(romType, this);
        newRom->loadFromFile(path);

        if (editor_ != nullptr) {
            editor_->deleteLater();
        }
        if (rom_ != nullptr) {
            rom_->deleteLater();
        }

        rom_ = newRom;
        setWindowFilePath(path);
        setWindowModified(false);
        setSaveEnabled();

        editor_ = new RomEditor(rom_, this);
        connect(editor_, &RomEditor::dataChanged, [this]()
        { setWindowModified(true); });
        setCentralWidget(editor_);
        updateRecentDocuments();
    }
    catch (const InvalidRomException &e) {
        QMessageBox::critical(this,
                              tr("ROM could not be loaded."),
                              tr("Either the ROM file is corrupted or is of an unknown type."));
    }
    catch (const std::runtime_error &e) {
        QMessageBox::critical(this,
                              tr("ROM could not be loaded."),
                              tr("An unknown error occurred loading the file."));
    }
}

void MainWindow::setSaveEnabled()
{
    const auto saveEnabled = (rom_ != nullptr && !windowFilePath().isEmpty());

    actions_.fileSave->setEnabled(saveEnabled);
    actions_.fileSaveAs->setEnabled(saveEnabled);
}

void MainWindow::updateRecentDocuments()
{
    // Update the stored paths.
    auto recents = Settings::GetRecentDocuments();
    if (!windowFilePath().isEmpty()) {
        // Include the current path in the list.
        recents.prepend(RecentDocument(windowFilePath(), rom_->type()));
    }
    // Remove duplicates.
    QSet<QString> paths;
    paths.reserve(recents.size());
    for (qsizetype ix = 0; ix < recents.size();) {
        const auto path = recents.at(ix).getPath();
        if (!paths.contains(path)) {
            paths.insert(path);
            ++ix;
        }
        else {
            recents.remove(ix);
        }
    }

    if (recents.size() > Settings::GetRecentDocumentsMax()) {
        recents.remove(Settings::GetRecentDocumentsMax() - 1, recents.size() - Settings::GetRecentDocumentsMax());
    }
    Settings::SetRecentDocuments(recents);

    // Update the menu.
    actions_.fileRecent->clear();
    actions_.fileRecent->setEnabled(!recents.isEmpty());
    for (const auto &recent: recents) {
        auto recentAction = new QAction(recent.getPath(), actions_.fileRecent);
        connect(recentAction, &QAction::triggered, [this, recent]()
        {
            openFrom(recent.getPath(), recent.getRomType());
        });
        actions_.fileRecent->addAction(recentAction);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Settings::SetMainWindowGeometry(saveGeometry());
    Settings::Sync();
    QWidget::closeEvent(event);
}

void MainWindow::open(Rom::Type romType)
{
    auto *fileDialog = new QFileDialog(this);
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setDirectory(Settings::GetLastFileDialogPath());
    if (fileDialog->exec() == QFileDialog::Accepted) {
        const auto &selectedFiles = fileDialog->selectedFiles();
        const QString path = selectedFiles.front();
        openFrom(path, romType);
        QDir dir(path);
        dir.cdUp();
        Settings::SetLastFileDialogPath(dir.path());
    }
}

void MainWindow::save()
{
    if (rom_ == nullptr) {
        return;
    }

    if (windowFilePath().isEmpty()) {
        saveAs();
    }
    else {
        saveTo(windowFilePath());
    }
}

void MainWindow::saveAs()
{
    if (rom_ == nullptr) {
        return;
    }

    auto *fileDialog = new QFileDialog(this);
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setFileMode(QFileDialog::AnyFile);
    fileDialog->setDirectory(Settings::GetLastFileDialogPath());
    if (fileDialog->exec() == QFileDialog::Accepted) {
        const auto &selectedFiles = fileDialog->selectedFiles();
        const QString path = selectedFiles.front();
        saveTo(path);
        QDir dir(path);
        dir.cdUp();
        Settings::SetLastFileDialogPath(dir.path());
    }
}

} // patchman
