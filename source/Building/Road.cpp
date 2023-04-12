/**
 * @file Road.cpp
 * @brief The loading utility for a road definition.
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Building/Road.h>
#include <CityBuilder/Tools/Markup.h>
#include <iostream>
USING_NS_CITY_BUILDER

bool RoadDef::load(const String &path, RoadDef &road) {
  // Aliases
  typedef RoadDef::Point Point;
  typedef RoadDef::Traffic Traffic;
  typedef RoadDef::Divider Divider;
  
  bool success = parseMarkup(path, road)
    .section("road")
      .field("name", road.name)
    .section("texture")
      .field("main", road.mainTexture)
    .section("profile")
      .records({ "M", "D", "C" }, road.profile)
        .set(&Point::type, {
          Point::Type::move,
          Point::Type::disjoint,
          Point::Type::connected
        })
        .point(&Point::position)
        .option("uv")
          .real(&Point::uv0)
        .option("normal")
          .vector(&Point::normal0)
        .option("normal")
          .vector(&Point::normal1)
      .end()
    .section("traffic")
      .records({ "U", "L", "R" }, road.traffic)
        .set(&Traffic::type, {
          Traffic::Type::unordered,
          Traffic::Type::left,
          Traffic::Type::right
        })
        .real(&Traffic::start)
        .identifier("-")
        .real(&Traffic::end)
        .comma()
        .real(&Traffic::elevation)
        .match(&Traffic::category, {
          { "all.peds", Traffic::Category::all_peds },
          { "all.vehicle", Traffic::Category::all_vehicles },
        })
        .option("speed")
          .integer(&Traffic::speedLimit)
          .identifier("mph")
      .end()
    .section("dividers")
      .records({ "cross-traffic", "lane" }, road.dividers)
        .set(&Divider::type, {
          Divider::Type::crossTraffic,
          Divider::Type::lane
        })
        .point(&Divider::position)
      .end()
  ;
  if (!success)
    return false;
  
  // Compute the bounds
  for (const Point &point : road.profile) {
    if (point.position.x > road.dimensions.x)
      road.dimensions.x = point.position.x;
    if (point.position.y > road.dimensions.y)
      road.dimensions.y = point.position.y;
  }
  return true;
}
