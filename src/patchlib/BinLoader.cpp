/**
 * @file IntelHex.cpp
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#include <QtEndian>
#include "patchlib/BinLoader.h"

namespace patchman
{

/**
 * Check if file is in Intel HEX format.
 * @param file
 * @return
 * @see https://www.intel.com/content/www/us/en/support/programmable/articles/000076770.html
 */
bool BinLoader::isIntelHex(QFile &file)
{
    QByteArray check = file.read(1);
    if (check.size() != 1 || check[0] != ':') {
        // Doesn't start with header.
        file.seek(0);
        return false;
    }

    char next_char;
    while (file.read(&next_char, sizeof(next_char)) > 0) {
        if (next_char == ':' || next_char == '\r' || next_char == '\n') {
            continue;
        }
        else if (!std::isxdigit(next_char)) {
            // All allowed characters are hex digits.
            file.seek(0);
            return false;
        }
    }
    file.seek(0);

    return true;
}

QByteArray BinLoader::readIntelHex(QFile &file)
{
    QByteArray data;
    QByteArray chunk;
    while (!file.atEnd()) {
        // Seek to start of record.
        chunk = file.read(1);
        if (chunk[0] != ':') {
            continue;
        }

        // Track the sum of read values; used for calculating the checksum at the end.
        uint8_t sum = 0;
        // Found start of record. Read the record header.
        const auto length = file.read(2).toUShort(nullptr, 16);
        sum += length;
        const auto address_bytes_hex = file.read(4);
        const auto address_bytes = QByteArray::fromHex(address_bytes_hex);
        sum += std::reduce(address_bytes.cbegin(), address_bytes.cend());
        const auto address_start = qFromLittleEndian(address_bytes_hex.toUShort(nullptr, 16));
        const auto type = file.read(2).toUShort(nullptr, 16);
        sum += type;
        if (type == 0) {
            // Data
            const auto address_end = address_start + length;
            // Ensure there is room to write the data.
            if (data.size() < address_end) {
                data.resize(address_end, 0);
            }
            chunk = QByteArray::fromHex(file.read(length * 2));
            sum = std::accumulate(chunk.cbegin(), chunk.cend(), sum);
            const uint8_t checksum = -std::uintmax_t(sum);
            const uint8_t file_checksum = file.read(2).toUShort(nullptr, 16);
            if (checksum != file_checksum) {
                throw std::runtime_error("Bad checksum");
            }
            data.replace(address_start, length, chunk);
        }
        else if (type == 1) {
            // End of file
            break;
        }
    }

    return data;
}

QByteArray BinLoader::loadFile(const QString &path)
{
    QFile file(path);
    return loadFile(file);
}

QByteArray BinLoader::loadFile(QFile &file)
{
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Could not open file.");
    }

    if (isIntelHex(file)) {
        return readIntelHex(file);
    }

    // Assume binary.
    return file.readAll();
}

} // patchlib
