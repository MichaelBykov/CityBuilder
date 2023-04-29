/**
 * @file Rounded.cpp
 * @brief A rounded rectangle.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/Primitive/Rounded.h>

USING_NS_CITY_BUILDER
using namespace UI;

Rounded::Rounded() {
  setTexture("Round");
}

Rounded::Rounded(Ref<Node &> other) {
  setTexture("Round");
  setDimensions(other->getDimensions());
  setPosition(other->getPosition());
  setColor(other->getColor());
  setZIndex(other->getZIndex());
  if (other->getTexture() != "Square") {
    setTexture(other->getTexture());
  }
}

void Rounded::setMesh(Real2 offset) {
  Real2 local = offset + _position;

  _mesh = new UIMesh();
  _mesh->add({	
    // Top Left Corner	
    { Real3(local.x, local.y, 1), Real2(0, 0), _color },	
    { Real3(local.x + _radius, local.y, 1), Real2(0.33, 0), _color },	
    { Real3(local.x, local.y + _radius, 1), Real2(0, 0.33), _color },	
    { Real3(local.x + _radius, local.y + _radius, 1), Real2(0.33, 0.33), _color },	
    // Top Right Corner	
    { Real3(local.x + (_size.x - _radius), local.y, 1), Real2(0.67, 0), _color },	
    { Real3(local.x + _size.x, local.y, 1), Real2(1, 0), _color },	
    { Real3(local.x + (_size.x - _radius), local.y + _radius, 1), Real2(0.67, 0.33), _color },	
    { Real3(local.x + _size.x, local.y + _radius, 1), Real2(1, 0.33), _color },	
    // Bottom Left Corner	
    { Real3(local.x, local.y + (_size.y - _radius), 1), Real2(0, 0.67), _color },	
    { Real3(local.x + _radius, local.y + (_size.y - _radius), 1), Real2(0.33, 0.67), _color },	
    { Real3(local.x, local.y + _size.y, 1), Real2(0, 1), _color },	
    { Real3(local.x + _radius, local.y + _size.y, 1), Real2(0.33, 1), _color },	
    // Bottom Right Corner	
    { Real3(local.x + (_size.x - _radius), local.y + (_size.y - _radius), 1), Real2(0.67, 0.67), _color },	
    { Real3(local.x + _size.x, local.y + (_size.y - _radius), 1), Real2(1, 0.67), _color },	
    { Real3(local.x + (_size.x - _radius), local.y + _size.y, 1), Real2(0.67, 1), _color },	
    { Real3(local.x + _size.x, local.y + _size.y, 1), Real2(1, 1), _color },	
  }, {	
    0, 1, 2, 1, 2, 3,       // Top Left Corner	
    4, 5, 6, 5, 6, 7,       // Top Right Corner	
    8, 9, 10, 9, 10, 11,    // Bottom Left Corner	
    12, 13, 14, 13, 14, 15, // Bottom Right Corner	
    2, 3, 8, 8, 9, 3,       // Left Inside Face	
    6, 7, 12, 12, 13, 7,    // Right Inside Face	
    1, 4, 11, 11, 14, 4,    // Middle Face	
  });
  _mesh->load();
}

void Rounded::setBorderRadius(Real radius) {
  _radius = radius;
  _isDirty = true;
}

Real Rounded::getBorderRadius() {
  return _radius;
}
