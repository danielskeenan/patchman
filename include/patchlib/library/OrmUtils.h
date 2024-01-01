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
#define PATCHMAN_ORM_SETTER_INTEGRAL(type, name, column) void set##name(type value) {(*this)[column] = value;}
#define PATCHMAN_ORM_SETTER(type, name, column) PATCHMAN_ORM_SETTER_INTEGRAL(const type &, name, column)
#define PATCHMAN_ORM_GETSET(type, name, column) PATCHMAN_ORM_GETTER(type, name, column) PATCHMAN_ORM_SETTER(type, name, column)
#define PATCHMAN_ORM_GETSET_INTEGRAL(type, name, column) PATCHMAN_ORM_GETTER(type, name, column) PATCHMAN_ORM_SETTER_INTEGRAL(type, name, column)

// Define column name constant.
#define PATCHMAN_ORM_COL_NAME_CONSTANT(name, column) inline static const auto kCol##name{column};

// Helpers for general-purpose columns.
#define PATCHMAN_ORM_COL(type, name, column) PATCHMAN_ORM_COL_NAME_CONSTANT(name, column) PATCHMAN_ORM_GETSET(type, name, kCol##name)
#define PATCHMAN_ORM_COL_INTEGRAL(type, name, column) PATCHMAN_ORM_COL_NAME_CONSTANT(name, column) PATCHMAN_ORM_GETSET_INTEGRAL(type, name, kCol##name)

#endif //ORMUTILS_H
