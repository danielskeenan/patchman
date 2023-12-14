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
#include "util.h"
#include "ReportBuilder.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QStatusBar>
#include <QDesktopServices>

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
    // New
    actions_.fileNew = new QMenu(tr("&New"), this);
    actions_.fileNew->setIcon(QIcon::fromTheme("document-new"));
    for (const auto romType: Rom::allTypes()) {
        auto newAction = new QAction(Rom::typeName(romType), actions_.fileNew);
        connect(newAction, &QAction::triggered, [romType, this]()
        {
            newFile(romType);
        });
        actions_.fileNew->addAction(newAction);
    }
    menuFile->addMenu(actions_.fileNew);
    // Open
    actions_.fileOpen = new QAction(tr("&Open"), this);
    actions_.fileOpen->setIcon(QIcon::fromTheme("document-open"));
    actions_.fileOpen->setShortcut(QKeySequence::StandardKey::Open);
    connect(actions_.fileOpen, &QAction::triggered, this, &MainWindow::open);
    menuFile->addAction(actions_.fileOpen);
    // Recent
    actions_.fileRecent = new QMenu(tr("&Recent"), this);
    actions_.fileRecent->setIcon(QIcon::fromTheme("folder-open-recent"));
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
    // Create Report
    actions_.fileCreateReport = new QAction(tr("Create &Report"), this);
    actions_.fileCreateReport->setIcon(QIcon::fromTheme("office-report"));
    connect(actions_.fileCreateReport, &QAction::triggered, this, &MainWindow::createReport);
    menuFile->addAction(actions_.fileCreateReport);
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

    widgets.romTitle = new QLabel(this);
    statusBar()->addPermanentWidget(widgets.romTitle);

    widgets.patchedRacksCount = new QLabel(this);
    statusBar()->addPermanentWidget(widgets.patchedRacksCount);
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
        showExceptionMessageBox(e);
    }
    catch (const std::runtime_error &e) {
        QMessageBox::critical(this, tr("Error saving ROM"), tr("An unknown error occurred saving the file."));
    }
}

void MainWindow::openFrom(const QString &path)
{
    try {
        auto romType = Rom::guessType(path);
        auto newRom = Rom::create(romType, this);
        newRom->loadFromFile(path);
        setWindowFilePath(path);
        setWindowModified(false);
        replaceOpenRom(newRom);
        setSaveEnabled();
    }
    catch (const InvalidRomException &e) {
        showExceptionMessageBox(e);
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
    actions_.fileCreateReport->setEnabled(saveEnabled);
}

void MainWindow::updateRecentDocuments()
{
    // Update the stored paths.
    auto recents = Settings::GetRecentDocuments();
    if (!windowFilePath().isEmpty()) {
        // Include the current path in the list.
        recents.prepend(windowFilePath());
    }
    // Remove duplicates.
    recents.removeDuplicates();

    if (recents.size() > Settings::GetRecentDocumentsMax()) {
        recents.resize(Settings::GetRecentDocumentsMax());
    }
    Settings::SetRecentDocuments(recents);

    // Update the menu.
    actions_.fileRecent->clear();
    actions_.fileRecent->setEnabled(!recents.isEmpty());
    for (const auto &recent: recents) {
        auto recentAction = new QAction(recent, actions_.fileRecent);
        connect(recentAction, &QAction::triggered, [this, recent]()
        {
            openFrom(recent);
        });
        actions_.fileRecent->addAction(recentAction);
    }
}

void MainWindow::replaceOpenRom(Rom *newRom)
{
    if (editor_ != nullptr) {
        editor_->deleteLater();
    }
    if (rom_ != nullptr) {
        rom_->deleteLater();
    }

    rom_ = newRom;
    editor_ = new RomEditor(rom_, this);
    romTitleChanged();
    connect(editor_, &RomEditor::dataChanged, this, &MainWindow::dataChanged);
    connect(rom_, &Rom::titleChanged, this, &MainWindow::romTitleChanged);
    setCentralWidget(editor_);
    updateRecentDocuments();
    updatePatchedRacksCount();
}

bool MainWindow::maybeSave()
{
    if (!isWindowModified()) {
        return true;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Data modified"));
    msgBox.setText(tr("The file has been modified. Do you want to save?"));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    const auto result = msgBox.exec();

    if (result == QMessageBox::Cancel) {
        return false;
    }
    else {
        if (result == QMessageBox::Save) {
            save();
        }
        return true;
    }
}

void MainWindow::updatePatchedRacksCount()
{
    if (rom_ == nullptr) {
        widgets.patchedRacksCount->setText("");
    }
    else {
        const auto patchedRackCount = rom_->countPatchedRacks();
        widgets.patchedRacksCount->setText(tr("%n rack(s) patched", "", patchedRackCount));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!maybeSave()) {
        event->ignore();
        return;
    }
    Settings::SetMainWindowGeometry(saveGeometry());
    Settings::Sync();
    QWidget::closeEvent(event);
}

void MainWindow::newFile(Rom::Type romType)
{
    if (!maybeSave()) {
        return;
    }
    auto *newRom = Rom::create(romType, this);
    replaceOpenRom(newRom);
    setWindowFilePath("");
    setWindowModified(false);
    setSaveEnabled();
}

void MainWindow::open()
{
    if (!maybeSave()) {
        return;
    }
    auto *fileDialog = new QFileDialog(this);
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setDirectory(Settings::GetLastFileDialogPath());
    if (fileDialog->exec() == QFileDialog::Accepted) {
        const auto &selectedFiles = fileDialog->selectedFiles();
        const QString path = selectedFiles.front();
        openFrom(path);
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

void MainWindow::createReport()
{
    if (rom_ == nullptr) {
        return;
    }

    ReportBuilder *builder = ReportBuilder::create(rom_, this);
    const auto reportPath = builder->createReport();
    QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(reportPath), QUrl::TolerantMode));
}

void MainWindow::dataChanged()
{
    setWindowModified(true);
    updatePatchedRacksCount();
}

void MainWindow::romTitleChanged()
{
    widgets.romTitle->setText(rom_->getTitle());
}

} // patchman
