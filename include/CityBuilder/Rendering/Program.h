/**
 * @file Program.h
 * @brief An interface for loading and handling shader programs.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/String.h>
#include "Resource.h"

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
  
  /// Submit the program to the GPU.
  /// \param[in] view
  ///   The view to submit the program to.
  inline void submit(bgfx::ViewId view) {
    bgfx::submit(view, _program);
  }
  
  /// The standard PBR shader.
  static Resource<Program> pbr;
  
  /// The standard UI shader.
  static Resource<Program> ui;
  
private:
  /// The loaded program handle.
  bgfx::ProgramHandle _program;
  
  // The vertex shader program.
  bgfx::ShaderHandle _vertex;
  
  // The fragment shader program.
  bgfx::ShaderHandle _fragment;
};

NS_CITY_BUILDER_END
