/**
 * @file Events.h
 * @brief The interface for all the main game events.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/UI/Node.h>
#include <CityBuilder/Storage/Ref.h>

NS_CITY_BUILDER_BEGIN
namespace UI {

class Node;

class System {
public:
  static void drawRootNode(Ref<Node &> root);
private:
  // Resource<Texture> _uiTexture;
};

}
NS_CITY_BUILDER_END
