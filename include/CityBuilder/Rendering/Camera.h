/**
 * @file Camera.h
 * @brief A camera controller.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>

NS_CITY_BUILDER_BEGIN

/// A simple camera.
struct Camera {
  /// The projection matrix of the camera.
  /// \remarks
  ///   The projection matrix should be in row major order.
  Real4x4 projectionMatrix;
  
  /// The view rectangle of the camera where `x = left`, `y = top`, `z = right`,
  /// and `w = bottom`.
  Real4 rect;
  
  /// The position of the camera.
  Real3 position;
  
  /// A vector that defines the direction the camera is looking at.
  Real3 viewDirection;
  
  /// The identifier of the view that this camera renders to.
  uint16_t viewID;
  
  
  
  /// Create an empty camera.
  Camera() { }
  
  /// Create a new camera.
  /// \param[in] viewID
  ///   The identifier of the view that this camera renders to.
  /// \param[in] rect
  ///   The view rectangle of the camera where `x = left`, `y = top`,
  ///   `z = right`, and `w = bottom`.
  /// \param[in] position
  ///   The position of the camera in world space.
  /// \param[in] viewDirection
  ///   A vector that defines the direction the camera is looking at.
  Camera(
    uint16_t viewID, Real4 rect, Real3 position = 0, Real3 viewDirection = 0
  ) : rect(rect), viewID(viewID), position(position),
      viewDirection(viewDirection) { }
  
  
  
  /// Set the view projection for the camera's view.
  void setViewProjection();
  
  /// Get the view matrix for the camera.
  Real4x4 viewMatrix() const;
  
  /// Get the view-projection matrix for the camera.
  Real4x4 viewProjection() const;
  
  
  
  /// Create a perspective projection matrix.
  /// \param fov
  ///   The field-of-view of the camera in degrees.
  /// \param near
  ///   The distance to the near plane from the camera's origin.
  /// \param fat
  ///   The distance to the far plane from the camera's origin.
  /// \param rect
  ///   The view rectangle of the camera where `x = left`, `y = top`,
  ///   `z = right`, and `w = bottom`.
  /// \returns
  ///   A perspective projection matrix with applicable properties in row major
  ///   order.
  static Real4x4 perspectiveProjection(
    Real fov, Real near, Real far, Real4 rect
  );
  
  /// Create an orthographic projection matrix.
  /// \param[in] near
  ///   The distance to the near plane from the camera's origin.
  /// \param[in] far
  ///   The distance to the far plane from the camera's origin.
  /// \param[in] rect
  ///   The view rectangle of the camera where `x = left`, `y = top`,
  ///   `z = right`, and `w = bottom`.
  /// \returns
  ///   An orthographic projection matrix with applicable properties in row
  ///   major order.
  static Real4x4 orthographicProjection(
    Real near, Real far, Real4 rect
  );
};

NS_CITY_BUILDER_END
