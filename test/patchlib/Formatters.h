/**
 * @file Formatters.h
 *
 * @author Dan Keenan
 * @date 7/29/23
 * @copyright GNU GPLv3
 */

#ifndef FORMATTERS_H_
#define FORMATTERS_H_

#include <catch2/catch_tostring.hpp>
#include <fmt/format.h>

std::string formatBinPosition(std::size_t pos);

template<std::forward_iterator Iterator>
std::string formatBytes(Iterator begin, Iterator end) {
  const unsigned int bytes_per_group = 4;
  const unsigned int bytes_per_line = 16;

  std::stringstream ss;
  std::size_t bytes_printed = 0;
  // Starting with the newline ensures that the position starts a line.
  // Catch will often start the stringification in the middle of an existing line.
  ss << "\n" << formatBinPosition(bytes_printed);
  for (std::forward_iterator auto it = begin; it != end; ++it) {
    ss << fmt::format(" {:02X}", *it);
    ++bytes_printed;
    if (bytes_printed % bytes_per_line == 0) {
      ss << "\n" << formatBinPosition(bytes_printed);
    } else if (bytes_printed % bytes_per_group == 0) {
      ss << " ";
    }
  }
  ss << "\n";

  return ss.str();
}

#endif //FORMATTERS_H_
