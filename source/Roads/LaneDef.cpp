/**
 * @file LaneDef.cpp
 * @brief The loading utility for a road lane definition.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Roads/LaneDef.h>
#include <CityBuilder/Tools/Markup.h>
#include <iostream>
USING_NS_CITY_BUILDER

Map<String, LaneDef> LaneDef::lanes { };

bool LaneDef::load(const String &path) {
  // Aliases
  typedef LaneDef::Traffic Traffic;
  
  LaneDef lane { };
  
  List<ProfilePoint> profile { };
  String texture;
  bool success = parseMarkup(path, lane)
    .section("lane")
      .field("name", lane.name)
    .section("texture")
      .field("main", texture)
    .section("profile")
      .profilePoints(profile)
    .section("traffic")
      .records({ "U", "D" }, lane.traffic)
        .set(&Traffic::type, {
          Traffic::Type::unordered,
          Traffic::Type::directional
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
        .option("connect")
          .match(&Traffic::connection, {
            { "none", Traffic::Connection::none },
            { "same-direction", Traffic::Connection::sameDirection },
            { "nearest", Traffic::Connection::nearest }
          })
      .end()
  ;
  if (!success)
    return false;
  
  // Compute the profile mesh
  lane.profile = profile;
  
  // Load the texture
  if (!texture.isEmpty())
    lane.mainTexture = new Texture("textures/" + texture);
  
  // Save
  LaneDef::lanes.set(lane.name, lane);
  
  return true;
}

bool LaneDef::loadBatch(const char *directory, ...) {
  bool success = true;
  
  String dir = directory;
  
  const char *path;
  va_list args;
  va_start(args, directory);
  while ((path = va_arg(args, const char *))) {
    success &= LaneDef::load(dir + path + ".lane");
  }
  va_end(args);
  
  return success;
}
