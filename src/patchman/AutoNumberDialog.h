/**
 * @file AutoNumberDialog.h
 *
 * @author Dan Keenan
 * @date 12/11/23
 * @copyright GNU GPLv3
 */

#ifndef AUTONUMBERDIALOG_H
#define AUTONUMBERDIALOG_H

#include <QDialog>
#include <QSpinBox>

namespace patchman
{

/**
 * Display the autonumbering dialog.
 */
class AutoNumberDialog: public QDialog
{
Q_OBJECT
public:
    struct Options
    {
        /** Start address */
        unsigned int start = 1;
        /** Add this to the start address for each circuit */
        int increment = 1;
        /** Add this to the circuit number (used for single-density vs. double-density modules) */
        unsigned int offset = 1;
    };

    explicit AutoNumberDialog(QWidget *parent = nullptr);
    explicit AutoNumberDialog(const Options &options, QWidget *parent = nullptr);

    [[nodiscard]] Options getOptions() const;

private:
    struct Widgets
    {
        QSpinBox *start = nullptr;
        QSpinBox *increment = nullptr;
        QSpinBox *offset = nullptr;
    };
    Widgets widgets_;
};

} // patchman

#endif //AUTONUMBERDIALOG_H
