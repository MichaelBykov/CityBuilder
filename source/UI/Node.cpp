/**
 * @file Node.cpp
 * @brief A base UI element.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/Node.h>
USING_NS_CITY_BUILDER
using namespace UI;

void Node::setBorderRadius(Real radius) {
  _radius = radius;
  if (_radius > 0) {
    setTexture("Round");
  } else {
    setTexture("Square");
  }
  _isDirty = true;
}

Real Node::getBorderRadius() {
  return _radius;
}

void Node::setTexture(const String& texture) {
  _texture = texture;
}

String& Node::getTexture() {
  return _texture;
}

void Node::setParent(Ref<Node &>* parent) {
  _parent = parent;
  _isDirty = true;
}

Ref<Node &>* Node::getParent() {
  return _parent;
}

void Node::draw() {
  _draw();
}

void Node::_draw() {
  // Don't draw empty elements
  if (_length.x < 1 || _length.y < 1) {
    return;
  }

  // Only recreate the mesh if it's dirty
  if (_isDirty) {    
    _uiMesh = new UIMesh();

    Real2 local = _position;
    if (_parent != nullptr) {
      local += (*_parent)->_position;
    }

    // Draw a rounded square or square
    if (_radius > 0) {
      _uiMesh->add({
        // Top Left Corner
        { Real3(local.x, local.y, _zIndex), Real2(0, 0), _color },
        { Real3(local.x + _radius, local.y, _zIndex), Real2(0.33, 0), _color },
        { Real3(local.x, local.y + _radius, _zIndex), Real2(0, 0.33), _color },
        { Real3(local.x + _radius, local.y + _radius, _zIndex), Real2(0.33, 0.33), _color },
        // Top Right Corner
        { Real3(local.x + (_length.x - _radius), local.y, _zIndex), Real2(0.67, 0), _color },
        { Real3(local.x + _length.x, local.y, _zIndex), Real2(1, 0), _color },
        { Real3(local.x + (_length.x - _radius), local.y + _radius, _zIndex), Real2(0.67, 0.33), _color },
        { Real3(local.x + _length.x, local.y + _radius, _zIndex), Real2(1, 0.33), _color },
        // Bottom Left Corner
        { Real3(local.x, local.y + (_length.y - _radius), _zIndex), Real2(0, 0.67), _color },
        { Real3(local.x + _radius, local.y + (_length.y - _radius), _zIndex), Real2(0.33, 0.67), _color },
        { Real3(local.x, local.y + _length.y, _zIndex), Real2(0, 1), _color },
        { Real3(local.x + _radius, local.y + _length.y, _zIndex), Real2(0.33, 1), _color },
        // Bottom Right Corner
        { Real3(local.x + (_length.x - _radius), local.y + (_length.y - _radius), _zIndex), Real2(0.67, 0.67), _color },
        { Real3(local.x + _length.x, local.y + (_length.y - _radius), _zIndex), Real2(1, 0.67), _color },
        { Real3(local.x + (_length.x - _radius), local.y + _length.y, _zIndex), Real2(0.67, 1), _color },
        { Real3(local.x + _length.x, local.y + _length.y, _zIndex), Real2(1, 1), _color },
      }, {
        0, 1, 2, 1, 2, 3,       // Top Left Corner
        4, 5, 6, 5, 6, 7,       // Top Right Corner
        8, 9, 10, 9, 10, 11,    // Bottom Left Corner
        12, 13, 14, 13, 14, 15, // Bottom Right Corner
        2, 3, 8, 8, 9, 3,       // Left Inside Face
        6, 7, 12, 12, 13, 7,    // Right Inside Face
        1, 4, 11, 11, 14, 4,    // Middle Face
      });
    } else {
      _uiMesh->add({
        // It's a box
        { Real3(local.x, local.y, 1), Real2(0, 0), _color },
        { Real3(local.x + _length.x, local.y, 1), Real2(1, 0), _color },
        { Real3(local.x, local.y + _length.y, 1), Real2(0, 1), _color },
        { Real3(local.x + _length.x, local.y + _length.y, 1), Real2(1, 1), _color },
      }, {
        0, 1, 2, 2, 3, 1, // Box
      });
    }

    // Send the mesh to the GPU
    _uiMesh->load();
    _isDirty = false;
  }

  _uiMesh->draw(Program::ui);

  // State is being reset between renders, this sets it back to what we want
  bgfx::setState(
    BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
    BGFX_STATE_MSAA |
    BGFX_STATE_DEPTH_TEST_ALWAYS |
    BGFX_STATE_BLEND_FUNC(
      BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
  );
}
