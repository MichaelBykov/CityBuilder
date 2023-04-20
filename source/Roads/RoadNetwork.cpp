/**
 * @file RoadNetwork.cpp
 * @brief The road network manager.
 * @date April 15, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Roads/RoadNetwork.h>
#include <CityBuilder/Rendering/Uniforms.h>
USING_NS_CITY_BUILDER

namespace {
  ProfileMesh dividerCrossTrafficMesh = {{
    ProfilePoint {
      .position = { 0, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.05,
      .type = ProfilePoint::Type::move
    },
    ProfilePoint {
      .position = { 0.2, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.2,
      .type = ProfilePoint::Type::move
    },
  }};
  
  ProfileMesh dividerCrossEdgeMesh = {{
    ProfilePoint {
      .position = { 0, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.3,
      .type = ProfilePoint::Type::move
    },
    ProfilePoint {
      .position = { 0.2, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.45,
      .type = ProfilePoint::Type::move
    },
  }};
  
  ProfileMesh dividerNormalMesh = {{
    ProfilePoint {
      .position = { 0, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.55,
      .type = ProfilePoint::Type::move
    },
    ProfilePoint {
      .position = { 0.2, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.7,
      .type = ProfilePoint::Type::move
    },
  }};
  
  ProfileMesh dividerEdgeMesh = {{
    ProfilePoint {
      .position = { 0, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.8,
      .type = ProfilePoint::Type::move
    },
    ProfilePoint {
      .position = { 0.2, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.95,
      .type = ProfilePoint::Type::move
    },
  }};
  
  ProfileMesh *dividerMeshes[] {
    &dividerNormalMesh,
    &dividerEdgeMesh,
    &dividerCrossTrafficMesh,
    &dividerCrossEdgeMesh,
  };
  
  
  
  /// The scale of road cross sections.
  const Real scale = 0.333333333333;
}

RoadNetwork::RoadNetwork() {
  if (
    !LaneDef::loadBatch("roads/",
      "sidewalk",
      "roadway",
      nullptr
    ) ||
    !RoadDef::loadBatch("roads/",
      "single",
      "highway",
      nullptr
    )
  ) exit(1);
  
  _markingTexture = new Texture("textures/lane-markers");
}

Road *RoadNetwork::add(Road *road) {
  _roads.append(road);
  road->_dirty = true;
  return road;
}

bool RoadNetwork::connect(Road *a, Road *b) {
  if (a == b)
    return false;
  
  // Check which points are connected
  if (a->path.path.start.approxEqual(b->path.path.end).verticalAnd()) {
    if (a->start.type != Connection::none ||
        b->end  .type != Connection::none)
      // Already connected to something
      return false;
    
    // Connect
    a->start = b;
    b->end = a;
  } else if (a->path.path.start.approxEqual(b->path.path.start).verticalAnd()) {
    if (a->start.type != Connection::none ||
        b->start.type != Connection::none)
      // Already connected to something
      return false;
    
    // Connect
    a->start = b;
    b->start = a;
  } else if (a->path.path.end.approxEqual(b->path.path.end).verticalAnd()) {
    if (a->end.type != Connection::none ||
        b->end.type != Connection::none)
      // Already connected to something
      return false;
    
    // Connect
    a->end = b;
    b->end = a;
  } else if (a->path.path.end.approxEqual(b->path.path.start).verticalAnd()) {
    if (a->end  .type != Connection::none ||
        b->start.type != Connection::none)
      // Already connected to something
      return false;
    
    // Connect
    a->end = b;
    b->start = a;
  } else {
    // Not connected
    return false;
  }
  
  return true;
}

Real3 RoadNetwork::snap(const Real3 &point, Road *&snappedRoad) {
  snappedRoad = nullptr;
  bool snapped = false;
  Real2 p = { point.x, point.z };
  Real2 closest;
  Real distance;
  
  for (Road *road : _roads) {
    Real2 projection = road->path.path.project(p);
    Real dist = p.squareDistance(projection);
    if (dist < (road->definition->dimensions.x * Real(0.5 * scale)).square()) {
      // Check if the point is closest
      if (!snapped || dist < distance) {
        snapped = true;
        closest = projection;
        distance = dist;
        snappedRoad = road;
      }
    }
  }
  
  if (snapped && snappedRoad->end.type != Connection::road) {
    // Snap to the end if within radius
    if (p.squareDistance(snappedRoad->path.path.end) <
      (snappedRoad->definition->dimensions.x * Real(0.5 * scale)).square()) {
      closest = snappedRoad->path.path.end;
    }
  }
  
  if (snapped && snappedRoad->start.type != Connection::road) {
    // Snap to the start if within radius
    if (p.squareDistance(snappedRoad->path.path.start) <
      (snappedRoad->definition->dimensions.x * Real(0.5 * scale)).square()) {
      closest = snappedRoad->path.path.start;
    }
  }
  
  return snapped ? Real3 { closest.x, point.y, closest.y } : point;
}

bool RoadNetwork::validate(RoadDef *roadDef, Real3 point) {
  // Check if the road would interfere with any other roads
  Road *_snappedRoad;
  snap(point, _snappedRoad);
  if (_snappedRoad != nullptr) {
    // Begin at the snap point
    return true;
  }
  
  // Check if the road would interfere with any other roads
  Real2 p = { point.x, point.z };
  for (Road *road : _roads) {
    Real2 projection = road->path.path.project(p);
    Real dist = p.squareDistance(projection);
    if (dist < (
      (road->definition->dimensions.x + roadDef->dimensions.x) *
      Real(0.5 * scale) + /* extra spacing */ scale
    ).square())
      // New road would overlap with the existing road
      return false;
  }
  
  // No overlap: valid placement
  return true;
}

