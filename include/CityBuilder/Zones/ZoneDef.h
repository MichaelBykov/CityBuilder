/**
 * @file ZoneDef.h
 * @brief The definition of a building zone.
 * @date April 26, 2023
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


/// A building zone description.
struct ZoneDef {
  /// The name of the zone.
  String name;
  
  
  
  /// A map of all of the loaded zones.
  static Map<String, ZoneDef> zones;
  
  
  
  /// Attempt to load a zone.
  /// \param[in] path
  ///   The path to the zone file.
  /// \returns
  ///   Whether or not the read was successful.
  static bool load(const String& path);
  
  /// Attempt to load a batch of zones.
  /// \param[in] directory
  ///   The path to the directory containing the zone files.
  /// \param[in] ...
  ///   A list of zone names to load.
  /// \returns
  ///   Whether or not all of the loading operations were successful.
  static bool loadBatch(const char *directory, ...);
};


NS_CITY_BUILDER_END
