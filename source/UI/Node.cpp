/**
 * @file Node.cpp
 * @brief A base UI element.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/Node.h>
USING_NS_CITY_BUILDER
using namespace UI;

void Node::_draw() {
  if (_isDirty) {    
    _uiMesh = new UIMesh();

    if (_radius > 0) {
      _uiMesh->add({
        // Top Left Corner
        { Real3(_position.x, _position.y, 1), Real2(0, 0) },
        { Real3(_position.x + _radius, _position.y, 1), Real2(0.33, 0) },
        { Real3(_position.x, _position.y + _radius, 1), Real2(0, 0.33) },
        { Real3(_position.x + _radius, _position.y + _radius, 1), Real2(0.33, 0.33) },
        // Top Right Corner
        { Real3(_position.x + (_length.x - _radius), _position.y, 1), Real2(0.67, 0) },
        { Real3(_position.x + _length.x, _position.y, 1), Real2(1, 0) },
        { Real3(_position.x + (_length.x - _radius), _position.y + _radius, 1), Real2(0.67, 0.33) },
        { Real3(_position.x + _length.x, _position.y + _radius, 1), Real2(1, 0.33) },
        // Bottom Left Corner
        { Real3(_position.x, _position.y + (_length.y - _radius), 1), Real2(0, 0.67) },
        { Real3(_position.x + _radius, _position.y + (_length.y - _radius), 1), Real2(0.33, 0.67) },
        { Real3(_position.x, _position.y + _length.y, 1), Real2(0, 1) },
        { Real3(_position.x + _radius, _position.y + _length.y, 1), Real2(0.33, 1) },
        // Bottom Right Corner
        { Real3(_position.x + (_length.x - _radius), _position.y + (_length.y - _radius), 1), Real2(0.67, 0.67) },
        { Real3(_position.x + _length.x, _position.y + (_length.y - _radius), 1), Real2(1, 0.67) },
        { Real3(_position.x + (_length.x - _radius), _position.y + _length.y, 1), Real2(0.67, 1) },
        { Real3(_position.x + _length.x, _position.y + _length.y, 1), Real2(1, 1) },
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
        { Real3(_position.x, _position.y, 1), Real2(0.5, 0.5) },
        { Real3(_position.x + _length.x, _position.y, 1), Real2(0.5, 0.5) },
        { Real3(_position.x, _position.y + _length.y, 1), Real2(0.5, 0.5) },
        { Real3(_position.x + _length.x, _position.y + _length.y, 1), Real2(0.5, 0.5) },
      }, {
        0, 1, 2, 2, 3, 1, // Box
      });
    }

    _uiMesh->load();
  }

  _uiMesh->draw(Program::ui);
}
