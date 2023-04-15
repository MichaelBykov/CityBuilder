/**
 * @file Events.cpp
 * @brief The main event loop event handler.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Events.h>
#include <CityBuilder/Game.h>
USING_NS_CITY_BUILDER

namespace {
  /// The main game instance.
  Game *game;
  
  /// The current global frame number.
  uint64_t frame = 0;
  
  
  
  // Global shader uniforms
  
  /// The global ambient light color.
  bgfx::UniformHandle u_ambient;
  
  /// The global sun color.
  bgfx::UniformHandle u_sunColor;
  
  /// The global sun direction.
  bgfx::UniformHandle u_sunDirection;
}

void Events::start() {
  game = new Game();
  
  
  
  // Upload the shader uniforms to the GPU
  u_ambient      = bgfx::createUniform("u_ambient"     , bgfx::UniformType::Vec4);
  u_sunColor     = bgfx::createUniform("u_sunColor"    , bgfx::UniformType::Vec4);
  u_sunDirection = bgfx::createUniform("u_sunDirection", bgfx::UniformType::Vec4);
  
  {
    // Normalize the colors before uploading them
    Real4 ambient = {
      Real(game->sun().ambient.x) / Real(255),
      Real(game->sun().ambient.y) / Real(255),
      Real(game->sun().ambient.z) / Real(255),
      1, // Unused
    };
    bgfx::setUniform(u_ambient, ambient);
    
    Real4 sun = {
      Real(game->sun().color.x) / Real(255),
      Real(game->sun().color.y) / Real(255),
      Real(game->sun().color.z) / Real(255),
      1, // Unused
    };
    bgfx::setUniform(u_sunColor, sun);
    
    bgfx::setUniform(u_sunDirection, game->sun().direction);
  }
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
  
  // Debug info
  bgfx::dbgTextClear();
  bgfx::dbgTextPrintf(4, 2, 0x0f,
    "frame %d : %d ns (%d FPS)",
    frame++,
    (int)bgfx::getStats()->cpuTimeFrame,
    (int)(1000000ll / bgfx::getStats()->cpuTimeFrame)
  );
  bgfx::dbgTextPrintf(4, 3, 0x0f,
    "%dx%d",
    (int)game->mainCamera().camera().rect.z,
    (int)game->mainCamera().camera().rect.w
  );
  bgfx::setDebug(BGFX_DEBUG_TEXT);
}

void Events::resize(Real4 rect) {
  game->mainCamera().setViewport(rect);
}

void Events::inputStart(Input &input) {
  
}

void Events::inputChange(Input &input) {
  
}

void Events::inputStop(Input &input) {
  
}
