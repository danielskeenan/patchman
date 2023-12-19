/**
 * @file MainWindow.h
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <patchlib/Rom.h>
#include <QLabel>
#include "RomEditor.h"

namespace patchman
{

/**
 * Main Window
 */
class MainWindow: public QMainWindow
{
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    RomEditor *editor_ = nullptr;
    struct Actions
    {
        QMenu *fileNew = nullptr;
        QAction *fileOpen = nullptr;
        QMenu *fileRecent = nullptr;
        QAction *fileSave = nullptr;
        QAction *fileSaveAs = nullptr;
        QAction *fileCreateReport = nullptr;
        QAction *fileExit = nullptr;
        QAction* helpAbout = nullptr;
        QAction* helpHomepage = nullptr;
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
    void openFrom(const QString &path);
    void setSaveEnabled();
    void updateRecentDocuments();
    void replaceOpenRom(Rom *newRom);
    bool maybeSave();
    void updatePatchedRacksCount();
    void updateChecksum();

protected Q_SLOTS:
    void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
    void newFile(Rom::Type romType);
    void open();
    void save();
    void saveAs();
    void createReport();
    void about();
    void homepage();
    void dataChanged();
    void romTitleChanged();
};

} // patchman

#endif //MAINWINDOW_H
