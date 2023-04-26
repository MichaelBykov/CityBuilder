/**
 * @file Game.cpp
 * @brief The central game-controlling code.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Game.h>
USING_NS_CITY_BUILDER

Game *Game::_instance = nullptr;

Game::Game() {
  // Setup the scene
  _sun = DistanceLight({ -0.2, -1, -0.2 }, 1, { 255, 255, 200 }, { 150 });
  _mainCamera = OrbitalCamera();
  
  // Create some initial roads
  RoadDef *highway = &RoadDef::roads["2-Lane Highway"];
  Road *road1 = _roads.add(new Road(highway, new Line2({ 0, 0 }, { 10, 10 })));
  Road *road2 = _roads.add(new Road(highway, new Bezier2({ 10, 10 }, { 20, 20 }, { 10, 30 })));
  _roads.connect(road1, road2);
  _roads.add(new Road(&RoadDef::roads["Single-Lane Road"], new Line2({ 0, -10 }, { 10, -10 })));
  _roads.update();
  
  // Add the ground plane
  {
    Resource<Mesh> mesh = new Mesh();
    mesh->add({ // Vertices
      { {  200, 0,  200 }, { 0, 1, 0 }, { 0, 0 } },
      { { -200, 0,  200 }, { 0, 1, 0 }, { 0, 1 } },
      { { -200, 0, -200 }, { 0, 1, 0 }, { 1, 1 } },
      { {  200, 0, -200 }, { 0, 1, 0 }, { 1, 0 } },
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
  Input::onQuickAction += [this](int action) {
    switch (action) {
    case 1:
      // Build a single-lane road
      buildRoad(&RoadDef::roads["Single-Lane Road"]);
      break;
    
    case 2:
      // Build a highway road
      buildRoad(&RoadDef::roads["2-Lane Highway"]);
      break;
    
    case 3:
      // Zone residential
      break;
    }
  };
  
  // Set as the main game instance
  _instance = this;
}

Ray3 Game::rayFromMouse() {
  // Get the mouse
  Real2 mouse = Input::mousePosition();
  
  // Get the mouse as a normalized device coordinate
  Real2 screen;
  {
    Real4 viewport = _mainCamera.camera().rect;
    screen.x = viewport.z - viewport.x;
    screen.y = viewport.w - viewport.y;
  }
  Real2 mouseNDC = {
    (mouse.x / screen.x) * Real(2) - Real(1),
    Real(1) - (mouse.y / screen.y) * Real(2)
  };
  
  // Homogenize the coordinates
  Real4 mouseHomogenous = { mouseNDC.x, mouseNDC.y, 0, 1 };
  Real4   outHomogenous = { mouseNDC.x, mouseNDC.y, 1, 1 };
  
  // Convert from clip space to world space
  Real4x4 inverse = _mainCamera.camera().projectionMatrix.inverse();
  Real4 mouseWorld = inverse * mouseHomogenous;
  Real4   outWorld = inverse *   outHomogenous;
  
  inverse = _mainCamera.camera().viewMatrix().inverse();
  mouseWorld = inverse * mouseWorld;
    outWorld = inverse *   outWorld;
  
  // Normalize the coordinates
  mouseWorld /= mouseWorld.w;
    outWorld /=   outWorld.w;
  
  // Create the ray
  outWorld -= mouseWorld;
  return Ray3(
    { mouseWorld.x, mouseWorld.y, mouseWorld.z },
    {   outWorld.x,   outWorld.y,   outWorld.z }
  );
}



// The game actions
#include "Game/RoadBuilding.ipp"



void Game::update(Real elapsed) {
  // Perform the action item
  switch (_action) {
  case Action::road_building: {
    Road_Building *state = (Road_Building *)_actionState;
    
    // Display
    bgfx::dbgTextPrintf(4, 6, 0x0f, "Building Road: %s",
      (const char *)state->road->name);
    
    // Project the mouse cursor into the world
    Optional<Real3> intersection = rayFromMouse().xzIntersection(0);
    if (intersection) {
      // Determine where to snap to
      Real3 snap = _roads.snap(*intersection);
      
      // Update the road display
      state->move(snap);
      state->displayVisible = true;
    } else {
      // Hide the road display
      state->displayVisible = false;
    }
  } break;
  
  default:
    // Nothing to do
    break;
  }
}

void Game::draw() {
  _ground->draw();
  
  _roads.draw();
  
  // Check what needs to be drawn for the current action
  switch (_action) {
  case Action::road_building: {
    // Draw the road being built
    Road_Building *state = (Road_Building *)_actionState;
  } break;
  
  default:
    // Nothing to draw
    break;
  }
}

void Game::drawHovers() {
  // Check what needs to be drawn for the current action
  switch (_action) {
  case Action::road_building: {
    // Draw the road being built
    Road_Building *state = (Road_Building *)_actionState;
    
    if (state->displayVisible)
      state->display->draw(Program::hover);
  } break;
  
  default:
    // Nothing to draw
    break;
  }
}

void Game::buildRoad(RoadDef *road) {
  _act(Action::road_building);
  
  // Create the road building state
  _actionState = new Road_Building(road);
}

void Game::cancel() {
  _act(Action::none);
}

void Game::_act(Action action) {
  // Perform clean-up for the previous action, as necessary
  switch (_action) {
  case Action::road_building: {
    // Destroy the building state
    Road_Building *state = (Road_Building *)_actionState;
    delete state;
    _actionState = nullptr;
  } break;
  
  default:
    // Nothing to clean-up
    break;
  }
  
  _action = action;
}
