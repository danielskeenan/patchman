/**
 * @file BrowserWindow.h
 *
 * @author Dan Keenan
 * @date 1/7/24
 * @copyright GNU GPLv3
 */

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include "patchlib/Rom.h"
#include "EditorWindow.h"
#include "RomLibraryModel.h"

namespace patchman
{

/**
 * Browser Window
 *
 * Show all ROMs the system knows about.
 */
class BrowserWindow: public QMainWindow
{
Q_OBJECT
public:
    explicit BrowserWindow(QWidget *parent = nullptr);

private:
    struct Actions
    {
        QMenu *fileNew = nullptr;
        QAction *fileOpen = nullptr;
        QMenu *fileRecent = nullptr;
        QAction *fileCreateReport = nullptr;
        QAction *fileSettings = nullptr;
        QAction *fileExit = nullptr;
        QAction *editEditRom = nullptr;
        QAction *editShowDuplicates = nullptr;
        QAction *editShowInFileBrowser = nullptr;
        QAction *helpAbout = nullptr;
        QAction *helpHomepage = nullptr;
    };
    Actions actions_;
    struct Widgets
    {
        QTableView *browser = nullptr;
    };
    Widgets widgets_;
    QList<QPointer<EditorWindow>> editors_;
    QFileSystemWatcher *fsWatcher_;
    RomLibraryModel *browserModel_;
    RomLibrarySortFilterModel *sortFilterModel_;

    void initMenus();
    void initWidgets();
    void initFsWatcher();
    void openFrom(const QString &path);
    void updateRecentDocuments(const QString &path = {});
    void showEditor(Rom *rom, const QString &path = {});
    [[nodiscard]] const RomInfo &getSelectedRomInfo() const;

protected Q_SLOTS:
    void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
    void newFile(Rom::Type romType);
    void open();
    void createReport();
    void settings();
    void editRom();
    void showInFileBrowser();
    void showDuplicates();
    void about();
    void homepage();
    void updateActionsFromSelection();
    void editorClosed();
};

} // patchman

#endif //BROWSERWINDOW_H
