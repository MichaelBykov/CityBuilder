/**
 * @file Road.cpp
 * @brief A road in the game.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Building/Road.h>
USING_NS_CITY_BUILDER

namespace {
  int roadID = 0;
  
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
}

Road::Road(RoadDef *definition, Path2 &path)
  : definition(definition), path(path) {
  // Extrude any decorations
  Real2 half = { -definition->decorations.dimensions.x / 2, 0 };
  if (!definition->decorations.triangles.isEmpty())
    mesh.addExtrusion(
      definition->decorations,
      path, half, 0.1,
      definition->decorationsTexture,
      { 1, 1 / path.length() }
    );
  
  // Extrude the lanes
  for (const RoadDef::Lane &lane : definition->lanes)
    mesh.addExtrusion(
      lane.definition->profile,
      path, half + lane.position, 0.1,
      lane.definition->mainTexture,
      { 1, 1 / path.length() }
    );
  
  // Extrude the lane markings
  if (!definition->dividers.isEmpty()) {
    half.y += 0.01;
    half.x -= 0.1;
    for (const RoadDef::Divider &divider : definition->dividers)
      mesh.addExtrusion(
        *dividerMeshes[(int)divider.type],
        path, half + divider.position, 0.1,
        "lane-markers.png",
        { 1, Real(0.5) / path.length() }
      );
  }
  
  // Done
  mesh.finish();
}

Road::~Road() {
  // Deallocate the attached path
  delete &path;
}
