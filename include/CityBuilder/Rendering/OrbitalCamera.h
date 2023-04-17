/**
 * @file OrbitalCamera.h
 * @brief An orbital camera controller.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Units/Angle.h>
#include "Camera.h"

NS_CITY_BUILDER_BEGIN

/// A simple orbital camera.
struct OrbitalCamera {
  /// Create a new camera.
  OrbitalCamera();
  
  /// Create a new camera at the world origin.
  /// \param[in] rect
  ///   The view rectangle of the camera where `x = left`, `y = top`,
  ///   `z = right`, and `w = bottom`.
  /// \param[in] viewID
  ///   The identifier of the view that this camera renders to.
  OrbitalCamera(Real4 rect, int viewID = 0);
  
  /// Slide the camera along the ground plane by the given absolute delta,
  /// relative to its yaw.
  /// \param[in] delta
  ///   The amount to offset the camera in the forward and perpendicular
  ///   axes.
  void slide(Real2 delta);
  
  /// Zoom the camera by the given delta.
  /// \param[in] delta
  ///    The amount to zoom the camera in towards the pivot point (positive) or
  ///    away from the pivot point (negative).
  void zoom(Real delta);
  
  /// Rotate the camera by the given angles.
  /// \param[in] yaw
  ///   The amount to rotate the camera in the yaw axes.
  /// \param[in] pitch
  ///   The amount to rotate the camera in the pitch axes.
  void rotate(Angle yaw, Angle pitch);
  
  /// Orbit the camera.
  /// \param[in] delta
  ///   The amount to offset the camera in the forward and perpendicular
  ///   axes.
  /// \param[in] yaw
  ///   The amount to rotate the camera in the yaw axes.
  /// \param[in] pitch
  ///   The amount to rotate the camera in the pitch axes.
  void orbit(Real2 delta, Angle yaw, Angle pitch);
  
  /// Set the viewport of the camera.
  /// \param[in] rect
  ///   The view rectangle of the camera where `x = left`, `y = top`,
  ///   `z = right`, and `w = bottom`.
  void setViewport(Real4 rect);
  
  /// The distance of the camera from its pivot point.
  inline Real distance() const { return _distance; }
  
  /// The pivot point of the camera in the world.
  inline Real3 pivot() const { return _pivot; }
  
  /// The yaw of the camera.
  inline Angle yaw() const { return _yaw; }
  
  /// The pitch of the camera.
  inline Angle pitch() const { return _pitch; }
  
  /// The underlying camera.
  inline Camera &camera() { return _camera; }
  
private:
  /// The underlying camera.
  Camera _camera;
  
  /// The pivot point of the camera in the world.
  Real3 _pivot;
  
  /// The yaw of the camera, in radians.
  Angle _yaw;
  
  /// The pitch of the camera, in radians.
  Angle _pitch;
  
  /// The distance from the pivot point to the camera, in radians.
  Real _distance;
  
  /// Update the camera orbit taking into account the set parameters.
  void _updateOrbit();
};

NS_CITY_BUILDER_END
