/**
 * @file Path2.cpp
 * @brief Implement the different path components.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/Path2.h>
USING_NS_CITY_BUILDER

List<Real2> Path2::_getPoints() {
  if (_pointCache.isEmpty())
    return _pointCache = _points();
  else
    return _pointCache;
}



Line2::Line2(const Real2 &a, const Real2 &b) : a(a), b(b) {
  // Calculate the bounds
  if (a.x < b.x) {
    _bounds.x = a.x;
    _bounds.z = b.x - a.x;
  } else {
    _bounds.x = b.x;
    _bounds.z = a.x - b.x;
  }
  
  if (a.y < b.y) {
    _bounds.y = a.y;
    _bounds.w = b.y - a.y;
  } else {
    _bounds.y = b.y;
    _bounds.w = a.y - b.y;
  }
}

Real Line2::length() {
  return (b - a).length();
}

List<Real2> Line2::_points() {
  return { a, b };
}



Cubic2::Cubic2(
  const Real2 &start, const Real2 &control0,
  const Real2 &control1, const Real2 &end
) : start(start), control0(control0), control1(control1), end(end) {
  // Calculate the bounds
  if (start.x < end.x) {
    _bounds.x = start.x;
    _bounds.z = end.x - start.x;
  } else {
    _bounds.x = end.x;
    _bounds.z = start.x - end.x;
  }
  
  if (start.y < end.y) {
    _bounds.y = start.y;
    _bounds.w = end.y - start.y;
  } else {
    _bounds.y = end.y;
    _bounds.w = start.y - end.y;
  }
  
  // Expand the bounds to include the control points
  if (control0.x < _bounds.x) {
    _bounds.z += _bounds.x - control0.x;
    _bounds.x = control0.x;
  } else if (control0.x > _bounds.x + _bounds.z) {
    _bounds.z = control0.x - _bounds.x;
  }
  
  if (control0.y < _bounds.y) {
    _bounds.w += _bounds.y - control0.y;
    _bounds.y = control0.y;
  } else if (control0.y > _bounds.y + _bounds.w) {
    _bounds.w = control0.y - _bounds.y;
  }
  
  if (control1.x < _bounds.x) {
    _bounds.z += _bounds.x - control1.x;
    _bounds.x = control1.x;
  } else if (control1.x > _bounds.x + _bounds.z) {
    _bounds.z = control1.x - _bounds.x;
  }
  
  if (control1.y < _bounds.y) {
    _bounds.w += _bounds.y - control1.y;
    _bounds.y = control1.y;
  } else if (control1.y > _bounds.y + _bounds.w) {
    _bounds.w = control1.y - _bounds.y;
  }
}

Real Cubic2::length() {
  _getPoints();
  return _length;
}

Real2 Cubic2::interpolate(Real t) const {
  // Exponents
  Real t2 = t * t;
  Real t3 = t2 * t;
  
  // Inverse
  Real i = 1 - t;
  Real i2 = i * i;
  Real i3 = i2 * i;
  
  return
    start    * i3 +
    control0 *  3 * i2 * t  +
    control1 *  3 * i  * t2 +
    end      * t3
  ;
}

List<Real2> Cubic2::_points() {
  // Generate a look-up table first
  // x,y = the coordinate itself
  // z = the distance to the previous point
  Real3 lut[33];
  _length = 0;
  lut[0] = { start.x, start.y, 0 };
  for (int i = 1; i < 33; i++) {
    Real2 point = interpolate(i / 32.0);
    lut[i] = { point.x, point.y, point.distance(lut[i - 1].xy()) };
    _length += lut[i].z;
  }
  
  // Generate the equidistant points
  List<Real2> points { start };
  int pointCount = _length / 2;
  Real distance = _length / (pointCount + 1);
  Real current = 0;
  Real total = 0;
  for (int i = 1; i < 33; i++) {
    if (current + lut[i].z < distance)
      current += lut[i].z;
    else while (current + lut[i].z >= distance) {
      // Add the point
      Real remaining = distance - current;
      Real t = remaining / lut[i].z;
      t += total;
      points.append(interpolate(t / _length));
      
      // Adjust current
      current = lut[i].z - remaining;
      total += distance;
    }
  }
  points.append(end);
  
  return points;
}
