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
#include "Profile.h"

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
  };
  
  /// The profile of the lane.
  Building::ProfileMesh profile;
  
  /// The traffic patterns of the lane.
  List<Traffic> traffic;
  
  /// The name of the main texture to use for the lane.
  String mainTexture;
  
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
