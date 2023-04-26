/**
 * @file System.h
 * @brief System for rendering UI elements.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/System.h>
#include <CityBuilder/Storage/Map.h>
#include <CityBuilder/UI/Primitive/Rectangle.h>
#include <CityBuilder/UI/Primitive/Rounded.h>

USING_NS_CITY_BUILDER
using namespace UI;

namespace {
  Map<String, Resource<Texture>&> _textures = Map<String, Resource<Texture>&>::buckets<128>();
  Ref<Rounded &> root;
}

void System::addTexture(const String& name, const String& path, int size, bool mipMaps) {
  _textures.set(name, *(new Resource(new Texture(path, size, mipMaps))));
}

Resource<Texture>& System::getTexture(const String& name) {
  return _textures[name];
}

void System::loadTexture(const String& name) {
  _textures[name]->load(1, Uniforms::s_ui);
}

void System::start() {
  // Create the UI shader
  Program::ui = new Program("ui.vertex", "ui.fragment");

  // Declare our textures
  System::addTexture("Round",     "ui/round",          128);
  System::addTexture("Square",    "ui/square",         128);
  System::addTexture("Bulldozer", "ui/bulldozer-icon", 24);
  System::addTexture("Road",      "ui/road-icon",      24);
  System::addTexture("Zone",      "ui/zone-icon",      24);

  // Make some elements
  root = new Rounded();
  root->setDimensions({ 200, 200 });
  root->setPosition({ 20, 20 });
  root->setColor({ 255, 255, 255, 255 });
  root->setBorderRadius(20);
}


void System::drawNode(Ref<Node &> root, Real2 offset) {
  // System::loadTexture(root->getTexture());
  // root->_draw(offset);

  // for (Ref<Node &>* child : root->getChildren()) {
  //   System::drawNode(*child, offset + root->_position);
  // }
}

void System::draw(const Real2& screen) {
  // Setup the UI projection
  Real4x4 projectionMatrix;
  bx::mtxOrtho(
    projectionMatrix,
    0, screen.x, screen.y, 0,
    0.1, 100, 0,
    bgfx::getCaps()->homogeneousDepth
  );
  bgfx::setViewTransform(1, NULL, projectionMatrix);
  bgfx::setViewRect(1, 0, 0, (uint16_t)screen.x, (uint16_t)screen.y);
  
  // Setup the UI view
  bgfx::touch(1);
  bgfx::setState(
    BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
    BGFX_STATE_MSAA |
    BGFX_STATE_DEPTH_TEST_ALWAYS |
    BGFX_STATE_BLEND_FUNC(
      BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
  );
  
  // System::drawNode(root);
  System::loadTexture("Round");
  root->drawMesh({ 0, 0 });
}
