/**
 * @file Rectangle.cpp
 * @brief A non-rounded rectangle.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/Primitive/Rectangle.h>

USING_NS_CITY_BUILDER
using namespace UI;

void Rectangle::setMesh(Real2 offset) {
  Real2 local = offset + _position;

  _mesh = new UIMesh();
  _mesh->add({
    // It's a box
    { Real3(local.x, local.y, 1), Real2(0, 0), _color },
    { Real3(local.x + _size.x, local.y, 1), Real2(1, 0), _color },
    { Real3(local.x, local.y + _size.y, 1), Real2(0, 1), _color },
    { Real3(local.x + _size.x, local.y + _size.y, 1), Real2(1, 1), _color },
  }, {
    0, 1, 2, 2, 3, 1, // Box
  });
  _mesh->load();
}
