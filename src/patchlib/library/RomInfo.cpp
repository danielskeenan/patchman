/**
 * @file RomInfo.cpp
 *
 * @author Dan Keenan
 * @date 1/1/24
 * @copyright GNU GPLv3
 */

#include "patchlib/library/RomInfo.h"

namespace patchman
{

QString RomInfo::getDDL()
{
    return QString(R"(
create table if not exists "rom_info"
(
    id         integer
        primary key,
    file_path  text collate NOCASE not null
        constraint rom_info_k
            unique,
    file_mtime text,
    hash_algo  integer,
    sw_hash    blob,
    patch_hash blob,
    rom_type   integer,
    rack_count integer
);
)");
}

} // patchman
