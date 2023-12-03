/**
 * @file Formatters.cpp
 *
 * @author Dan Keenan
 * @date 7/27/23
 * @copyright GNU GPLv3
 */

#include "Formatters.h"

std::string formatBinPosition(std::size_t pos) {
  return fmt::format("0x{:04X}:{:04X}|", pos & 0xFFFF0000, pos & 0x0000FFFF);
}
