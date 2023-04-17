/**
 * @file Road.h
 * @brief A road in the game.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Geometry/Path2.h>
#include <CityBuilder/Rendering/Mesh.h>
#include "Connection.h"
#include "RoadDef.h"

NS_CITY_BUILDER_BEGIN

/// A road in the game.
struct Road {
  /// The road definition.
  RoadDef *definition;
  
  /// The road's path.
  Path2 &path;
  
  /// The road's start connection.
  Connection start;
  
  /// The road's end connection.
  Connection end;
  
  
  
  /// Create a new road.
  /// \param definition
  ///   The road definition.
  ///   Should be a shared pointer.
  /// \param path
  ///   The road's path.
  ///   Should be a unique instance.
  /// \param scene
  ///   The scene manager.
  Road(RoadDef *definition, Path2 &path);
  
  // We will never need to be transferring the road around.
  Road(const Road &other) = delete;
  
  ~Road();
  
private:
  friend struct RoadNetwork;
  
  /// Whether or not the road needs to be redrawn.
  bool _dirty = true;
  
  /// The road's meshes.
  List<Resource<Mesh>> _meshes { };
};

NS_CITY_BUILDER_END
