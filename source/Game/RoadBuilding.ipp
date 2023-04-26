/**
 * @file RoadBuilding.ipp
 * @brief The road-building code for the game.
 * @date April 25, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Game.h>
#include <CityBuilder/Rendering/DynamicMesh.h>
USING_NS_CITY_BUILDER

namespace {
  struct Road_Building {
    /// The current point where the road is being built.
    Real3 point;
    
    /// The start point of the road.
    Real3 start;
    
    /// The direction of the control point of the road.
    Real3 control;
    
    /// The current build stage.
    /// \remarks
    ///   0: Find a starting point.
    ///   1: Find a control point.
    ///   2: Find an end point.
    int stage = 0;
    
    /// The ID of the listener for mouse clicks.
    int clickListener;
    
    /// The ID of the listener for cancel events.
    int cancelListener;
    
    /// The road being built.
    RoadDef *road;
    
    /// A display of the road being built.
    Resource<DynamicMesh> display;
    
    /// Whether or not the display is visible.
    bool displayVisible = false;
    
    /// Whether or not the road being built is valid.
    bool roadValid = false;
    
    /// The last shown path.
    Ref<Path2 &> path;
    
    Road_Building(RoadDef *road) : road(road) {
      // Create the display
      display = new DynamicMesh();
      
      // Listen for cancel events
      cancelListener = Input::onCancel += [this]() {
        switch (stage) {
        case 0:
          // Cancel the road building
          Game::instance().cancel();
          break;
          
        case 1:
          stage = 0;
          move(point);
          break;
        
        case 2:
          stage = 1;
          move(point);
          break;
        }
      };
      
      // Listen for mouse clicks
      clickListener = Input::onPrimaryMouseDown += [this]() {
        switch (stage) {
        case 0:
          // Set the start point
          if (displayVisible && roadValid) {
            start = point;
            stage = 1;
          }
          break;
        
        case 1:
          if (Input::shiftDown()) {
            // Add the control point
            if (point.squareDistance(control) < 0.333333333333) {
              // No control direction selected
              break;
            }
            
            control = (point - start).normalized();
            stage = 2;
          } else {
            // Attempt to build the road
            if (displayVisible && roadValid)
              if (Game::instance().roads().build(this->road, path)) {
                Game::instance().roads().update();
                start = point;
                stage = 1;
                path = nullptr;
                move(point);
              }
          }
          break;
        
        case 2:
          // Attempt to build the road
            if (displayVisible && roadValid)
              if (Game::instance().roads().build(this->road, path)) {
                Game::instance().roads().update();
                start = point;
                
                // Find the new control point direction
                Real2 p = { start.x, start.z };
                if (path->start.squareDistance(p) < path->end.squareDistance(p)) {
                  Real4 normal = path->pointNormals().first();
                  control = { normal.w, 0, -normal.z };
                } else {
                  Real4 normal = path->pointNormals().last();
                  control = { -normal.w, 0, normal.z };
                }
                
                stage = 2;
                path = nullptr;
                move(point);
              }
          break;
        
        default:
          break;
        }
      };
    }
    
    ~Road_Building() {
      Input::onPrimaryMouseDown -= clickListener;
      Input::onCancel -= cancelListener;
    }
    
    /// Move the display to the given point.
    /// \param[in] origin
    ///   The point to move the display to.
    void move(Real3 origin) {
      const Real scale = 0.333333333333;
      
      path = nullptr;
      
      // Check for validity
      bool arc = false;
      bool flipped = false;
      switch (stage) {
      case 0:
        // Check for a valid start point
        roadValid = Game::instance().roads().validate(road, { origin.x, origin.z });
        break;
      
      case 1:
        // Check for a valid line
        if (start.squareDistance(origin) < scale)
          roadValid = false;
        else {
          path = new Line2({ start.x, start.z }, { origin.x, origin.z });
          roadValid = Game::instance().roads().validate(road, path);
        }
        break;
      
      case 2:
        // Check for a valid curve
        if (start.squareDistance(origin) < scale)
          roadValid = false;
        else if (control.dot((origin - start).normalized()) < 0.2) {
          roadValid = false;
          path = new Line2({ start.x, start.z }, { origin.x, origin.z });
        } else {
          // Finding the control point of the circle such that the control point
          // is equidistant from the end points:
          //
          //            point
          //          /
          //       /  | c
          //    /     |
          //   +-------
          //   |   b
          // a |
          //   |
          //  start
          //
          // (note that a is the vertical distance from point to start, not
          //  just the line segment it's shown next to)
          //
          // a - c = sqrt(b^2 + c^2)
          // a^2 - 2ac + c^2 = b^2 + c^2
          // a^2 - b^2 = 2ac
          // c = (a^2 - b^2) / 2a
          //
          // projection = (point - start) - direction
          // a = projection magnitude
          // b = (point - projection) magnitude
          // c = (a^2 - b^2) / 2a
          // controlPoint = start + direction * (a - c)
          Real3 projection = (origin - start).project(control);
          Real a = projection.magnitude();
          Real b = (origin - start - projection).magnitude();
          Real c = (a * a - b * b) / (Real(2) * a);
          Real3 controlPoint = start + control * Real3(a - c);
          
          // Check the orientation to see if we need to flip the start and end
          // points
          Real2 direction = { control.x, control.z };
          Real3 p = (origin - start).normalized();
          Real orientation = direction.leftPerpendicular().dot({ p.x, p.z });
          if (orientation.abs() < 0.1) {
            // Pretty much a line
            roadValid = false;
            path = new Line2({ start.x, start.z }, { origin.x, origin.z });
          } else {
            Real2 _start = { start.x, start.z };
            Real2 _end = { origin.x, origin.z };
            
            Bezier2 *arcPath = new Bezier2(
              orientation.isPositive() ? _start : _end,
              { controlPoint.x, controlPoint.z },
              orientation.isPositive() ? _end : _start
            );
            path = arcPath;
            flipped = orientation.isNegative();
            
            // if (arcPath->radius() < road->dimensions.x * scale + Real(0.1)) {
            //   // Turns in on itself
            //   path = new Line2({ start.x, start.z }, { origin.x, origin.z });
            //   roadValid = false;
            //   flipped = false;
            // } else {
              roadValid = Game::instance().roads().validate(road, path);
              arc = true;
            // }
          }
        }
        break;
      }
      
      bool curvePoint = stage == 1 && Input::shiftDown();
      
      const Color4 hoverColor  = curvePoint ? Color4(120, 255, 100, 100) : roadValid ? Color4(100, 155, 255, 100) : Color4(255, 120, 100, 100);
      const Color4 hoverColor0 = curvePoint ? Color4(120, 255, 100,   0) : roadValid ? Color4(100, 155, 255,   0) : Color4(255, 120, 100,   0);
      Real2 radius = Real2(road->dimensions.x * Real(0.5 * scale));
      point = origin;
      
      List<DynamicMesh::Vertex> vertices { };
        List<int> indices { };
      
      // Update the mesh
      switch (stage) {
      case 0: {
        // Selection point
        vertices.append({ origin, hoverColor0 });
        for (int i = 0; i < 32; i++) {
          Real2 xz = Angle::cosSin(i * Angle::pi2 / 32) * radius;
          vertices.append({ origin + Real3(xz.x, 0, xz.y), hoverColor });
        }
        indices.append(0);
        indices.append(32);
        indices.append(1);
        for (int i = 0; i < 31; i++) {
          indices.append(0);
          indices.append(i + 1);
          indices.append(i + 2);
        }
      } break;
      
      case 1: {
        if (start.squareDistance(origin) < scale) {
          // Single selection point
          vertices.append({ start, hoverColor0 });
          for (int i = 0; i < 32; i++) {
            Real2 xz = Angle::cosSin(i * Angle::pi2 / 32) * radius;
            vertices.append({ start + Real3(xz.x, 0, xz.y), hoverColor });
          }
          indices.append(0);
          indices.append(32);
          indices.append(1);
          for (int i = 0; i < 31; i++) {
            indices.append(0);
            indices.append(i + 1);
            indices.append(i + 2);
          }
          break;
        }
        
        // Straight line
        Line2 &line = *(Line2 *)&*path;
        Real4 pointNormal = line.pointNormals().first();
        Angle angle = -Angle({ pointNormal.z, pointNormal.w });
        
        if (curvePoint) {
          // Start end cap
          vertices.append({ start, hoverColor0 });
          for (int i = -2; i < 17 + 2; i++) {
            Real2 xz = Angle::cosSin(angle + i * Angle::pi2 / 32) * radius;
            vertices.append({ start + Real3(xz.x, 0, xz.y), hoverColor });
          }
          for (int i = 0; i < 16 + 4; i++) {
            indices.append(0);
            indices.append(i + 1);
            indices.append(i + 2);
          }
          
          // Extend forward to show the curve direction
          Real2 direction = Real2(pointNormal.z, pointNormal.w).leftPerpendicular();
          Real3 forward = Real3(direction.x, 0, direction.y) * Real3(2 * radius.x);
          Real2 xz = Angle::cosSin(angle) * radius * Real2(0.0);
          Real3 point = start + forward;
          vertices.append({ point, hoverColor0 });
          vertices.append({ point - Real3(xz.x, 0, xz.y), hoverColor });
          vertices.append({ point + Real3(xz.x, 0, xz.y), hoverColor });
          
          indices.append(0);
          indices.append(1);
          indices.append(20 + 4);
          indices.append(20 + 4);
          indices.append(18 + 4);
          indices.append(0);
          
          indices.append(18 + 4);
          indices.append(19 + 4);
          indices.append(0);
          indices.append(19 + 4);
          indices.append(17 + 4);
          indices.append(0);
        } else {
          // Start end cap
          vertices.append({ start, hoverColor0 });
          for (int i = 0; i < 17; i++) {
            Real2 xz = Angle::cosSin(angle + i * Angle::pi2 / 32) * radius;
            vertices.append({ start + Real3(xz.x, 0, xz.y), hoverColor });
          }
          for (int i = 0; i < 16; i++) {
            indices.append(0);
            indices.append(i + 1);
            indices.append(i + 2);
          }
          
          // End end cap
          pointNormal = line.pointNormals().last();
          angle = Angle({ pointNormal.z, pointNormal.w });
          vertices.append({ point, hoverColor0 });
          for (int i = 0; i < 17; i++) {
            Real2 xz = Angle::cosSin(angle + i * Angle::pi2 / 32) * radius;
            vertices.append({ point + Real3(xz.x, 0, xz.y), curvePoint ? hoverColor0 : hoverColor });
          }
          for (int i = 0; i < 16; i++) {
            indices.append(18);
            indices.append(i + 19);
            indices.append(i + 20);
          }
          
          // Connect the two lines
          indices.append(0);
          indices.append(1);
          indices.append(35);
          indices.append(35);
          indices.append(18);
          indices.append(0);
          
          indices.append(18);
          indices.append(19);
          indices.append(0);
          indices.append(19);
          indices.append(17);
          indices.append(0);
        }
      } break;
      
      case 2: {
        if (start.squareDistance(origin) < scale) {
          // Single selection point
          vertices.append({ start, hoverColor0 });
          for (int i = 0; i < 32; i++) {
            Real2 xz = Angle::cosSin(i * Angle::pi2 / 32) * radius;
            vertices.append({ start + Real3(xz.x, 0, xz.y), hoverColor });
          }
          indices.append(0);
          indices.append(32);
          indices.append(1);
          for (int i = 0; i < 31; i++) {
            indices.append(0);
            indices.append(i + 1);
            indices.append(i + 2);
          }
          break;
        }
        
        Real4 pointNormal = flipped ?
          path->pointNormals().last() :
          path->pointNormals().first();
        Angle angle = Angle({ pointNormal.z, pointNormal.w });
        if (!flipped)
          angle = -angle;
        
        // Start end cap
        vertices.append({ start, hoverColor0 });
        for (int i = 0; i < 17; i++) {
          Real2 xz = Angle::cosSin(angle + i * Angle::pi2 / 32) * radius;
          vertices.append({ start + Real3(xz.x, 0, xz.y), hoverColor });
        }
        for (int i = 0; i < 16; i++) {
          indices.append(0);
          indices.append(i + 1);
          indices.append(i + 2);
        }
        
        // Draw the arc itself
        bool first = true;
        for (Real4 pointNormal : path->pointNormals()) {
          int start = vertices.count();
          
          Real3 point = { pointNormal.x, 0, pointNormal.y };
          Real3 normal = { pointNormal.z, 0, pointNormal.w };
          normal *= radius.x;
          vertices.append({ point - normal, hoverColor  });
          vertices.append({ point         , hoverColor0 });
          vertices.append({ point + normal, hoverColor  });
          
          if (first)
            first = false;
          else {
            indices.append(start - 3);
            indices.append(start + 0);
            indices.append(start + 1);
            
            indices.append(start - 3);
            indices.append(start + 1);
            indices.append(start - 2);
            
            indices.append(start - 2);
            indices.append(start + 1);
            indices.append(start + 2);
            
            indices.append(start - 2);
            indices.append(start + 2);
            indices.append(start - 1);
          }
        }
        
        // End cap
        pointNormal = flipped ?
          path->pointNormals().first() :
          path->pointNormals().last()  ;
        angle = Angle({ pointNormal.z, pointNormal.w });
        if (flipped)
          angle = -angle;
        
        // Start end cap
        int vertexCount = vertices.count();
        vertices.append({ origin, hoverColor0 });
        for (int i = 0; i < 17; i++) {
          Real2 xz = Angle::cosSin(angle + i * Angle::pi2 / 32) * radius;
          vertices.append({ origin + Real3(xz.x, 0, xz.y), hoverColor });
        }
        for (int i = 0; i < 16; i++) {
          indices.append(vertexCount);
          indices.append(vertexCount + i + 1);
          indices.append(vertexCount + i + 2);
        }
        
        // Show the curve point
        // if (arc) {
          // Bezier2 &arc = *(Bezier2 *)&*path;
          // Real3 point = { arc.control.x, 0, arc.control.y };
          // int start = vertices.count();
          // vertices.append({ point, hoverColor0 });
          // for (int i = 0; i < 16; i++) {
          //   Real2 xz = Angle::cosSin(i * Angle::pi2 / 16);
          //   vertices.append({ point + Real3(xz.x, 0, xz.y), hoverColor });
          // }
          // indices.append(start + 0);
          // indices.append(start + 16);
          // indices.append(start + 1);
          // for (int i = 0; i < 15; i++) {
          //   indices.append(start + 0);
          //   indices.append(start + i + 1);
          //   indices.append(start + i + 2);
          // }
        // }
      }
      }
      
      // Load the mesh
      display->add(vertices, indices);
      display->load();
    }
  };
}
