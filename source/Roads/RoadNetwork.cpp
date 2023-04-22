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
  if (a->path.path.start.squareDistance(b->path.path.end) < 0.1) {
    if (a->start.type != Connection::none ||
        b->end  .type != Connection::none)
      // Already connected to something
      return false;
    
    // Connect
    a->start = b;
    b->end = a;
  } else if (a->path.path.start.squareDistance(b->path.path.start) < 0.1) {
    if (a->start.type != Connection::none ||
        b->start.type != Connection::none)
      // Already connected to something
      return false;
    
    // Connect
    a->start = b;
    b->start = a;
  } else if (a->path.path.end.squareDistance(b->path.path.end) < 0.1) {
    if (a->end.type != Connection::none ||
        b->end.type != Connection::none)
      // Already connected to something
      return false;
    
    // Connect
    a->end = b;
    b->end = a;
  } else if (a->path.path.end.squareDistance(b->path.path.start) < 0.1) {
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
  RadiusPath2 _path { path, roadDef->dimensions.x * Real(0.5 * scale) };
  for (Road *road : _roads) {
    Real2 start = road->path.path.project(path.start);
    Real2   end = road->path.path.project(path.end  );
    bool _start = start.squareDistance(path.start) < 0.1;
    bool _end   =   end.squareDistance(path.end  ) < 0.1;
    if (_start || _end) {
      // Adding to the existing road: skip
      if (_start && _end) {
        // Can't add to the same road
        return false;
      }
      
      // Check for valid angle first
      Real2 forward = path.normal(_start ? 0 : 1);
      if (_start)
        forward = forward.leftPerpendicular();
      else
        forward = forward.rightPerpendicular();
      
      Real t = road->path.path.inverse(_start ? path.start : path.end);
      if (t.approxZero()) {
        // Check with the start of the road
        Real2 _forward = road->path.path.normal(0).rightPerpendicular();
        Real angle = _forward.dot(forward);
        if (angle < -44_deg)
          // Too sharp of an angle
          return false;
      } else if ((t - Real(1)).approxZero()) {
        // Check with the end of the road
        Real2 _forward = road->path.path.normal(1).leftPerpendicular();
        Real angle = _forward.dot(forward);
        if (angle < -44_deg)
          // Too sharp of an angle
          return false;
      } else {
        // Check that the intersection is not within a radius of the road end
        if (road->start.type != Connection::road &&
            _path.circleTest(road->path.path.start, road->path.radius))
          return false;
        if (road->end.type != Connection::road &&
            _path.circleTest(road->path.path.end, road->path.radius))
          return false;
        
        // Check with the middle of the road
        Real2 normal = road->path.path.normal(t);
        Real angle = normal.dot(forward).abs();
        if (angle < 29_deg)
          // Too sharp of an angle
          return false;
      }
      
      continue;
    }
    
    if (!road->path.intersectionTest(_path))
      // New road would overlap with the existing road
      return false;
    if (road->start.type != Connection::road &&
        _path.circleTest(road->path.path.start, road->path.radius))
      return false;
    if (road->end.type != Connection::road &&
        _path.circleTest(road->path.path.end, road->path.radius))
      return false;
    
    // Check that the intersection is at a valid angle
    List<Real2> intersections = road->path.path.intersections(path);
    for (Real2 intersection : intersections) {
      Real2 forward = path.normal(path.inverse(intersection)).leftPerpendicular();
      Real2 normal = road->path.path.normal(road->path.path.inverse(intersection));
      Real angle = normal.dot(forward).abs();
      if (angle < 29_deg)
        // Too sharp of an angle
        return false;
    }
  }
  
  return true;
}

bool RoadNetwork::build(RoadDef *road, Path2 &path) {
  // Check that the given path is valid
  if (!validate(road, path))
    return false;
  
  // Create the road
  Road *r = add(new Road(road, path));
  
  // Attempt to attach to other roads
  for (Road *_road : _roads) {
    if (connect(_road, r)) {
      // Make sure the connected road is redrawn too to remove the previous
      // end cap
      _road->_dirty = true;
      
      if (r->start.type != Connection::none &&
          r->end.type   != Connection::none)
        break;
    }
  }
  
  return true;
}

void RoadNetwork::update() {
  for (Road *road : _roads)
    if (road->_dirty) {
      if (!road->_meshes.isEmpty()) {
        // Remove all the previous meshes
        for (Road::_mesh &mesh : road->_meshes) {
          if (mesh.texture == nullptr) {
            // Remove from the divider meshes
            for (intptr_t i = 0; i < _markings.count(); i++)
              if (_markings[i].mesh.address() == mesh.mesh.address()) {
                _markings.remove(i);
                break;
              }
          } else {
            // Remove from the standard meshes
            auto &meshes = _meshes[mesh.texture];
            for (intptr_t i = 0; i < meshes.count(); i++)
              if (meshes[i].mesh.address() == mesh.mesh.address()) {
                meshes.remove(i);
                break;
              }
          }
        }
        road->_meshes.removeAll();
      }
      
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
        road->_meshes.append({
          road->definition->decorationsTexture.address(), mesh
        });
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
        road->_meshes.append({ nullptr, dividers });
        
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
      for (Road::_mesh &mesh : road->_meshes)
        mesh.mesh->load();
      
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
    mesh = road->_meshes[*selected].mesh;
    return mesh;
  }
  
  // Create a new mesh
  mesh = new Mesh();
  lanes.insert(lane, road->_meshes.count());
  road->_meshes.append({ lane->mainTexture.address(), mesh });
  _meshes[lane->mainTexture.address()]
    .append({ mesh, { 1, road->path.path.length() } });
  return mesh;
}
