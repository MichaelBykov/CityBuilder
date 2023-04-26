/**
 * @file Road.h
 * @brief A road in the game.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Geometry/RadiusPath2.h>
#include <CityBuilder/Rendering/Mesh.h>
#include <CityBuilder/Rendering/ColorMesh.h>
#include <CityBuilder/Zones/ZoneDef.h>
#include "Connection.h"
#include "RoadDef.h"

NS_CITY_BUILDER_BEGIN

/// A road in the game.
struct Road {
  /// The road definition.
  RoadDef *definition;
  
  /// The road's path.
  RadiusPath2 path;
  
  /// The road's start connection.
  Connection start { };
  
  /// The road's end connection.
  Connection end { };
  
  
  
  /// Create a new road.
  /// \param[in] definition
  ///   The road definition.
  ///   Should be a shared pointer.
  /// \param[in] path
  ///   The road's path.
  Road(RoadDef *definition, Ref<Path2 &> path);
  
  
  
  /// Get the road's left zone.
  ZoneDef *leftZone() const;
  
  /// Set the road's left zone.
  /// \param[in] zone
  ///   The zone to set
  void setLeftZone(ZoneDef *zone);
  
  /// Get the road's right zone.
  ZoneDef *rightZone() const;
  
  /// Set the road's right zone.
  /// \param[in] zone
  ///   The zone to set
  void setRightZone(ZoneDef *zone);
  
private:
  friend struct RoadNetwork;
  friend struct Intersection;
  
  /// Whether or not the road needs to be redrawn.
  bool _dirty = true;
  
  struct _mesh {
    Texture *texture;
    Resource<Mesh> mesh;
  };
  
  /// The road's meshes.
  List<_mesh> _meshes { };
  
  /// The road's left zone.
  ZoneDef * _leftZone = nullptr;
  
  /// The road's right zone.
  ZoneDef * _rightZone = nullptr;
  
  /// The road's zone mesh.
  Resource<ColorMesh> _zoneMesh = nullptr;
};

NS_CITY_BUILDER_END
