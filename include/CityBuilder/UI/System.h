/**
 * @file Events.h
 * @brief The interface for all the main game events.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/UI/Node.h>
#include <CityBuilder/Rendering/Resource.h>
#include <CityBuilder/Rendering/Texture.h>
#include <CityBuilder/Storage/Ref.h>
#include <CityBuilder/Storage/Map.h>

NS_CITY_BUILDER_BEGIN
namespace UI {

class Node;

class System {
public:
  static void drawRootNode(Ref<Node &> root);
  static void addTexture(const String& name, const String& path, int size, bool mipMaps = false);
  static Resource<Texture>& getTexture(const String& name);
  static void loadTexture(const String& name);

  /// Startup of the UI=.
  static void start();

  /// The main UI loop.
  /// \remarks
  ///   Called every single frame.
  ///   Should produce something to render.
  static void draw(const Real2& screen);
};

}
NS_CITY_BUILDER_END
