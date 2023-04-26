/**
 * @file ZoneDef.cpp
 * @brief The loading utility for a building zone definition.
 * @date April 26, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Zones/ZoneDef.h>
#include <CityBuilder/Tools/Markup.h>
#include <iostream>
USING_NS_CITY_BUILDER

Map<String, ZoneDef> ZoneDef::zones { };

bool ZoneDef::load(const String &path) {
  ZoneDef zone { };
  
  bool success = parseMarkup(path, zone)
    .section("zone")
      .field("name", zone.name)
      .field("color", zone.color, {
        { "green", Color3(125, 255, 65) }
      })
  ;
  if (!success)
    return false;
  
  // Save
  ZoneDef::zones.set(zone.name, zone);
  
  return true;
}

bool ZoneDef::loadBatch(const char *directory, ...) {
  bool success = true;
  
  String dir = directory;
  
  const char *path;
  va_list args;
  va_start(args, directory);
  while ((path = va_arg(args, const char *))) {
    success &= ZoneDef::load(dir + path + ".zone");
  }
  va_end(args);
  
  return success;
}
