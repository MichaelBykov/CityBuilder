/**
 * @file Rounded.h
 * @brief A rounded rectangle.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/UI/Primitive/Node.h>

NS_CITY_BUILDER_BEGIN
namespace UI {

class Rounded: public Node {
public:
  Rounded();
  Rounded(Ref<Node &> other);
  ~Rounded() {}

  /// @brief Sets up the rounded rectangle mesh
  void setMesh(Real2 offset) override;

  /// @brief Sets the radius of the rounded corners
  /// @param radius
  void setBorderRadius(Real radius);

  /// @brief Gets the radius of the rounded corners
  /// @return radius
  Real getBorderRadius();

private:
  Real _radius;
};

}
NS_CITY_BUILDER_END