bool RoadNetwork::validate(RoadDef *roadDef, Path2 &path) {
  // A road must be at least square
  if (path.length() < roadDef->dimensions.x * scale)
    return false;
  
  // Check if the road would interfere with any other roads
  /*List<Real2> points { };
  int pointCount = (path.length() / scale).ceil().max(2);
  for (int i = 0; i < pointCount; i++)
    points.append(path.point(Real(i) / Real(pointCount - 1)));
  
  for (Road *road : _roads) {
    // Check that the two roads don't overlap
    
    // There are two valid ways two roads can intersect:
    // 1. One road goes completely through the other (creates an intersection)
    // 2. One road ends/starts at the other (continuation or T-intersection)
    // 3. Any other overlap is not allowed
    
    // Rules for valid intersections:
    // - The roads must be separated by atleast 30°
    // - Cannot occur (any part, entire radius) within the end cap radius of
    //   the end of the road
    
    // Record the intersections between the line and the road
    struct Intersection {
      Real2 point;
      Real2 projection;
      bool connected;
    };
    List<Intersection> intersections { };
    bool previousIntersect = false;
    for (Real2 &point : points) {
      Real2 projection = road->path.project(point);
      Real dist = point.squareDistance(projection);
      if (dist < (
        (road->definition->dimensions.x + roadDef->dimensions.x) *
        Real(0.5 * scale) + / extra spacing / scale
      ).square()) {
        bool intersects = true;
        if (projection.approxEqual(road->path.start).verticalAnd() && road->start.type != Connection::none) {
          // Check if the path is on this road
          Real4 pointNormal = road->path.pointNormals().first();
          Real2 forward = Real2(pointNormal.z, pointNormal.w).rightPerpendicular();
          if (forward.dot(point - road->path.start) > 0.1)
            intersects = false;
        }
        if (projection.approxEqual(road->path.end).verticalAnd() && road->end.type != Connection::none) {
          // Check if the path is on this road
          Real4 pointNormal = road->path.pointNormals().last();
          Real2 forward = Real2(pointNormal.z, pointNormal.w).leftPerpendicular();
          if (forward.dot(point - road->path.end) > 0.1)
            intersects = false;
        }
        
        if (intersects) {
          intersections.append({ point, projection, previousIntersect });
          previousIntersect = true;
        } else
          previousIntersect = false;
      } else
        previousIntersect = false;
    }
    
    if (!intersections.isEmpty()) {
      if (intersections.first().point.approxEqual(points.first()).verticalAnd()) {
        // Check for a valid start
        Intersection i = intersections.first();
        if (i.point.approxEqual(road->path.start).verticalAnd() ||
            i.point.approxEqual(road->path.end  ).verticalAnd()) {
          // Valid start
          intersections.remove(0);
          while (!intersections.isEmpty() && intersections.first().connected)
            intersections.remove(0);
        // Check if an intersection can to be created
        } else if (road->start.type == Connection::none && i.projection.squareDistance(road->path.start) < (
          road->definition->dimensions.x * Real(0.5 * scale)
        ).square() ||
        road->end.type == Connection::none && i.projection.squareDistance(road->path.end) < (
          road->definition->dimensions.x * Real(0.5 * scale)
        ).square())
          // Invalid start: intersection would be too close to the end cap
          return false;
        else {
          // Check that the road is actually on the path
          if (i.projection.squareDistance(i.point) > scale)
            // The road is not actually on the path
            return false;
          
          // Determine the normal at the intersection
          Real t = road->path.inverse(i.projection);
          Real2 point = road->path.point(t);
          Real2 normal = road->path.normal(t);
          
          // Check that the remaining points form a valid angle for an
          // intersection
          intersections.remove(0);
          while (!intersections.isEmpty() && intersections.first().connected) {
            i = intersections.remove(0);
            if (acos((i.point - point).normalized().dot(normal).abs()) > 60_deg)
              // Invalid angle
              return false;
          }
        }
        
        if (intersections.isEmpty())
          // No further conflicts
          continue;
      }
      
      if (intersections.last().point.approxEqual(points.last()).verticalAnd()) {
        // Check for a valid end
        Intersection i = intersections.last();
        if (i.point.approxEqual(road->path.start).verticalAnd() ||
            i.point.approxEqual(road->path.end  ).verticalAnd()) {
          // Valid end
          while (!intersections.isEmpty() && intersections.last().connected)
            intersections.remove(intersections.count() - 1);
          intersections.remove(intersections.count() - 1);
        // Check if an intersection can to be created
        } else if (road->start.type == Connection::none && i.projection.squareDistance(road->path.start) < (
          road->definition->dimensions.x * Real(0.5 * scale)
        ).square() ||
        road->end.type == Connection::none && i.projection.squareDistance(road->path.end) < (
          road->definition->dimensions.x * Real(0.5 * scale)
        ).square())
          // Invalid end: intersection would be too close to the end cap
          return false;
        else {
          // Check that the road is actually on the path
          if (i.projection.squareDistance(i.point) > scale)
            // The road is not actually on the path
            return false;
          
          // Determine the normal at the intersection
          Real t = road->path.inverse(i.projection);
          Real2 point = road->path.point(t);
          Real2 normal = road->path.normal(t);
          
          // Check that the remaining points form a valid angle for an
          // intersection
          bool connected = intersections.last().connected;
          intersections.remove(intersections.count() - 1);
          if (connected)
            while (!intersections.isEmpty()) {
              i = intersections.remove(intersections.count() - 1);
              if (acos((i.point - point).normalized().dot(normal).abs()) > 60_deg)
                // Invalid angle
                return false;
              if (!i.connected)
                break;
            }
        }
        
        if (intersections.isEmpty())
          // No further conflicts
          continue;
      }
      
      // Check for any remaining intersections
      for (intptr_t index = 0; index < intersections.count();) {
        // Check that the this is an intersection and not just a pass through
        intptr_t start = index;
        intptr_t end = index++;
        while (index < intersections.count() && intersections[index].connected)
          end = index++;
        
        if (start == end)
          // Not an intersection
          return false;
        
        // Check that the start and end points go through the line
        Intersection i = intersections[start];
        Intersection j = intersections[end];
        Real2 point = (i.point + j.point) * Real2(0.5);
        point = road->path.project(point);
        
        // Check that the intersection point is valid
        if (road->start.type == Connection::none && point.squareDistance(road->path.start) < (
          road->definition->dimensions.x * Real(0.5 * scale)
        ).square() ||
        road->end.type == Connection::none && point.squareDistance(road->path.end) < (
          road->definition->dimensions.x * Real(0.5 * scale)
        ).square())
          // Invalid intersection: too close to the end cap
          return false;
        
        // Verify the angle of the intersection
        Real t = road->path.inverse(point);
        Real2 normal = road->path.normal(t);
        Real dotI = (i.point - point).normalized().dot(normal);
        Real dotJ = (j.point - point).normalized().dot(normal);
        if (dotI.sign().value == dotJ.sign().value)
          // Start and end point on the same side of the curve:
          // not an intersection
          return false;
        if (acos(dotI.abs()) > 60_deg || acos(dotJ.abs()) > 60_deg)
          // Invalid angle
          return false;
      }
    }
  }*/
  
  // No overlap: valid placement
  return true;
}

