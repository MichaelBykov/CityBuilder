/**
 * @file RoadDef.h
 * @brief An interface for constructing and designing roads.
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/String.h>
#include <CityBuilder/Storage/List.h>
#include "LaneDef.h"

NS_CITY_BUILDER_BEGIN


/// A definition for a road type.
struct RoadDef {
  /// A description of a traffic lane.
  struct Lane {
    /// The lane direction.
    enum class Direction {
      /// An unordered traffic flow.
      /// Only applicable to pedestrian sidewalks.
      unordered,
      /// A left traffic-flow lane.
      left,
      /// A right traffic-flow lane.
      right
    };
    
    /// The lane definition.
    LaneDef *definition;
    
    /// The position of the lane from the origin of the road.
    Real2 position;
    
    /// The traffic direction.
    Direction direction;
    
    /// The speed limit of the lane, in miles per hour.
    int speedLimit;
  };
  
  /// A lane divider description.
  struct Divider {
    /// The lane divider type.
    enum class Type {
      /// A lane-to-lane divider between two lanes traveling in the same
      /// direction.
      lane,
      /// A lane-to-lane divider between two lanes traveling in opposite
      /// directions.
      crossTraffic,
    };
    
    /// The center point of the lane divider.
    Real2 position;
    /// The lane divider type.
    Type type;
  };
  
  enum class Buildings {
    none,
    left,
    right,
    all
  };
  
  /// The decorations of the road.
  Building::ProfileMesh decorations;
  
  /// The lanes of the road.
  List<Lane> lanes;
  
  /// Any dividers drawn on the road.
  List<Divider> dividers;
  
  /// The name of the texture to use for road decorations.
  String decorationsTexture;
  
  /// The name of the road.
  String name;
  
  /// The total width and height of the road.
  Real2 dimensions;
  
  /// The places in which a building can be built on the road.
  Buildings allowBuildings = Buildings::all;
  
  
  
  /// A map of all of the loaded roads.
  static Map<String, RoadDef> roads;
  
  
  
  /// Attempt to load a road.
  /// \param[in] path
  ///   The path to the road file.
  /// \returns
  ///   Whether or not the read was successful.
  static bool load(const String& path);
  
  /// Attempt to load a batch of roads.
  /// \param[in] directory
  ///   The path to the directory containing the road files.
  /// \param[in] ...
  ///   A list of road names to load.
  /// \returns
  ///   Whether or not all of the loading operations were successful.
  static bool loadBatch(const char *directory, ...);
};


NS_CITY_BUILDER_END
