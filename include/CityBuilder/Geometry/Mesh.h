/**
 * @file Mesh.h
 * @brief A simple mesh interface.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/List.h>
#include <CityBuilder/Storage/String.h>
#include "Profile.h"

NS_CITY_BUILDER_BEGIN

/// A simple mesh interface.
struct Mesh {
  /// A vertex.
  struct Vertex {
    /// The position of the vertex within the mesh.
    Real3 position;
    /// The normal of the vertex.
    Real3 normal;
    /// The texture coordinates of the vertex.
    Real2 uv;
  };
  
  /// Creates a new mesh.
  Mesh();
  
  Mesh(const Mesh &other) = delete;
  
  ~Mesh();
  
  /// Add a sub-mesh to the mesh.
  /// \param vertices
  ///   The vertices of the sub-mesh.
  /// \param indices
  ///   The triangle indices of the sub-mesh.
  /// \param textureName
  ///   The name of the texture to use for the sub-mesh.
  /// \param textureScale
  ///   The scale of the sub-mesh's texture.
  Mesh &addSubMesh(
    const List<Vertex> &vertices,
    const List<int> &indices,
    const String &textureName,
    Real2 textureScale = { 1, 1 }
  );
  
  /// A description of an extrusion of a cross section.
  struct Extrusion {
    /// The cross-section to extrude.
    const ProfileMesh &profile;
    /// The path to extrude the cross-section along.
    Path2 &path;
    /// The offset of the cross-section from the path.
    Real2 offset;
    /// The scale of the cross-section.
    /// Does not affect the path points.
    Real scale;
  };
  
  /// Extrude a cross-section along a path and add it as a sub-mesh.
  /// \param extrusion
  ///   The description of the extrusion.
  /// \param textureName
  ///   The name of the texture to use for the sub-mesh.
  /// \param textureScale
  ///   The scale of the sub-mesh's texture.
  Mesh &addExtrusion(
    Extrusion extrusion,
    const String &textureName,
    Real2 textureScale = { 1, 1 }
  );
  
  /// Extrude a set of cross-sections along paths and add them as a single
  /// sub-mesh.
  /// \param extrusions
  ///   The extrusion descriptions.
  /// \param textureName
  ///   The name of the texture to use for the sub-mesh.
  /// \param textureScale
  ///   The scale of the sub-mesh's texture.
  Mesh &addExtrusions(
    List<Extrusion> extrusions,
    const String &textureName,
    Real2 textureScale = { 1, 1 }
  );
  
  /// Finish the mesh.
  /// \remarks
  ///   Must be called after all sub-meshes have been added.
  void finish();
  
private:
  /// The bounds of the mesh.
  Ogre::AxisAlignedBox _bounds;
  
  /// The actual underlying mesh.
  Ogre::MeshPtr _mesh;
  
  /// The entity that the mesh is attached to.
  Ogre::Entity *_entity;
  
  /// The mesh's scene node.
  Ogre::SceneNode *_node;
};

NS_CITY_BUILDER_END
