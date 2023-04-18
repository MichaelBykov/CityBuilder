/**
 * @file Ray3.cpp
 * @brief A 3D ray.
 * @date April 17, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/Ray3.h>
USING_NS_CITY_BUILDER

Optional<Real3> Ray3::xzIntersection(Real y) const {
  // Check if the ray is parallel to the plane
  if (direction.y.approxZero())
    return nullptr;
  
  // Check that the ray is traveling in the direction of the plane
  if (direction.y.sign().value != (y - start.y).sign().value)
    return nullptr;
  
  // Intersect
  Real t = (y - start.y) / direction.y;
  Real3 point = start + direction * Real3(t);
  point.y = y; // Normalize
  return point;
}
