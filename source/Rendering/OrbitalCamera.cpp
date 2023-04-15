/**
 * @file OrbitalCamera.cpp
 * @brief The implementation of an orbital camera.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Rendering/OrbitalCamera.h>
USING_NS_CITY_BUILDER

OrbitalCamera::OrbitalCamera() {
  // Create the camera
  _camera = Camera(0, { 0.0 }, { 0.0 }, { 0.0 });
  
  // Default properties
  _pivot = { 0.0 };
  _yaw = 0_deg;
  _pitch = 45_deg;
  _distance = 10;
}

OrbitalCamera::OrbitalCamera(Real4 rect, int viewID) {
  // Create the camera
  _camera = Camera(viewID, rect, { 0.0 }, { 0.0 });
  
  _camera.projectionMatrix =
    Camera::perspectiveProjection(60_deg, 0.1, 1000, rect);
  
  // Default properties
  _pivot = { 0.0 };
  _yaw = 0_deg;
  _pitch = 45_deg;
  _distance = 10;
  
  // Set the original orbit
  _updateOrbit();
}

void OrbitalCamera::slide(Real2 delta) {
  Real2 sinCos = Angle::sinCos(_yaw);
  Real3 forward = { -sinCos.x, 0, -sinCos.y };
  Real3 right = forward.cross({ 0, 1, 0 });
  
  _pivot += forward * Real3(delta.y);
  _pivot += right   * Real3(delta.x);
  
  _updateOrbit();
}

void OrbitalCamera::zoom(Real delta) {
  _distance += delta;
  
  // Clamp
  if (_distance < 1)
    _distance = 1;
  else if (_distance > 100)
    _distance = 100;
  
  _updateOrbit();
}

void OrbitalCamera::rotate(Angle yaw, Angle pitch) {
  _yaw += yaw;
  
  // Make sure that the pitch is in the range [10°, 90°]
  _pitch += pitch;
  if (_pitch.radians > 90_deg)
    _pitch = 90_deg;
  else if (_pitch.radians < 10_deg)
    _pitch = 10_deg;
  
  _updateOrbit();
}

void OrbitalCamera::orbit(Real2 delta, Angle yaw, Angle pitch) {
  // Slide
  Real2 sinCos = Angle::sinCos(_yaw);
  Real3 forward = { -sinCos.x, 0, -sinCos.y };
  Real3 right = forward.cross({ 0, 1, 0 });
  
  _pivot += forward * Real3(delta.y);
  _pivot += right   * Real3(delta.x);
  
  // Rotate
  _yaw += yaw;
  
  // Make sure that the pitch is in the range [10°, 90°]
  _pitch += pitch;
  if (_pitch.radians > 90_deg)
    _pitch = 90_deg;
  else if (_pitch.radians < 10_deg)
    _pitch = 10_deg;
  
  _updateOrbit();
}

void OrbitalCamera::setViewport(Real4 rect) {
  _camera.rect = rect;
  _camera.projectionMatrix =
    Camera::perspectiveProjection(60, 0.1, 1000, rect);
  _updateOrbit();
}

Real OrbitalCamera::distance() const {
  return _distance;
}

void OrbitalCamera::_updateOrbit() {
  Real2 forwardSinCos = Angle::sinCos(_yaw);
  Real2   pitchSinCos = Angle::sinCos(_pitch);
  
  // Calculate the offset from the pivot point
  Real3 forward = { forwardSinCos.x * pitchSinCos.y, -pitchSinCos.x, forwardSinCos.y * pitchSinCos.y };
  Real3 offset = {
    forwardSinCos.x * pitchSinCos.y, pitchSinCos.x,
    forwardSinCos.y * pitchSinCos.y
  };
  
  _camera.position = {5, 0, 0};
  _camera.viewDirection = {-5, 0, 0};
  _camera.setViewProjection();
}
