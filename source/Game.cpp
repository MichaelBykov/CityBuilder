/**
 * @file Game.cpp
 * @brief The central game-controlling code.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Game.h>
USING_NS_CITY_BUILDER

Game *Game::_instance = nullptr;

Game::Game() : _ctx("City Builder") {
  // Setup Ogre3D
  _ctx.initApp();
  
  _root = _ctx.getRoot();
  _scene = _root->createSceneManager();
  
  auto shaders = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  shaders->addSceneManager(_scene);
  
  // Create the sun
  _sun = _scene->createLight("Sun");
  _sun->setType(Ogre::Light::LT_DIRECTIONAL);
  
  Ogre::SceneNode* lightNode = _scene->getRootSceneNode()->createChildSceneNode();
  lightNode->setDirection(0, -1, 0);
  lightNode->attachObject(_sun);
  
  // Everything else
  _mainCamera = new Camera(&_ctx, _scene);
  
  Ogre::Entity* ent = _scene->createEntity("TestLevel_b0.mesh");
  Ogre::SceneNode* node = _scene->getRootSceneNode()->createChildSceneNode();
  node->attachObject(ent);
  
  // Handle input
  _ctx.addInputListener(&_input);
  Input::setMoveKeys(KeyCode::w, KeyCode::s, KeyCode::a, KeyCode::d);
  Input::setOrbitKeys(KeyCode::up, KeyCode::down, KeyCode::left, KeyCode::right);
  Input::listenForAxes();
  
  // Set as the main game instance
  _instance = this;
}

void Game::run() {
  _root->startRendering();
}

void Game::close() {
  _root->queueEndRendering();
  _ctx.closeApp();
}

void Game::update(Real elapsed) {
  // Move the camera
  {
    Real2 move = Input::getMoveAxes();
    Real2 rotate = Input::getOrbitAxes();
    
    _mainCamera->orbit(
      move * elapsed *
      Input::keyboardMoveSpeed() * sqrtf(_mainCamera->distance()),
      rotate.x * elapsed * Input::keyboardOrbitSpeed().x,
      rotate.y * elapsed * Input::keyboardOrbitSpeed().y
    );
  }
  
  // TODO: Finish
}
