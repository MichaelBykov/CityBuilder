/**
 * @file SharedMesh.h
 * @brief A simple shared mesh interface.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/List.h>
#include <CityBuilder/Storage/String.h>
#include <CityBuilder/Storage/Ref.h>
#include "Profile.h"

NS_CITY_BUILDER_BEGIN

/// A simple shared mesh interface.
struct SharedMesh {
  /// A vertex.
  struct Vertex {
    /// The position of the vertex within the mesh.
    Real3 position;
    /// The normal of the vertex.
    Real3 normal;
    /// The texture coordinates of the vertex.
    Real2 uv;
  };
  
  /// Create a new mesh.
  SharedMesh();
  
  SharedMesh(const SharedMesh &other) = delete;
  
  ~SharedMesh();
  
  /// Add a sub-mesh to the mesh.
  /// \param vertices
  ///   The vertices of the sub-mesh.
  /// \param indices
  ///   The triangle indices of the sub-mesh.
  /// \param textureName
  ///   The name of the texture to use for the sub-mesh.
  /// \param textureScale
  ///   The scale of the sub-mesh's texture.
  SharedMesh &addSubMesh(
    const List<Vertex> &vertices,
    const List<int> &indices,
    const String &textureName,
    Real2 textureScale = { 1, 1 }
  );
  
  /// A description of a revolution of a cross section.
  struct Revolution {
    /// The cross-section to extrude.
    const ProfileMesh &profile;
    /// The offset of the cross-section from the origin.
    Real2 offset;
    /// The scale of the cross-section.
    Real scale;
  };
  
  /// Revolve a cross-section about the mesh origin and add it as a sub-mesh.
  /// \param revolution
  ///   The description of the revolution.
  /// \param textureName
  ///   The name of the texture to use for the sub-mesh.
  /// \param textureScale
  ///   The scale of the sub-mesh's texture.
  SharedMesh &addRevolution(
    Revolution extrusion,
    const String &textureName,
    Real2 textureScale = { 1, 1 }
  );
  
  /// Revolve a set of cross-sections about the mesh origin and add them as a
  /// single sub-mesh.
  /// \param revolutions
  ///   The revolution descriptions.
  /// \param textureName
  ///   The name of the texture to use for the sub-mesh.
  /// \param textureScale
  ///   The scale of the sub-mesh's texture.
  SharedMesh &addRevolutions(
    List<Revolution> extrusions,
    const String &textureName,
    Real2 textureScale = { 1, 1 }
  );
  
  /// Finish the mesh.
  /// \remarks
  ///   Must be called after all sub-meshes have been added.
  void finish();
  
  
  
  /// An instance of a shared mesh.
  struct Instance {
    Instance(const Instance &other) = delete;
    
    ~Instance();
    
    /// Set the position of the instance.
    /// \param position
    ///   The position of the instance in world space.
    Instance &setPosition(Real3 position);
    
    /// Set the orientation of the instance.
    /// \param orientation
    ///   The orientation of the instance in world space.
    Instance &setOrientation(Ogre::Quaternion orientation);
    
  private:
    friend SharedMesh;
    
    /// The entity that the mesh is attached to.
    Ogre::Entity *_entity;
    
    /// The instance scene node.
    Ogre::SceneNode *_node;
    
    /// Instantiate a shared mesh.
    Instance(const SharedMesh &mesh);
  };
  
  /// Create an instance of the mesh.
  Ref<Instance &> instantiate();
  
private:
  /// The bounds of the mesh.
  Ogre::AxisAlignedBox _bounds;
  
  /// The actual underlying mesh.
  Ogre::MeshPtr _mesh;
};

NS_CITY_BUILDER_END
