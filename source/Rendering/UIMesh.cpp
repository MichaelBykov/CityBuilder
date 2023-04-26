/**
 * @file UIMesh.cpp
 * @brief The implementation of the UI mesh interface.
 * @date April 17, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Rendering/UIMesh.h>
#include <CityBuilder/Rendering/Uniforms.h>
USING_NS_CITY_BUILDER

UIMesh::~UIMesh() {
  if (loaded) {
    // Unload the mesh from the GPU
    bgfx::destroy(_vertexBuffer);
    bgfx::destroy(_indexBuffer);
    
    // Safety
    loaded = false;
  }
}

UIMesh &UIMesh::add(const List<Vertex> &vertices, const List<int> &indices) {
  // Store the index offset
  uint16_t offset = _vertices.count();
  
  // Add the vertices
  _vertices.appendList(vertices);
  
  // Add the offset indices
  for (int index : indices)
    _indices.append((uint16_t)index + offset);
  
  return *this;
}



void UIMesh::load() {
  if (loaded)
    // Safety
    return;
  
  // Create the vertex buffer
  bgfx::VertexLayout layout;
  layout.begin()
      .add(bgfx::Attrib::Position , 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
  .end();
  _vertexBuffer = bgfx::createVertexBuffer(
    bgfx::copy(&_vertices[0], sizeof(Vertex) * _vertices.count()), layout);
  
  // Create the index buffer
  _indexBuffer = bgfx::createIndexBuffer(
    bgfx::copy(&_indices[0], sizeof(uint16_t) * _indices.count()));
  
  // Clear the user mesh data
  _vertices.removeAll();
  _indices.removeAll();
  
  loaded = true;
}

void UIMesh::draw(Resource<Program> shader) const {
  // Submit the mesh to the GPU for rendering
  bgfx::setVertexBuffer(0, _vertexBuffer);
  bgfx::setIndexBuffer(_indexBuffer);
  shader->submit(1);
}
