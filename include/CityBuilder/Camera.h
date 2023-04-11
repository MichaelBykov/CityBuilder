/**
 * @file Camera.h
 * @brief A camera controller.
 * @version 1.0
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include "Units/Angle.h"

NS_CITY_BUILDER_BEGIN

/// A simple orbital camera.
struct Camera {
  /// Create a new camera at the world origin.
  Camera(OgreBites::ApplicationContext *ctx, Ogre::SceneManager *scene);
  
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
  
  /// The distance of the camera from its pivot point.
  Real distance() const;
  
private:
  /// The underlying Ogre3D camera.
  Ogre::Camera *_camera;
  
  /// The Ogre3D node that the camera is attached to.
  Ogre::SceneNode *_node;
  
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
