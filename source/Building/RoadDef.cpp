/**
 * @file RoadDef.cpp
 * @brief The loading utility for a road definition.
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Building/RoadDef.h>
#include <CityBuilder/Tools/Markup.h>
#include <CityBuilder/Storage/Map.h>
#include <iostream>
USING_NS_CITY_BUILDER

Map<String, RoadDef> RoadDef::roads { };

bool RoadDef::load(const String &path) {
  // Aliases
  typedef RoadDef::Lane Lane;
  typedef RoadDef::Divider Divider;
  
  RoadDef road { };
  
  List<ProfilePoint> decorations { };
  bool success = parseMarkup(path, road)
    .section("road")
      .field("name", road.name)
      .field("allow-buildings", road.allowBuildings, {
        { "none" , RoadDef::Buildings::none },
        { "left" , RoadDef::Buildings::left },
        { "right", RoadDef::Buildings::right },
        { "all"  , RoadDef::Buildings::all }
      })
    .section("texture")
      .field("decorations", road.decorationsTexture)
    .section("decorations")
      .profilePoints(decorations)
    .section("lanes")
      .records({ "U", "L", "R" }, road.lanes)
        .set(&Lane::direction, {
          Lane::Direction::unordered,
          Lane::Direction::left,
          Lane::Direction::right
        })
        .matchString(&Lane::definition, LaneDef::lanes)
        .point(&Lane::position)
        .option("speed")
          .integer(&Lane::speedLimit)
          .identifier("mph")
      .end()
    .section("dividers")
      .records({ "cross-traffic", "cross-edge", "lane", "edge" }, road.dividers)
        .set(&Divider::type, {
          Divider::Type::crossTraffic,
          Divider::Type::crossEdge,
          Divider::Type::lane,
          Divider::Type::edge
        })
        .point(&Divider::position)
      .end()
  ;
  if (!success)
    return false;
  
  road.decorations = decorations;
  
  // Compute the bounds
  road.dimensions = { 0, 0 };
  for (const Lane &lane : road.lanes) {
    Real2 bound = lane.position + lane.definition->profile.dimensions;
    if (bound.x > road.dimensions.x) road.dimensions.x = bound.x;
    if (bound.y > road.dimensions.y) road.dimensions.y = bound.y;
  }
  if (road.decorations.dimensions.x > road.dimensions.x)
    road.dimensions.x = road.decorations.dimensions.x;
  if (road.decorations.dimensions.y > road.dimensions.y)
    road.dimensions.y = road.decorations.dimensions.y;
  
  // Generate the end cap
  road.endCap = new SharedMesh();
  for (const Lane &lane : road.lanes)
    if (lane.position.x + lane.definition->profile.dimensions.x / 2 <= road.dimensions.x / 2 + 0.01) {
      road.endCap->addRevolution(
        {
          lane.definition->profile,
          lane.position,
          0.1
        },
        lane.definition->mainTexture,
        { 1, 1 }
      );
    }
  road.endCap->finish();
  
  // Save
  RoadDef::roads.set(road.name, road);
  
  return true;
}

bool RoadDef::loadBatch(const char *directory, ...) {
  bool success = true;
  
  String dir = directory;
  
  const char *path;
  va_list args;
  va_start(args, directory);
  while ((path = va_arg(args, const char *))) {
    success &= RoadDef::load(dir + path + ".road");
  }
  va_end(args);
  
  return success;
}
