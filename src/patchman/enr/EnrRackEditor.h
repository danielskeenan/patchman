/**
 * @file EnrRackEditor.h
 *
 * @author Dan Keenan
 * @date 12/13/23
 * @copyright GNU GPLv3
 */

#ifndef ENRRACKEDITOR_H
#define ENRRACKEDITOR_H


#include <QTableView>
#include "../RackEditor.h"
#include "patchlib/Enr.h"
#include "EnrRackModel.h"

namespace patchman
{

/**
 * ENR rack editor widget.
 */
class EnrRackEditor: public RackEditor
{
Q_OBJECT
public:
    explicit EnrRackEditor(EnrRack *rack, QWidget *parent);

    [[nodiscard]] QList<unsigned int> getSelectedCircuits() const override;

private:
    EnrRack *rack_;
    QTableView *table_;
    EnrRackModel *model_;

private Q_SLOTS:
    void setModuleRowSpans();
};

} // patchman

#endif //ENRRACKEDITOR_H
