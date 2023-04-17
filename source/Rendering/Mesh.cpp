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
  _vertices.appendList(vertices);
  
  // Add the offset indices
  for (int index : indices)
    _indices.append((uint16_t)index + offset);
  
  return *this;
}



Mesh &Mesh::extrude(const ProfileMesh &profile, Path2 &path, Real2 offset, Real scale) {
  uint16_t indexOffset = _vertices.count();
  
  // Get the path points
  List<Real4> points = path.pointNormals();
  
  if (points.count() < 2)
    // Nothing to extrude over
    return *this;
  
  // Extrude the profile
  for (int i = 0; i < points.count(); i++) {
    // Get the current point data
    const Real4 &pointNormal = points[i];
    Real2 point  = { pointNormal.x, pointNormal.y };
    Real2 normal = { pointNormal.z, pointNormal.w };
    
    // Add the vertices
    for (const ProfileMesh::Vertex &vertex : profile.vertices) {
      Real2 norm = normal * Real2(vertex.normal.x);
      _vertices.append({
        Real3(
          point.x,
          (offset.y + vertex.position.y) * scale,
          point.y
        ) +
        Real3(normal.x, 0, normal.y) *
        Real3((vertex.position.x + offset.x) * scale),
        { norm.x, vertex.normal.y, norm.y },
        { vertex.uv, Real(i) / Real((int)points.count() - 1) }
      });
    }
    
    if (i > 0) {
      // Connect triangles with the previous extrusion
      int prev = indexOffset + (i - 1) * profile.vertices.count();
      int curr = prev + profile.vertices.count();
      for (intptr_t j = 0; j < profile.triangles.count(); j += 2) {
        _indices.append(prev + profile.triangles[j    ]);
        _indices.append(prev + profile.triangles[j + 1]);
        _indices.append(curr + profile.triangles[j    ]);
        
        _indices.append(curr + profile.triangles[j    ]);
        _indices.append(prev + profile.triangles[j + 1]);
        _indices.append(curr + profile.triangles[j + 1]);
      }
    }
  }
  
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

void Mesh::draw(Resource<Program> shader) const {
  // Submit the mesh to the GPU for rendering
  bgfx::setVertexBuffer(0, _vertexBuffer);
  bgfx::setIndexBuffer(_indexBuffer);
  shader->submit();
}

void Mesh::draw(Resource<Material> material) const {
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
