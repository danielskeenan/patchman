/**
 * @file EditorWindow.h
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>
#include <patchlib/Rom.h>
#include <QLabel>
#include "RomEditor.h"

namespace patchman
{

/**
 * ROM Editor window.
 */
class EditorWindow: public QMainWindow
{
Q_OBJECT
public:
    /**
     * New EditorWindow.
     *
     * This window will take ownership of the passed @p rom.
     * @param rom
     * @param parent
     */
    explicit EditorWindow(Rom *rom, const QString &path, QWidget *parent = nullptr);

private:
    RomEditor *editor_ = nullptr;
    struct Actions
    {
        QAction *fileSave = nullptr;
        QAction *fileSaveAs = nullptr;
        QAction *fileCreateReport = nullptr;
        QAction *fileClose = nullptr;
        QAction *helpAbout = nullptr;
        QAction *helpHomepage = nullptr;
    };
    Actions actions_;
    struct Widgets
    {
        QLabel *romTitle = nullptr;
        QLabel *patchedRacksCount = nullptr;
        QLabel *checksum = nullptr;
    };
    Widgets widgets;
    Rom *rom_ = nullptr;

    void initMenus();
    void initWidgets();

    void saveTo(const QString &path);
    void setSaveEnabled();
    bool maybeSave();
    void updatePatchedRacksCount();
    void updateChecksum();

protected Q_SLOTS:
    void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
    void save();
    void saveAs();
    void createReport();
    void about();
    void homepage();
    void dataChanged();
    void romTitleChanged();
};

} // patchman

#endif //EDITORWINDOW_H
