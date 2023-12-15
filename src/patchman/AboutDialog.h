/**
 * @file AboutDialog.h
 *
 * @author Dan Keenan
 * @date 12/15/2023
 * @copyright GNU GPLv3
 */

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace patchman
{

/**
 * About Dialog.
 */
class AboutDialog: public QDialog
{
Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr);

private Q_SLOTS:
    void linkActivated(const QString &link);
};

} // patchman

#endif //ABOUTDIALOG_H
