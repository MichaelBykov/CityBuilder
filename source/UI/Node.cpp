/**
 * @file Node.cpp
 * @brief A base UI element.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/Node.h>
USING_NS_CITY_BUILDER
using namespace UI;

void Node::draw() {
  _draw();
}

void Node::_draw() {
  if (_isDirty) {    
    _uiMesh = new UIMesh();

    if (_radius > 0) {
      _uiMesh->add({
        // Top Left Corner
        { Real3(_position.x, _position.y, _zIndex), Real2(0, 0), _color },
        { Real3(_position.x + _radius, _position.y, _zIndex), Real2(0.33, 0), _color },
        { Real3(_position.x, _position.y + _radius, _zIndex), Real2(0, 0.33), _color },
        { Real3(_position.x + _radius, _position.y + _radius, _zIndex), Real2(0.33, 0.33), _color },
        // Top Right Corner
        { Real3(_position.x + (_length.x - _radius), _position.y, _zIndex), Real2(0.67, 0), _color },
        { Real3(_position.x + _length.x, _position.y, _zIndex), Real2(1, 0), _color },
        { Real3(_position.x + (_length.x - _radius), _position.y + _radius, _zIndex), Real2(0.67, 0.33), _color },
        { Real3(_position.x + _length.x, _position.y + _radius, _zIndex), Real2(1, 0.33), _color },
        // Bottom Left Corner
        { Real3(_position.x, _position.y + (_length.y - _radius), _zIndex), Real2(0, 0.67), _color },
        { Real3(_position.x + _radius, _position.y + (_length.y - _radius), _zIndex), Real2(0.33, 0.67), _color },
        { Real3(_position.x, _position.y + _length.y, _zIndex), Real2(0, 1), _color },
        { Real3(_position.x + _radius, _position.y + _length.y, _zIndex), Real2(0.33, 1), _color },
        // Bottom Right Corner
        { Real3(_position.x + (_length.x - _radius), _position.y + (_length.y - _radius), _zIndex), Real2(0.67, 0.67), _color },
        { Real3(_position.x + _length.x, _position.y + (_length.y - _radius), _zIndex), Real2(1, 0.67), _color },
        { Real3(_position.x + (_length.x - _radius), _position.y + _length.y, _zIndex), Real2(0.67, 1), _color },
        { Real3(_position.x + _length.x, _position.y + _length.y, _zIndex), Real2(1, 1), _color },
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
        { Real3(_position.x, _position.y, 1), Real2(0.5, 0.5), _color },
        { Real3(_position.x + _length.x, _position.y, 1), Real2(0.5, 0.5), _color },
        { Real3(_position.x, _position.y + _length.y, 1), Real2(0.5, 0.5), _color },
        { Real3(_position.x + _length.x, _position.y + _length.y, 1), Real2(0.5, 0.5), _color },
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
