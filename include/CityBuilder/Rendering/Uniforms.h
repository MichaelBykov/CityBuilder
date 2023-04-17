/**
 * @file Uniforms.h
 * @brief A set of shader uniforms.
 * @date April 15, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>

NS_CITY_BUILDER_BEGIN
namespace Uniforms {

/// The global ambient light color.
extern bgfx::UniformHandle u_ambient;

/// The global sun color.
extern bgfx::UniformHandle u_sunColor;

/// The global sun direction.
extern bgfx::UniformHandle u_sunDirection;

/// The global texture tile.
extern bgfx::UniformHandle u_textureTile;



/// The albedo texture.
extern bgfx::UniformHandle s_albedo;



/// Create the global shader uniforms.
void create();

} // namespace Uniforms
NS_CITY_BUILDER_END
