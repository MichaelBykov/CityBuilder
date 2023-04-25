/**
 * @file RadiusPath2.h
 * @brief A 2D path with a constant radius around it.
 * @date April 19, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/Ref.h>
#include "Path2.h"

NS_CITY_BUILDER_BEGIN

/// A path with a constant radius.
struct RadiusPath2 {
private:
  /// The central path.
  Ref<Path2 &> _path;
  
  /// The radius of the path.
  Real _radius;
public:
  
  
  
  RadiusPath2(Ref<Path2 &> path, Real radius)
    : _path(path), _radius(radius), _bounds(path->bounds().inflated(radius)) { }
  
  
  inline Path2 &path() {
    return *_path;
  }
  
  inline Real radius() {
    return _radius;
  }
  
  inline Bounds2 bounds() {
    return _bounds;
  }
  
  inline Real length() {
    return _path->length();
  }
  
  inline Real2 point(Real t) {
    return _path->point(t);
  }
  
  inline Real2 normal(Real t) {
    return _path->normal(t);
  }
  
  inline Real inverse(Real2 point) {
    return _path->inverse(point);
  }
  
  inline List<Real4> pointNormals() {
    return _path->pointNormals();
  }
  
  inline Real2 start() {
    return _path->start;
  }
  
  inline Real2 end() {
    return _path->end;
  }
  
  inline Path2::Type type() {
    return _path->type();
  }
  
  inline RadiusPath2 split(Real tStart, Real tEnd) {
    return RadiusPath2(_path->split(tStart, tEnd), _radius);
  }
  
  
  
  /// Check if this radius path intersects another radius path in a valid way.
  /// \param[in] other
  ///   The other radius path to check for intersection with.
  /// \returns
  ///   Whether or not the other path crosses this path in such a way that the
  ///   other path only either ends or fully crosses at all intersection points
  ///   (the path does not brush up against this path without crossing it).
  bool intersectionTest(RadiusPath2 &other);
  
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
