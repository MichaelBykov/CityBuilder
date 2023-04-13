/**
 * @file Path2.cpp
 * @brief Implement the different path components.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/Path2.h>
#include <CityBuilder/Units/Angle.h>
USING_NS_CITY_BUILDER

List<Real4> Path2::_getPointNormals() {
  if (_pointCache.isEmpty())
    return _pointCache = _pointNormals();
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

Path2 *Line2::offset(Real distance) {
  Real2 normal = b - a;
  normal.normalise();
  normal = { normal.y, -normal.x };
  
  return new Line2(
    a + normal * distance,
    b + normal * distance
  );
}

List<Real4> Line2::_pointNormals() {
  Real2 normal = b - a;
  normal.normalise();
  normal = { normal.y, -normal.x };
  return {
    { a.x, a.y, normal.x, normal.y },
    { b.x, b.y, normal.x, normal.y },
  };
}



Arc2::Arc2(const Real2 &start, const Real2 &control, const Real2 &end)
  : start(start), control(control), end(end) {
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
  
  // Expand the bounds to include the control point
  if (control.x < _bounds.x) {
    _bounds.z += _bounds.x - control.x;
    _bounds.x = control.x;
  } else if (control.x > _bounds.x + _bounds.z) {
    _bounds.z = control.x - _bounds.x;
  }
  
  if (control.y < _bounds.y) {
    _bounds.w += _bounds.y - control.y;
    _bounds.y = control.y;
  } else if (control.y > _bounds.y + _bounds.w) {
    _bounds.w = control.y - _bounds.y;
  }
}

Real Arc2::length() {
  _getPointNormals();
  return _length;
}

Path2 *Arc2::offset(Real distance) {
  _getPointNormals();
  Real cs = (start - _center).length();
  Real ce = (end   - _center).length();
  
  Real2 _start   = _center + (start   - _center) * ((cs + distance) / cs);
  Real2 _end     = _center + (end     - _center) * ((ce + distance) / ce);
  Real2 _control = _center + (control - _center) * ((cs + distance) / cs);
  
  return new Arc2(_start, _control, _end);
}

List<Real4> Arc2::_pointNormals() {
  // Determine the radius and center of the arc
  Real2 middle = (start + end) / 2;
  Real2 cm = (middle - control);
  Real controlMiddle = cm.length();
  Real controlStart = (control - start).length();
  Real theta = acos(controlMiddle / controlStart);
  _center = control + cm * (controlStart / (controlMiddle * sin(theta)));
  Real radius = (_center - start).length();
  Real angle = Angle::pi - theta * 2;
  _length = radius * angle;
  Real startAngle = atan2(start.y - _center.y, start.x - _center.x);
  
  // Generate the equidistant points
  Real2 normal = (start - _center).normalisedCopy();
  List<Real4> points {{ start.x, start.y, normal.x, normal.y }};
  int pointCount = _length;
  for (int i = 1; i < pointCount; i++) {
    Real2 sinCos = Angle::sinCos(startAngle + angle * i / pointCount) * radius;
    sinCos = { sinCos.y, sinCos.x };
    Real2 point = _center + sinCos;
    normal = (point - _center).normalisedCopy();
    points.append({ point.x, point.y, normal.x, normal.y });
  }
  normal = (end - _center).normalisedCopy();
  points.append({ end.x, end.y, normal.x, normal.y });
  
  return points;
}
