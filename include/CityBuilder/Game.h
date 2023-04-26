/**
 * @file Game.h
 * @brief The main controller for the game.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include "Rendering/OrbitalCamera.h"
#include "Rendering/DistanceLight.h"
#include "Rendering/Object.h"
#include "Roads/RoadNetwork.h"
#include "Geometry/Ray3.h"
#include "Input.h"

NS_CITY_BUILDER_BEGIN

/// The main game controller.
struct Game {
  /// Start a new game.
  Game();
  
  /// The current main camera.
  inline OrbitalCamera &mainCamera() {
    return _mainCamera;
  }
  
  /// The sun.
  inline DistanceLight &sun() {
    return _sun;
  }
  
  /// The ground plane.
  inline Object &ground() {
    return *_ground;
  }
  
  /// The road network.
  inline RoadNetwork &roads() {
    return _roads;
  }
  
  /// The current game instance.
  inline static Game &instance() {
    return *_instance;
  }
  
  /// Create a ray from the camera in the direction of where the mouse is
  /// currently pointing.
  Ray3 rayFromMouse();
  
  /// Called when a new frame has started.
  /// \param[in] elapsed
  ///   The total time elapsed since the last frame.
  void update(Real elapsed);
  
  /// Draw the game scene.
  void draw();
  
  /// Draw any hovers in the game scene.
  void drawHovers();
  
  /// A description of the action the user is currently performing.
  enum class Action {
    /// The user is not performing any actions, simply observing.
    none,
    
    /// The user is currently placing a road.
    road_building,
  };
  
  /// Start building a road.
  void buildRoad(RoadDef *road);
  
  /// Cancel the current action.
  void cancel();
  
private:
  /// The scene's sun.
  DistanceLight _sun;
  
  /// The main camera.
  OrbitalCamera _mainCamera;
  
  /// The ground plane.
  Object *_ground;
  
  /// The road network.
  RoadNetwork _roads;
  
  /// The action the user is currently performing.
  Action _action = Action::none;
  
  /// Change which action the user is currently performing.
  void _act(Action action);
  
  /// The current action state.
  void *_actionState = nullptr;
  
  /// The main game instance.
  static Game *_instance;
};

NS_CITY_BUILDER_END
