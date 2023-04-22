/**
 * @file RadiusPath2.h
 * @brief A 2D path with a constant radius around it.
 * @date April 19, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include "Path2.h"

NS_CITY_BUILDER_BEGIN

/// A path with a constant radius.
struct RadiusPath2 {
  /// The central path.
  Path2 &path;
  
  /// The radius of the path.
  const Real radius;
  
  
  
  RadiusPath2(Path2 &path, Real radius)
    : path(path), radius(radius), _bounds(path.bounds().inflate(radius)) { }
  
  
  
  /// Check if this radius path intersects another radius path in a valid way.
  /// \param[in] other
  ///   The other radius path to check for intersection with.
  /// \returns
  ///   Whether or not the other path crosses this path in such a way that the
  ///   other path only either ends or fully crosses at all intersection points
  ///   (the path does not brush up against this path without crossing it).
  bool intersectionTest(const RadiusPath2 &other);
  
  /// Check if this radius path intersects a circle.
  /// \param[in] center
  ///   The center of the circle.
  /// \param[in] radius
  ///   The radius of the circle.
  bool circleTest(Real2 center, Real radius);
  
private:
  Bounds2 _bounds;
};

NS_CITY_BUILDER_END
