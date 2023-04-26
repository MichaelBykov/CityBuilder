/**
 * @file Rectangle.h
 * @brief A single letter.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/Primitive/Character.h>

USING_NS_CITY_BUILDER
using namespace UI;

void Character::setCharacter(char c) {
  _character = c;
}

char Character::getCharacter() {
  return _character;
}

void Character::setMesh(Real2 offset) {
  Real2 local = offset + _position;

  // Make sure the character is valid
  if (_character < 32 || _character > 126) {
    _character = 32;
  }

  // Find the texture coords
  Real2 tl, tr, bl, br = { 0, 0 };
  int index = _character - 32;

  // Find top left
  int anchor = index - (index % 20);
  for (int i = 0; i < (index % 20) - 1; i++) {
    tl.x += Real(charWidth[anchor + i] * 2) / Real(384);
  }
  for (int i = 0; i < index / 20; i++) {
    tl.y += Real(15 * 2) / Real(384);
  }

  // Find top right
  tr.x = tl.x + (Real(charWidth[index] * 2) / Real(384));
  tr.y = tl.y;

  // Find bottom left
  bl.x = tl.x;
  bl.y = tl.y + Real(15 * 2) / Real(384);

  // Find bottom right
  br.x = tr.x;
  br.y = bl.y;

  // Make the mesh
  _mesh = new UIMesh();
  _mesh->add({
    // It's a box
    { Real3(local.x, local.y, 1), tl, _color },
    { Real3(local.x + _size.x, local.y, 1), tr, _color },
    { Real3(local.x, local.y + _size.y, 1), bl, _color },
    { Real3(local.x + _size.x, local.y + _size.y, 1), br, _color },
  }, {
    0, 1, 2, 2, 3, 1, // Box
  });
  _mesh->load();
}
