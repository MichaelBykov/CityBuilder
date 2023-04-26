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

void RoadNetwork::remove(Road *road) {
  // Remove the meshes
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
  
  for (intptr_t i = 0; i < _roads.count(); i++)
    if (_roads[i] == road) {
      _roads.remove(i);
      return;
    }
}

Intersection *RoadNetwork::add(Intersection *intersection) {
  _intersections.append(intersection);
  intersection->_dirty = true;
  return intersection;
}

namespace {
  void addJoint(Road *a, bool aStart, Road *b, bool bStart, RoadNetwork *roads) {
    // Check if we need to add a connective joint
    Real4 pointNormalA = aStart ? a->path.pointNormals().first() : a->path.pointNormals().last();
    Real4 pointNormalB = bStart ? b->path.pointNormals().first() : b->path.pointNormals().last();
    
    Real2 normalA = { pointNormalA.z, pointNormalA.w };
    if (aStart) normalA = normalA. leftPerpendicular();
    else        normalA = normalA.rightPerpendicular();
    
    Real2 normalB = { pointNormalB.z, pointNormalB.w };
    if (bStart) normalB = normalB. leftPerpendicular();
    else        normalB = normalB.rightPerpendicular();
    
    Real dot = normalA.dot(normalB);
    if (dot > -0.9999) {
      // Add a joint
      
      // Determine the amount to offset
      Real2 intersection = aStart ? a->path.start() : a->path.end();
      
      Angle angle = acos(dot);
      
      Real offset =
        (a->definition->dimensions.x * Real(0.5) * scale) /
        (angle * 0.5).tan()
        + Real(0.1);
      
      // Offset the original road
      if (a->path.type() == Path2::Type::bezier) {
        // Push back just the end point
        Bezier2 *bezier = (Bezier2 *)&a->path.path();
        if (aStart)
          a->path = RadiusPath2(new Bezier2(
            bezier->start + normalA * Real2(offset),
            bezier->control1,
            bezier->control2,
            bezier->end
          ), a->path.radius());
        else
          a->path = RadiusPath2(new Bezier2(
            bezier->start,
            bezier->control1,
            bezier->control2,
            bezier->end + normalA * Real2(offset)
          ), a->path.radius());
      } else {
        a->path = aStart ?
          a->path.split(offset / a->path.length(), 1) :
          a->path.split(0, 1 - offset / a->path.length()) ;
      }
      
      if (b->path.type() == Path2::Type::bezier) {
        // Push back just the end point
        Bezier2 *bezier = (Bezier2 *)&b->path.path();
        if (bStart)
          b->path = RadiusPath2(new Bezier2(
            bezier->start + normalB * Real2(offset),
            bezier->control1,
            bezier->control2,
            bezier->end
          ), b->path.radius());
        else
          b->path = RadiusPath2(new Bezier2(
            bezier->start,
            bezier->control1,
            bezier->control2,
            bezier->end + normalB * Real2(offset)
          ), b->path.radius());
      } else {
        b->path = bStart ?
          b->path.split(offset / b->path.length(), 1) :
          b->path.split(0, 1 - offset / b->path.length()) ;
      }
      
      // Add the joint
      Road *joint = roads->add(new Road(a->definition, new Bezier2(
        aStart ? a->path.start() : a->path.end(),
        intersection,
        bStart ? b->path.start() : b->path.end()
      )));
      joint->start = a;
      joint->end = b;
      
      (aStart ? a->start : a->end) = joint;
      (bStart ? b->start : b->end) = joint;
    } else {
      // Connect
      (aStart ? a->start : a->end) = b;
      (bStart ? b->start : b->end) = a;
    }
  }
  
}

