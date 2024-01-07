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

QStringList RomInfo::getDDL()
{
    return {
        R"(
create table if not exists rom_info
(
    id         integer
        primary key,
    file_path  text collate NOCASE not null
        constraint rom_info_k
            unique,
    file_mtime text,
    hash_algo  integer,
    sw_hash    BLOB,
    patch_hash BLOB,
    rom_type   integer,
    rack_count  integer,
    rom_checksum BLOB
);
)",
        R"(
create unique index if not exists rom_info_file_path_uindex
    on rom_info (file_path);
)",
        R"(
create index if not exists rom_info_patch_hash_index
    on rom_info (patch_hash);
)"
    };
}

} // patchman
