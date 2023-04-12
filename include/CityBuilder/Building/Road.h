/**
 * @file Road.h
 * @brief An interface for constructing and designing roads.
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/String.h>
#include <CityBuilder/Storage/List.h>

NS_CITY_BUILDER_BEGIN


/// A definition for a road type.
struct RoadDef {
  /// A point on the road profile.
  struct Point {
    /// The point type.
    enum class Type {
      /// A "move" node that connects to either the previous or next non-move
      /// node.
      /// \remarks
      ///   Has a single position, normal, and uv.
      move,
      /// A disjoint node that separates a point into two distinct vertices used
      /// to create sharp edges.
      /// \remarks
      ///   Has a single position and uv but two normals.
      disjoint,
      /// A connected node that connects to the previous and next nodes.
      /// \remarks
      ///   Has a single position, normal, and uv.
      connected
    };
    
    /// The position of the point in the profile.
    /// \remarks
    ///   Positions start at the origin (0, 0) and extend into the positive x-
    ///   and y-axes.
    ///   Points should never have a negative x or y value.
    Real2 position;
    /// The first normal vector.
    Real2 normal0;
    /// The second normal vector.
    Real2 normal1;
    /// The first uv coordinate.
    Real uv0;
    /// The point type.
    Type type;
  };
  
  /// A description of a traffic lane.
  struct Traffic {
    /// The lane type.
    enum class Type {
      /// An unordered traffic flow.
      /// Only applicable to pedestrian sidewalks.
      unordered,
      /// A left traffic-flow lane.
      left,
      /// A right traffic-flow lane.
      right
    };
    
    /// The category of traffic that can operate on the lane.
    enum class Category {
      /// Any pedestrian.
      all_peds,
      /// Any vehicle.
      all_vehicles,
    };
    
    /// The start x-coordinate of the lane.
    Real start;
    /// The end x-coordinate of the lane.
    Real end;
    /// The elevation of the lane.
    Real elevation;
    /// The speed limit of the lane in miles per hour.
    int speedLimit;
    /// The lane type.
    Type type;
    /// The traffic category of the lane.
    Category category;
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
  
  /// The profile of the road.
  List<Point> profile;
  
  /// The traffic patterns of the road.
  List<Traffic> traffic;
  
  /// Any dividers drawn on the road.
  List<Divider> dividers;
  
  /// The name of the main texture to use for the road.
  String mainTexture;
  
  /// The name of the road.
  String name;
  
  /// The total width and height of the road.
  Real2 dimensions;
  
  
  
  /// Attempt to load a road.
  /// \param[in] path
  ///   The path to the road file.
  /// \param[out] road
  ///   The read road, if successful.
  /// \returns
  ///   Whether or not the read was successful.
  static bool load(const String& path, RoadDef& road);
};


NS_CITY_BUILDER_END
