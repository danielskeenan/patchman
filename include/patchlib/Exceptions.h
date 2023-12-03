/**
 * @file Exceptions.h
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

namespace patchlib {

/**
 * Thrown when a ROM being loaded is invalid in some way.
 */
class InvalidRomException : public std::runtime_error {
public:
    explicit InvalidRomException(const char *msg) : std::runtime_error(msg) {}
};

/**
 * Thrown when a patch table cannot be represented in the target format.
 */
class UnrepresentableException : public std::runtime_error {
public:
    explicit UnrepresentableException(const char *msg) : std::runtime_error(msg) {}
};

} // patchlib

#endif //EXCEPTIONS_H
