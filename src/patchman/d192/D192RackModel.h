/**
 * @file D192RackModel.h
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#ifndef D192RACKMODEL_H
#define D192RACKMODEL_H


#include "../RackModel.h"
#include "patchlib/D192.h"

namespace patchman
{

/**
 * Model class for D192 Racks.
 */
class D192RackModel: public RackModel
{
Q_OBJECT
public:
    explicit D192RackModel(D192Rack *rack, QObject* parent = nullptr)
        : RackModel(rack, parent)
    {}
};

} // patchman

#endif //D192RACKMODEL_H
