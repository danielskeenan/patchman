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
    explicit RackEditor(Rack *rack, QWidget *parent = nullptr)
        : QWidget(parent)
    {}

    static RackEditor *create(Rack *rack, QWidget *parent = nullptr);

    [[nodiscard]] virtual QList<unsigned int> getSelectedCircuits() const = 0;

Q_SIGNALS:
    void dataChanged();
};

/**
 * Contains a RackEditor widget, adding tool buttons around the widget.
 */
class RackEditorContainer: public QWidget
{
Q_OBJECT
public:
    explicit RackEditorContainer(Rack *rack, QWidget *parent = nullptr);

Q_SIGNALS:
    void dataChanged();

private:
    Rack *rack_;
    RackEditor *editor_;

private Q_SLOTS:
    void autonumber();
    void unpatch();
};

} // patchman

#endif //RACKEDITOR_H
