/**
 * @file RoadNetwork.h
 * @brief Manages the road network.
 * @date April 15, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Rendering/Mesh.h>
#include <CityBuilder/Storage/BSTree.h>
#include "Road.h"

NS_CITY_BUILDER_BEGIN

/// Manages the road network.
struct RoadNetwork {
  
  RoadNetwork();
  
  /// Add a road to the network.
  /// \param road
  ///   The road to add.
  /// \returns
  ///   The road that was added.
  Road *add(Road *road);
  
  /// Connect two roads together.
  /// \param a
  ///   The first road to connect.
  /// \param b
  ///   The second road to connect.
  /// \returns
  ///   Whether or not the two roads could be connected.
  bool connect(Road *a, Road *b);
  
  
  
  /// Update any roads in the network.
  void update();
  
  
  
  /// Draw the roads.
  void draw();
  
private:
  /// A mesh in the road network.
  struct _mesh {
    /// The mesh.
    Resource<Mesh> mesh;
    
    /// The scale of the texture.
    Real2 textureTiling;
  };
  
  /// Add a mesh to a road for a given lane.
  /// \param[in] road
  ///   The road to add the lane to.
  /// \param[in] lane
  ///   The lane to add a mesh for.
  /// \param[in] lanes
  ///   The lane mesh store for the road.
  /// \returns
  ///   The mesh that was added or loaded, as applicable.
  Resource<Mesh> _addMesh(Road *road, LaneDef *lane, BSTree<LaneDef *, int> &lanes);
  
  /// The road meshes in the network.
  Map<Texture *, List<_mesh>> _meshes;
  
  /// The road marking meshes in the network.
  List<_mesh> _markings;
  
  /// The roads in the network.
  List<Road *> _roads;
  
  /// The texture for road markings
  Resource<Texture> _markingTexture;
};

NS_CITY_BUILDER_END
