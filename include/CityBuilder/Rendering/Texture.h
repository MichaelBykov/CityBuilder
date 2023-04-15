/**
 * @file Texture.h
 * @brief A texture resource.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/String.h>

NS_CITY_BUILDER_BEGIN

/// A texture resource.
struct Texture {
  /// Load a texture from a file.
  /// \param[in] name
  ///   The resource name of the texture to load.
  Texture(const String &name);
  
  // Prevent texture transfer.
  Texture(const Texture &other) = delete;
  
  ~Texture();
  
  
  
  /// Submit the texture to the GPU.
  /// \param[in] uniform
  ///   The uniform to write the texture to.
  inline void load(bgfx::UniformHandle uniform) {
    bgfx::setTexture(0, uniform, _handle);
  }
  
private:
  /// The internal texture handle.
  bgfx::TextureHandle _handle;
};

NS_CITY_BUILDER_END
