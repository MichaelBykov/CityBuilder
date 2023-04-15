/**
 * @file Mesh.cpp
 * @brief The implementation of the mesh interface.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Rendering/Mesh.h>
#include <CityBuilder/Rendering/Uniforms.h>
USING_NS_CITY_BUILDER

Mesh::~Mesh() {
  if (loaded) {
    // Unload the mesh from the GPU
    bgfx::destroy(_vertexBuffer);
    bgfx::destroy(_indexBuffer);
    
    // Safety
    loaded = false;
  }
}

Mesh &Mesh::add(const List<Vertex> &vertices, const List<int> &indices) {
  // Store the index offset
  uint16_t offset = _vertices.count();
  
  // Add the vertices
  _vertices.append(vertices);
  
  // Add the offset indices
  for (int index : indices)
    _indices.append((uint16_t)index + offset);
  
  return *this;
}

void Mesh::load() {
  if (loaded)
    // Safety
    return;
  
  // Create the vertex buffer
  bgfx::VertexLayout layout;
  layout.begin()
      .add(bgfx::Attrib::Position , 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Normal   , 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
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

void Mesh::draw(Resource<Material> material) const {
  if (!loaded)
    // Safety
    return;
  
  // Submit the material
  bgfx::setUniform(Uniforms::u_textureTile, { material->textureTile });
  if (material->texture)
    material->texture->load(Uniforms::s_albedo);
  // TODO
  
  // Submit the mesh to the GPU for rendering
  bgfx::setVertexBuffer(0, _vertexBuffer);
  bgfx::setIndexBuffer(_indexBuffer);
  bgfx::setState(BGFX_STATE_DEFAULT);
  material->shader->submit();
}
