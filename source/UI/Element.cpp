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

Element::Element() {
  _isDirty = true;
  _gap = { 0, 0 };
}

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
  _isDirty = true;
}

Real2 Element::getDimensions() {
  return _getActiveNode()->getDimensions();
}

void Element::setPosition(Real2 position) {
  _getActiveNode()->setPosition(position);
  _isDirty = true;
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
  _isDirty = true;
}

Real4 Element::getPadding() {
  return _padding;
}

void Element::setMargin(Real4 margin) {
  _margin = margin;
  _isDirty = true;
}

Real4 Element::getMargin() {
  return _margin;
}

void Element::setBorderWidth(Real width) {
  _borderWidth = width;
  _isDirty = true;
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
    _square = new Rectangle((Ref<Node &>)_round);
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

void Element::setBackgroundImage(const String& textureKey) {
  _getActiveNode()->setTexture(textureKey);
}

String& Element::getBackgroundImage() {
  return _getActiveNode()->getTexture();
}

void Element::appendChild(Ref<Element &> child) {
  _children.append(child);
  _isDirty = true;
}

void Element::removeChild(Ref<Element &> child) {
  for (int i = 0; i < _children.count(); i++) {
    if (_children[i] == child) {
      _children.remove(i);
      _isDirty = true;
      break;
    }
  }
}

List<Ref<Element &>> Element::getChildren() {
  return _children;
}

Real2 Element::getBounds() {
  return _bounds;
}

void Element::draw(Real2 offset) {
  if (_isDirty) {
    _bounds = { 0, 0 };

    // Add up size of children
    for (auto &child : _children) {
      _bounds.x += child->getBounds().x;

      if (_bounds.y < getDimensions().y) {
        _bounds.y = getDimensions().y;
      }

      // Apply gap if not first child
      if (child != _children[0]) {
        _bounds.x += _gap.x;
      }
    }

    // Don't go smaller than current width
    if (_bounds.x < getDimensions().x) {
      _bounds.x = getDimensions().x;
    }
    if (_bounds.y < getDimensions().y) {
      _bounds.y = getDimensions().y;
    }

    // Consider padding
    _bounds.x += _padding.w + _padding.y;
    _bounds.y += _padding.x + _padding.z;

    // Set node size
    _getActiveNode()->setDimensions(_bounds);
  }

  System::loadTexture(_getActiveNode()->getTexture());

  // Draw this node relative to parent
  _getActiveNode()->drawMesh(offset);
  for (auto &child : _children) {
    child->draw(offset + getPosition());
  }

}
