/**
 * @file Ray3.h
 * @brief A three-dimensional ray.
 * @date April 17, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/Optional.h>

NS_CITY_BUILDER_BEGIN

/// A three-dimensional ray.
struct Ray3 {
  /// The start point of the ray.
  Real3 start;
  
  /// The direction that the ray is traveling in.
  Real3 direction;
  
  
  
  /// Create an empty ray.
  Ray3() { }
  
  /// Create a ray.
  /// \param[in] start
  ///   The start point of the ray.
  /// \param[in] direction
  ///   The direction of the ray.
  Ray3(Real3 start, Real3 direction) : start(start), direction(direction) { }
  
  
  
  /// Calculate where this ray intersects with the x-z plane.
  /// \param[in] y
  ///   The y-coordinate of the x-z plane.
  /// \returns
  ///   The intersection point with the plane, if one exists.
  Optional<Real3> xzIntersection(Real y) const;
};

NS_CITY_BUILDER_END
