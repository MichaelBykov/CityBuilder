/**
 * @file Events.h
 * @brief The interface for all the main game events.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/System.h>
USING_NS_CITY_BUILDER
using namespace UI;

void System::drawRootNode(Ref<Node &> root) {
  root->_draw();
}
