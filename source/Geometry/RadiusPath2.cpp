/**
 * @file RadiusPath2.cpp
 * @brief A 2D path with a constant radius around it.
 * @date April 19, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/RadiusPath2.h>
#include <CityBuilder/Storage/Ref.h>
USING_NS_CITY_BUILDER

bool RadiusPath2::intersectionTest(RadiusPath2 &other) {
  // Check bounds first
  if (!_bounds.intersects(other._bounds))
    return true;
  
  // Check for crossing validity
  {
    Ref<Path2 &> lhs = path().offset(-radius());
    Ref<Path2 &> rhs = path().offset( radius());
    
    Ref<Path2 &> _lhs = other.path().offset(-other.radius());
    Ref<Path2 &> _rhs = other.path().offset( other.radius());
    
    List<Real2> lhs_lhs = lhs->intersections(*_lhs);
    List<Real2> lhs_rhs = lhs->intersections(*_rhs);
    List<Real2> rhs_lhs = rhs->intersections(*_lhs);
    List<Real2> rhs_rhs = rhs->intersections(*_rhs);
    
    int lhsCount;
    if (lhs_lhs.count() != rhs_lhs.count()) {
      if (lhs_lhs.count() == 0 && rhs_lhs.count() == 1 ||
          lhs_lhs.count() == 1 && rhs_lhs.count() == 0 ) {
        // End point
        lhsCount = 1;
      } else
        // Invalid: brush
        return false;
    } else
      lhsCount = lhs_lhs.count();
    
    int rhsCount;
    if (lhs_rhs.count() != rhs_rhs.count()) {
      if (lhs_rhs.count() == 0 && rhs_rhs.count() == 1 ||
          lhs_rhs.count() == 1 && rhs_rhs.count() == 0 ) {
        // End point
        rhsCount = 1;
      } else
        // Invalid: brush
        return false;
    } else
      rhsCount = lhs_rhs.count();
    
    if (lhsCount != rhsCount) {
      if (lhsCount == 1 || rhsCount == 1) {
        // End point or crossing at end
      } else
        // Invalid: brush
        return false;
    }
  }
  
  return true;
}

bool RadiusPath2::circleTest(Real2 center, Real radius) {
  // Check bounds first
  Real2 radius2 = Real2(radius);
  if (!_bounds.intersects(Bounds2(center - radius, radius2 + radius2)))
    return false;
  
  // Check if the path has a point that is within
  // the radius of the circle + this path's radius
  Real2 point = path().project(center);
  Real distance = (point - center).magnitude();
  return distance < radius + this->radius();
}
