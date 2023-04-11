/**
 * @file main.cpp
 * @brief The main program driver.
 * @date April 8, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Game.h>
USING_NS_CITY_BUILDER

int main(int argc, char** argv) {
  Game game { };
  game.run();
  game.close();
  return 0;
}
