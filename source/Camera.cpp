/**
 * @file Camera.cpp
 * @brief The implementation of an orbital camera.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Camera.h>
USING_NS_CITY_BUILDER

Camera::Camera(OgreBites::ApplicationContext *ctx, Ogre::SceneManager *scene) {
  // Create the camera
  _camera = scene->createCamera("Main Camera");
  _camera->setNearClipDistance(0.1);
  _camera->setFarClipDistance(1000);
  _camera->setAutoAspectRatio(true);
  ctx->getRenderWindow()->addViewport(_camera);
  
  // Create the camera node
  _node = scene->getRootSceneNode()->createChildSceneNode();
  _node->attachObject(_camera);
  
  // Default properties
  _pivot = Real3::ZERO;
  _yaw = 0;
  _pitch = 45_deg;
  _distance = 10;
  
  // Set the original orbit
  _updateOrbit();
}

void Camera::slide(Real2 delta) {
  Real2 sinCos = Angle::sinCos(_yaw);
  Real3 forward = { -sinCos.x, 0, -sinCos.y };
  Real3 right = forward.crossProduct(Real3::UNIT_Y);
  
  _pivot += forward * delta.y;
  _pivot += right * delta.x;
  
  _updateOrbit();
}

void Camera::zoom(Real delta) {
  _distance += delta;
  
  // Clamp
  if (_distance < 1)
    _distance = 1;
  else if (_distance > 100)
    _distance = 100;
  
  _updateOrbit();
}

void Camera::rotate(Angle yaw, Angle pitch) {
  _yaw += yaw;
  
  // Make sure that the pitch is in the range [10°, 90°]
  _pitch += pitch;
  if (_pitch > 90_deg)
    _pitch = 90_deg;
  else if (_pitch < 10_deg)
    _pitch = 10_deg;
  
  _updateOrbit();
}

void Camera::orbit(Real2 delta, Angle yaw, Angle pitch) {
  // Slide
  Real2 sinCos = Angle::sinCos(_yaw);
  Real3 forward = { -sinCos.x, 0, -sinCos.y };
  Real3 right = forward.crossProduct(Real3::UNIT_Y);
  
  _pivot += forward * delta.y;
  _pivot += right * delta.x;
  
  // Rotate
  _yaw += yaw;
  
  // Make sure that the pitch is in the range [10°, 90°]
  _pitch += pitch;
  if (_pitch > 90_deg)
    _pitch = 90_deg;
  else if (_pitch < 10_deg)
    _pitch = 10_deg;
  
  _updateOrbit();
}

Real Camera::distance() const {
  return _distance;
}

void Camera::_updateOrbit() {
  Real2 forwardSinCos = Angle::sinCos(_yaw);
  Real2   pitchSinCos = Angle::sinCos(_pitch);
  
  // Calculate the offset from the pivot point
  Real3 forward = { forwardSinCos.x, 0, forwardSinCos.y };
  Real3 offset = {
    forwardSinCos.x * pitchSinCos.y, pitchSinCos.x,
    forwardSinCos.y * pitchSinCos.y
  };
  
  _node->setPosition(_pivot + offset * _distance);
  // Set a standard orientation
  _node->setOrientation(
    Ogre::Quaternion(Ogre::Radian(_yaw), Ogre::Vector3::UNIT_Y));
  // Adjust the pitch
  _node->lookAt(_pivot, Ogre::Node::TS_WORLD);
}
