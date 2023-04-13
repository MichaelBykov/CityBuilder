/**
 * @file Hash.h
 * @brief A simple utility for finding a hash value of types.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "String.h"

namespace Storage {
  /// Generate a hash for a value.
  /// \param value
  ///   The value to generate a hash for.
  /// \returns
  ///   A 32-bit hash of the given value.
  template<typename T>
  uint32_t hash(const T &value) {
    return (uint32_t)value;
  }
  
  template<typename T>
  uint32_t hash(T *value) {
    uintptr_t ptr = (uintptr_t)value;
    return (uint32_t)((ptr >> 32) ^ ptr);
  }
  
  template<typename T>
  uint32_t hash(const T *value) {
    uintptr_t ptr = (uintptr_t)value;
    return (uint32_t)((ptr >> 32) ^ ptr);
  }
  
  template<>
  inline uint32_t hash(const String &value) {
    return value.hash();
  }
}
