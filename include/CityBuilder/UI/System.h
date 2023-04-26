/**
 * @file System.h
 * @brief System for rendering UI elements.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/Ref.h>
#include <CityBuilder/Rendering/Resource.h>
#include <CityBuilder/Rendering/Texture.h>
#include <CityBuilder/Rendering/Uniforms.h>
#include <CityBuilder/UI/Primitive/Node.h>

NS_CITY_BUILDER_BEGIN
namespace UI {

class System {
public:
  /// @brief Add a texture to the UI system.
  /// @param name texture key
  /// @param path 
  /// @param size 
  /// @param mipMaps 
  static void addTexture(const String& name, const String& path, int size, bool mipMaps = false);

  /// @brief Gets the texture from a texture key
  /// @param name 
  /// @return texture
  static Resource<Texture>& getTexture(const String& name);

  /// @brief Sends a texture to the GPU
  /// @param name texture key
  static void loadTexture(const String& name);

  /// @brief Sets up the UI program and texture atlas
  static void start();

  /// @brief Draws a node and all of its children
  /// @param root
  /// @param offset
  static void drawNode(Ref<Node &> root, Real2 offset = { 0, 0 });

  /// @brief Sets up the UI projection and draws all the nodes
  /// @param screen 
  static void draw(const Real2& screen);
};

}
NS_CITY_BUILDER_END
