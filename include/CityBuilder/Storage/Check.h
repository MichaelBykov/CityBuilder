/**
 * @file Check.h
 * @brief An interface for checking memory integrity.
 * @date February 10, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once

/* -------------------------------------------------------------------------- *\
|                                                                              |
| Configuration                                                                |
|                                                                              |
\* -------------------------------------------------------------------------- */

/// Track the number of managed objects.
// #define TRACK_MANAGED_OBJECTS
/// Track the number and type of managed objects.
// #define TRACK_MANAGED_OBJECT_TYPES
/// Track double-frees.
// #define TRACK_DOUBLE_FREE





/* -------------------------------------------------------------------------- *\
|                                                                              |
| Functions                                                                    |
|                                                                              |
\* -------------------------------------------------------------------------- */

#include <stddef.h> // size_t
#include <typeinfo>
#include <typeindex>
#include <functional>

#if defined(TRACK_MANAGED_OBJECT_TYPES) && !defined(TRACK_MANAGED_OBJECTS)
#define TRACK_MANAGED_OBJECTS
#endif



#ifdef TRACK_MANAGED_OBJECTS

/// Get the number of managed objects there are.
size_t getManagedObjectCount();

#endif



#ifdef TRACK_MANAGED_OBJECT_TYPES

/// Get all of the managed object types.
/// \param[in] callback
///   The callback for each managed object type and its instantiated count.
void getManagedObjects(std::function<void(const char *, size_t)> callback);

/// Track a managed object.
/// \param[in] type
///   The type of the managed object.
void trackManagedObject(const std::type_info &type);

/// Track a managed object.
/// \param[in] type
///   The type of the managed object.
void trackManagedObject(const std::type_index &type);

/// Stop tracking a managed object.
/// \param[in] type
///   The type of the managed object.
void removeManagedObject(const std::type_info &type);

/// Stop tracking a managed object.
/// \param[in] type
///   The type of the managed object.
void removeManagedObject(const std::type_index &type);

#elif defined(TRACK_MANAGED_OBJECTS)

/// Track a managed object.
void trackManagedObject();

/// Stop tracking a managed object.
void removeManagedObject();

#endif



#ifdef TRACK_DOUBLE_FREE

/// Record a zombie object.
void addZombieObject();

/// Get the total number of zombie objects.
size_t getZombieObjects();

#endif
