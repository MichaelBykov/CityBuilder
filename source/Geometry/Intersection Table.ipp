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
  
  List<Real2> line_bezier(Line2 &line, Bezier2 &bezier, int iteration) {
    // Compute the intersection between a line and a bezier curve through
    // subdivision
    if (line.bounds().intersects(bezier.bounds())) {
      if (iteration > 64 || line.bounds().size.approxZero().verticalAnd()) {
        // Intersection point
        return { (line.start + line.end) * Real2(0.5) };
      } else if (bezier.isDegenerate()) {
        // Line-line intersection
        Line2 line2(bezier.start, bezier.end);
        return line_line(line, line2);
      } else if (bezier.bounds().size.approxZero().verticalAnd()) {
        // Intersection point
        return { (bezier.start + bezier.end) * Real2(0.5) };
      } else {
        // Split
        Ref<Path2 &> line1, line2, curve1, curve2;
        line.split(0.5, line1, line2);
        bezier.split(0.5, curve1, curve2);
        
        // Compute all the intersections
        List<Real2> intersections;
        intersections.appendList(line_bezier(
          static_cast<Line2 &>(*line1),
          static_cast<Bezier2 &>(*curve1),
          iteration + 1
        ));
        intersections.appendList(line_bezier(
          static_cast<Line2 &>(*line1),
          static_cast<Bezier2 &>(*curve2),
          iteration + 1
        ));
        intersections.appendList(line_bezier(
          static_cast<Line2 &>(*line2),
          static_cast<Bezier2 &>(*curve1),
          iteration + 1
        ));
        intersections.appendList(line_bezier(
          static_cast<Line2 &>(*line2),
          static_cast<Bezier2 &>(*curve2),
          iteration + 1
        ));
        
        // Filter out the duplicates
        intersections.sort([](const Real2 &lhs, const Real2 &rhs) {
          return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
        });
        for (int i = 1; i < intersections.count(); i++)
          if (intersections[i].approxEqual(intersections[i - 1]).verticalAnd()) {
            intersections.remove(i);
            i--;
          }
        
        return intersections;
      }
    }
    
    return { };
  }
  
  
  
/* -------------------------------------------------------------------------- *\
|                                                                              |
| Bezier - X                                                                   |
|                                                                              |
\* -------------------------------------------------------------------------- */
  
  List<Real2> bezier_bezier(Bezier2 &bezier1, Bezier2 &bezier2, int iteration) {
    // Compute the intersection between two bezier curves through subdivision
    if (bezier1.bounds().intersects(bezier2.bounds())) {
      if (iteration > 64 || bezier1.bounds().size.approxZero().verticalAnd()) {
        // Intersection point
        return { (bezier1.start + bezier1.end) * Real2(0.5) };
      } else if (bezier2.bounds().size.approxZero().verticalAnd()) {
        // Intersection point
        return { (bezier2.start + bezier2.end) * Real2(0.5) };
      } else if (bezier1.isDegenerate() && bezier2.isDegenerate()) {
        // Line-line intersection
        Line2 a(bezier1.start, bezier1.end);
        Line2 b(bezier2.start, bezier2.end);
        return line_line(a, b);
      } else {
        // Split
        Ref<Path2 &> bezier1_1, bezier1_2, bezier2_1, bezier2_2;
        bezier1.split(0.5, bezier1_1, bezier1_2);
        bezier2.split(0.5, bezier2_1, bezier2_2);
        
        // Compute all the intersections
        List<Real2> intersections;
        intersections.appendList(bezier_bezier(
          static_cast<Bezier2 &>(*bezier1_1),
          static_cast<Bezier2 &>(*bezier2_1),
          iteration + 1
        ));
        intersections.appendList(bezier_bezier(
          static_cast<Bezier2 &>(*bezier1_1),
          static_cast<Bezier2 &>(*bezier2_2),
          iteration + 1
        ));
        intersections.appendList(bezier_bezier(
          static_cast<Bezier2 &>(*bezier1_2),
          static_cast<Bezier2 &>(*bezier2_1),
          iteration + 1
        ));
        intersections.appendList(bezier_bezier(
          static_cast<Bezier2 &>(*bezier1_2),
          static_cast<Bezier2 &>(*bezier2_2),
          iteration + 1
        ));
        
        // Filter out the duplicates
        intersections.sort([](const Real2 &lhs, const Real2 &rhs) {
          return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
        });
        for (int i = 1; i < intersections.count(); i++)
          if (intersections[i].approxEqual(intersections[i - 1]).verticalAnd()) {
            intersections.remove(i);
            i--;
          }
        
        return intersections;
      }
    }
    
    return { };
  }
  
  
  
} // namespace _Internal_Intersection_Table_
} // namespace
