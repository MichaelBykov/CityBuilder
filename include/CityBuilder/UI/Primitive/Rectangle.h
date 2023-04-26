/**
 * @file Rectangle.h
 * @brief A non-rounded rectangle.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/UI/Primitive/Node.h>

NS_CITY_BUILDER_BEGIN
namespace UI {

class Rectangle: public Node {
public:
  Rectangle() {
    setTexture("Square");
  }
  ~Rectangle() {}

  /// @brief Sets up the rectangle mesh
  void setMesh(Real2 offset) override;
};

}
NS_CITY_BUILDER_END
