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
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QCloseEvent>

namespace patchman
{

BrowserWindow::BrowserWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initMenus();
    initWidgets();
}

void BrowserWindow::initMenus()
{
    // File menu
    QMenu *menuFile = menuBar()->addMenu(tr("&File"));
    // New
    actions_.fileNew = new QMenu(tr("&New"), this);
    actions_.fileNew->setIcon(QIcon::fromTheme("document-new"));
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
    actions_.fileOpen->setIcon(QIcon::fromTheme("document-open"));
    actions_.fileOpen->setShortcut(QKeySequence::StandardKey::Open);
    connect(actions_.fileOpen, &QAction::triggered, this, &BrowserWindow::open);
    menuFile->addAction(actions_.fileOpen);
    // Recent
    actions_.fileRecent = new QMenu(tr("&Recent"), this);
    actions_.fileRecent->setIcon(QIcon::fromTheme("folder-open-recent"));
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
    actions_.fileExit->setIcon(QIcon::fromTheme("application-exit"));
    actions_.fileExit->setShortcut(QKeySequence::StandardKey::Quit);
    connect(actions_.fileExit, &QAction::triggered, this, &BrowserWindow::close);
    menuFile->addAction(actions_.fileExit);

    // Help menu
    QMenu *menuHelp = menuBar()->addMenu(tr("&Help"));
    // About
    actions_.helpAbout = new QAction(tr("&About"), this);
    actions_.helpAbout->setIcon(QIcon::fromTheme("help-about"));
    connect(actions_.helpAbout, &QAction::triggered, this, &BrowserWindow::about);
    menuHelp->addAction(actions_.helpAbout);
    // Homepage
    actions_.helpHomepage = new QAction(tr("&Homepage"), this);
    actions_.helpHomepage->setIcon(QIcon::fromTheme("internet-web-browser"));
    connect(actions_.helpHomepage, &QAction::triggered, this, &BrowserWindow::homepage);
    menuHelp->addAction(actions_.helpHomepage);

    updateRecentDocuments();
}

void BrowserWindow::initWidgets()
{
    if (!restoreGeometry(Settings::GetMainWindowGeometry())) {
        resize(1024, 768);
        Settings::SetMainWindowGeometry(saveGeometry());
    }
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

void BrowserWindow::showEditor(Rom *rom, const QString &path)
{
    auto *editor = new EditorWindow(rom, path, this);
    editors_.emplace_back(editor);
    connect(editor, &EditorWindow::destroyed, this, &BrowserWindow::editorClosed, Qt::QueuedConnection);
    editor->show();
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
    // TODO
}

void BrowserWindow::settings()
{
    SettingsDialog dialog(this);
    dialog.exec();
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

} // patchman
