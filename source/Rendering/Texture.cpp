/**
 * @file Texture.cpp
 * @brief Implement texture loading.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Rendering/Texture.h>
#include <CityBuilder/../../driver/Driver.h>
USING_NS_CITY_BUILDER

Texture::Texture(const String &name) {
  // Load the texture
  char *contents;
  size_t length;
  Driver::loadResource((const char *)name, "texture", &contents, &length);
  
  // Create the texture
  // _handle = bgfx::createTexture2D(
  //   1, 1, false, 1, bgfx::TextureFormat::RGBA8,
  //   BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
  //   bgfx::copy(contents, length)
  // );
  
  // Free the texture data
  free(contents);
}

Texture::~Texture() {
  // Destroy the texture
  // bgfx::destroy(_handle);
}
