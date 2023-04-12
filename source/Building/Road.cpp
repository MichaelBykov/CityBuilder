/**
 * @file Road.cpp
 * @brief 
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Building/Road.h>
#include <CityBuilder/Tools/Markup.h>
#include <iostream>
USING_NS_CITY_BUILDER

bool RoadDef::load(const String &path, RoadDef &road) {
  return parseMarkup(path, road)
    .section("road")
      .field("name", road.name)
    .section("texture")
      .field("main", road.mainTexture)
    .section("profile")
      .records({ "M", "D", "C" }, road.profile)
        .set(&RoadDef::Point::type, {
          RoadDef::Point::Type::move,
          RoadDef::Point::Type::disjoint,
          RoadDef::Point::Type::connected
        })
        .point(&RoadDef::Point::position)
        .option("uv")
          .real(&RoadDef::Point::uv0)
        .option("normal")
          .vector(&RoadDef::Point::normal0)
        .option("normal")
          .vector(&RoadDef::Point::normal1)
      .end()
    .section("traffic")
      .records({ "U", "L", "R" }, road.traffic)
        .set(&RoadDef::Traffic::type, {
          RoadDef::Traffic::Type::unordered,
          RoadDef::Traffic::Type::left,
          RoadDef::Traffic::Type::right
        })
        .real(&RoadDef::Traffic::start)
        .identifier("-")
        .real(&RoadDef::Traffic::end)
        .comma()
        .real(&RoadDef::Traffic::elevation)
        .match(&RoadDef::Traffic::category, {
          { "all.peds", RoadDef::Traffic::Category::all_peds },
          { "all.vehicle", RoadDef::Traffic::Category::all_vehicles },
        })
        .option("speed")
          .integer(&RoadDef::Traffic::speedLimit)
          .identifier("mph")
      .end()
    .section("dividers")
      .records({ "cross-traffic", "lane" }, road.dividers)
        .set(&RoadDef::Divider::type, {
          RoadDef::Divider::Type::crossTraffic,
          RoadDef::Divider::Type::lane
        })
        .point(&RoadDef::Divider::position)
      .end()
  ;
}
