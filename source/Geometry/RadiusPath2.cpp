/**
 * @file RadiusPath2.cpp
 * @brief A 2D path with a constant radius around it.
 * @date April 19, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/RadiusPath2.h>
#include <CityBuilder/Storage/Ref.h>
USING_NS_CITY_BUILDER

List<RadiusPath2::Intersection> RadiusPath2::intersections(const RadiusPath2 &other) {
  // Check bounds first
  if (!_bounds.intersects(other._bounds))
    return { };
  
  // Create the bounding curves
  Path2 &center1 = path;
  Real radius1 = radius;
  Ref<Path2 &> lhs1 = *center1.offset(-radius1);
  Ref<Path2 &> rhs1 = *center1.offset( radius1);
  
  Path2 &center2 = other.path;
  Real radius2 = other.radius;
  Ref<Path2 &> lhs2 = *center2.offset(-radius2);
  Ref<Path2 &> rhs2 = *center2.offset( radius2);
  
  // Find the intersections
  List<Real2> intersections1, intersections2, intersections3;
  intersections1.appendList(   lhs1->intersections(*lhs2));
  intersections2.appendList(center1. intersections(*lhs2));
  intersections3.appendList(   rhs1->intersections(*lhs2));
  intersections1.appendList(   lhs1->intersections( center2));
  intersections2.appendList(center1. intersections( center2));
  intersections3.appendList(   rhs1->intersections( center2));
  intersections1.appendList(   lhs1->intersections(*rhs2));
  intersections2.appendList(center1. intersections(*rhs2));
  intersections3.appendList(   rhs1->intersections(*rhs2));
  
  
}
