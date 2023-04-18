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



Line2::Line2(const Real2 &start, const Real2 &end) : Path2(start, end) {
  // Calculate the bounds
  Real2 min = start.min(end);
  Real2 max = start.max(end);
  _bounds = { min.x, min.y, max.x - min.x, max.y - min.y };
}

Real Line2::length() {
  return (end - start).magnitude();
}

Path2 *Line2::offset(Real distance) {
  Real2 normal = (end - start).normalized().rightPerpendicular() * Real2(distance);
  
  return new Line2(start + normal, end + normal);
}

Real2 Line2::project(Real2 point) {
  Real2 projection = (point - start).project(end - start) + start;
  return
    (end - start).dot(projection - start).isPositive() &&
    (start - end).dot(projection -   end).isPositive() ?
    projection :
    point.squareDistance(start).exactlyLess(point.squareDistance(end)) ?
      start : end;
}

List<Real4> Line2::_pointNormals() {
  Real2 normal = (end - start).normalized().rightPerpendicular();
  return {
    { start.x, start.y, normal.x, normal.y },
    {   end.x,   end.y, normal.x, normal.y },
  };
}



Arc2::Arc2(const Real2 &start, const Real2 &control, const Real2 &end)
  : Path2(start, end), control(control) {
  // Calculate the bounds
  Real2 min = start.min(end).min(control);
  Real2 max = start.max(end).max(control);
  _bounds = { min.x, min.y, max.x - min.x, max.y - min.y };
}

Real Arc2::length() {
  _getPointNormals();
  return _length;
}

Path2 *Arc2::offset(Real distance) {
  _getPointNormals();
  Real cs = (start - _center).magnitude();
  Real ce = (end   - _center).magnitude();
  
  Real2 _start   = _center + (start   - _center) * Real2((cs + distance) / cs);
  Real2 _end     = _center + (end     - _center) * Real2((ce + distance) / ce);
  Real2 _control = _center + (control - _center) * Real2((cs + distance) / cs);
  
  return new Arc2(_start, _control, _end);
}

namespace {
  Real2 axisPoints[4] {
    {  1,  0 },
    {  0,  1 },
    { -1,  0 },
    {  0, -1 },
  };
}

Real2 Arc2::project(Real2 point) {
  _getPointNormals();
  
  // Project onto the circle
  Real2 projection = (point - _center).normalized() * Real2(_radius) + _center;
  
  // Check if the projection is within the arc
  Real2 startVector = start - _center;
  Real2 endVector = end - _center;
  Real2 projectionVector = projection - _center;
  
  // Determine if the orientation is correct
  bool lhs = Real2x2::columnMajor(startVector, projectionVector).determinant() >= 0;
  bool rhs = Real2x2::columnMajor(  endVector, projectionVector).determinant() <= 0;
  if (lhs && rhs) {
    // The projection is within the arc
    return projection;
  }
  
  // The projection is outside the arc
  if (startVector.squareMagnitude().exactlyLess(endVector.squareMagnitude()))
    return start;
  else
    return end;
}

List<Real4> Arc2::_pointNormals() {
  // Determine the radius and center of the arc
  Real2 middle = (start + end) * Real2(0.5);
  Real2 cm = (middle - control);
  Real controlMiddle = cm.magnitude();
  Real controlStart = (control - start).magnitude();
  Real theta = acos(controlMiddle / controlStart);
  _center = control + cm * Real2(controlStart / (controlMiddle * theta.sin()));
  _radius = (_center - start).magnitude();
  Real angle = Angle::pi - theta * Real(2);
  _length = _radius * angle;
  Real startAngle = atan2(start.y - _center.y, start.x - _center.x);
  
  // Generate the equidistant points
  Real2 normal = (start - _center).normalized();
  List<Real4> points {{ start.x, start.y, normal.x, normal.y }};
  int pointCount = _length;
  for (int i = 1; i < pointCount; i++) {
    Real2 sinCos = Angle::sinCos(startAngle + angle * Real(i) / Real(pointCount)) * Real2(_radius);
    sinCos = { sinCos.y, sinCos.x };
    Real2 point = _center + sinCos;
    normal = (point - _center).normalized();
    points.append({ point.x, point.y, normal.x, normal.y });
  }
  normal = (end - _center).normalized();
  points.append({ end.x, end.y, normal.x, normal.y });
  
  return points;
}
