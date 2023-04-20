/**
 * @file Bounds.h
 * @brief A 2D bounding box.
 * @date April 19, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>

NS_CITY_BUILDER_BEGIN

/// A 2D bounding box.
struct Bounds2 {
  /// The origin of the bounding box.
  Real2 origin;
  
  /// The size of the bounding box.
  Real2 size;
  
  
  Bounds2() { }
  
  Bounds2(const Real2 &origin) : origin(origin), size(0, 0) { }
  
  Bounds2(const Real2 &origin, const Real2 &size) :
    origin(origin), size(size) { }
  
  
  
  /// Fit the bounding box to encapsulate a point.
  /// \param[in] point
  ///   The point to encapsulate.
  Bounds2 &fit(const Real2 &point) {
    Real2 a = origin.min(point);
    Real2 b = (origin + size).max(point);
    origin = a;
    size = b - a;
    return *this;
  }
  
  /// Inflate a bounding box evenly by a given amount.
  /// \param[in] amount
  ///   The amount to inflate the bounding box by.
  Bounds2 &inflate(Real amount) {
    origin -= Real2(amount);
    size += Real2(amount + amount);
    return *this;
  }
  
  /// Check if this bounding box intersects another bounding box.
  /// \param[in] other
  ///   The other bounding box.
  bool intersects(const Bounds2 &other) const {
    Real2 a1 = origin;
    Real2 a2 = other.origin;
    Real2 b1 = origin + size;
    Real2 b2 = other.origin + other.size;
    
    return b1.exactlyLess(a2).verticalOr() || a1.exactlyLess(b2).verticalOr();
  }
};

NS_CITY_BUILDER_END
