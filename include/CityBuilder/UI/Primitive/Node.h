/**
 * @file Node.h
 * @brief Smallest renderable UI element.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Rendering/Resource.h>
#include <CityBuilder/Rendering/UIMesh.h>

NS_CITY_BUILDER_BEGIN
namespace UI {

class Node {
public:
  /// @brief Set the width and height of the node.
  /// @param size 
  void setDimensions(Real2 size);

  /// @brief Get the width and height of the node.
  /// @return 
  Real2 getDimensions();
  
  /// @brief  Set the position of the node.
  /// @param position 
  void setPosition(Real2 position);

  /// @brief Get the position of the node.
  /// @return position
  Real2 getPosition();

  /// @brief Set the color of the node.
  /// @param color 
  void setColor(Color4 color);

  /// @brief Get the color of the node.
  /// @return color
  Color4 getColor();

  /// @brief Set the z-index of the node.
  /// @param zIndex 
  void setZIndex(Real zIndex);

  /// @brief Get the z-index of the node.
  /// @return zIndex
  Real getZIndex();

  /// @brief Set which texture key to use.
  /// @param textureKey 
  void setTexture(const String& textureKey);

  /// @brief The texture key used.
  /// @return textureKey
  String& getTexture();

  /// @brief Generate the mesh and set it locally.
  /// @param offset
  virtual void setMesh(Real2 offset) = 0;

  /// @brief Draw the mesh.
  /// @param offset 
  virtual void drawMesh(Real2 offset);

protected:
  bool _isDirty;
  Real2 _size;
  Real2 _position;
  Color4 _color;
  Real _zIndex;
  String _textureKey;
  Resource<UIMesh> _mesh;
};

}
NS_CITY_BUILDER_END
