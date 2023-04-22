/**
 * @file RadiusPath2.cpp
 * @brief A 2D path with a constant radius around it.
 * @date April 19, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/RadiusPath2.h>
#include <CityBuilder/Storage/Ref.h>
USING_NS_CITY_BUILDER

bool RadiusPath2::intersectionTest(const RadiusPath2 &other) {
  // Check bounds first
  if (!_bounds.intersects(other._bounds))
    return true;
  
  // Check for crossing validity
  {
    Path2 *lhs = path.offset(-radius);
    Path2 *rhs = path.offset( radius);
    
    Path2 *_lhs = other.path.offset(-other.radius);
    Path2 *_rhs = other.path.offset( other.radius);
    
    List<Real2> lhs_lhs = lhs->intersections(*_lhs);
    List<Real2> lhs_rhs = lhs->intersections(*_rhs);
    List<Real2> rhs_lhs = rhs->intersections(*_lhs);
    List<Real2> rhs_rhs = rhs->intersections(*_rhs);
    
    delete lhs;
    delete rhs;
    delete _lhs;
    delete _rhs;
    
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
  Real2 point = path.project(center);
  Real distance = (point - center).magnitude();
  return distance < radius + this->radius;
}


/*
  // Determine where the two curves cross
  List<Real2> intersections = path.intersections(other.path);
  
  
  // Split the other curve into individual intersection segments
  List<Path2 *> segments { };
  
  if (intersections.count() < 2) {
    segments.append(&other.path);
  } else {
    // Split all the segments at the half way points between intersections
    Real split = 0;
    Real lastT = path.inverse(intersections[0]);
    
    for (intptr_t i = 1; i < intersections.count(); i++) {
      Real newT = path.inverse(intersections[i]);
      Real t = (lastT + newT) * Real(0.5);
      
      segments.append(path.split(split, t));
      
      split = t;
      lastT = newT;
    }
    
    segments.append(path.split(split, 1));
  }
  
}




  
  // Test each of the sub curves individually as they will all yield at most
  // one crossing point
  List<Intersection> result { };
  
  // Get the radius paths outer and inner curves
  Path2 *lhs    = path.offset(-radius);
  Path2 *center = &path;
  Path2 *rhs    = path.offset( radius);
  
  for (Path2 *_segment : segments) {
    Path2 &segment = *_segment;
    
    // We know that the segment intersects the center curve at most once
    
    // Check how the segment intersects with the curves
    List<Real2> _lhs    =    lhs->intersections(segment);
    List<Real2> _center = center->intersections(segment);
    List<Real2> _rhs    =    rhs->intersections(segment);
    
    // Compile the intersections
    struct Point {
      enum {
        lhs,
        center,
        rhs,
      } curve;
      Real t;
      
      // For sorting
      bool operator < (const Point &other) const { return t < other.t; }
    };
    List<Point> outer { };
    for (Real2 i : _lhs   ) outer.append({ Point::lhs   , path.inverse(i) });
    for (Real2 i : _center) outer.append({ Point::center, path.inverse(i) });
    for (Real2 i : _rhs   ) outer.append({ Point::rhs   , path.inverse(i) });
    outer.sort();
    
    // Find the center intersection point
    intptr_t index = -1;
    for (intptr_t i = 0; i < outer.count(); i++)
      if (outer[i].curve == Point::center) {
        index = i;
        break;
      }
    
    if (index == -1) {
      // No center intersection point
      continue;
    } else {
      // Check how the curves before and after interact
      if (index == 0 || index == outer.count() - 1) {
        // The segment ends at the center
        result.append({
          Intersection::end,
          path.point(outer[index].t)
        });
      } else if (outer[index - 1].curve != outer[index + 1].curve) {
        // The segment crosses the center
        result.append({
          Intersection::crossing,
          path.point(outer[index].t)
        });
      } else {
        // The segment reflects off the center
        result.append({
          Intersection::reflection,
          path.point(outer[index].t)
        });
      }
    }
  }
  
  // Check that the bounding curves are not intersecting
  for (Intersection &i : result) {
    if (i.type == Intersection::crossing)
      
  }
  
  return result;
}




namespace {
  struct IntersectionPoint {
    enum {
      lhs,
      center,
      rhs
    } path;
    enum {
      _lhs,
      _center,
      _rhs
    } self;
    Real2 point;
    Real t;
    enum {
      brush,
      crossing,
      reflection,
      end,
    } type;
    
    // For sorting
    bool operator < (const IntersectionPoint &other) const {
      return point.x < other.point.x;
    }
  };
  
  void classifyIntersections(List<IntersectionPoint> &points) {
    for (intptr_t i = 1; i < points.count(); i++) {
      if (points[i].path == IntersectionPoint::center) {
        // Check how it passes through the center
        if (i + 1 >= points.count()) {
          // Ends at the center
          lhsIntersections.append({
            Intersection::end,
            points[i].point
          });
        } else if (points[i - 1].path != IntersectionPoint::center &&
                   points[i + 1].path != IntersectionPoint::center) {
          // The point either completely crosses through our goes back on itself
          lhsIntersections.append({
            points[i - 1].path == points[i + 1].path ?
              Intersection::reflection :
              Intersection::crossing   ,
            points[i].point
          });
          i++;
        } else {
          // Collect all the center points
          
        }
      }
  }
  }
}
*/

