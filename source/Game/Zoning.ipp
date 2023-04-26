/**
 * @file Zoning.ipp
 * @brief The zoning code for the game.
 * @date April 26, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Game.h>
#include <CityBuilder/Rendering/DynamicMesh.h>
USING_NS_CITY_BUILDER

namespace {
  struct Zoning {
    
    /// The zone type being zoned.
    ZoneDef *zone;
    
    
    
    Zoning(ZoneDef *zone) : zone(zone) { }
  };
}
