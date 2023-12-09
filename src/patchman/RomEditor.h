/**
 * @file RomEditor.h
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#ifndef ROMEDITOR_H
#define ROMEDITOR_H

#include <QWidget>
#include <QTabWidget>
#include "patchlib/Rom.h"

namespace patchman
{

/**
 * Base class for ROM editors.
 */
class RomEditor: public QWidget
{
Q_OBJECT
public:
    explicit RomEditor(Rom *rom, QWidget *parent);

Q_SIGNALS:
    void dataChanged();

protected:
    Rom *rom_;

private:
    QTabWidget *tabs_;

    void addTabForRack(Rack *rack);

private Q_SLOTS:
    void rackRemoved(unsigned int rackNum);
};

} // patchman

#endif //ROMEDITOR_H
