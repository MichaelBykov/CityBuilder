/**
 * @file LaneDef.h
 * @brief The definition of a road traffic lane.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/String.h>
#include <CityBuilder/Storage/Map.h>
#include <CityBuilder/Geometry/Profile.h>
#include <CityBuilder/Rendering/Resource.h>
#include <CityBuilder/Rendering/Texture.h>

NS_CITY_BUILDER_BEGIN


/// A road traffic lane description.
struct LaneDef {
  /// A description of a traffic lane.
  struct Traffic {
    /// The lane type.
    enum class Type {
      /// An unordered traffic flow.
      /// Only applicable to pedestrian sidewalks.
      unordered,
      /// A lane in which traffic travels in a single direction.
      directional
    };
    
    /// The category of traffic that can operate on the lane.
    enum class Category {
      /// Any pedestrian.
      all_peds,
      /// Any vehicle.
      all_vehicles,
    };
    
    /// The way the lane should connect at intersections.
    enum class Connection {
      /// The lane should not connect at intersections.
      none,
      /// The lane should connect to all of the same lane traveling in the same
      /// direction.
      sameDirection,
      /// The lane should connect to the nearest lane of the same type, if it
      /// exists.
      nearest
    };
    
    /// The start x-coordinate of the lane.
    Real start;
    /// The end x-coordinate of the lane.
    Real end;
    /// The elevation of the lane.
    Real elevation;
    /// The lane type.
    Type type;
    /// The traffic category of the lane.
    Category category;
    /// The way the lane should connect at intersections.
    Connection connection = Connection::none;
  };
  
  /// The profile of the lane.
  ProfileMesh profile;
  
  /// The traffic patterns of the lane.
  List<Traffic> traffic;
  
  /// The main texture to use for the lane.
  Resource<Texture> mainTexture;
  
  /// The name of the lane.
  String name;
  
  
  
  /// A map of all of the loaded lanes.
  static Map<String, LaneDef> lanes;
  
  
  
  /// Attempt to load a road lane.
  /// \param[in] path
  ///   The path to the lane file.
  /// \returns
  ///   Whether or not the read was successful.
  static bool load(const String& path);
  
  /// Attempt to load a batch of road lanes.
  /// \param[in] directory
  ///   The path to the directory containing the lane files.
  /// \param[in] ...
  ///   A list of lane names to load.
  /// \returns
  ///   Whether or not all of the loading operations were successful.
  static bool loadBatch(const char *directory, ...);
};


NS_CITY_BUILDER_END