bool RoadNetwork::build(RoadDef *road, Path2 &path) {
  // Check that the given path is valid
  if (!validate(road, path))
    return false;
  
  // Create the road
  Road *r = add(new Road(road, path));
  
  return true;
}

void RoadNetwork::update() {
  for (Road *road : _roads)
    if (road->_dirty) {
      if (!road->_meshes.isEmpty()) {
        // Redraw the mesh
        // TODO
      } else {
        // Create a new mesh
        BSTree<LaneDef *, int> lanes;
        
        Real2 half = { -road->definition->dimensions.x * Real(0.5), 0 };
        
        // Add caps as appropriate
        Angle startStart, startEnd;
        bool startCap = false;
        if (road->start.type == Connection::none) {
          // Add an end cap
          Real4 pointNormal = road->path.path.pointNormals().first();
          Real2 point = { pointNormal.z, pointNormal.w };
          startStart =  Angle(point);
          startEnd   = -Angle(point);
          startCap = true;
        }
        
        Angle endStart, endEnd;
        bool endCap = false;
        if (road->end.type == Connection::none) {
          // Add an end cap
          Real4 pointNormal = road->path.path.pointNormals().last();
          Real2 point = { pointNormal.z, pointNormal.w };
          endStart = -Angle(point);
          endEnd   =  Angle(point);
          endCap = true;
        }
        
        
        
        // Add a decorator if one exists
        if (!road->definition->decorations.triangles.isEmpty()) {
          if (!_meshes.has(road->definition->decorationsTexture.address()))
            _meshes.set(road->definition->decorationsTexture.address(), { });
          
          Resource<Mesh> mesh = new Mesh();
          road->_meshes.append(mesh);
          _meshes[road->definition->decorationsTexture.address()]
            .append({ mesh, { 1, road->path.path.length() } });
          
          // Extrude
          mesh->extrude(road->definition->decorations,
            road->path.path, half, scale);
          
          // Add caps as appropriate
          if (startCap)
            mesh->halfRevolve(road->definition->decorations,
              road->path.path.start, startStart, startEnd, half, scale);
          if (endCap)
            mesh->halfRevolve(road->definition->decorations,
              road->path.path.end, endStart, endEnd, half, scale);
        }
        
        // Add the lanes
        for (const RoadDef::Lane &lane : road->definition->lanes) {
          Resource<Mesh> mesh = _addMesh(road, lane.definition, lanes);
          mesh->extrude(lane.definition->profile,
            road->path.path, lane.position + half, scale);
          
          if (startCap)
            mesh->halfRevolve(lane.definition->profile,
              road->path.path.start, startStart, startEnd,
              lane.position + half, scale);
          if (endCap)
            mesh->halfRevolve(lane.definition->profile,
              road->path.path.end, endStart, endEnd,
              lane.position + half, scale);
        }
        
        // Add any markings
        if (!road->definition->dividers.isEmpty()) {
          // Update where the markings are drawn
          half.y += 0.01;
          half.x -= 0.1;
          
          // Create the divider mesh
          Resource<Mesh> dividers = new Mesh();
          _markings.append({ dividers, { 1, road->path.path.length() } });
          road->_meshes.append(dividers);
          
          // Extrude the dividers
          for (const RoadDef::Divider &divider : road->definition->dividers) {
            dividers->extrude(
              *dividerMeshes[(int)divider.type],
              road->path.path, divider.position + half, scale
            );
            
            // Add caps as appropriate
            if (startCap)
              dividers->halfRevolve(*dividerMeshes[(int)divider.type],
                road->path.path.start, startStart, startEnd,
                divider.position + half, scale);
            if (endCap)
              dividers->halfRevolve(*dividerMeshes[(int)divider.type],
                road->path.path.end, endStart, endEnd,
                divider.position + half, scale);
          }
        }
        
        // Push all the created meshes to the GPU
        for (Resource<Mesh> &mesh : road->_meshes)
          mesh->load();
      }
      
      road->_dirty = false;
    }
}

