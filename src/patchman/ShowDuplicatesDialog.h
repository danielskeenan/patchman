/**
 * @file ShowDuplicatesDialog.h
 *
 * @author Dan Keenan
 * @date 1/7/24
 * @copyright GNU GPLv3
 */

#ifndef SHOWDUPLICATESDIALOG_H
#define SHOWDUPLICATESDIALOG_H

#include <QDialog>
#include <QListWidget>
#include "patchlib/library/RomInfo.h"

namespace patchman
{

/**
 * Display duplicates of the given ROM.
 */
class ShowDuplicatesDialog: public QDialog
{
Q_OBJECT
public:
    explicit ShowDuplicatesDialog(const RomInfo &romInfo, QWidget *parent = nullptr);

private:
    struct Actions
    {
        QAction *showInFileBrowser = nullptr;
    };
    Actions actions_;
    RomInfo romInfo_;
    QListWidget *fileList_;

    QString getSelectedPath() const;

private Q_SLOTS:
    void refreshDuplicates();
    void showInFileBrowser();
    void updateActionsFromSelection();
};

} // patchman

#endif //SHOWDUPLICATESDIALOG_H
