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
  
  RoadDef singleRoad;
  RoadDef::load("roads/single.road", singleRoad);
  
  Game game { };
  game.run();
  game.close();
  return 0;
}
