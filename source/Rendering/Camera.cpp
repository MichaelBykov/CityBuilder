/**
 * @file Camera.cpp
 * @brief The implementation of a camera.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Rendering/Camera.h>
#include <bgfx/bgfx.h>
#define BX_CONFIG_DEBUG 0
#include <bx/math.h>
USING_NS_CITY_BUILDER

void Camera::setViewProjection() {
  Real3 forward = viewDirection.normalized();
  Real3 right   = Real3(0, 1, 0).cross(forward).normalized();
  Real3 up      = forward.cross(right); 
  
  // Transpose the view matrix because bgfx is row major
  Real4x4 viewMatrix = Real4x4(
    Real4(right   , -position.dot(right  )),
    Real4(up      , -position.dot(up     )),
    Real4(forward , -position.dot(forward)),
    { 0, 0, 0, 1 }
  ).transpose();
  
  bgfx::setViewTransform(viewID, viewMatrix, projectionMatrix);
}



Real4x4 Camera::perspectiveProjection(
  Real fov, Real near, Real far, Real4 rect
) {
  Real4x4 projection;
  bx::mtxProj(
    projection,
    fov, (rect.z - rect.x) / (rect.w - rect.y), near, far,
    bgfx::getCaps()->homogeneousDepth
  );
  return projection;
}

Real4x4 Camera::orthographicProjection(
  Real near, Real far, Real4 rect
) {
  Real4x4 projection;
  bx::mtxOrtho(
    projection,
    rect.x, rect.z, rect.w, rect.y, near, far, 0,
    bgfx::getCaps()->homogeneousDepth
  );
  return projection;
}
