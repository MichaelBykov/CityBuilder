/**
 * @file Game.cpp
 * @brief The central game-controlling code.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Game.h>
#include <CityBuilder/Rendering/Object.h>
#include <CityBuilder/Rendering/DynamicMesh.h>
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
  Road *road2 = _roads.add(new Road(highway, *new Arc2({ 10, 10 }, { 20, 20 }, { 10, 30 })));
  _roads.connect(road1, road2);
  _roads.add(new Road(&RoadDef::roads["Single-Lane Road"], *new Line2({ 0, -10 }, { 10, -10 })));
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

// Define the action states
namespace {
  struct Road_Building {
    /// The current point where the road is being built.
    Real3 point;
    
    /// The start point of the road.
    Real3 start;
    
    /// The control point of the road.
    Real3 control;
    
    /// The current build stage.
    /// \remarks
    ///   0: Find a starting point.
    ///   1: Find a control point.
    ///   2: Find an end point.
    int stage = 0;
    
    /// The ID of the listener for mouse clicks.
    int clickListener;
    
    /// The ID of the listener for cancel events.
    int cancelListener;
    
    /// The road being built.
    RoadDef *road;
    
    /// A display of the road being built.
    Resource<DynamicMesh> display;
    
    /// Whether or not the display is visible.
    bool displayVisible = false;
    
    /// Whether or not the road being built is valid.
    bool roadValid = false;
    
    Road_Building(RoadDef *road) : road(road) {
      // Create the display
      display = new DynamicMesh();
      
      // Listen for mouse clicks
      clickListener = Input::onPrimaryMouseDown += [this]() {
        switch (stage) {
        case 0:
          // Set the start point
          if (displayVisible && roadValid) {
            start = point;
            stage = 1;
            
            cancelListener = Input::onCancel += [this]() {
              stage = 0;
              move(point);
              Input::onCancel -= cancelListener;
            };
          }
          break;
        
        case 1:
          // Attempt to build the road
          if (displayVisible && roadValid) {
            Line2 *line = new Line2({ start.x, start.z }, { point.x, point.z });
            if (Game::instance().roads().build(this->road, *line)) {
              Game::instance().roads().update();
              stage = 0;
              start = point;
              stage = 1;
              move(point);
            } else
              delete line;
          }
        
        default:
          break;
        }
      };
    }
    
    ~Road_Building() {
      Input::onPrimaryMouseDown -= clickListener;
    }
    
    /// Move the display to the given point.
    /// \param[in] origin
    ///   The point to move the display to.
    void move(Real3 origin) {
      const Real scale = 0.333333333333;
      
      // Check for validity
      switch (stage) {
      case 0:
        // Check for a valid start point
        roadValid = Game::instance().roads().validate(road, { origin.x, origin.z });
        break;
      
      case 1:
        // Check for a valid line
        if (start.squareDistance(origin) < scale)
          roadValid = false;
        else {
          Line2 line { { start.x, start.z }, { origin.x, origin.z } };
          roadValid = Game::instance().roads().validate(road, line);
        }
        break;
      }
      
      const Color4 hoverColor  = roadValid ? Color4(100, 155, 255, 100) : Color4(255, 120, 100, 100);
      const Color4 hoverColor0 = roadValid ? Color4(100, 155, 255,   0) : Color4(255, 120, 100,   0);
      Real2 radius = Real2(road->dimensions.x * Real(0.5 * scale));
      point = origin;
      
      List<DynamicMesh::Vertex> vertices { };
        List<int> indices { };
      
      // Update the mesh
      switch (stage) {
      case 0: {
        // Selection point
        vertices.append({ origin, hoverColor0 });
        for (int i = 0; i < 32; i++) {
          Real2 xz = Angle::cosSin(i * Angle::pi2 / 32) * radius;
          vertices.append({ origin + Real3(xz.x, 0, xz.y), hoverColor });
        }
        indices.append(0);
        indices.append(32);
        indices.append(1);
        for (int i = 0; i < 31; i++) {
          indices.append(0);
          indices.append(i + 1);
          indices.append(i + 2);
        }
      } break;
      
      case 1: {
        if (start.squareDistance(origin) < scale) {
          // Single selection point
          vertices.append({ start, hoverColor0 });
          for (int i = 0; i < 32; i++) {
            Real2 xz = Angle::cosSin(i * Angle::pi2 / 32) * radius;
            vertices.append({ start + Real3(xz.x, 0, xz.y), hoverColor });
          }
          indices.append(0);
          indices.append(32);
          indices.append(1);
          for (int i = 0; i < 31; i++) {
            indices.append(0);
            indices.append(i + 1);
            indices.append(i + 2);
          }
          break;
        }
        
        // Straight line
        Line2 line { { start.x, start.z }, { origin.x, origin.z } };
        
        // Start end cap
        Real4 pointNormal = line.pointNormals().first();
        Angle angle = -Angle({ pointNormal.z, pointNormal.w });
        vertices.append({ start, hoverColor0 });
        for (int i = 0; i < 17; i++) {
          Real2 xz = Angle::cosSin(angle + i * Angle::pi2 / 32) * radius;
          vertices.append({ start + Real3(xz.x, 0, xz.y), hoverColor });
        }
        for (int i = 0; i < 16; i++) {
          indices.append(0);
          indices.append(i + 1);
          indices.append(i + 2);
        }
        
        // End end cap
        pointNormal = line.pointNormals().last();
        angle = Angle({ pointNormal.z, pointNormal.w });
        vertices.append({ point, hoverColor0 });
        for (int i = 0; i < 17; i++) {
          Real2 xz = Angle::cosSin(angle + i * Angle::pi2 / 32) * radius;
          vertices.append({ point + Real3(xz.x, 0, xz.y), hoverColor });
        }
        for (int i = 0; i < 16; i++) {
          indices.append(18);
          indices.append(i + 19);
          indices.append(i + 20);
        }
        
        // Connect the two lines
        indices.append(0);
        indices.append(1);
        indices.append(35);
        indices.append(35);
        indices.append(18);
        indices.append(0);
        
        indices.append(18);
        indices.append(19);
        indices.append(0);
        indices.append(19);
        indices.append(17);
        indices.append(0);
      } break;
      }
      
      // Load the mesh
      display->add(vertices, indices);
      display->load();
    }
  };
}

void Game::update(Real elapsed) {
  // Perform the action item
  switch (_action) {
  case Action::road_building: {
    Road_Building *state = (Road_Building *)_actionState;
    
    // Project the mouse cursor into the world
    Optional<Real3> intersection = rayFromMouse().xzIntersection(0);
    if (intersection) {
      bgfx::dbgTextPrintf(4, 6, 0x0f,
        "mouse: %f, %f, %f",
        (float)intersection->x,
        (float)intersection->y,
        (float)intersection->z
      );
      
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
