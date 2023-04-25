/**
 * @file Intersection.cpp
 * @brief A road intersection.
 * @date April 25, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Roads/Intersection.h>
USING_NS_CITY_BUILDER

namespace {
  const Real scale = 0.333333333333;
}

Intersection::Intersection(Real2 center)
  : center(center), radius(0) {
  
}

void Intersection::addRoad(Road *road) {
  // Check which end of the road connects to the intersection
  bool start =
    road->path.start().squareDistance(center) <
    road->path.  end().squareDistance(center) ;
  Real2 forward = start ?
    road->path.normal(0). leftPerpendicular() :
    road->path.normal(1).rightPerpendicular() ;
  
  if (start)
    road->start = this;
  else
    road->end = this;
  road->_dirty = true;
  
  // Find the angle of the road
  Angle angle = forward;
  
  // Add the arm
  arms.append({ road, start, angle });
  
  // Sort the arms
  arms.sort();
  
  // Find the new radius of the intersection
  if (arms.count() > 1) {
    Arm *prev = (Arm *)&arms.last();
    for (Arm &arm : arms) {
      Angle span = Angle::span(prev->angle, arm.angle);
      Real offset =
        (
          arm  .road->definition->dimensions.x.max(
          prev->road->definition->dimensions.x)
        * Real(0.5) * scale) /
        (span * 0.5).tan()
        + Real(0.1);
      radius = radius.max(offset);
      prev = &arm;
    }
  }
  
  // Push back all roads to reflect the new radius
  for (Arm &arm : arms)
    if (arm.pushed + Real(0.01) < radius) {
      arm.road->path.pushBack(arm.start, radius - arm.pushed);
      arm.road->_dirty = true;
      arm.pushed = radius;
    }
  
  // Mark the intersection as dirty
  _dirty = true;
}
