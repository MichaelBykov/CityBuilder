/**
 * @file Element.h
 * @brief A higher order UI element.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/UI/Primitive/Node.h>
#include <CityBuilder/UI/Primitive/Rectangle.h>
#include <CityBuilder/UI/Primitive/Rounded.h>

NS_CITY_BUILDER_BEGIN
namespace UI {

class Element {
public:
  Element() {}
  ~Element() {}

  /// @brief Set the width and height of the node.
  /// @param size 
  void setDimensions(Real2 size);

  /// @brief Get the width and height of the node.
  /// @return 
  Real2 getDimensions();
  
  /// @brief  Set the position of the node.
  /// @param position 
  void setPosition(Real2 position);

  /// @brief Get the position of the node.
  /// @return position
  Real2 getPosition();

  /// @brief Set the color of the node.
  /// @param color 
  void setColor(Color4 color);

  /// @brief Get the color of the node.
  /// @return color
  Color4 getColor();

  /// @brief Sets the padding for each side
  /// @param padding 
  void setPadding(Real4 padding);
  
  /// @brief Gets the padding for each side
  /// @return padding
  Real4 getPadding();

  /// @brief Sets the margin for each side
  /// @param margin 
  void setMargin(Real4 margin);
  
  /// @brief Gets the margin for each side
  /// @return margin
  Real4 getMargin();

  /// @brief Set's the border width
  /// \remarks
  ///   This lays out like content: border-box; from css land.
  /// @param width 
  void setBorderWidth(Real width);

  /// @brief Get's the border width
  /// @return width
  Real getBorderWidth();

  /// @brief Sets the radius of the rounded corners
  /// @param radius
  void setBorderRadius(Real radius);

  /// @brief Gets the radius of the rounded corners
  /// @return radius
  Real getBorderRadius();

  /// @brief Sets the border color
  /// @param color 
  void setBorderColor(Color4 color);
  
  /// @brief Gets the border color
  /// @return color
  Color4 getBorderColor();

  /// @brief Sets the parent element
  /// @param parent 
  void setParent(Ref<Element> parent);
  
  /// @brief Gets the parent element
  /// @return parent
  Ref<Element> getParent();

  /// @brief Adds a child element
  /// @param child
  void appendChild(Ref<Element> child);

  /// @brief Finds and removes a child
  /// @param child
  void removeChild(Ref<Element> child);

  /// @brief Gets all children
  /// @return list of children
  List<Ref<Element>> getChildren();

  /// @brief Draws the element.
  void draw();

private:
  // The representative node of this element.
  Ref<Rectangle &> _square;
  Ref<Rounded &> _round;
  Ref<Node &> _border;
  Real4 _padding;
  Real4 _margin;
  Real _borderWidth;
  Color4 _borderColor;
  List<Ref<Element>> _children;

  /// @brief Figures out which node is in use
  /// @return node-like
  Ref<Node &> _getActiveNode();
};

}
NS_CITY_BUILDER_END
