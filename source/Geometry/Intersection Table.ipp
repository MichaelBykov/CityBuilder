/**
 * @file Intersection Table.ipp
 * @brief A table of various path-path intersection tests.
 * @date April 19, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Geometry/Path2.h>

namespace {
namespace _Internal_Intersection_Table_ {
  USING_NS_CITY_BUILDER
  
  
  
/* -------------------------------------------------------------------------- *\
|                                                                              |
| Line - X                                                                     |
|                                                                              |
\* -------------------------------------------------------------------------- */
  
  List<Real2> line_line(Line2 &line1, const Line2 &line2) {
    Real2 vector1 = line1.end - line1.start;
    Real2 vector2 = line2.end - line2.start;
    Real determinant = vector1.x * vector2.y - vector1.y * vector2.x;
    
    // Check that the lines aren't parallel
    if (determinant.approxZero())
      return { };
    
    // Find the intersection point
    Real2 diff = line2.start - line1.start;
    Real s = (diff.x * vector2.y - diff.y * vector2.x) / determinant;
    Real t = (diff.x * vector1.y - diff.y * vector1.x) / determinant;
    
    // Check that the intersection point is on both lines
    if (s < 0 || s > 1 || t < 0 || t > 1)
      return { };
    
    return { line1.start + vector1 * Real2(s) };
  }
  
  List<Real2> line_arc(Line2 &line, Arc2 &arc) {
    Real2 linePoint = line.project(arc.center());
    
    Real offset = linePoint.squareDistance(arc.center()) - arc.radius().square();
    
    if (offset.approxZero()) {
      // The line is tangent to the arc
      return { linePoint };
    }
    
    if (offset > 0) {
      // The line is outside the arc
      return { };
    }
    
    // Find the two intersection points of the line and the arc
    Real2 center = (line.end - line.start).project(arc.center()) + line.start;
    Real2 cos = Real2((Real(1) - (center - arc.center()).squareMagnitude()).sqrt());
    Real2 normal = (center - arc.center()).normalized();
    Real2 point1 = center + normal * cos;
    Real2 point2 = center - normal * cos;
    
    // Check that the intersection points are on the line
    bool p1 =
      (point1 - line.start).dot(line.end - line.start) >= 0 &&
      (point1 - line.end  ).dot(line.start - line.end) >= 0;
    bool p2 =
      (point1 - line.start).dot(line.end - line.start) >= 0 &&
      (point1 - line.end  ).dot(line.start - line.end) >= 0;
    
    // Check that the intersection points are on the arc
    bool a1 = arc.project(point1).squareDistance(point1).approxZero();
    bool a2 = arc.project(point2).squareDistance(point2).approxZero();
    
    // Return the found valid intersection points
    List<Real2> points;
    if (p1 && a1) points.append(point1);
    if (p2 && a2) points.append(point2);
    return points;
  }
  
  
  
/* -------------------------------------------------------------------------- *\
|                                                                              |
| Arc - X                                                                      |
|                                                                              |
\* -------------------------------------------------------------------------- */
  
  List<Real2> arc_arc(Arc2 &arc1, Arc2 &arc2) {
    Real distance = arc1.center().distance(arc2.center());
    
    if ((distance - arc1.radius() - arc2.radius()).approxZero()) {
      // The arcs are tangent
      Real2 point =
        (arc2.center() - arc1.center()).normalized() * Real2(arc1.radius()) +
         arc1.center();
      if (arc1.project(point).squareDistance(point).approxZero() &&
          arc2.project(point).squareDistance(point).approxZero())
        return { point };
      else
        // Not on both of the arcs
        return { };
    } else if (distance > arc1.radius() + arc2.radius())
      // The arcs are fully separated
      return { };
    
    // Find the intersection points
    Real2 center =
      (arc2.center() - arc1.center()).normalized() *
      Real2(arc1.radius() + (distance - arc1.radius() - arc2.radius()) * Real(0.5)) +
      arc1.center();
    Real2 cos = Real2((Real(1) - (center - arc1.center()).squareMagnitude()).sqrt());
    Real2 normal = (center - arc1.center()).normalized();
    Real2 point1 = center + normal * cos;
    Real2 point2 = center - normal * cos;
    
    // Check that the intersection points are on the first arc
    bool a1 = arc1.project(point1).squareDistance(point1).approxZero();
    bool a2 = arc1.project(point2).squareDistance(point2).approxZero();
    
    // Check that the intersection points are on the second arc
    bool b1 = arc2.project(point1).squareDistance(point1).approxZero();
    bool b2 = arc2.project(point2).squareDistance(point2).approxZero();
    
    // Return the found valid intersection points
    List<Real2> points;
    if (a1 && b1) points.append(point1);
    if (a2 && b2) points.append(point2);
    return points;
  }
  
  
  
} // namespace _Internal_Intersection_Table_
} // namespace
