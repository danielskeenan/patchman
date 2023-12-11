/**
 * @file RackEditor.h
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#ifndef RACKEDITOR_H
#define RACKEDITOR_H

#include <QWidget>
#include "patchlib/Rack.h"

namespace patchman
{

/**
 * Base class for rack editor widgets.
 */
class RackEditor: public QWidget
{
Q_OBJECT
public:
    explicit RackEditor(Rack *rack, QWidget *parent)
        : QWidget(parent)
    {}

    static RackEditor *create(Rack *rack, QWidget *parent);

Q_SIGNALS:
    void dataChanged();
};

} // patchman

#endif //RACKEDITOR_H
