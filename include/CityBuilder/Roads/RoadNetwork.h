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
#include "Intersection.h"

NS_CITY_BUILDER_BEGIN

/// Manages the road network.
struct RoadNetwork {
  
  RoadNetwork();
  
  /// Add a road to the network.
  /// \param[inout] road
  ///   The road to add.
  /// \returns
  ///   The road that was added.
  Road *add(Road *road);
  
  /// Remove a road from the network.
  /// \param[inout] road
  ///   The road to remove.
  void remove(Road *road);
  
  /// Add an intersection to the network.
  /// \param[inout] intersection
  ///   The intersection to add.
  /// \returns
  ///   The intersection that was added.
  Intersection *add(Intersection *intersection);
  
  /// Connect two roads together.
  /// \param[inout] a
  ///   The first road to connect.
  /// \param[inout] b
  ///   The second road to connect.
  /// \returns
  ///   Whether or not the two roads could be connected.
  bool connect(Road *a, Road *b);
  
  /// Attempt to connect two roads through an intersection.
  /// \param[inout] a
  ///   The road to connect.
  /// \param[inout] b
  ///   The road to connect to.
  /// \returns
  ///   A list of the sub-roads that the road was split into.
  List<Road *> intersect(Road *a, Road *b);
  
  /// Snap a point to the nearest road.
  /// \param[in] point
  ///   The point to snap.
  /// \param[out] snapped
  ///   The road that was snapped to.
  /// \param[out] snappedIntersection
  ///   The intersection that was snapped to.
  /// \returns
  ///   The snapped point or the point untouched if there are no nearby roads
  ///   to snap to.
  Real3 snap(const Real3 &point, Road *&snapped, Intersection *&snappedIntersection);
  
  /// Snap a point to the nearest road.
  /// \param[in] point
  ///   The point to snap.
  /// \param[out] snapped
  ///   The road that was snapped to.
  /// \returns
  ///   The snapped point or the point untouched if there are no nearby roads
  ///   to snap to.
  inline Real3 snap(const Real3 &point) {
    Road *snapped;
    Intersection *snappedIntersection;
    return snap(point, snapped, snappedIntersection);
  }
  
  
  
  /// Validate if a point can start a road.
  /// \param[in] road
  ///   The road definition that will be built at the point.
  /// \param[in] point
  ///   The point to validate.
  /// \returns
  ///   Whether or not the point can serve as a start point for a road.
  bool validate(RoadDef *road, Real3 point);
  
  /// Validate if a road can follow a given path.
  /// \param[in] road
  ///   The road definition that will be built along the path.
  /// \param[in] path
  ///   The path to validate.
  /// \returns
  ///   Whether or not the given road can be built along the given path.
  bool validate(RoadDef *road, Ref<Path2 &> path);
  
  
  
  /// Build a road along a given path.
  /// \param[in] road
  ///   The road to build.
  /// \param[in] path
  ///   The path to build the road along.
  /// \returns
  ///   Whether or not the road was built.
  bool build(RoadDef *road, Ref<Path2 &> path);
  
  
  
  /// Get a zone at a given point, if one exists there.
  /// \param[in] point
  ///   The point to test for zones.
  /// \param[out] side
  ///   Whether or not the zone is on the left or right side of the road.
  /// \returns
  ///   The road to which the belongs to at the given point, if one exists.
  Road *getZone(Real3 point, bool &side);
  
  /// Set a zone for a road.
  /// \param[in] road
  ///   The road to zone.
  /// \param[in] side
  ///   The side of the road to zone.
  /// \param[in] zone
  ///   The zone to set.
  void setZone(Road *road, bool side, ZoneDef *zone);
  
  
  
  /// Update any roads in the network.
  void update();
  
  
  
  /// Draw the roads.
  void draw();
  
  /// Draw the zones.
  void drawZones();
  
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
  
  /// Add a mesh to an intersection for a given lane.
  /// \param[in] intersection
  ///   The intersection to add the lane to.
  /// \param[in] lane
  ///   The lane to add a mesh for.
  /// \param[in] lanes
  ///   The lane mesh store for the road.
  /// \returns
  ///   The mesh that was added or loaded, as applicable.
  Resource<Mesh> _addMesh(Intersection *intersection, LaneDef *lane, BSTree<LaneDef *, int> &lanes);
  
  /// The road meshes in the network.
  Map<Texture *, List<_mesh>> _meshes;
  
  /// The road marking meshes in the network.
  List<_mesh> _markings;
  
  /// The roads in the network.
  List<Road *> _roads;
  
  /// The intersections in the network.
  List<Intersection *> _intersections;
  
  /// The zone meshes
  List<Resource<ColorMesh>> _zoneMeshes;
  
  /// The texture for road markings
  Resource<Texture> _markingTexture;
  
  /// The texture for zones
  Resource<Texture> _zoneTexture;
};

NS_CITY_BUILDER_END
