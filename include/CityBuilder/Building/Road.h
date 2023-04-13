/**
 * @file Road.h
 * @brief A road in the game.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Geometry/Path2.h>
#include <CityBuilder/Geometry/Mesh.h>
#include "RoadDef.h"

NS_CITY_BUILDER_BEGIN

/// A road in the game.
struct Road {
  /// The road definition.
  RoadDef *definition;
  
  /// The road's path.
  Path2 &path;
  
  /// The road's mesh.
  Mesh mesh;
  
  
  
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
};

NS_CITY_BUILDER_END
