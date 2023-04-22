/**
 * @file Path2.cpp
 * @brief Implement the different path components.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/Path2.h>
#include <CityBuilder/Units/Angle.h>
USING_NS_CITY_BUILDER

#include "Intersection Table.ipp"

List<Real2> Path2::intersections(Path2 &other) {
  // The intersection table
  switch (_type) {
  case Type::line:
    switch (other._type) {
    case Type::line:
      return _Internal_Intersection_Table_::
        line_line(
          static_cast<Line2 &>(*this),
          static_cast<Line2 &>(other)
        );
    case Type::arc:
      return _Internal_Intersection_Table_::
        line_arc(
          static_cast<Line2 &>(*this),
          static_cast<Arc2 &>(other)
        );
    }
  
  case Type::arc:
    switch (other._type) {
    case Type::line:
      return _Internal_Intersection_Table_::
        line_arc(
          static_cast<Line2 &>(other),
          static_cast<Arc2 &>(*this)
        );
    case Type::arc:
      return _Internal_Intersection_Table_::
        arc_arc(
          static_cast<Arc2 &>(*this),
          static_cast<Arc2 &>(other)
        );
    }
  }
}

List<Real4> Path2::_getPointNormals() {
  if (_pointCache.isEmpty())
    return _pointCache = _pointNormals();
  else
    return _pointCache;
}



Line2::Line2(const Real2 &start, const Real2 &end)
  : Path2(Type::line, start, end) {
  // Calculate the bounds
  Real2 min = start.min(end);
  Real2 max = start.max(end);
  _bounds = { min, max - min };
}

Real Line2::length() {
  return (end - start).magnitude();
}

Path2 *Line2::offset(Real distance) {
  Real2 normal = (end - start).normalized().rightPerpendicular() * Real2(distance);
  
  return new Line2(start + normal, end + normal);
}

Path2 *Line2::split(Real tStart, Real tEnd) {
  Real2 pointStart = start + (end - start) * Real2(tStart);
  Real2 pointEnd   = start + (end - start) * Real2(tEnd);
  return new Line2(pointStart, pointEnd);
}

void Line2::split(Real t, Path2 *&lhs, Path2 *&rhs) {
  Real2 point = start + (end - start) * Real2(t);
  lhs = new Line2(start, point);
  rhs = new Line2(point, end);
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

Real2 Line2::point(Real t) {
  return start + (end - start) * Real2(t);
}

Real2 Line2::normal(Real t) {
  return (end - start).normalized().rightPerpendicular();
}

Real Line2::inverse(Real2 point) {
  return (point - start).dot(end - start) / (end - start).squareMagnitude();
}

List<Real4> Line2::_pointNormals() {
  Real2 normal = (end - start).normalized().rightPerpendicular();
  return {
    { start.x, start.y, normal.x, normal.y },
    {   end.x,   end.y, normal.x, normal.y },
  };
}



Arc2::Arc2(const Real2 &start, const Real2 &control, const Real2 &end)
  : Path2(Type::arc, start, end), control(control) {
  // Calculate the bounds
  Real2 min = start.min(end).min(control);
  Real2 max = start.max(end).max(control);
  _bounds = { min, max - min };
}

Real Arc2::length() {
  _getPointNormals();
  return _length;
}

Real2 Arc2::center() {
  _getPointNormals();
  return _center;
}

Real Arc2::radius() {
  _getPointNormals();
  return _radius;
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

Path2 *Arc2::split(Real tStart, Real tEnd) {
  _getPointNormals();
  
  // Calculate the split points and their normals
  Real2 pStart = point(tStart);
  Real2 pEnd   = point(tEnd);
  Real2 normalStart = pStart - _center;
  Real2 normalEnd   = pEnd   - _center;
  
  // Calculate the new control point
  Real2 control = normalStart.rightPerpendicular().project(normalEnd - pStart) + pStart;
  
  // Split the arc
  return new Arc2(pStart, control, pEnd);
}

void Arc2::split(Real t, Path2 *&lhs, Path2 *&rhs) {
  _getPointNormals();
  
  // Calculate the split point and its normal
  Real2 p = point(t);
  Real2 normal = p - _center;
  
  // Calculate the new control points
  Real2 startControl = normal.rightPerpendicular().project(start - p) + p;
  Real2   endControl = normal. leftPerpendicular().project(end   - p) + p;
  
  // Split the arc into two arcs
  lhs = new Arc2(start, startControl, p);
  rhs = new Arc2(p, endControl, end);
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
  bool lhs = Real2x2::rowMajor(startVector, projectionVector).determinant() >= 0;
  bool rhs = Real2x2::rowMajor(  endVector, projectionVector).determinant() <= 0;
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

Real2 Arc2::point(Real t) {
  _getPointNormals();
  Angle startAngle = start - _center;
  Angle   endAngle =   end - _center;
  Angle angle = Angle::span(startAngle, endAngle);
  return _center +
    Angle::cosSin(angle.radians * t + startAngle.radians) * Real2(_radius);
}

Real2 Arc2::normal(Real t) {
  _getPointNormals();
  Angle startAngle = start - _center;
  Angle   endAngle =   end - _center;
  Angle angle = Angle::span(startAngle, endAngle);
  return Angle::cosSin(angle.radians * t + startAngle.radians);
}

Real Arc2::inverse(Real2 point) {
  _getPointNormals();
  Angle startAngle = start - _center;
  Angle   endAngle =   end - _center;
  Angle angle = Angle::span(startAngle, endAngle);
  Angle pointAngle = Angle::span(startAngle, point - _center);
  return pointAngle.radians / angle.radians;
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
