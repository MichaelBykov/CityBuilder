/**
 * @file Element.cpp
 * @brief A higher order UI element.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/Element.h>
#include <CityBuilder/UI/System.h>

USING_NS_CITY_BUILDER
using namespace UI;

Ref<Node &> Element::_getActiveNode() {
  if (!_square.empty()) {
    return (Ref<Node &>)_square;
  } else if (!_round.empty()) {
    return (Ref<Node &>)_round;
  } else {
    _square = new Rectangle();
    return (Ref<Node &>)_square;
  }
}

void Element::setDimensions(Real2 size) {
  _getActiveNode()->setDimensions(size);
}

Real2 Element::getDimensions() {
  return _getActiveNode()->getDimensions();
}

void Element::setPosition(Real2 position) {
  _getActiveNode()->setPosition(position);
}

Real2 Element::getPosition() {
  return _getActiveNode()->getPosition();
}

void Element::setColor(Color4 color) {
  _getActiveNode()->setColor(color);
}

Color4 Element::getColor() {
  return _getActiveNode()->getColor();
}

void Element::setPadding(Real4 padding) {
  _padding = padding;
}

Real4 Element::getPadding() {
  return _padding;
}

void Element::setMargin(Real4 margin) {
  _margin = margin;
}

Real4 Element::getMargin() {
  return _margin;
}

void Element::setBorderWidth(Real width) {
  _borderWidth = width;
}

Real Element::getBorderWidth() {
  return _borderWidth;
}

void Element::setBorderRadius(Real radius) {
  if (radius > 0) {
    _round = new Rounded((Ref<Node &>)_square);
    _round->setBorderRadius(radius);
    _square = nullptr;
  } else {
    _round = nullptr;
  }
}

Real Element::getBorderRadius() {
  if (_round.empty()) {
    return _round->getBorderRadius();
  } else {
    return 0;
  }
}

void Element::setBorderColor(Color4 color) {
  if (!_round.empty()) {
    _round = new Rounded();
  }
  _round->setColor(color);
}

Color4 Element::getBorderColor() {
  if (_round.empty()) {
    return _round->getColor();
  } else {
    return Color4(0, 0, 0, 0);
  }
}

void Element::appendChild(Ref<Element> child) {
  _children.append(child);
}

void Element::removeChild(Ref<Element> child) {
  for (int i = 0; i < _children.count(); i++) {
    if (_children[i] == child) {
      _children.remove(i);
      break;
    }
  }
}

List<Ref<Element>> Element::getChildren() {
  return _children;
}

void Element::draw() {
  if (_children.count() > 0) {
    for (int i = 0; i < _children.count(); i++) {
      _children[i]->draw();
    }
  } else {
    System::loadTexture(_getActiveNode()->getTexture());
    _getActiveNode()->drawMesh({ 0, 0 });
  }
}
