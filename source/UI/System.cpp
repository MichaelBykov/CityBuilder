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
#include <CityBuilder/UI/Primitive/Character.h>
#include <CityBuilder/UI/Element.h>

USING_NS_CITY_BUILDER
using namespace UI;

namespace {
  Map<String, Resource<Texture>&> _textures = Map<String, Resource<Texture>&>::buckets<128>();
  Ref<Rounded &> hotbar_bg;
  Ref<Rectangle &> zone_ico;
  Ref<Rectangle &> road_ico;
  Ref<Rectangle &> dozer_ico;
  Ref<Element &> hotbar;
  Ref<Element &> zone;
  Ref<Element &> road;
  Ref<Element &> dozer;
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
  System::addTexture("Font",      "ui/font",           384);

  // Make some elements
  hotbar_bg = new Rounded();
  hotbar_bg->setColor({ 45, 45, 45, 255 });
  hotbar_bg->setBorderRadius(7);

  zone_ico = new Rectangle();
  zone_ico->setColor({ 179, 179, 179, 255 });
  zone_ico->setTexture("Zone");

  road_ico = new Rectangle();
  road_ico->setColor({ 179, 179, 179, 255 });
  road_ico->setTexture("Road");

  dozer_ico = new Rectangle();
  dozer_ico->setColor({ 179, 179, 179, 255 });
  dozer_ico->setTexture("Bulldozer");

  // hotbar = new Element();
  // hotbar->setColor({ 45, 45, 45, 255 });
  // hotbar->setBorderRadius(7);

  // zone = new Element();
  // zone->setDimensions({ 24, 24 });
  // zone->setBackgroundImage("Zone");
  // hotbar->appendChild(zone);
}

void System::resize(const Real2& screen) {
  Real icoPercent = screen.x * Real(0.025);
  Real2 ico_size = { icoPercent, icoPercent };
  zone_ico->setDimensions(ico_size);
  road_ico->setDimensions(ico_size);
  dozer_ico->setDimensions(ico_size);

  Real2 hotbar_size = {
    (ico_size.x * Real(3)) + Real(7 * 4),
    ico_size.y + Real(7 * 2)
  };
  Real2 hotbar_pos = {
    (screen.x / Real(2)) - (hotbar_size.x / Real(2)),
    screen.y - hotbar_size.y - Real(7)
  };
  hotbar_bg->setDimensions(hotbar_size);
  hotbar_bg->setPosition(hotbar_pos);

  Real2 zone_ico_pos = { hotbar_pos.x + Real(7), hotbar_pos.y + Real(7) };
  zone_ico->setPosition(zone_ico_pos);
  Real2 road_ico_pos = { zone_ico_pos.x + ico_size.x + Real(7), zone_ico_pos.y };
  road_ico->setPosition(road_ico_pos);
  Real2 dozer_ico_pos = { road_ico_pos.x + ico_size.x + Real(7), road_ico_pos.y };
  dozer_ico->setPosition(dozer_ico_pos);

  // hotbar->setDimensions({ 100, 100 });
  // hotbar->setPosition({ 100, 100 });
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
  
  // Draw the UI
  System::loadTexture("Round");
  hotbar_bg->drawMesh({ 0, 0 });

  System::loadTexture("Zone");
  zone_ico->drawMesh({ 0, 0 });

  System::loadTexture("Road");
  road_ico->drawMesh({ 0, 0 });

  System::loadTexture("Bulldozer");
  dozer_ico->drawMesh({ 0, 0 });

  // hotbar->draw();
}
