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
    case Type::bezier:
      return _Internal_Intersection_Table_::
        line_bezier(
          static_cast<Line2 &>(*this),
          static_cast<Bezier2 &>(other),
          0
        );
    }
  
  case Type::bezier:
    switch (other._type) {
    case Type::line:
      return _Internal_Intersection_Table_::
        line_bezier(
          static_cast<Line2 &>(other),
          static_cast<Bezier2 &>(*this),
          0
        );
    case Type::bezier:
      return _Internal_Intersection_Table_::
        bezier_bezier(
          static_cast<Bezier2 &>(*this),
          static_cast<Bezier2 &>(other),
          0
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



Line2::Line2(Real2 start, Real2 end)
  : Path2(Type::line, start, end) {
  // Calculate the bounds
  Real2 min = start.min(end);
  Real2 max = start.max(end);
  _bounds = { min, max - min };
}

Real Line2::length() {
  return (end - start).magnitude();
}

Ref<Path2 &> Line2::offset(Real distance) {
  Real2 normal = (end - start).normalized().rightPerpendicular() * Real2(distance);
  
  return new Line2(start + normal, end + normal);
}

Ref<Path2 &> Line2::split(Real tStart, Real tEnd) {
  Real2 pointStart = start + (end - start) * Real2(tStart);
  Real2 pointEnd   = start + (end - start) * Real2(tEnd);
  return new Line2(pointStart, pointEnd);
}

void Line2::split(Real t, Ref<Path2 &> &lhs, Ref<Path2 &> &rhs) {
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
  return ((point - start).dot(end - start) / (end - start).squareMagnitude()).min(1).max(0);
}

Ref<Path2 &> Line2::pushedBack(bool start, Real amount) {
  if (start)
    return new Line2(
      this->start + (this->end - this->start).normalized() * Real2(amount),
      this->end
    );
  else
    return new Line2(
      this->start,
      this->end + (this->start - this->end).normalized() * Real2(amount)
    );
}

List<Real4> Line2::_pointNormals() {
  Real2 normal = (end - start).normalized().rightPerpendicular();
  return {
    { start.x, start.y, normal.x, normal.y },
    {   end.x,   end.y, normal.x, normal.y },
  };
}



Bezier2::Bezier2(Real2 start, Real2 control, Real2 end)
  : Path2(Type::bezier, start, end),
    control1(start + Real2(0.55) * (control - start)),
    control2(end   + Real2(0.55) * (control -   end)) {
  // Calculate the bounds
  Real2 min = start.min(end).min(control1).min(control2);
  Real2 max = start.max(end).max(control1).max(control2);
  _bounds = { min, max - min };
}

Bezier2::Bezier2(Real2 start, Real2 control1, Real2 control2, Real2 end)
  : Path2(Type::bezier, start, end), control1(control1), control2(control2) {
  // Calculate the bounds
  Real2 min = start.min(end).min(control1).min(control2);
  Real2 max = start.max(end).max(control1).max(control2);
  _bounds = { min, max - min };
}

Real Bezier2::length() {
  _getPointNormals();
  return _lengths.last().z;
}

bool Bezier2::isDegenerate() {
  Line2 line(start, end);
  return
    line.project(control1).approxEqual(control1).verticalAnd() &&
    line.project(control2).approxEqual(control2).verticalAnd() ;
}

Ref<Path2 &> Bezier2::offset(Real distance) {
  _getPointNormals();
  
  // For our use case (mostly arc approximations for intersection testing),
  // we can simply offset by a constant
  Real4 pnStart = pointNormals().first();
  Real4 pnEnd   = pointNormals().last ();
  Real2 newStart = start + Real2(pnStart.z, pnStart.w) * Real2(distance);
  Real2 newEnd   = end   + Real2(pnEnd  .z, pnEnd  .w) * Real2(distance);
  
  Real2 vector1 = control1 - start;
  Real2 vector2 = control2 - end  ;
  Real2 newControl1 = newStart + Real2(1 + distance / Real(3 * vector1.magnitude())) * vector1;
  Real2 newControl2 = newEnd   + Real2(1 + distance / Real(3 * vector2.magnitude())) * vector2;
  
  return new Bezier2(newStart, newControl1, newControl2, newEnd);
}

namespace {
  Real2 _L(Real2 a, Real2 b, Real t) {
    return a + (b - a) * Real2(t);
  }
  
  Real2 _Q(Real2 a, Real2 b, Real2 c, Real t) {
    return _L(_L(a, b, t), _L(b, c, t), t);
  }
}

Ref<Path2 &> Bezier2::split(Real tStart, Real tEnd) {
  Real2 newStart = point(tStart);
  Real2 newEnd   = point(tEnd  );
  
  // Split from tStart-1
  Real2 n0 = newStart;
  Real2 n1 = _Q(control1, control2, end, tStart);
  Real2 n2 = _L(control2, end, tStart);
  Real2 n3 = end;
  
  // Normalize the tEnd for the previously split curve
  Real t = (tEnd - tStart) / Real(1 - tStart);
  
  // Split from tStart-tEnd
  Real2 v0 = newStart;
  Real2 v1 = _L(n0, n1, t);
  Real2 v2 = _Q(n0, n1, n2, t);
  Real2 v3 = newEnd;
  
  return new Bezier2(v0, v1, v2, v3);
}

void Bezier2::split(Real t, Ref<Path2 &> &lhs, Ref<Path2 &> &rhs) {
  Real2 p = point(t);
  
  Real2 n0 = start;
  Real2 n2 = _L(start, control1, t);
  Real2 n1 = _Q(start, control1, control2, t);
  Real2 n3 = p;
  
  Real2 v0 = p;
  Real2 v1 = _Q(control1, control2, end, t);
  Real2 v2 = _L(control2, end, t);
  Real2 v3 = end;
  
  lhs = new Bezier2(n0, n1, n2, n3);
  rhs = new Bezier2(v0, v1, v2, v3);
}

Real2 Bezier2::project(Real2 point) {
  return this->point(inverse(point));
}

Real2 Bezier2::point(Real t) {
  Real _t = 1 - t;
  
  Real  t2 =  t  *  t;
  Real  t3 =  t2 *  t;
  Real _t2 = _t  * _t;
  Real _t3 = _t2 * _t;
  
  return
    start    * Real2(_t3) +
    control1 * Real2(3 * _t2 * t) +
    control2 * Real2(3 * _t * t2) +
    end      * Real2(t3);
}

Real2 Bezier2::normal(Real t) {
  Real _t = 1 - t;
  
  Real  t2 =  t  *  t;
  Real  t3 =  t2 *  t;
  Real _t2 = _t  * _t;
  Real _t3 = _t2 * _t;
  
  Real2 tangent =
    start    * Real2(-3 * _t2) +
    control1 * Real2(3 * _t2 - 6 * _t * t) +
    control2 * Real2(6 * _t * t - 3 * t2) +
    end      * Real2(3 * t2);
  
  return tangent.normalized().rightPerpendicular();
}

Real Bezier2::inverse(Real2 point) {
  _getPointNormals();
  
  // Find the closest point
  int closest = 0;
  Real min = start.squareDistance(point);
  for (int i = 1; i < _lengths.count(); i++) {
    Real dist = Real2(_lengths[i]).squareDistance(point);
    if (dist < min) {
      min = dist;
      closest = i;
    }
  }
  
  // Interpolate on the left and right sides
  Real t = Real(closest) / Real((int)_lengths.count() - 1);
  
  // Left
  Real t1 = t;
  Real min1 = min;
  if (closest > 0) {
    Line2 line { Real2(_lengths[closest - 1]), Real2(_lengths[closest]) };
    t1 = Real(closest - 1 + line.inverse(point)) / Real((int)_lengths.count() - 1);
    min1 = this->point(t1).squareDistance(point);
  }
  
  // Right
  Real t2 = t;
  Real min2 = min;
  if (closest < _lengths.count() - 1) {
    Line2 line { Real2(_lengths[closest]), Real2(_lengths[closest + 1]) };
    t2 = Real(closest + line.inverse(point)) / Real((int)_lengths.count() - 1);
    min2 = this->point(t2).squareDistance(point);
  }
  
  // Return the closest point
  return min1 < min2 ? t1 : t2;
}

Ref<Path2 &> Bezier2::pushedBack(bool start, Real amount) {
  if (start) {
    Real2 normal = (this->control1 - this->start).normalized();
    return new Bezier2(
      this->start    + normal * Real2(amount),
      this->control1 + normal * Real2(amount * Real(0.5)),
      this->control2,
      this->end
    );
  } else {
    Real2 normal = (this->control2 - this->end).normalized();
    return new Bezier2(
      this->start,
      this->control1,
      this->control2 + normal * Real2(amount * Real(0.5)),
      this->end      + normal * Real2(amount)
    );
  }
}

Real Bezier2::_lengthLookup(Real t) {
  // Binary search for the appropriate length
  Real len = t * _lengths.last().z;
  int start = 0, end = _lengths.count();
  while (end - start > 1) {
    int mid = (start + end) / 2;
    if (_lengths[mid].z < len)
      start = mid;
    else
      end = mid;
  }
  
  // Interpolate between the two points
  Real remaining;
  if (end == _lengths.count())
    remaining = 0;
  else {
    Real t1 = _lengths[start].z;
    Real t2 = _lengths[end  ].z;
    remaining = (len - t1) / (t2 - t1);
  }
  return (start + remaining) / Real((int)_lengths.count() - 1);
}

List<Real4> Bezier2::_pointNormals() {
  // Generate the length table
  int count =
       start.distance(control1) +
    control1.distance(control2) +
    control2.distance(end);
  if (count < 2)
    count = 2;
  
  // Find the lengths along the curve
  Real length = 0;
  Real2 previous = start;
  _lengths.append({ start, 0 });
  for (int i = 1; i <= count; i++) {
    Real t = Real(i) / Real(count);
    Real2 p = point(t);
    length += p.distance(previous);
    _lengths.append({ p, length });
    previous = p;
  }
  
  // Generate evenly-spaced points
  List<Real4> points;
  count = length;
  if (count < 2)
    count = 2;
  for (int i = 0; i <= count; i++) {
    Real t = _lengthLookup(Real(i) / Real(count));
    Real2 p = point(t);
    Real2 n = normal(t);
    points.append(Real4(p.x, p.y, n.x, n.y));
  }
  
  return points;
}
