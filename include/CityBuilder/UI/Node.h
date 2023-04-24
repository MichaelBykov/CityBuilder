/**
 * @file Node.cpp
 * @brief A base UI element.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/UI/System.h>
#include <CityBuilder/Rendering/Uniforms.h>
#include <CityBuilder/Rendering/UIMesh.h>

NS_CITY_BUILDER_BEGIN
namespace UI {

/// A base UI element
class Node {
public:
  friend class System;
  Node(): _length(0, 0), _position(0, 0), _radius(0), _color(255, 255, 255, 255), _isDirty(true) {}
  Node(Real w, Real h): _length(w, h), _position(0, 0), _radius(0), _color(255, 255, 255, 255), _isDirty(true) {}
  Node(Real w, Real h, Real x, Real y): _length(w, h), _position(x, y), _radius(0), _color(255, 255, 255, 255), _isDirty(true) {}
  Node(Real w, Real h, Real x, Real y, Real r): _length(w, h), _position(x, y), _radius(r), _color(255, 255, 255, 255), _isDirty(true) {}
  Node(Real w, Real h, Real x, Real y, Real r, Color4 color): _length(w, h), _position(x, y), _radius(r), _color(color), _isDirty(true) {}
  ~Node() {};

  void setWidth();
  void getWidth();
  void setHeight();
  void getHeight();
  void setXPos();
  void getXPos();
  void setYPos();
  void getYPos();
  void setBorderRadius();
  void getBorderRadius();
  void setColor(Color4 color);
  void getColor();
private:
  Real2 _length;
  Real2 _position;
  Real _radius;
  Color4 _color;
  bool _isDirty;
  Resource<UIMesh> _uiMesh;

  void _draw();
};

}
NS_CITY_BUILDER_END
