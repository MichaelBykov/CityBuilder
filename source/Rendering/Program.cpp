/**
 * @file Program.cpp
 * @brief Loaders for shader programs.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Rendering/Program.h>
#include <CityBuilder/../../driver/Driver.h>
USING_NS_CITY_BUILDER

Resource<Program> Program::pbr = nullptr;

bgfx::ShaderHandle loadShader(const char *name, const char *extension) {
  char *contents;
  size_t length;
  Driver::loadResource(name, extension, &contents, &length);
  
  const bgfx::Memory *memory = bgfx::copy(contents, length + 1);
  free(contents);
  
  return bgfx::createShader(memory);
}

Program::Program(const String &vertex, const String &fragment) {
  // Load the shaders
  _vertex = loadShader((const char *)vertex, "shader");
  _fragment = loadShader((const char *)fragment, "shader");
  
  // Create the program
  _program = bgfx::createProgram(_vertex, _fragment, true);
}

Program::~Program() {
  // Destroy the program and shaders
  bgfx::destroy(_program);
  bgfx::destroy(_vertex);
  bgfx::destroy(_fragment);
}
