/**
 * @file EnrRackEditor.h
 *
 * @author Dan Keenan
 * @date 12/13/23
 * @copyright GNU GPLv3
 */

#ifndef ENRRACKEDITOR_H
#define ENRRACKEDITOR_H

#include "../RackEditor.h"
#include "EnrRackModel.h"
#include "EnrRackPreview.h"
#include "patchlib/Enr.h"
#include <QTableView>

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
    EnrRackPreview* preview_;

private Q_SLOTS:
    void setModuleRowSpans();
    void tableSelectionChanged();
    void previewSelectionChanged();
};

} // patchman

#endif //ENRRACKEDITOR_H
