/**
 * @file BrowserWindow.cpp
 *
 * @author Dan Keenan
 * @date 1/7/24
 * @copyright GNU GPLv3
 */

#include "BrowserWindow.h"
#include "EditorWindow.h"
#include "patchlib/Exceptions.h"
#include "util.h"
#include "Settings.h"
#include "AboutDialog.h"
#include "patchman_config.h"
#include "SettingsDialog.h"
#include "ReportBuilder.h"
#include "showPathInFileBrowser.h"
#include "ShowDuplicatesDialog.h"
#include "patchlib/library/RomLibrary.h"
#include "help.h"
#include "updater.h"
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QCloseEvent>
#include <QStatusBar>
#include <QClipboard>

#include "qiconFromTheme.h"

namespace patchman
{

BrowserWindow::BrowserWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initMenus();
    initWidgets();
    initFsWatcher();

    if (Settings::GetRomSearchPaths().empty()) {
        QMessageBox::information(this,
                                 tr("No ROM paths set."),
                                 tr("Set ROM search paths in the Settings before using this program."));
        settings();
    }
}

void BrowserWindow::initMenus()
{
    // File menu
    QMenu *menuFile = menuBar()->addMenu(tr("&File"));
    // New
    actions_.fileNew = new QMenu(tr("&New"), this);
    actions_.fileNew->setIcon(qiconFromTheme(ThemeIcon::DocumentNew));
    for (const auto romType : Rom::allTypes()) {
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
    actions_.fileOpen->setIcon(qiconFromTheme(ThemeIcon::DocumentOpen));
    actions_.fileOpen->setShortcut(QKeySequence::StandardKey::Open);
    connect(actions_.fileOpen, &QAction::triggered, this, &BrowserWindow::open);
    menuFile->addAction(actions_.fileOpen);
    // Recent
    actions_.fileRecent = new QMenu(tr("&Recent"), this);
    actions_.fileRecent->setIcon(qiconFromTheme(ThemeIcon::DocumentOpenRecent));
    menuFile->addMenu(actions_.fileRecent);
    // Create Report
    actions_.fileCreateReport = new QAction(tr("&Create Report"), this);
    actions_.fileCreateReport->setIcon(QIcon::fromTheme("office-report"));
    connect(actions_.fileCreateReport, &QAction::triggered, this, &BrowserWindow::createReport);
    menuFile->addAction(actions_.fileCreateReport);
    // ------
    menuFile->addSeparator();
    // Settings
    actions_.fileSettings = new QAction(tr("Se&ttings"), this);
    actions_.fileSettings->setIcon(QIcon::fromTheme("configure"));
    connect(actions_.fileSettings, &QAction::triggered, this, &BrowserWindow::settings);
    menuFile->addAction(actions_.fileSettings);
    // ------
    menuFile->addSeparator();
    // Exit
    actions_.fileExit = new QAction(tr("E&xit"), this);
    actions_.fileExit->setIcon(qiconFromTheme(ThemeIcon::ApplicationExit));
    actions_.fileExit->setShortcut(QKeySequence::StandardKey::Quit);
    connect(actions_.fileExit, &QAction::triggered, this, &BrowserWindow::close);
    menuFile->addAction(actions_.fileExit);

    // Edit menu
    QMenu *menuEdit = menuBar()->addMenu(tr("&Edit"));
    // Edit ROM
    actions_.editEditRom = new QAction(tr("&Edit ROM"), this);
    actions_.editEditRom->setIcon(QIcon::fromTheme("document-edit"));
    connect(actions_.editEditRom, &QAction::triggered, this, &BrowserWindow::editRom);
    menuEdit->addAction(actions_.editEditRom);
    // Show Duplicates
    actions_.editShowDuplicates = new QAction(tr("Show &Duplicates"), this);
    actions_.editShowDuplicates->setIcon(QIcon::fromTheme("document-duplicate"));
    connect(actions_.editShowDuplicates, &QAction::triggered, this, &BrowserWindow::showDuplicates);
    menuEdit->addAction(actions_.editShowDuplicates);
    // Show In File Browser
    actions_.editShowInFileBrowser = new QAction(tr("&Show in Explorer"), this);
    actions_.editShowInFileBrowser->setIcon(QIcon::fromTheme("system-file-manager"));
    connect(actions_.editShowInFileBrowser, &QAction::triggered, this, &BrowserWindow::showInFileBrowser);
    menuEdit->addAction(actions_.editShowInFileBrowser);
    // Copy Checksum
    actions_.editCopyChecksum = new QAction(tr("&Copy Checksum"), this);
    actions_.editCopyChecksum->setIcon(QIcon::fromTheme("checksum"));
    connect(actions_.editCopyChecksum, &QAction::triggered, this, &BrowserWindow::copyChecksum);
    menuEdit->addAction(actions_.editCopyChecksum);

    // Help menu
    QMenu *menuHelp = menuBar()->addMenu(tr("&Help"));
    // Help
    actions_.helpHelp = new QAction(tr("&Help"), this);
    actions_.helpHelp->setIcon(qiconFromTheme(ThemeIcon::HelpFaq));
    connect(actions_.helpHelp, &QAction::triggered, this, &BrowserWindow::help);
    menuHelp->addAction(actions_.helpHelp);
    // About
    actions_.helpAbout = new QAction(tr("&About"), this);
    actions_.helpAbout->setIcon(qiconFromTheme(ThemeIcon::HelpAbout));
    connect(actions_.helpAbout, &QAction::triggered, this, &BrowserWindow::about);
    menuHelp->addAction(actions_.helpAbout);
    // Homepage
    actions_.helpHomepage = new QAction(tr("&Homepage"), this);
    actions_.helpHomepage->setIcon(QIcon::fromTheme("internet-web-browser"));
    connect(actions_.helpHomepage, &QAction::triggered, this, &BrowserWindow::homepage);
    menuHelp->addAction(actions_.helpHomepage);
    // Check for updates
    actions_.helpUpdate = new QAction(tr("Check for &Updates"), this);
    actions_.helpUpdate->setIcon(qiconFromTheme(ThemeIcon::SoftwareUpdateAvailable));
    connect(actions_.helpUpdate, &QAction::triggered, &checkForUpdates);
    menuHelp->addAction(actions_.helpUpdate);

    updateRecentDocuments();
}

void BrowserWindow::initWidgets()
{
    if (!restoreGeometry(Settings::GetMainWindowGeometry())) {
        resize(1024, 768);
        Settings::SetMainWindowGeometry(saveGeometry());
    }

    // Browser
    widgets_.browser = new QTableView(this);
    setCentralWidget(widgets_.browser);
    browserModel_ = new RomLibraryModel(this);
    widgets_.browser->setSelectionMode(QTableView::SingleSelection);
    widgets_.browser->setSelectionBehavior(QTableView::SelectRows);
    widgets_.browser->setContextMenuPolicy(Qt::ActionsContextMenu);

    // ROM context menu actions
    widgets_.browser->addAction(actions_.editEditRom);
    widgets_.browser->addAction(actions_.fileCreateReport);
    widgets_.browser->addAction(actions_.editShowDuplicates);
    widgets_.browser->addAction(actions_.editShowInFileBrowser);
    widgets_.browser->addAction(actions_.editCopyChecksum);

    browserModel_->checkForFilesystemChanges();
    sortFilterModel_ = new RomLibrarySortFilterModel(browserModel_, this);
    widgets_.browser->setSortingEnabled(true);
    widgets_.browser->setModel(sortFilterModel_);
    widgets_.browser->hideColumn(static_cast<int>(RomLibraryModel::Column::PatchHash));
    connect(widgets_.browser->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &BrowserWindow::updateActionsFromSelection);
    connect(widgets_.browser, &QTableView::doubleClicked, this, &BrowserWindow::editRom);
    // Resize everything to fit on first data load.
    connect(browserModel_, &RomLibraryModel::modelReset, this, [this]()
    { widgets_.browser->resizeColumnsToContents(); }, Qt::SingleShotConnection);

    // Progress message
    widgets_.progressMsg = new QLabel(this);
    statusBar()->addPermanentWidget(widgets_.progressMsg);
    connect(browserModel_, &RomLibraryModel::progressTextChanged, widgets_.progressMsg, &QLabel::setText);

    // Progress bar
    widgets_.progress = new QProgressBar(this);
    statusBar()->addPermanentWidget(widgets_.progress);
    connect(browserModel_, &RomLibraryModel::progressRangeChanged, this, &BrowserWindow::progressRangeChanged);
    connect(browserModel_, &RomLibraryModel::progressValueChanged, this, &BrowserWindow::progressValueChanged);

    updateActionsFromSelection();
}

void BrowserWindow::initFsWatcher()
{
    fsWatcher_ = new QFileSystemWatcher(this);
    for (const auto &searchPath : Settings::GetRomSearchPaths()) {
        watchPath(searchPath);
    }
    connect(fsWatcher_, &QFileSystemWatcher::directoryChanged, this, &BrowserWindow::directoryChanged);
}

void BrowserWindow::openFrom(const QString &path)
{
    try {
        auto romType = Rom::guessType(path);
        auto rom = Rom::create(romType, this);
        rom->loadFromFile(path);
        showEditor(rom, path);
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

void BrowserWindow::updateRecentDocuments(const QString &path)
{
    // Update the stored paths.
    auto recents = Settings::GetRecentDocuments();
    if (!path.isEmpty()) {
        // Include the current path in the list.
        recents.prepend(path);
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
    for (const auto &recent : recents) {
        auto recentAction = new QAction(recent, actions_.fileRecent);
        connect(recentAction, &QAction::triggered, [this, recent]()
        {
            openFrom(recent);
        });
        actions_.fileRecent->addAction(recentAction);
    }
}

void BrowserWindow::updateActionsFromSelection()
{
    const bool hasSelection = widgets_.browser->selectionModel()->hasSelection();
    actions_.fileCreateReport->setEnabled(hasSelection);
    actions_.editEditRom->setEnabled(hasSelection);
    actions_.editShowDuplicates->setEnabled(hasSelection);
    actions_.editShowInFileBrowser->setEnabled(hasSelection);
    actions_.editCopyChecksum->setEnabled(hasSelection);
}

void BrowserWindow::showEditor(Rom *rom, const QString &path)
{
    auto *editor = new EditorWindow(rom, path, this);
    editors_.emplace_back(editor);
    connect(editor, &EditorWindow::destroyed, this, &BrowserWindow::editorClosed, Qt::QueuedConnection);
    editor->show();
}

const RomInfo &BrowserWindow::getSelectedRomInfo() const
{
    const auto selection = widgets_.browser->selectionModel()->selectedIndexes();
    if (selection.empty()) {
        qFatal("Tried to get selected ROM info when no ROM is selected.");
    }
    const auto row = sortFilterModel_->mapToSource(selection.first()).row();
    return browserModel_->getRomInfoForRow(row);
}

void BrowserWindow::watchPath(const QString &path)
{
    for (QDirIterator it(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories); it.hasNext();) {
        const auto &dir = it.next();
        fsWatcher_->addPath(dir);
    }
}

void BrowserWindow::hideProgressIfComplete()
{
    const bool complete = (widgets_.progress->value() >= widgets_.progress->maximum());
    widgets_.progress->setHidden(complete);
    widgets_.progressMsg->setHidden(complete);
}

void BrowserWindow::closeEvent(QCloseEvent *event)
{
    Settings::SetMainWindowGeometry(saveGeometry());
    Settings::Sync();

    // Close open editors.
    for (auto &editor : editors_) {
        if (!editor.isNull() && !editor->close()) {
            // Editor wants to stay open, so don't quit.
            event->ignore();
            return;
        }
    }
    QFutureWatcher<void> futureWatcher;
    auto result = RomLibrary::get()->cleanup();
    futureWatcher.setFuture(result);
    futureWatcher.waitForFinished();

    QWidget::closeEvent(event);
}

void BrowserWindow::newFile(Rom::Type romType)
{
    auto *rom = Rom::create(romType, this);
    showEditor(rom);
}

void BrowserWindow::open()
{
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

void BrowserWindow::createReport()
{
    const auto romInfo = getSelectedRomInfo();
    auto romType = Rom::guessType(romInfo.getFilePath());
    auto rom = Rom::create(romType, this);
    rom->loadFromFile(romInfo.getFilePath());
    ReportBuilder *builder = ReportBuilder::create(rom, this);
    const auto reportPath = builder->createReport();
    QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(reportPath), QUrl::TolerantMode));
}

void BrowserWindow::settings()
{
    SettingsDialog dialog(this);
    dialog.exec();
    initFsWatcher();
    browserModel_->checkForFilesystemChanges();
}

void BrowserWindow::editRom()
{
    const auto romInfo = getSelectedRomInfo();
    openFrom(romInfo.getFilePath());
}

void BrowserWindow::showInFileBrowser()
{
    const auto romInfo = getSelectedRomInfo();
    showPathInFileBrowser(romInfo.getFilePath());
}

void BrowserWindow::showDuplicates()
{
    const auto romInfo = getSelectedRomInfo();
    ShowDuplicatesDialog dialog(romInfo, this);
    dialog.exec();
}

void BrowserWindow::copyChecksum()
{
    const auto romInfo = getSelectedRomInfo();
    const QString checksumString(romInfo.getRomChecksum().toHex());
    auto *clipboard = qApp->clipboard();
    clipboard->setText(checksumString);
}

void BrowserWindow::help()
{
    showHelp();
}

void BrowserWindow::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void BrowserWindow::homepage()
{
    QDesktopServices::openUrl(QUrl(config::kProjectHomepageUrl));
}

void BrowserWindow::editorClosed()
{
    editors_.removeIf(
        [](const QPointer<EditorWindow> &editor)
        {
            return editor.isNull();
        }
    );
}

void BrowserWindow::directoryChanged(const QString &path)
{
    if (!QDir(path).exists()) {
        fsWatcher_->removePath(path);
    }
    else {
        // Ensure any newly-added subdirectories are monitored.
        watchPath(path);
    }
    browserModel_->checkForFilesystemChanges();
}

void BrowserWindow::progressRangeChanged(int min, int max)
{
    widgets_.progress->setRange(min, max);
    hideProgressIfComplete();
}

void BrowserWindow::progressValueChanged(int value)
{
    widgets_.progress->setValue(value);
    hideProgressIfComplete();
}

} // patchman
