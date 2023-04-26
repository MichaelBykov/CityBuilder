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
class Icon: public Node {
public:
  Icon(){ setTexture(""); }
  Icon(String icoName) { setTexture(icoName); }
  Icon(Real w, Real h): Node(w, h){ setTexture(""); }
  Icon(String icoName, Real w, Real h): Node(w, h) { setTexture(icoName); }
  Icon(Real w, Real h, Real x, Real y): Node(w, h, x, y){ setTexture(""); }
  Icon(Real w, Real h, Real x, Real y, Real r): Node(w, h, x, y, r){ setTexture(""); }
  Icon(String icoName, Real w, Real h, Real x, Real y): Node(w, h, x, y) { setTexture(icoName); }
  Icon(Real w, Real h, Real x, Real y, Real r, Color4 color): Node(w, h, x, y, r, color){ setTexture(""); }
  Icon(String icoName, Real w, Real h, Real x, Real y, Real r): Node(w, h, x, y, r) { setTexture(icoName); }
  Icon(String icoName, Real w, Real h, Real x, Real y, Real r, Color4 color): Node(w, h, x, y, r, color) { setTexture(icoName); }
  ~Icon() {}

  void setIcon(const String& icoName);
  String& getIcon();

private:
};

}
NS_CITY_BUILDER_END
