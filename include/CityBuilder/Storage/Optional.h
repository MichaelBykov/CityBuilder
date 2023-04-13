/**
 * @file Optional.h
 * @brief A simple optional type.
 * @date January 30, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "Exceptions.h"
#include <stddef.h> // nullptr_t

/// An optional (nullable) value.
template<typename T>
struct Optional {
private:
  /// The value itself.
  T value;
  /// Whether or not the value is populated (not null).
  bool populated;
public:
  
  /// Create a null value.
  Optional()
    : value(), populated(false) { }
  
  /// Create a null value.
  Optional(nullptr_t)
    : value(), populated(false) { }
  
  /// Create a concrete value
  /// \param[in] value
  ///   The concrete value.
  Optional(const T &value)
    : value(value), populated(true) { }
  
  
  
  /// Test whether or not this optional is populated (not null).
  operator bool() {
    return populated;
  }
  
  /// Access the value of the optional.
  T operator *() {
    if (!populated)
      throw NullAccess();
    return value;
  }
  
  /// Access a member of the value of the optional.
  T *operator->() {
    if (!populated)
      throw NullAccess();
    return &value;
  }
  
  /// Null-coalescing operator.
  T operator || (const T &other) {
    return populated ? value : other;
  }
};



/// An optional (nullable) value.
template<typename T>
struct Optional<T &> {
private:
  /// The value itself.
  T *value;
  /// Whether or not the value is populated (not null).
  bool populated;
public:
  
  /// Create a null value.
  Optional()
    : value(nullptr), populated(false) { }
  
  /// Create a null value.
  Optional(nullptr_t)
    : value(nullptr), populated(false) { }
  
  /// Create a concrete value
  /// \param[in] value
  ///   The concrete value.
  Optional(const T &value)
    : value((T *)&value), populated(true) { }
  
  
  
  /// Test whether or not this optional is populated (not null).
  operator bool() {
    return populated;
  }
  
  /// Access the value of the optional.
  T &operator *() {
    if (!populated)
      throw NullAccess();
    return *value;
  }
  
  /// Access a member of the value of the optional.
  T *operator->() {
    if (!populated)
      throw NullAccess();
    return value;
  }
  
  /// Null-coalescing operator.
  T operator || (const T &other) {
    return populated ? *value : other;
  }
};
