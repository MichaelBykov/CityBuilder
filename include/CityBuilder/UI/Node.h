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
  Node():
    _length(0, 0),
    _position(0, 0),
    _radius(0),
    _zIndex(1), 
    _color(255, 255, 255, 255),
    _isDirty(true),
    _texture("Square")
  {}

  Node(Real w, Real h):
    _length(w, h),
    _position(0, 0),
    _radius(0),
    _zIndex(1),
    _color(255, 255, 255, 255),
    _isDirty(true),
    _texture("Square")
  {}

  Node(Real w, Real h, Real x, Real y):
    _length(w, h),
    _position(x, y),
    _radius(0),
    _zIndex(1),
    _color(255, 255, 255, 255),
    _isDirty(true),
    _texture("Square")
  {}

  Node(Real w, Real h, Real x, Real y, Real r):
    _length(w, h),
    _position(x, y),
    _zIndex(1),
    _color(255, 255, 255, 255),
    _isDirty(true)
  { setBorderRadius(r); }

  Node(Real w, Real h, Real x, Real y, Real r, Color4 color):
    _length(w, h),
    _position(x, y),
    _zIndex(1),
    _color(color),
    _isDirty(true)
  { setBorderRadius(r); }

  ~Node() {};

  void setWidth();
  void getWidth();
  void setHeight();
  void getHeight();
  void setXPos();
  void getXPos();
  void setYPos();
  void getYPos();
  void setBorderRadius(Real radius);
  Real getBorderRadius();
  void setColor(Color4 color);
  void getColor();
  void setTexture(const String& texture);
  String& getTexture();
  void draw();

protected:
  Real2 _length;
  Real2 _position;
  Real _radius;
  Real _zIndex;
  Color4 _color;
  bool _isDirty;
  String _texture;
  Resource<UIMesh> _uiMesh;

  void _draw();
  // virtual void _draw();
};

}
NS_CITY_BUILDER_END
