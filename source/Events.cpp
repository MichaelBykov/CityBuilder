/**
 * @file Events.cpp
 * @brief The main event loop event handler.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Events.h>
#include <CityBuilder/Game.h>
#include <CityBuilder/Rendering/Object.h>
USING_NS_CITY_BUILDER

namespace {
  /// The main game instance.
  Game *game;
  
  /// The cube mesh.
  Resource<Mesh> mesh;
  
  /// The cube shader.
  Resource<Material> material;
  
  /// The cube shader.
  Resource<Program> shader;
  
  /// The cube.
  Object *cube;
  
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
  
  mesh = new Mesh();
  mesh->add({ // Vertices
    // +X face
    { {  1,  1,  1 }, { 1,  0,  0 }, {   0,   0, 255 , 255,} },
    { {  1,  1, -1 }, { 1,  0,  0 }, { 255,   0, 255 , 255,} },
    { {  1, -1, -1 }, { 1,  0,  0 }, { 255, 255, 255 , 255,} },
    { {  1, -1,  1 }, { 1,  0,  0 }, {   0, 255, 255 , 255,} },
    
    // -X face
    { { -1, -1,  1 }, { -1,  0,  0 }, {   0, 255,   0,  255 } },
    { { -1, -1, -1 }, { -1,  0,  0 }, { 255, 255,   0,  255 } },
    { { -1,  1, -1 }, { -1,  0,  0 }, { 255,   0,   0,  255 } },
    { { -1,  1,  1 }, { -1,  0,  0 }, {   0,   0,   0,  255 } },
    
    // +Y face
    { {  1,  1,  1 }, {  0,  1,  0 }, {   0,   0, 255,  255 } },
    { { -1,  1,  1 }, {  0,  1,  0 }, {   0,   0,   0,  255 } },
    { { -1,  1, -1 }, {  0,  1,  0 }, { 255,   0,   0,  255 } },
    { {  1,  1, -1 }, {  0,  1,  0 }, { 255,   0, 255,  255 } },
    
    // -Y face
    { {  1, -1,  1 }, {  0, -1,  0 }, {   0, 255, 255,  255 } },
    { {  1, -1, -1 }, {  0, -1,  0 }, { 255, 255, 255,  255 } },
    { { -1, -1, -1 }, {  0, -1,  0 }, { 255, 255,   0,  255 } },
    { { -1, -1,  1 }, {  0, -1,  0 }, {   0, 255,   0,  255 } },
    
    // +Z face
    { {  1,  1,  1 }, {  0,  0,  1 }, {   0,   0, 255,  255 } },
    { {  1, -1,  1 }, {  0,  0,  1 }, {   0, 255, 255,  255 } },
    { { -1, -1,  1 }, {  0,  0,  1 }, {   0, 255,   0,  255 } },
    { { -1,  1,  1 }, {  0,  0,  1 }, {   0,   0,   0,  255 } },
    
    // -Z face
    { {  1,  1, -1 }, {  0,  0, -1 }, { 255,   0, 255,  255 } },
    { { -1,  1, -1 }, {  0,  0, -1 }, { 255,   0,   0,  255 } },
    { { -1, -1, -1 }, {  0,  0, -1 }, { 255, 255,   0,  255 } },
    { {  1, -1, -1 }, {  0,  0, -1 }, { 255, 255, 255,  255 } },
  }, { // Triangles
    // +X face
    0,  1,  2,
    2,  3,  0,
    
    // -X face
    4,  5,  6,
    6,  7,  4,
    
    // +Y face
    8,  9, 10,
    10, 11,  8,
    
    // -Y face
    12, 13, 14,
    14, 15, 12,
    
    // +Z face
    16, 17, 18,
    18, 19, 16,
    
    // -Z face
    20, 21, 22,
    22, 23, 20,
  }).load();
  
  // Load the shader
  shader = new Program("vertex", "fragment");
  
  // Create the material
  material = new Material(shader);
  
  // Create the object
  cube = new Object(mesh, material);
  
  
  
  // Create the shader uniforms
  u_ambient      = bgfx::createUniform("u_ambient"     , bgfx::UniformType::Vec4);
  u_sunColor     = bgfx::createUniform("u_sunColor"    , bgfx::UniformType::Vec4);
  u_sunDirection = bgfx::createUniform("u_sunDirection", bgfx::UniformType::Vec4);
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
  
  cube->draw();
  
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
