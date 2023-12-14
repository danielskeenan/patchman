/**
 * @file util.h
 *
 * @author Dan Keenan
 * @date 12/14/2023
 * @copyright GNU GPLv3
 */

#ifndef UTIL_H
#define UTIL_H

#include "patchlib/Exceptions.h"
#include <QMessageBox>

namespace patchman
{

/**
 * Show the exception @p e in a QMessageBox.
 * @param e
 * @param parent
 */
void showExceptionMessageBox(const PatchmanUserException &e, QWidget *parent = nullptr)
{
    QMessageBox msgbox(parent);
    msgbox.setIcon(QMessageBox::Critical);
    msgbox.setText(e.getMessage());
    msgbox.setInformativeText(e.getInfo());
    msgbox.exec();
}

} // patchman

#endif //UTIL_H
