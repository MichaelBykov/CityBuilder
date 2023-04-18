/**
 * @file Events.cpp
 * @brief The main event loop event handler.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Events.h>
#include <CityBuilder/Game.h>
#include <CityBuilder/Rendering/Object.h>
#include <CityBuilder/Rendering/Uniforms.h>
USING_NS_CITY_BUILDER

namespace {
  /// The main game instance.
  Game *game;
  
  /// The current global frame number.
  uint64_t frame = 0;
}

void Events::start() {
  game = new Game();
  
  // Load the default shaders
  Program::pbr   = new Program(      "vertex",       "fragment");
  Program::hover = new Program("hover.vertex", "hover.fragment");
  Program::ui    = new Program(   "ui.vertex",    "ui.fragment");
  
  // Create the shader uniforms
  Uniforms::create();
  
  // Start building roads for testing
  game->buildRoad(&RoadDef::roads["2-Lane Highway"]);
}

void Events::stop() {
  
}

void Events::pause() {
  // Overlay a paused text over the scene
  bgfx::dbgTextClear();
  bgfx::dbgTextPrintf(4, 6, 0x0f, "PAUSED");
  bgfx::setDebug(BGFX_DEBUG_TEXT);
}

void Events::update() {
  Real dt = bgfx::getStats()->cpuTimeFrame / 1000000.0;
  
  // Update the camera
  {
    Real2 move = NS_CITY_BUILDER Input::getMoveAxes() * NS_CITY_BUILDER Input::keyboardMoveSpeed() * Real2(dt * game->mainCamera().distance().sqrt());
    Real2 rotate = NS_CITY_BUILDER Input::getOrbitAxes() * NS_CITY_BUILDER Input::keyboardOrbitSpeed() * Real2(dt);
    game->mainCamera().orbit(move, rotate.x, rotate.y);
  }
  
  // Update the sun
  {
    // Normalize the colors before uploading them
    Real4 ambient = {
      Real(game->sun().ambient.x) / Real(255),
      Real(game->sun().ambient.y) / Real(255),
      Real(game->sun().ambient.z) / Real(255),
      1, // Unused
    };
    bgfx::setUniform(Uniforms::u_ambient, ambient);
    
    Real4 sun = {
      Real(game->sun().color.x) / Real(255),
      Real(game->sun().color.y) / Real(255),
      Real(game->sun().color.z) / Real(255),
      1, // Unused
    };
    bgfx::setUniform(Uniforms::u_sunColor, sun - ambient * Real4(0.8));
    
    bgfx::setUniform(Uniforms::u_sunDirection, game->sun().direction);
  }
  
  // Update the scene
  bgfx::dbgTextClear();
  game->update(dt);
  
  // Setup
  // bgfx::dbgTextClear();
  bgfx::setState(BGFX_STATE_DEFAULT);
  Real2 screen;
  {
    Real4 viewport = game->mainCamera().camera().rect;
    screen.x = viewport.z - viewport.x;
    screen.y = viewport.w - viewport.y;
  }
  
  // Draw the scene
  game->draw();
  
  // Draw any hover components
  bgfx::setState(
    BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
    BGFX_STATE_MSAA |
    BGFX_STATE_BLEND_FUNC(
      BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
  );
  game->drawHovers();
  
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
      BGFX_STATE_BLEND_FUNC(
        BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
    );
    
    // Draw all the UI components
  }
  
  // Debug info
  {
    bgfx::dbgTextPrintf(4, 2, 0x0f,
      "frame %d : %d ns (%d FPS)",
      frame++,
      (int)bgfx::getStats()->cpuTimeFrame,
      (int)(1000000ll / bgfx::getStats()->cpuTimeFrame)
    );
    bgfx::dbgTextPrintf(4, 3, 0x0f,
      "%dx%d",
      (int)screen.x,
      (int)screen.y
    );
    bgfx::setDebug(BGFX_DEBUG_TEXT);
  }
}

void Events::resize(Real4 rect) {
  game->mainCamera().setViewport(rect);
}
