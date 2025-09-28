/**
 * @file EditorWindow.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include "EditorWindow.h"
#include "Settings.h"
#include "patchlib/Exceptions.h"
#include "util.h"
#include "ReportBuilder.h"
#include "AboutDialog.h"
#include "patchman_config.h"
#include "help.h"
#include "updater.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QStatusBar>
#include <QDesktopServices>

namespace patchman
{

EditorWindow::EditorWindow(Rom *rom, const QString &path, QWidget *parent)
    : QMainWindow(parent), rom_(rom)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setWindowFilePath(path);
    rom_->setParent(this);
    initMenus();
    initWidgets();
}

void EditorWindow::initMenus()
{
    // File menu
    QMenu *menuFile = menuBar()->addMenu(tr("&File"));
    // Save
    actions_.fileSave = new QAction(tr("&Save"), this);
    actions_.fileSave->setIcon(QIcon::fromTheme("document-save"));
    actions_.fileSave->setShortcut(QKeySequence::StandardKey::Save);
    connect(actions_.fileSave, &QAction::triggered, this, &EditorWindow::save);
    menuFile->addAction(actions_.fileSave);
    // Save As
    actions_.fileSaveAs = new QAction(tr("Save &As"), this);
    actions_.fileSaveAs->setIcon(QIcon::fromTheme("document-save-as"));
    actions_.fileSaveAs->setShortcut(QKeySequence::StandardKey::SaveAs);
    connect(actions_.fileSaveAs, &QAction::triggered, this, &EditorWindow::saveAs);
    menuFile->addAction(actions_.fileSaveAs);
    // Create Report
    actions_.fileCreateReport = new QAction(tr("&Create Report"), this);
    actions_.fileCreateReport->setIcon(QIcon::fromTheme("office-report"));
    connect(actions_.fileCreateReport, &QAction::triggered, this, &EditorWindow::createReport);
    menuFile->addAction(actions_.fileCreateReport);
    // Exit
    actions_.fileClose = new QAction(tr("E&xit"), this);
    actions_.fileClose->setIcon(QIcon::fromTheme("application-exit"));
    actions_.fileClose->setShortcut(QKeySequence::StandardKey::Close);
    connect(actions_.fileClose, &QAction::triggered, this, &EditorWindow::close);
    menuFile->addAction(actions_.fileClose);

    // Help menu
    QMenu *menuHelp = menuBar()->addMenu(tr("&Help"));
    // Help
    actions_.helpHelp = new QAction(tr("&Help"), this);
    actions_.helpHelp->setIcon(QIcon::fromTheme("help-contents"));
    connect(actions_.helpHelp, &QAction::triggered, this, &EditorWindow::help);
    menuHelp->addAction(actions_.helpHelp);
    // About
    actions_.helpAbout = new QAction(tr("&About"), this);
    actions_.helpAbout->setIcon(QIcon::fromTheme("help-about"));
    connect(actions_.helpAbout, &QAction::triggered, this, &EditorWindow::about);
    menuHelp->addAction(actions_.helpAbout);
    // Homepage
    actions_.helpHomepage = new QAction(tr("&Homepage"), this);
    actions_.helpHomepage->setIcon(QIcon::fromTheme("internet-web-browser"));
    connect(actions_.helpHomepage, &QAction::triggered, this, &EditorWindow::homepage);
    menuHelp->addAction(actions_.helpHomepage);
    // Check for updates
    actions_.helpUpdate = new QAction(tr("Check for &Updates"), this);
    actions_.helpUpdate->setIcon(QIcon::fromTheme("download"));
    connect(actions_.helpUpdate, &QAction::triggered, &checkForUpdates);
    menuHelp->addAction(actions_.helpUpdate);

    setSaveEnabled();
}

void EditorWindow::initWidgets()
{
    resize(1024, 768);

    widgets.romTitle = new QLabel(this);
    statusBar()->addPermanentWidget(widgets.romTitle);

    widgets.patchedRacksCount = new QLabel(this);
    statusBar()->addPermanentWidget(widgets.patchedRacksCount);

    widgets.checksum = new QLabel(this);
    statusBar()->addPermanentWidget(widgets.checksum);

    editor_ = new RomEditor(rom_, this);
    connect(editor_, &RomEditor::dataChanged, this, &EditorWindow::dataChanged);
    connect(rom_, &Rom::titleChanged, this, &EditorWindow::romTitleChanged);
    setCentralWidget(editor_);
    romTitleChanged();
    updatePatchedRacksCount();
    updateChecksum();
}

void EditorWindow::saveTo(const QString &path)
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

void EditorWindow::setSaveEnabled()
{
    const auto saveEnabled = (rom_ != nullptr);

    actions_.fileSave->setEnabled(saveEnabled);
    actions_.fileSaveAs->setEnabled(saveEnabled);
    actions_.fileCreateReport->setEnabled(saveEnabled);
}

bool EditorWindow::maybeSave()
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

void EditorWindow::updatePatchedRacksCount()
{
    if (rom_ == nullptr) {
        widgets.patchedRacksCount->clear();
    }
    else {
        const auto patchedRackCount = rom_->countPatchedRacks();
        widgets.patchedRacksCount->setText(tr("%n rack(s) patched", "", patchedRackCount));
    }
}

void EditorWindow::updateChecksum()
{
    if (rom_ == nullptr) {
        widgets.checksum->clear();
    }
    else {
        const auto checksum = rom_->getChecksum();
        widgets.checksum->setText(tr("Checksum: %1").arg(checksum.toHex()));
    }
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
    if (!maybeSave()) {
        event->ignore();
        return;
    }
    Settings::Sync();
    QWidget::closeEvent(event);
}

void EditorWindow::save()
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

void EditorWindow::saveAs()
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

void EditorWindow::createReport()
{
    if (rom_ == nullptr) {
        return;
    }

    ReportBuilder *builder = ReportBuilder::create(rom_, this);
    const auto reportPath = builder->createReport();
    QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(reportPath), QUrl::TolerantMode));
}

void EditorWindow::help()
{
    showHelp();
}

void EditorWindow::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void EditorWindow::homepage()
{
    QDesktopServices::openUrl(QUrl(config::kProjectHomepageUrl));
}

void EditorWindow::dataChanged()
{
    setWindowModified(true);
    updatePatchedRacksCount();
    updateChecksum();
}

void EditorWindow::romTitleChanged()
{
    widgets.romTitle->setText(rom_->getTitle());
}

} // patchman
