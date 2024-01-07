/**
 * @file OrmUtils.h
 *
 * Utilities to interact with the ORM API.
 *
 * @author Dan Keenan
 * @date 1/1/24
 * @copyright GNU GPLv3
 */

#ifndef ORMUTILS_H
#define ORMUTILS_H

// Getters and Setters.
#define PATCHMAN_ORM_GETTER(type, name, column) [[nodiscard]] type get##name() const {return (*this)[column].value<type>();}
// Can't use the built-in converter because it makes model instances non thread-safe. (It hits the DB connection).
#define PATCHMAN_ORM_GETTER_DATETIME(type, name, column) [[nodiscard]] type get##name() const {return QDateTime::fromString((*this)[column].toString(), Qt::DateFormat::ISODateWithMs);}
#define PATCHMAN_ORM_SETTER_INTEGRAL(type, name, column) void set##name(type value) {(*this)[column] = value;}
#define PATCHMAN_ORM_SETTER(type, name, column) PATCHMAN_ORM_SETTER_INTEGRAL(const type &, name, column)
#define PATCHMAN_ORM_SETTER_DATETIME(type, name, column) void set##name(const type & value) {(*this)[column] = value.toString(Qt::DateFormat::ISODateWithMs);}
#define PATCHMAN_ORM_GETSET(type, name, column) PATCHMAN_ORM_GETTER(type, name, column) PATCHMAN_ORM_SETTER(type, name, column)
#define PATCHMAN_ORM_GETSET_INTEGRAL(type, name, column) PATCHMAN_ORM_GETTER(type, name, column) PATCHMAN_ORM_SETTER_INTEGRAL(type, name, column)
#define PATCHMAN_ORM_GETSET_DATETIME(type, name, column) PATCHMAN_ORM_GETTER_DATETIME(type, name, column) PATCHMAN_ORM_SETTER_DATETIME(type, name, column)

// Define column name constant.
#define PATCHMAN_ORM_COL_NAME_CONSTANT(name, column) inline static const auto kCol##name{column};

// Helpers for general-purpose columns.
#define PATCHMAN_ORM_COL(type, name, column) PATCHMAN_ORM_COL_NAME_CONSTANT(name, column) PATCHMAN_ORM_GETSET(type, name, kCol##name)
#define PATCHMAN_ORM_COL_INTEGRAL(type, name, column) PATCHMAN_ORM_COL_NAME_CONSTANT(name, column) PATCHMAN_ORM_GETSET_INTEGRAL(type, name, kCol##name)
#define PATCHMAN_ORM_COL_DATETIME(type, name, column) PATCHMAN_ORM_COL_NAME_CONSTANT(name, column) PATCHMAN_ORM_GETSET_DATETIME(type, name, kCol##name)

#endif //ORMUTILS_H
