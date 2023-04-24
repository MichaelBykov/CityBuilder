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
#include <CityBuilder/Rendering/UIMesh.h>
#include <CityBuilder/UI/Node.h>
#include <CityBuilder/UI/System.h>
#include <CityBuilder/Storage/Ref.h>
USING_NS_CITY_BUILDER

namespace {
  /// The main game instance.
  Game *game;
  
  /// The current global frame number.
  uint64_t frame = 0;
  
  
  // For UI testing
  Resource<Texture> uiTexture;
  Resource<UIMesh> uiMesh;
  Ref<UI::Node &> node;
}

void Events::start() {
  game = new Game();
  
  // Load the default shader
  Program::pbr = new Program("vertex", "fragment");
  
  Program::ui = new Program("ui.vertex", "ui.fragment");
  
  // Create the shader uniforms
  Uniforms::create();
  
  
  
  // UI testing
  uiTexture = new Texture("ui/round", 128, false);
  node = new UI::Node(1000, 600, 80, 80);

  // uiMesh->add({ // Vertices
  //   { Real3(30 +   0, 120 +  0, 1), Real2(0, 0) },
  //   { Real3(30 +  30, 120 +  0, 1), Real2(0, 0.33) },
  //   { Real3(30 + 100, 120 +  0, 1), Real2(0, 0.67) },
  //   { Real3(30 + 130, 120 +  0, 1), Real2(0, 1) },
    
  //   { Real3(30 +   0, 120 + 30, 1), Real2(0.33, 0) },
  //   { Real3(30 +  30, 120 + 30, 1), Real2(0.33, 0.33) },
  //   { Real3(30 + 100, 120 + 30, 1), Real2(0.33, 0.67) },
  //   { Real3(30 + 130, 120 + 30, 1), Real2(0.33, 1) },
    
  //   { Real3(30 +   0, 120 + 60, 1), Real2(0.67, 0) },
  //   { Real3(30 +  30, 120 + 60, 1), Real2(0.67, 0.33) },
  //   { Real3(30 + 100, 120 + 60, 1), Real2(0.67, 0.67) },
  //   { Real3(30 + 130, 120 + 60, 1), Real2(0.67, 1) },
    
  //   { Real3(30 +   0, 120 + 90, 1), Real2(1, 0) },
  //   { Real3(30 +  30, 120 + 90, 1), Real2(1, 0.33) },
  //   { Real3(30 + 100, 120 + 90, 1), Real2(1, 0.67) },
  //   { Real3(30 + 130, 120 + 90, 1), Real2(1, 1) },
  // }, { // Triangles
    
  //   4, 0, 1, 4, 1, 5,
  //   5, 1, 2, 5, 2, 6,
  //   6, 2, 3, 6, 3, 7,
    
  //   8, 4, 5, 8, 5, 9,
  //   9, 6, 5, 9, 6, 10,
  //   10, 6, 7, 10, 7, 11,
    
  //   12, 8, 9, 12, 9, 13,
  //   13, 9, 10, 13, 10, 14,
  //   14, 10, 11, 14, 11, 15,
  // });

  // uiMesh->load();
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
  
  // Setup
  bgfx::dbgTextClear();
  bgfx::setState(BGFX_STATE_DEFAULT);
  Real2 screen;
  {
    Real4 viewport = game->mainCamera().camera().rect;
    screen.x = viewport.z - viewport.x;
    screen.y = viewport.w - viewport.y;
  }
  
  // Draw the scene
  game->ground().draw();
  
  game->roads().draw();
  
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
    uiTexture->load(1, Uniforms::s_ui);
    UI::System::drawRootNode(node);
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
