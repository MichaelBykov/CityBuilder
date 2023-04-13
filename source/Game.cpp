/**
 * @file Game.cpp
 * @brief The central game-controlling code.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Game.h>
#include <CityBuilder/Building/Road.h>
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
  _scene->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
  
  Ogre::SceneNode* lightNode = _scene->getRootSceneNode()->createChildSceneNode();
  lightNode->setDirection(-0.1, -1, -0.1);
  lightNode->attachObject(_sun);
  
  // Everything else
  _mainCamera = new Camera(&_ctx, _scene);
  
  // Create the road
  new Road(&RoadDef::roads["2-Lane Highway"], *new Line2({ 0, 0 }, { 10, 10 }), _scene);
  new Road(&RoadDef::roads["2-Lane Highway"], *new Arc2({ 10, 10 }, { 15, 15 }, { 10, 20 }), _scene);
  
  // Add the ground plane
  Ogre::MaterialPtr planeMaterial = Ogre::MaterialManager::getSingleton().create(
    "Ground Plane Material", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  
  Ogre::TextureUnitState* planeTexture =
    planeMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("grass-tmp.jpg");
  planeTexture->setTextureUScale(0.01);
  planeTexture->setTextureVScale(0.01);
  
  Ogre::MovablePlane *planeDef = new Ogre::MovablePlane("Ground Plane");
  planeDef->d = 0;
  planeDef->normal = Ogre::Vector3::UNIT_Y;
  
  Ogre::MeshManager::getSingleton().createPlane(
    "Ground Plane Mesh",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    *planeDef,
    120, 120, 1, 1,
    true,
    1, 1, 1,
    Ogre::Vector3::UNIT_Z);
  Ogre::Entity* plane = _scene->createEntity("Ground Plane Mesh");
  plane->setMaterial(planeMaterial);
  _scene->getRootSceneNode()->createChildSceneNode()->attachObject(plane);
  
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
