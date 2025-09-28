/**
 * @file RackPreview.h
 *
 * @author Dan Keenan
 * @date 9/28/25
 * @copyright GNU GPLv3
 */

#ifndef SRC_PATCHMAN_RACKPREVIEW_H
#define SRC_PATCHMAN_RACKPREVIEW_H

#include "patchlib/Rack.h"
#include <QWidget>

namespace patchman {

class RackPreview : public QWidget
{
    Q_OBJECT
public:
    using QWidget::QWidget;

    static RackPreview *create(Rack *rack, QWidget *parent = nullptr);
    virtual QList<unsigned int> getSelectedCircuits() const = 0;

Q_SIGNALS:
    void selectionChanged();

public Q_SLOTS:
    virtual void selectCircuits(const QList<unsigned int> &circuits) = 0;
};

} // namespace patchman

#endif //SRC_PATCHMAN_RACKPREVIEW_H