bool RoadNetwork::connect(Road *a, Road *b) {
  if (a == b)
    return false;
  
  // Check which points are connected
  if (a->path.start().squareDistance(b->path.end()) < 0.1) {
    if (a->start.type != Connection::none ||
        b->end  .type != Connection::none)
      // Already connected to something
      return false;
    
    if (a->definition != b->definition) {
      // Intersection
    } else
      // Add a joint as appropriate
      addJoint(a, true, b, false, this);
  } else if (a->path.start().squareDistance(b->path.start()) < 0.1) {
    if (a->start.type != Connection::none ||
        b->start.type != Connection::none)
      // Already connected to something
      return false;
    
    if (a->definition != b->definition) {
      // Intersection
    } else
      // Add a joint as appropriate
      addJoint(a, true, b, true, this);
  } else if (a->path.end().squareDistance(b->path.end()) < 0.1) {
    if (a->end.type != Connection::none ||
        b->end.type != Connection::none)
      // Already connected to something
      return false;
    
    if (a->definition != b->definition) {
      // Intersection
    } else
      // Add a joint as appropriate
      addJoint(a, false, b, false, this);
  } else if (a->path.end().squareDistance(b->path.start()) < 0.1) {
    if (a->end  .type != Connection::none ||
        b->start.type != Connection::none)
      // Already connected to something
      return false;
    
    if (a->definition != b->definition) {
      // Intersection
    } else
      // Add a joint as appropriate
      addJoint(a, false, b, true, this);
  } else {
    // Not connected
    return false;
  }
  
  return true;
}

namespace {
  List<Road *> splitRoad(Road *road, const List<Real2> &intersections, bool &start, bool &end, RoadNetwork *network) {
    if (intersections.isEmpty())
      return { road };
    
    List<Real> t = intersections.map([road](Real2 p) { return road->path.inverse(p); });
    t.sort();
    
    if (t.first() < 1.0 / road->path.length()) {
      start = true;
      t.remove(0);
    }
    if (!t.isEmpty() && 1 - t.last() < 1.0 / road->path.length()) {
      end = true;
      t.remove(t.count() - 1);
    }
    
    List<Road *> roads;
    if (!t.isEmpty()) {
      Real start = 0;
      for (Real t : t) {
        roads.append(network->add(new Road(
          road->definition,
          road->path.path().split(start, t)
        )));
        start = t;
      }
      roads.append(network->add(new Road(
        road->definition,
        road->path.path().split(start, 1)
      )));
      network->remove(road);
    } else
      roads.append(road);
    
    // Reconnect the roads
    switch (road->start.type) {
    case Connection::none: break;
    case Connection::road:
      roads.first()->start = road->start.other.road;
      road->start.other.road = roads.first();
      break;
    
    case Connection::intersection:
      roads.first()->start = road->start.other.intersection;
      road->start.other.intersection->replaceRoad(road, roads.first());
      break;
    }
    
    switch (road->end.type) {
    case Connection::none: break;
    case Connection::road:
      roads.last()->end = road->end.other.road;
      road->end.other.road = roads.last();
      break;
    
    case Connection::intersection:
      roads.last()->end = road->end.other.intersection;
      road->end.other.intersection->replaceRoad(road, roads.last());
      break;
    }
    
    return roads;
  }
}

