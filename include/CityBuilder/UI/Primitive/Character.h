/**
 * @file Rectangle.h
 * @brief A single letter.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/UI/Primitive/Node.h>

NS_CITY_BUILDER_BEGIN
namespace UI {

class Character: public Node {
public:
  Character() {
    setTexture("Font");
  }
  ~Character() {}

  /// @brief Sets up the rectangle mesh
  void setMesh(Real2 offset) override;

  /// @brief Set which letter to display
  /// @param c 
  void setCharacter(char c);

  /// @brief Currently displayed letter
  /// @return char
  char getCharacter();

  const int charWidth[94] = {4,5,8,8,10,8,3,5,5,6,8,4,6,4,5,8,6,8,8,8,8,8,8,8,8,4,4,8,8,8,7,12,9,8,9,9,8,8,9,9,4,7,8,7,11,10,10,8,10,8,8,8,9,9,12,8,8,8,5,5,5,6,6,6,7,8,7,8,7,5,8,8,3,3,7,3,11,8,8,8,8,5,7,5,7,7,10,7,7,7,5,4,5,8};
private:
  char _character;
};

}
NS_CITY_BUILDER_END
