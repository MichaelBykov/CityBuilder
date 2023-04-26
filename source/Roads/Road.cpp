/**
 * @file Road.cpp
 * @brief A road in the game.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Roads/Road.h>
USING_NS_CITY_BUILDER

Road::Road(RoadDef *definition, Ref<Path2 &> path)
  : definition(definition),
    path(path, definition->dimensions.x * Real(0.5 * 0.333333333333)) {
  
}

ZoneDef *Road::leftZone() const {
  return _leftZone;
}

void Road::setLeftZone(ZoneDef *zone) {
  _leftZone = zone;
}

ZoneDef *Road::rightZone() const {
  return _rightZone;
}

void Road::setRightZone(ZoneDef *zone) {
  _rightZone = zone;
}
