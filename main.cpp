/**
 * @file main.cpp
 * @brief The main program driver.
 * @date April 8, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Game.h>
#include <CityBuilder/Building/Road.h>
USING_NS_CITY_BUILDER

int main(int argc, char** argv) {
  // Load the roads
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
  ) return 1;
  
  Game game { };
  game.run();
  game.close();
  return 0;
}
