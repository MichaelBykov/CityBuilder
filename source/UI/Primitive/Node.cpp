/**
 * @file Node.cpp
 * @brief Smallest renderable UI element.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/Primitive/Node.h>

USING_NS_CITY_BUILDER
using namespace UI;

void Node::setDimensions(Real2 size) {
  _size = size;
  _isDirty = true;
}

Real2 Node::getDimensions() {
  return _size;
}

void Node::setPosition(Real2 position) {
  _position = position;
  _isDirty = true;
}

Real2 Node::getPosition() {
  return _position;
}

void Node::setColor(Color4 color) {
  _color = color;
  _isDirty = true;
}

Color4 Node::getColor() {
  return _color;
}

void Node::setZIndex(Real zIndex) {
  _zIndex = zIndex;
}

Real Node::getZIndex() {
  return _zIndex;
}

void Node::setTexture(const String& textureKey) {
  _textureKey = textureKey;
  _isDirty = true;
}

String& Node::getTexture() {
  return _textureKey;
}

void Node::drawMesh(Real2 offset) {
  // Don't draw empty elements
  if (_size.x < 1 || _size.y < 1) return;

  // Only recreate the mesh if it's dirty
  if (_isDirty) {
    setMesh(offset);
    _isDirty = false;
  }

  // Draw the mesh
  _mesh->draw(Program::ui);

  // State is being reset...
  bgfx::setState(
    BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
    BGFX_STATE_MSAA |
    BGFX_STATE_DEPTH_TEST_ALWAYS |
    BGFX_STATE_BLEND_FUNC(
      BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
  );
}