void RoadNetwork::draw() {
  int count = 0;
  int meshes = 0;
  for (auto &_lane : _meshes) {
    for (auto &lane : _lane) {
    // Setup the material
    lane.key->load(Uniforms::s_albedo);
    count++;
    
    for (_mesh &mesh : lane.value) {
      // Update tiling
      bgfx::setUniform(Uniforms::u_textureTile, Real4(mesh.textureTiling));
      
      // Draw
      mesh.mesh->draw(Program::pbr);
      meshes++;
    }
    }
  }
  
  // Draw the markings
  if (!_markings.isEmpty()) {
    // Setup the material
    _markingTexture->load(Uniforms::s_albedo);
    
    for (_mesh &mesh : _markings) {
      // Update tiling
      bgfx::setUniform(Uniforms::u_textureTile, Real4(mesh.textureTiling));
      
      // Draw
      mesh.mesh->draw(Program::pbr);
      meshes++;
    }
  }
  
  bgfx::dbgTextPrintf(4, 5, 0x0f,
    "%d roads, %d meshes",
    count, meshes
  );
}



Resource<Mesh> RoadNetwork::_addMesh(Road *road, LaneDef *lane, BSTree<LaneDef *, int> &lanes) {
  Optional<int> selected;
  Resource<Mesh> mesh;
  if (!_meshes.has(lane->mainTexture.address())) {
    _meshes.set(lane->mainTexture.address(), { });
  } else if ((selected = lanes[lane])) {
    // Add to the lane
    mesh = road->_meshes[*selected];
    return mesh;
  }
  
  // Create a new mesh
  mesh = new Mesh();
  lanes.insert(lane, road->_meshes.count());
  road->_meshes.append(mesh);
  _meshes[lane->mainTexture.address()]
    .append({ mesh, { 1, road->path.path.length() } });
  return mesh;
}
