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
  
  
  
  /// An intersection between two radius paths.
  struct Intersection {
    enum {
      /// The other path intersects the radius path but does not cross the
      /// center line at all.
      /// \remarks
      ///   The intersection point is the closest center path point.
      brush,
      /// The other path crosses through the radius path completely.
      /// \remarks
      ///   The intersection point is the point at which the two center lines
      ///   cross.
      crossing,
      /// The other path intersects the radius path and crosses the center line
      /// but does not go to the other side of the radius path completely.
      /// \remarks
      ///   The intersection point is the point at which the two center lines
      ///   cross.
      ///   If the path crosses back over the center line, there will be a
      ///   second intersection adjacent to this one about that intersection.
      reflection,
      /// The other path ends at the center line of the radius path.
      /// \remarks
      ///   The intersection point is the end point of the other path.
      end
    } type;
    
    /// The point of intersection.
    Real2 point;
  };
  
  /// Find all the intersections between this radius path and another radius
  /// path.
  /// \param[in] other
  ///   The other radius path to intersect this path with.
  /// \returns
  ///   A list of the found intersections or an empty list if none were found.
  List<Intersection> intersections(const RadiusPath2 &other);
  
private:
  Bounds2 _bounds;
};

NS_CITY_BUILDER_END