/*
  
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
  List<Real2>    lhs_lhs = lhs1  ->intersections(*lhs2);
  List<Real2> center_lhs = center1.intersections(*lhs2);
  List<Real2>    rhs_lhs = rhs1  ->intersections(*lhs2);
  
  List<Real2>    lhs_center = lhs1  ->intersections(center2);
  List<Real2> center_center = center1.intersections(center2);
  List<Real2>    rhs_center = rhs1  ->intersections(center2);
  
  List<Real2>    lhs_rhs = lhs1  ->intersections(*rhs2);
  List<Real2> center_rhs = center1.intersections(*rhs2);
  List<Real2>    rhs_rhs = rhs1  ->intersections(*rhs2);
  
  // Track all intersection points
  List<IntersectionPoint> _lhs { };
  for (const Real2 &point :    lhs_lhs) _lhs.append({ IntersectionPoint::lhs   , IntersectionPoint::_lhs, point, lhs2->inverse(point) });
  for (const Real2 &point : center_lhs) _lhs.append({ IntersectionPoint::center, IntersectionPoint::_lhs, point, lhs2->inverse(point) });
  for (const Real2 &point :    rhs_lhs) _lhs.append({ IntersectionPoint::rhs   , IntersectionPoint::_lhs, point, lhs2->inverse(point) });
  
  List<IntersectionPoint> _center { };
  for (const Real2 &point :    lhs_center) _center.append({ IntersectionPoint::lhs   , IntersectionPoint::_center, point, center2.inverse(point) });
  for (const Real2 &point : center_center) _center.append({ IntersectionPoint::center, IntersectionPoint::_center, point, center2.inverse(point) });
  for (const Real2 &point :    rhs_center) _center.append({ IntersectionPoint::rhs   , IntersectionPoint::_center, point, center2.inverse(point) });
  
  List<IntersectionPoint> _rhs { };
  for (const Real2 &point :    lhs_rhs) _rhs.append({ IntersectionPoint::lhs   , IntersectionPoint::_rhs, point, rhs2->inverse(point) });
  for (const Real2 &point : center_rhs) _rhs.append({ IntersectionPoint::center, IntersectionPoint::_rhs, point, rhs2->inverse(point) });
  for (const Real2 &point :    rhs_rhs) _rhs.append({ IntersectionPoint::rhs   , IntersectionPoint::_rhs, point, rhs2->inverse(point) });
  
  // Sort the intersection points
     _lhs.sort();
  _center.sort();
     _rhs.sort();
  
  // Determine the intersection types
  classifyIntersections(_lhs   );
  classifyIntersections(_center);
  classifyIntersections(_rhs   );
  
  // Compile a total list of the intersections
  List<IntersectionPoint> total { };
  total.appendList(_lhs   );
  total.appendList(_center);
  total.appendList(_rhs   );
  total.sort();
  
  // Pick the final intersections
  List<Intersection> intersections { };
  
  return intersections;
}

In order for a curve to cross here are the preconditions:
- the center curve must go completely through the other curve
- the two side curves must also go completely through the other curve

*/
