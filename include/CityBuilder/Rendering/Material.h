/**
 * @file Material.h
 * @brief A material for rendering meshes.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include "Resource.h"
#include "Program.h"
#include "Texture.h"

NS_CITY_BUILDER_BEGIN

struct Material {
  /// The color of the material.
  Color4 color;
  
  /// How rough the material is.
  /// \remarks
  ///   `0.0` is perfectly smooth, `1.0` is fully rough.
  Real roughness;
  
  /// How metallic the material is.
  /// \remarks
  ///   `0.0` is completely non-metallic, `1.0` is completely metallic.
  Real metallic;
  
  /// The main texture of the material.
  Resource<Texture> texture;
  
  /// The bump map of the material.
  Resource<Texture> bump;
  
  /// The shader program to use for rendering.
  Resource<Program> shader;
  
  
  
  /// Create a new material.
  /// \param[in] shader
  ///   The shader program to use for rendering.
  Material(Resource<Program> shader) : shader(shader) { }
};

NS_CITY_BUILDER_END
