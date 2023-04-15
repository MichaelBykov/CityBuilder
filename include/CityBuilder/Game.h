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
  
  /// The current game instance.
  inline static Game &instance() {
    return *_instance;
  }
  
  /// Begin or resume running the game.
  void run();
  
  /// Close (shutdown) the game.
  void close();
  
  /// Called when a new frame has started.
  /// \param[in] elapsed
  ///   The total time elapsed since the last frame.
  void update(Real elapsed);
  
private:
  /// The scene's sun.
  DistanceLight _sun;
  
  /// The main camera.
  OrbitalCamera _mainCamera;
  
  /// The ground plane.
  Object *_ground;
  
  /// The main game instance.
  static Game *_instance;
};

NS_CITY_BUILDER_END
