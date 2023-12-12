/**
 * @file D192RackEditor.h
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#ifndef D192RACKEDITOR_H
#define D192RACKEDITOR_H

#include <QTableView>
#include "../RackEditor.h"
#include "patchlib/D192.h"
#include "D192RackModel.h"

namespace patchman
{

/**
 * D192 rack editor widget.
 */
class D192RackEditor: public RackEditor
{
Q_OBJECT
public:
    explicit D192RackEditor(D192Rack *rack, QWidget *parent);

    [[nodiscard]] QList<unsigned int> getSelectedCircuits() const override;

private:
    D192Rack *rack_;
    QTableView *table_;
    D192RackModel *model_;

private Q_SLOTS:
    void autonumber();
    void unpatch();
    void setModuleRowSpans();
};

} // patchman

#endif //D192RACKEDITOR_H
