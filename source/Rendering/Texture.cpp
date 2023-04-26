/**
 * @file Texture.cpp
 * @brief Implement texture loading.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Rendering/Texture.h>
#include <CityBuilder/../../driver/Driver.h>
USING_NS_CITY_BUILDER

Texture::Texture(const String &name, int size, bool mipMaps) {
  // Load the texture
  char *contents;
  size_t length;
  Driver::loadResource((const char *)name, "texture", &contents, &length);
  
  // Ignore the DDS header
  contents += 128;
  
  // Create the texture
  _handle = bgfx::createTexture2D(
    size, size, mipMaps, 1, bgfx::TextureFormat::RGBA8,
    BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
    bgfx::copy(contents, length - 128)
  );
  
  // Free the texture data
  free(contents - 128);
}

Texture::Texture(const String &name, uint64_t flags, int size, bool mipMaps) {
  // Load the texture
  char *contents;
  size_t length;
  Driver::loadResource((const char *)name, "texture", &contents, &length);
  
  // Ignore the DDS header
  contents += 128;
  
  // Create the texture
  _handle = bgfx::createTexture2D(
    size, size, mipMaps, 1, bgfx::TextureFormat::RGBA8,
    flags,
    bgfx::copy(contents, length - 128)
  );
  
  // Free the texture data
  free(contents - 128);
}

Texture::~Texture() {
  // Destroy the texture
  bgfx::destroy(_handle);
}
