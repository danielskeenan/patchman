/**
 * @file RecentDocument.cpp
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#include "RecentDocument.h"

namespace patchman
{

QDataStream &operator<<(QDataStream &ds, const RecentDocument &obj)
{
    ds << obj.path_
       << obj.romType_;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, RecentDocument &obj)
{
    ds >> obj.path_
       >> obj.romType_;

    return ds;
}

} // patchman
