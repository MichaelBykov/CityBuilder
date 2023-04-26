/**
 * @file Events.h
 * @brief The interface for all the main game events.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/UI/System.h>
#include <CityBuilder/UI/Icon.h>
USING_NS_CITY_BUILDER
using namespace UI;

namespace {
  Map<String, Resource<Texture>&> _textures = Map<String, Resource<Texture>&>::buckets<128>();
  Ref<Node &> root;
  Ref<Icon &> bulldozer;
}

void System::drawRootNode(Ref<Node &> root) {
  root->_draw();
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
  root = new Node(1280, 720, 60, 60, 12, { 64, 64, 64, 255 });
  bulldozer = new Icon("Bulldozer", 64, 64, 60 + 250, 60 + 250, 0, { 0, 0, 255, 255 });
}

void System::draw(const Real2& screen) {
  // Draw the UI
  {
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
    
    // Draw all the UI elements
    System::loadTexture("Round");
    root->draw();
    System::loadTexture("Bulldozer");
    bulldozer->draw();
  }
}