List<Road *> RoadNetwork::intersect(Road *a, Road *b) {
  if (a == b || !a->path.bounds().intersects(b->path.bounds()))
    return { a };
  
  // Find the intersection points
  List<Real2> intersections = a->path.path().intersections(b->path.path());
  { // Project points
    Real2 projection;
    if ((projection = a->path.path().project(b->path.start())).squareDistance(b->path.start()) < 1) {
      // Add if it's not already there
      bool exists = false;
      for (Real2 p : intersections)
        if (p.approxEqual(projection).verticalAnd()) {
          exists = true;
          break;
        }
      if (!exists)
        intersections.append(projection);
    }
    if ((projection = a->path.path().project(b->path.end())).squareDistance(b->path.end()) < 1) {
      // Add if it's not already there
      bool exists = false;
      for (Real2 p : intersections)
        if (p.approxEqual(projection).verticalAnd()) {
          exists = true;
          break;
        }
      if (!exists)
        intersections.append(projection);
    }
    if ((projection = b->path.path().project(a->path.start())).squareDistance(a->path.start()) < 1) {
      // Add if it's not already there
      bool exists = false;
      for (Real2 p : intersections)
        if (p.approxEqual(projection).verticalAnd()) {
          exists = true;
          break;
        }
      if (!exists)
        intersections.append(projection);
    }
    if ((projection = b->path.path().project(a->path.end())).squareDistance(a->path.end()) < 1) {
      // Add if it's not already there
      bool exists = false;
      for (Real2 p : intersections)
        if (p.approxEqual(projection).verticalAnd()) {
          exists = true;
          break;
        }
      if (!exists)
        intersections.append(projection);
    }
  }
  if (intersections.isEmpty())
    // Nothing to intersect
    return { a };
  
  // Split up the roads
  bool aStart = false, aEnd = false;
  List<Road *> _a = splitRoad(a, intersections, aStart, aEnd, this);
  
  bool bStart = false, bEnd = false;
  List<Road *> _b = splitRoad(b, intersections, bStart, bEnd, this);
  
  // Form the intersections
  List<Intersection *> _intersections { };
  for (Real2 p : intersections) {
    Intersection *intersection = add(new Intersection(p));
    
    // Add all roads at the intersection
    for (Road *road : _a)
      if (road->path.start().squareDistance(p) < 0.1 ||
          road->path.end()  .squareDistance(p) < 0.1)
        intersection->addRoad(road);
    for (Road *road : _b)
      if (road->path.start().squareDistance(p) < 0.1 ||
          road->path.end()  .squareDistance(p) < 0.1)
        intersection->addRoad(road);
    
    _intersections.append(intersection);
  }
  
  return _a;
}

