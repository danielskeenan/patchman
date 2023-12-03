/**
 * @file Phase.h
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#ifndef PHASE_H
#define PHASE_H

#include <QObject>

namespace patchlib
{
Q_NAMESPACE

/**
 * AC power phase
 */
enum class Phase
{
    A,
    B,
    C
};

Q_ENUM_NS(Phase)

} // patchlib

#endif //PHASE_H
