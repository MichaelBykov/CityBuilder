/**
 * @file Game.h
 * @brief The main controller for the game.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include "Camera.h"
#include "Input.h"

NS_CITY_BUILDER_BEGIN

/// The main game controller.
struct Game : private OgreBites::InputListener {
  /// Start a new game.
  Game();
  
  /// The current main camera.
  inline Camera &mainCamera() {
    return *_mainCamera;
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
  // Ogre3D
  OgreBites::ApplicationContext _ctx;
  Ogre::Root *_root;
  Ogre::SceneManager *_scene;
  
  /// The scene's sun.
  Ogre::Light *_sun;
  
  /// The main camera.
  Camera *_mainCamera;
  
  /// The central input delegate.
  InputDelegate _input;
  
  /// The main game instance.
  static Game *_instance;
};

NS_CITY_BUILDER_END
