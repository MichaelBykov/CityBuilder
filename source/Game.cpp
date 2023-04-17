/**
 * @file Game.cpp
 * @brief The central game-controlling code.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Game.h>
#include <CityBuilder/Rendering/Object.h>
#include <CityBuilder/Roads/Road.h>
USING_NS_CITY_BUILDER

Game *Game::_instance = nullptr;

Game::Game() {
  // Setup the scene
  _sun = DistanceLight({ -0.2, -1, -0.2 }, 1, { 255, 255, 200 }, { 150 });
  _mainCamera = OrbitalCamera();
  
  // Create some initial roads
  RoadDef *highway = &RoadDef::roads["2-Lane Highway"];
  Road *road1 = _roads.add(new Road(highway, *new Line2({ 0, 0 }, { 10, 10 })));
  Road *road2 = _roads.add(new Road(highway, *new Arc2({ 10, 10 }, { 15, 15 }, { 10, 20 })));
  _roads.connect(road1, road2);
  _roads.add(new Road(&RoadDef::roads["Single-Lane Road"], *new Line2({ 0, -5 }, { 10, -5 })));
  _roads.update();
  
  // Add the ground plane
  {
    Resource<Mesh> mesh = new Mesh();
    mesh->add({ // Vertices
      { {  1000, 0,  1000 }, { 0, 1, 0 }, { 0, 0 } },
      { { -1000, 0,  1000 }, { 0, 1, 0 }, { 0, 1 } },
      { { -1000, 0, -1000 }, { 0, 1, 0 }, { 1, 1 } },
      { {  1000, 0, -1000 }, { 0, 1, 0 }, { 1, 0 } },
    }, { // Triangles
      0, 1, 2,
      2, 3, 0,
    }).load();
    
    Resource<Program> shader = new Program("vertex", "fragment");
    
    Resource<Material> material = new Material(shader);
    material->texture = new Texture("grass", 256);
    material->textureTile = { 200, 200 };
    
    _ground = new Object(mesh, material);
  }
  
  // Handle input
  Input::setMoveKeys(KeyCode::w, KeyCode::s, KeyCode::a, KeyCode::d);
  Input::setOrbitKeys(KeyCode::up, KeyCode::down, KeyCode::left, KeyCode::right);
  Input::listenForAxes();
  
  // Set as the main game instance
  _instance = this;
}

void Game::run() {
  // _root->startRendering();
}

void Game::close() {
  // _root->queueEndRendering();
  // _ctx.closeApp();
}

void Game::update(Real elapsed) {
  // Move the camera
  // {
  //   Real2 move = Input::getMoveAxes();
  //   Real2 rotate = Input::getOrbitAxes();
    
  //   _mainCamera->orbit(
  //     move * elapsed *
  //     Input::keyboardMoveSpeed() * sqrtf(_mainCamera->distance()),
  //     rotate.x * elapsed * Input::keyboardOrbitSpeed().x,
  //     rotate.y * elapsed * Input::keyboardOrbitSpeed().y
  //   );
  // }
  
  // TODO: Finish
}
