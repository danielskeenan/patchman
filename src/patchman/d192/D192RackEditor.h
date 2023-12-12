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
#include <QLabel>
#include "../RackEditor.h"
#include "patchlib/D192.h"
#include "D192RackModel.h"
#include <array>

namespace patchman
{

/**
 * Part of D192RackMimic, displays a signel slot's patch.
 */
class D192RackModuleMimic: public QWidget
{
Q_OBJECT
public:
    explicit D192RackModuleMimic(D192Rack *rack,
                                 unsigned int firstLug,
                                 QWidget *parent = nullptr);

    void setAddress(unsigned int lug, unsigned int address);

private:
    D192Rack *rack_;
    unsigned int firstLug_;
    QLabel *label_;
    std::array<unsigned int, 2> addresses_{};

    void updateLabel();
};

/**
 * Mimic the appearance of an actual D192 Rack.
 */
class D192RackMimic: public QWidget
{
Q_OBJECT
public:
    explicit D192RackMimic(D192Rack *rack, QWidget *parent = nullptr);

private:
    D192Rack *rack_;
    QList<D192RackModuleMimic *> slots_;

private Q_SLOTS:
    void lugChanged(unsigned int lug);
};

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
    D192RackMimic *mimic_;

private Q_SLOTS:
    void setModuleRowSpans();
};

} // patchman

#endif //D192RACKEDITOR_H
