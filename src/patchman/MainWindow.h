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
        QMenu *fileOpen = nullptr;
        QMenu *fileRecent = nullptr;
        QAction *fileSave = nullptr;
        QAction *fileSaveAs = nullptr;
        QAction *fileExit = nullptr;
    };
    Actions actions_;
    Rom *rom_ = nullptr;

    void initMenus();
    void initWidgets();

    void saveTo(const QString &path);
    void open(Rom::Type romType);
    void openFrom(const QString &path, Rom::Type romType);
    void setSaveEnabled();
    void updateRecentDocuments();

protected Q_SLOTS:
    void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
    void save();
    void saveAs();
};

} // patchman

#endif //MAINWINDOW_H
