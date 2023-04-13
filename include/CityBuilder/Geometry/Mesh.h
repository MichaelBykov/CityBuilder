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
  
  /// Extrude a cross-section along a path and add it as a sub-mesh.
  /// \param profile
  ///   The cross-section to extrude.
  /// \param path
  ///   The path to extrude the cross-section along.
  /// \param offset
  ///   The offset of the cross-section from the path.
  /// \param scale
  ///   The scale of the cross-section.
  ///   Does not affect the path points.
  /// \param textureName
  ///   The name of the texture to use for the sub-mesh.
  /// \param textureScale
  ///   The scale of the sub-mesh's texture.
  Mesh &addExtrusion(
    const ProfileMesh &profile,
    Path2 &path,
    const Real2 offset,
    Real scale,
    const String &textureName,
    Real2 textureScale = { 1, 1 }
  );
  
  /// Finish the mesh.
  /// \remarks
  ///   Must be called after all sub-meshes have been added.
  void finish();
  
  
  
  /// Set the current global scene manager.
  /// \param sceneManager
  ///   The scene manager to use.
  static void setSceneManager(Ogre::SceneManager *sceneManager);
  
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
