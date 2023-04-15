/**
 * @file Program.h
 * @brief An interface for loading and handling shader programs.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/String.h>

NS_CITY_BUILDER_BEGIN

/// A shader program.
struct Program {
  /// Load a shader program.
  /// \param[in] vertex
  ///   The resource name of the vertex shader to load.
  /// \param[in] fragment
  ///   The resource name of the fragment shader to load.
  Program(const String &vertex, const String &fragment);
  
  // Prevent program transfer.
  Program(const Program &other) = delete;
  
  ~Program();
  
  /// Submit the program to the GPU.
  inline void submit() {
    bgfx::submit(0, _program);
  }
  
private:
  /// The loaded program handle.
  bgfx::ProgramHandle _program;
  
  // The vertex shader program.
  bgfx::ShaderHandle _vertex;
  
  // The fragment shader program.
  bgfx::ShaderHandle _fragment;
};

NS_CITY_BUILDER_END
