/**
 * @file DynamicMesh.h
 * @brief An interface for building and working with a dynamic mesh.
 * @date April 18, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/List.h>
#include <CityBuilder/Geometry/Profile.h>
#include "Material.h"

NS_CITY_BUILDER_BEGIN

/// A dynamic mesh description.
struct DynamicMesh {
  /// A vertex.
  struct Vertex {
    /// The position of the vertex within the mesh.
    Real3 position;
    /// The color of the vertex.
    Color4 color;
  };
  
  /// Create a new mesh.
  DynamicMesh() { }
  
  // Prevent mesh transfer.
  DynamicMesh(const DynamicMesh &other) = delete;
  
  ~DynamicMesh();
  
  
  
  /// Add a set of vertices and triangles connecting them to the mesh.
  /// \param[in] vertices
  ///   The vertices to add.
  /// \param[in] indices
  ///   The triangle indices to add.
  ///   The indices should reference the passed vertices, not the absolute
  ///   vertex list of the mesh.
  DynamicMesh &add(const List<Vertex> &vertices, const List<int> &indices);
  
  
  
  /// Extrude a cross-section along a path and add it as a sub-mesh.
  /// \param[in] profile
  ///   The cross-section to extrude.
  /// \param[in] path
  ///   The path to extrude the cross-section along.
  /// \param[in] color
  ///   The color of the extrusion.
  /// \param[in] offset
  ///   The offset of the cross-section from the path.
  /// \param[in] scale
  ///   The scale of the cross-section.
  ///   Does not affect the path points.
  DynamicMesh &extrude(const ProfileMesh &profile, Path2 &path, Color4 color, Real2 offset = { 0, 0 }, Real scale = 1);
  
  
  
  /// Load the mesh to the GPU.
  void load();
  
  
  
  /// Submit the mesh to the GPU for rendering.
  /// \param[in] shader
  ///   The shader to use for drawing the mesh.
  /// \remarks
  ///   A material should be set before calling this.
  void draw(Resource<Program> shader) const;
  
  /// Submit the mesh to the GPU for rendering.
  /// \param[in] material
  ///   The material to use for drawing the mesh.
  void draw(Resource<Material> material) const;
  
  
  
private:
  /// The GPU vertex buffer of the mesh.
  /// \remarks
  ///   Should be freed once the mesh is no longer in use.
  bgfx::DynamicVertexBufferHandle _vertexBuffer;
  
  /// The GPU index buffer of the mesh.
  /// \remarks
  ///   Should be freed once the mesh is no longer in use.
  bgfx::DynamicIndexBufferHandle _indexBuffer;
  
  /// The vertices of the mesh.
  List<Vertex> _vertices { };
  
  /// The indices of the mesh.
  List<uint16_t> _indices { };
  
  /// The last uploaded vertex count.
  int _vertexCount = 0;
  
  /// The last uploaded index count.
  int _indexCount = 0;
  
  /// Whether or not the mesh has been loaded to the GPU.
  bool loaded = false;
};

NS_CITY_BUILDER_END
