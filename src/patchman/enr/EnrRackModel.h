/**
 * @file EnrRackModel.h
 *
 * @author Dan Keenan
 * @date 12/13/23
 * @copyright GNU GPLv3
 */

#ifndef ENRRACKMODEL_H
#define ENRRACKMODEL_H

#include "../RackModel.h"
#include "patchlib/Enr.h"

namespace patchman
{

/**
 * Model class for ENR Racks.
 */
class EnrRackModel: public RackModel
{
Q_OBJECT
public:
    explicit EnrRackModel(EnrRack *rack, QObject *parent = nullptr)
        : RackModel(rack, parent), rack_(rack)
    {}

private:
    EnrRack *rack_;
};

} // patchman

#endif //ENRRACKMODEL_H
