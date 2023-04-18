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
  if (a->path.start.approxEqual(b->path.end).verticalAnd()) {
    if (a->start.type != Connection::none ||
        b->end  .type != Connection::none)
      // Already connected to something
      return false;
    
    // Connect
    a->start = b;
    b->end = a;
  } else if (a->path.start.approxEqual(b->path.start).verticalAnd()) {
    if (a->start.type != Connection::none ||
        b->start.type != Connection::none)
      // Already connected to something
      return false;
    
    // Connect
    a->start = b;
    b->start = a;
  } else if (a->path.end.approxEqual(b->path.end).verticalAnd()) {
    if (a->end.type != Connection::none ||
        b->end.type != Connection::none)
      // Already connected to something
      return false;
    
    // Connect
    a->end = b;
    b->end = a;
  } else if (a->path.end.approxEqual(b->path.start).verticalAnd()) {
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

Real3 RoadNetwork::snap(const Real3 &point) {
  bool snapped = false;
  Real2 p = { point.x, point.z };
  Real2 closest;
  Real distance;
  
  for (Road *road : _roads) {
    Real2 projection = road->path.project(p);
    Real dist = p.squareDistance(projection);
    if (dist < (road->definition->dimensions.x * Real(0.5 * scale)).square()) {
      // Check if the point is closest
      if (!snapped || dist < distance) {
        snapped = true;
        closest = projection;
        distance = dist;
      }
    }
  }
  
  return snapped ? Real3 { closest.x, point.y, closest.y } : point;
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
          road->_meshes.append(mesh);
          _meshes[road->definition->decorationsTexture.address()]
            .append({ mesh, { 1, road->path.length() } });
          
          // Extrude
          mesh->extrude(road->definition->decorations, road->path, half, scale);
          
          // Add caps as appropriate
          if (startCap)
            mesh->halfRevolve(road->definition->decorations,
              road->path.start, startStart, startEnd, half, scale);
          if (endCap)
            mesh->halfRevolve(road->definition->decorations,
              road->path.end, endStart, endEnd, half, scale);
        }
        
        // Add the lanes
        for (const RoadDef::Lane &lane : road->definition->lanes) {
          Resource<Mesh> mesh = _addMesh(road, lane.definition, lanes);
          mesh->extrude(lane.definition->profile, road->path, lane.position + half, scale);
          
          if (startCap)
            mesh->halfRevolve(lane.definition->profile,
              road->path.start, startStart, startEnd, lane.position + half, scale);
          if (endCap)
            mesh->halfRevolve(lane.definition->profile,
              road->path.end, endStart, endEnd, lane.position + half, scale);
        }
        
        // Add any markings
        if (!road->definition->dividers.isEmpty()) {
          // Update where the markings are drawn
          half.y += 0.01;
          half.x -= 0.1;
          
          // Create the divider mesh
          Resource<Mesh> dividers = new Mesh();
          _markings.append({ dividers, { 1, road->path.length() } });
          road->_meshes.append(dividers);
          
          // Extrude the dividers
          for (const RoadDef::Divider &divider : road->definition->dividers) {
            dividers->extrude(
              *dividerMeshes[(int)divider.type],
              road->path, divider.position + half, scale
            );
            
            // Add caps as appropriate
            if (startCap)
              dividers->halfRevolve(*dividerMeshes[(int)divider.type],
                road->path.start, startStart, startEnd, divider.position + half, scale);
            if (endCap)
              dividers->halfRevolve(*dividerMeshes[(int)divider.type],
                road->path.end, endStart, endEnd, divider.position + half, scale);
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
    .append({ mesh, { 1, road->path.length() } });
  return mesh;
}
