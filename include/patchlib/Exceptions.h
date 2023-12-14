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

namespace patchman {

/**
 * An exception containing text that can be user-facing.
 */
class PatchmanUserException: public std::runtime_error
{
public:
    explicit PatchmanUserException(const QString &message, const QString &info = QString())
        : std::runtime_error(message.toStdString()), message_(message), info_(info)
    {}

    [[nodiscard]] const QString &getMessage() const
    {
        return message_;
    }
    [[nodiscard]] const QString &getInfo() const
    {
        return info_;
    }

private:
    QString message_;
    QString info_;
};

/**
 * Thrown when a ROM being loaded is invalid in some way.
 */
class InvalidRomException: public PatchmanUserException
{
public:
    using PatchmanUserException::PatchmanUserException;
};

/**
 * Thrown when a patch table cannot be represented in the target format.
 */
class UnrepresentableException: public PatchmanUserException
{
public:
    using PatchmanUserException::PatchmanUserException;
};

} // patchlib

#endif //EXCEPTIONS_H
