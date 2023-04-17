/**
 * @file DistanceLight.h
 * @brief
 *   A light that exists at some point in the far distance,
 *   most commonly the sun.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>

NS_CITY_BUILDER_BEGIN

/// A light at some point in the far distance.
struct DistanceLight {
  /// The direction of the light.
  Real3 direction;
  
  /// The intensity of the light.
  Real intensity;
  
  /// The color that the light emits.
  Color3 color;
  
  /// The ambient color of the light.
  Color3 ambient;
  
  
  
  /// Create an empty directional light
  DistanceLight()
    : direction { 0, 0, -1 }, intensity(1), color(255), ambient(85) { }
  
  /// Create a directional light.
  /// \param direction
  ///   The direction that the light shines in.
  /// \param intensity
  ///   The intensity of the light.
  /// \param color
  ///   The color that the light emits.
  /// \param ambient
  ///   The ambient color of the light.
  DistanceLight(Real3 direction, Real intensity, Color3 color, Color3 ambient)
    : direction(direction), intensity(intensity), color(color),
      ambient(ambient) { }
};

NS_CITY_BUILDER_END
