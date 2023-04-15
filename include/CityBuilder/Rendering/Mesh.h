/**
 * @file Mesh.h
 * @brief An interface for building and working with a mesh.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/List.h>
#include "Material.h"

NS_CITY_BUILDER_BEGIN

/// A mesh description.
struct Mesh {
  /// A vertex.
  struct Vertex {
    /// The position of the vertex within the mesh.
    Real3 position;
    /// The normal of the vertex.
    Real3 normal;
    /// The alpha-blue-green-red color of the vertex.
    Color4 color;
  };
  
  /// Create a new mesh.
  Mesh() { }
  
  // Prevent mesh transfer.
  Mesh(const Mesh &other) = delete;
  
  ~Mesh();
  
  
  
  /// Add a set of vertices and triangles connecting them to the mesh.
  /// \param[in] vertices
  ///   The vertices to add.
  /// \param[in] indices
  ///   The triangle indices to add.
  ///   The indices should reference the passed vertices, not the absolute
  ///   vertex list of the mesh.
  Mesh &add(const List<Vertex> &vertices, const List<int> &indices);
  
  
  
  /// Load the mesh to the GPU.
  void load();
  
  
  
  /// Submit the mesh to the GPU for rendering.
  /// \param[in] material
  ///   The material to use for drawing the mesh.
  void draw(Resource<Material> material) const;
  
  
  
private:
  /// The GPU vertex buffer of the mesh.
  /// \remarks
  ///   Should be freed once the mesh is no longer in use.
  bgfx::VertexBufferHandle _vertexBuffer;
  
  /// The GPU index buffer of the mesh.
  /// \remarks
  ///   Should be freed once the mesh is no longer in use.
  bgfx::IndexBufferHandle _indexBuffer;
  
  /// The vertices of the mesh.
  /// \remarks
  ///   Only stored while the mesh is being constructed.
  ///   Once the mesh has been uploaded to the GPU, this list is kept empty.
  List<Vertex> _vertices { };
  
  /// The indices of the mesh.
  /// \remarks
  ///   Only stored while the mesh is being constructed.
  ///   Once the mesh has been uploaded to the GPU, this list is kept empty.
  List<uint16_t> _indices { };
  
  /// Whether or not the mesh has been loaded to the GPU.
  bool loaded = false;
};

NS_CITY_BUILDER_END
