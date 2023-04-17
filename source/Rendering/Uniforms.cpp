/**
 * @file Uniforms.cpp
 * @brief A set of shader uniforms.
 * @date April 15, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Rendering/Uniforms.h>

NS_CITY_BUILDER_BEGIN
namespace Uniforms {
  
  bgfx::UniformHandle u_ambient;
  bgfx::UniformHandle u_sunColor;
  bgfx::UniformHandle u_sunDirection;
  bgfx::UniformHandle u_textureTile;
  
  bgfx::UniformHandle s_albedo;
  bgfx::UniformHandle s_ui;
  
} // namespace Uniforms
NS_CITY_BUILDER_END

USING_NS_CITY_BUILDER

void Uniforms::create() {
  u_ambient      = bgfx::createUniform("u_ambient"     , bgfx::UniformType::Vec4);
  u_sunColor     = bgfx::createUniform("u_sunColor"    , bgfx::UniformType::Vec4);
  u_sunDirection = bgfx::createUniform("u_sunDirection", bgfx::UniformType::Vec4);
  u_textureTile  = bgfx::createUniform("u_textureTile" , bgfx::UniformType::Vec4);
  
  s_albedo = bgfx::createUniform("s_albedo", bgfx::UniformType::Sampler);
  s_ui     = bgfx::createUniform("s_ui"    , bgfx::UniformType::Sampler);
}
