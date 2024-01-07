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
#include "patchlib/Rom.h"
#include "EditorWindow.h"

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
        QAction *fileExit = nullptr;
        QAction *helpAbout = nullptr;
        QAction *helpHomepage = nullptr;
    };
    Actions actions_;
    QList<QPointer<EditorWindow>> editors_;

    void initMenus();
    void initWidgets();
    void openFrom(const QString &path);
    void updateRecentDocuments(const QString &path = {});
    void showEditor(Rom *rom, const QString &path = {});

protected Q_SLOTS:
    void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
    void newFile(Rom::Type romType);
    void open();
    void createReport();
    void about();
    void homepage();
    void editorClosed();
};

} // patchman

#endif //BROWSERWINDOW_H