Real3 RoadNetwork::snap(const Real3 &point, Road *&snappedRoad) {
  snappedRoad = nullptr;
  bool snapped = false;
  Real2 p = { point.x, point.z };
  Real2 closest;
  Real distance;
  
  for (Road *road : _roads) {
    if (!road->path.bounds().contains(p))
      continue;
    Real2 projection = road->path.path().project(p);
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
    if (p.squareDistance(snappedRoad->path.end()) <
      (snappedRoad->definition->dimensions.x * Real(0.5 * scale)).square()) {
      closest = snappedRoad->path.end();
    }
  }
  
  if (snapped && snappedRoad->start.type != Connection::road) {
    // Snap to the start if within radius
    if (p.squareDistance(snappedRoad->path.start()) <
      (snappedRoad->definition->dimensions.x * Real(0.5 * scale)).square()) {
      closest = snappedRoad->path.start();
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
  Real radius = roadDef->dimensions.x * Real(0.5) * scale;
  Bounds2 bounds = { p - Real2(radius), Real2(radius * Real(2)) };
  for (Road *road : _roads) {
    if (!road->path.bounds().intersects(bounds))
      continue;
    Real2 projection = road->path.path().project(p);
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

bool RoadNetwork::validate(RoadDef *roadDef, Ref<Path2 &> path) {
  // A road must be at least square
  if (path->length() < roadDef->dimensions.x * scale)
    return false;
  
  // Check if the road would interfere with any other roads
  RadiusPath2 _path { path, roadDef->dimensions.x * Real(0.5 * scale) };
  for (Road *road : _roads) {
    if (!road->path.bounds().intersects(_path.bounds()))
      continue;
    
    Real2 start = road->path.path().project(path->start);
    Real2   end = road->path.path().project(path->end  );
    bool _start = start.squareDistance(path->start) < 0.1;
    bool _end   =   end.squareDistance(path->end  ) < 0.1;
    if (_start || _end) {
      // Adding to the existing road: skip
      if (_start && _end) {
        // Can't add to the same road
        return false;
      }
      
      // Check for valid angle first
      Real2 forward = path->normal(_start ? 0 : 1);
      if (_start)
        forward = forward.leftPerpendicular();
      else
        forward = forward.rightPerpendicular();
      
      Real t = road->path.inverse(_start ? path->start : path->end);
      if (t.approxZero()) {
        // Check with the start of the road
        Real2 _forward = road->path.normal(0).rightPerpendicular();
        Real angle = _forward.dot(forward);
        if (angle < -44_deg)
          // Too sharp of an angle
          return false;
      } else if ((t - Real(1)).approxZero()) {
        // Check with the end of the road
        Real2 _forward = road->path.normal(1).leftPerpendicular();
        Real angle = _forward.dot(forward);
        if (angle < -44_deg)
          // Too sharp of an angle
          return false;
      } else {
        // Check that the intersection is not within a radius of the road end
        if (road->start.type != Connection::road &&
            _path.circleTest(road->path.start(), road->path.radius()))
          return false;
        if (road->end.type != Connection::road &&
            _path.circleTest(road->path.end(), road->path.radius()))
          return false;
        
        // Check with the middle of the road
        Real2 normal = road->path.normal(t);
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
        _path.circleTest(road->path.start(), road->path.radius()))
      return false;
    if (road->end.type != Connection::road &&
        _path.circleTest(road->path.end(), road->path.radius()))
      return false;
    
    // Check that the intersection is at a valid angle
    List<Real2> intersections = road->path.path().intersections(*path);
    for (Real2 intersection : intersections) {
      Real2 forward = path->normal(path->inverse(intersection)).leftPerpendicular();
      Real2 normal = road->path.normal(road->path.inverse(intersection));
      Real angle = normal.dot(forward).abs();
      if (angle < 29_deg)
        // Too sharp of an angle
        return false;
    }
  }
  
  return true;
}

bool RoadNetwork::build(RoadDef *road, Ref<Path2 &> path) {
  // Check that the given path is valid
  if (!validate(road, path))
    return false;
  intptr_t count = _roads.count();
  
  // Create the road
  Road *r = add(new Road(road, path));
  
  // Attempt to attach to other roads
  for (intptr_t i = 0; i < count; i++) {
    Road *_road = _roads[i];
    if (connect(_road, r)) {
      // Make sure the connected road is redrawn too to remove the previous
      // end cap
      _road->_dirty = true;
      
      if (r->start.type != Connection::none &&
          r->end.type   != Connection::none)
        break;
    }
  }
  
  // Create intersections
  List<Road *> segments { r };
  Bounds2 bounds = r->path.bounds();
  for (intptr_t i = 0; i < count; i++) {
    Road *_road = _roads[i];
    if (!bounds.intersects(_road->path.bounds()))
      // No possible intersection
      continue;
    
    List<Road *> produced { };
    for (Road *segment : segments)
      produced.appendList(intersect(segment, _road));
    segments = produced;
  }
  
  return true;
}

void RoadNetwork::update() {
  // Update the roads
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
        Real4 pointNormal = road->path.pointNormals().first();
        Real2 point = { pointNormal.z, pointNormal.w };
        startStart =  Angle(point);
        startEnd   = -Angle(point);
        startCap = true;
      }
      
      Angle endStart, endEnd;
      bool endCap = false;
      if (road->end.type == Connection::none) {
        // Add an end cap
        Real4 pointNormal = road->path.pointNormals().last();
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
          .append({ mesh, { 1, road->path.length() } });
        
        // Extrude
        mesh->extrude(road->definition->decorations,
          road->path.path(), half, scale);
        
        // Add caps as appropriate
        if (startCap)
          mesh->halfRevolve(road->definition->decorations,
            road->path.start(), startStart, startEnd, half, scale);
        if (endCap)
          mesh->halfRevolve(road->definition->decorations,
            road->path.end(), endStart, endEnd, half, scale);
      }
      
      // Add the lanes
      for (const RoadDef::Lane &lane : road->definition->lanes) {
        Resource<Mesh> mesh = _addMesh(road, lane.definition, lanes);
        mesh->extrude(lane.definition->profile,
          road->path.path(), lane.position + half, scale);
        
        if (startCap)
          mesh->halfRevolve(lane.definition->profile,
            road->path.start(), startStart, startEnd,
            lane.position + half, scale);
        if (endCap)
          mesh->halfRevolve(lane.definition->profile,
            road->path.end(), endStart, endEnd,
            lane.position + half, scale);
      }
      
      // Add any markings
      if (!road->definition->dividers.isEmpty()) {
        // Update where the markings are drawn
        half.y += 0.01;
        half.x -= 0.1;
        
        // Create the divider mesh
        Resource<Mesh> dividers = new Mesh();
        _markings.append({ dividers, { 1, road->path.length() } });
        road->_meshes.append({ nullptr, dividers });
        
        // Extrude the dividers
        for (const RoadDef::Divider &divider : road->definition->dividers) {
          dividers->extrude(
            *dividerMeshes[(int)divider.type],
            road->path.path(), divider.position + half, scale
          );
          
          // Add caps as appropriate
          if (startCap)
            dividers->halfRevolve(*dividerMeshes[(int)divider.type],
              road->path.start(), startStart, startEnd,
              divider.position + half, scale);
          if (endCap)
            dividers->halfRevolve(*dividerMeshes[(int)divider.type],
              road->path.end(), endStart, endEnd,
              divider.position + half, scale);
        }
      }
      
      // Push all the created meshes to the GPU
      for (Road::_mesh &mesh : road->_meshes)
        mesh.mesh->load();
      
      road->_dirty = false;
    }
  
  // Update the intersections
  for (Intersection *intersection : _intersections)
    if (intersection->_dirty) {
      if (!intersection->_meshes.isEmpty()) {
        // Remove all the previous meshes
        for (Intersection::_mesh &mesh : intersection->_meshes) {
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
        intersection->_meshes.removeAll();
      }
      
      // Create a new mesh
      BSTree<LaneDef *, int> lanes;
      
      for (Intersection::Arm &arm : intersection->arms) {
        for (const RoadDef::Lane &lane : arm.road->definition->lanes)
          for (const auto &traffic : lane.definition->traffic)
            switch (traffic.connection) {
            case LaneDef::Traffic::Connection::none:
              // TODO
              break;
            
            case LaneDef::Traffic::Connection::sameDirection:
              if (!lanes[lane.definition]) {
                // Find all other traffic lanes of the same type in the
                // intersection and connect them as a single polygon
                Resource<Mesh> mesh = _addMesh(intersection, lane.definition, lanes);
                
                Real3 center { intersection->center.x, lane.definition->profile.dimensions.y * scale, intersection->center.y };
                for (intptr_t i = 0; i < intersection->arms.count(); i++) {
                  Intersection::Arm &arm = intersection->arms[i];
                  
                  for (const RoadDef::Lane &_lane : arm.road->definition->lanes)
                    if (_lane.definition == lane.definition) {
                      // Determine the bounds of the lane
                      Real4 pointNormal = arm.start ?
                        arm.road->path.pointNormals().first() :
                        arm.road->path.pointNormals(). last() ;
                      Real2 point  = { pointNormal.x, pointNormal.y };
                      Real2 normal = { pointNormal.z, pointNormal.w };
                      if (!arm.start)
                        normal = -normal;
                      
                      Real2  leftEdge = point - normal * Real2(arm.road->definition->dimensions.x * Real(0.5) * scale);
                      Real2 rightEdge = point + normal * Real2(arm.road->definition->dimensions.x * Real(0.5) * scale);
                      
                      // Find the edges of the surrounding lanes
                      Intersection::Arm &prev = intersection->arms[i - 1 < 0 ? intersection->arms.count() - 1 : i - 1];
                      Intersection::Arm &next = intersection->arms[i + 1 >= intersection->arms.count() ? 0 : i + 1];
                      
                      Real4 prevPointNormal = prev.start ?
                        prev.road->path.pointNormals().first() :
                        prev.road->path.pointNormals(). last() ;
                      Real2 prevPoint  = { prevPointNormal.x, prevPointNormal.y };
                      Real2 prevNormal = { prevPointNormal.z, prevPointNormal.w };
                      if (!prev.start)
                        prevNormal = -prevNormal;
                      Real2 prevRightEdge = prevPoint + prevNormal * Real2(prev.road->definition->dimensions.x * Real(0.5) * scale);
                      
                      Real4 nextPointNormal = next.start ?
                        next.road->path.pointNormals().first() :
                        next.road->path.pointNormals(). last() ;
                      Real2 nextPoint  = { nextPointNormal.x, nextPointNormal.y };
                      Real2 nextNormal = { nextPointNormal.z, nextPointNormal.w };
                      if (!next.start)
                        nextNormal = -nextNormal;
                      Real2 nextLeftEdge = nextPoint - nextNormal * Real2(next.road->definition->dimensions.x * Real(0.5) * scale);
                      
                      Real2  left = ((leftEdge + prevRightEdge) * Real2(0.5) - intersection->center).normalized();
                      Real2 right = ((rightEdge + nextLeftEdge) * Real2(0.5) - intersection->center).normalized();
                      
                      left  /= Real(( normal).dot(left ));
                      right /= Real((-normal).dot(right));
                      
                      
                      
                      // Extrude the lane into the left-center-right triangle
                      Real offset = (-arm.road->definition->dimensions.x * Real(0.5) + _lane.position.x) * scale;
                      List<Mesh::Vertex> vertices { };
                      for (const ProfileMesh::Vertex &v : _lane.definition->profile.vertices) {
                        Real3 _position = Real3(point.x, v.position.y * scale, point.y) + Real3(normal.x, 0, normal.y) * Real3(offset + v.position.x * scale);
                        Real3 _normal = Real3(normal.x, 0, normal.y) * Real3(v.normal.x) + Real3(0, v.normal.y, 0);
                        
                        vertices.append({
                          .position = _position,
                          .normal   = _normal,
                          .uv       = { v.uv, 0 }
                        });
                        
                        Real3 _projection;
                        Real _offset = offset + v.position.x * scale;
                        if (_offset.approxZero()) {
                          _projection = center;
                        } else if (_offset < 0) {
                          // On the right side
                          _projection = center + Real3(right.x, 0, right.y) * Real3(-_offset);
                        } else {
                          // On the left side
                          _projection = center + Real3(left.x, 0, left.y) * Real3(_offset);
                        }
                        
                        vertices.append({
                          .position = _projection,
                          .normal   = _normal,
                          .uv       = { v.uv, _projection.distance(_position) }
                        });
                      }
                      
                      // Add the triangles
                      List<int> triangles { };
                      for (int i = 0; i < _lane.definition->profile.triangles.count() - 1; i += 2) {
                        int a = _lane.definition->profile.triangles[i + 0];
                        int b = _lane.definition->profile.triangles[i + 1];
                        
                        triangles.append(b * 2 + 0);
                        triangles.append(a * 2 + 0);
                        triangles.append(b * 2 + 1);
                        
                        triangles.append(a * 2 + 1);
                        triangles.append(b * 2 + 1);
                        triangles.append(a * 2 + 0);
                      }
                      
                      mesh->add(vertices, triangles);
                    }
                }
              }
              break;
            }
        
        // Extend decorations as applicable
        switch (arm.road->definition->decorationsExtent) {
        case RoadDef::DecorExtent::center: {
          // Decor to the center
          Line2 line {
            arm.start ? arm.road->path.start() : arm.road->path.end(),
            intersection->center
          };
          
          if (!_meshes.has(arm.road->definition->decorationsTexture.address()))
            _meshes.set(arm.road->definition->decorationsTexture.address(), { });
          
          Resource<Mesh> mesh = new Mesh();
          intersection->_meshes.append({
            arm.road->definition->decorationsTexture.address(), mesh
          });
          _meshes[arm.road->definition->decorationsTexture.address()]
            .append({ mesh, { 1, line.length() } });
          
          // Extrude
          mesh->extrude(arm.road->definition->decorations,
            line, { -arm.road->definition->dimensions.x * Real(0.5), 0 }, scale);
        } break;
        
        default:
          break;
        }
      }
      
      // Push all the created meshes to the GPU
      for (Intersection::_mesh &mesh : intersection->_meshes)
        mesh.mesh->load();
      
      intersection->_dirty = false;
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
      
      bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA);
      
      // Draw
      mesh.mesh->draw(Program::pbr);
      meshes++;
    }
  }
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
    .append({ mesh, { 1, road->path.length() } });
  return mesh;
}

Resource<Mesh> RoadNetwork::_addMesh(Intersection *road, LaneDef *lane, BSTree<LaneDef *, int> &lanes) {
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
  _meshes[lane->mainTexture.address()].append({ mesh, { 1, 1 } });
  return mesh;
}
