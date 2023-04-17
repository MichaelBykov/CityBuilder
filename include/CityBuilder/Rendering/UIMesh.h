/**
 * @file UIMesh.h
 * @brief An interface for building and working with a mesh for UIs.
 * @date April 17, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/List.h>
#include <CityBuilder/Geometry/Profile.h>
#include "Material.h"

NS_CITY_BUILDER_BEGIN

/// A UI mesh description.
struct UIMesh {
  /// A vertex.
  struct Vertex {
    /// The position of the vertex within the mesh.
    Real3 position;
    /// The texture coordinates of the vertex.
    Real2 uv;
  };
  
  /// Create a new UI mesh.
  UIMesh() { }
  
  // Prevent UI mesh transfer.
  UIMesh(const UIMesh &other) = delete;
  
  ~UIMesh();
  
  
  
  /// Add a set of vertices and triangles connecting them to the mesh.
  /// \param[in] vertices
  ///   The vertices to add.
  /// \param[in] indices
  ///   The triangle indices to add.
  ///   The indices should reference the passed vertices, not the absolute
  ///   vertex list of the mesh.
  UIMesh &add(const List<Vertex> &vertices, const List<int> &indices);
  
  
  
  /// Load the mesh to the GPU.
  void load();
  
  
  
  /// Submit the mesh to the GPU for rendering.
  /// \param[in] shader
  ///   The shader to use for drawing the mesh.
  /// \remarks
  ///   A material should be set before calling this.
  void draw(Resource<Program> shader) const;
  
  
  
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
