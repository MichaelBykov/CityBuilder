/**
 * @file Intersection.h
 * @brief A road intersection.
 * @date April 25, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Rendering/Mesh.h>
#include "Road.h"

NS_CITY_BUILDER_BEGIN

/// A road intersection.
struct Intersection {
  /// An arm of an intersection.
  struct Arm {
    /// The road that the arm is connected to.
    Road *road;
    
    /// The connection that the arm is connected to.
    bool start;
    
    /// The angle of the arm relative to the center point of the intersection.
    Angle angle;
    
    /// The amount that the arm has been pushed back.
    Real pushed;
    
    // For sorting
    bool operator < (const Arm &other) const {
      return angle.radians < other.angle.radians;
    }
  };
  
  /// The arms of the intersection.
  List<Arm> arms { };
  
  /// The center point of the intersection.
  Real2 center;
  
  /// The (maximum) radius of the intersection.
  Real radius;
  
  
  /// Create a new intersection.
  /// \param[in] center
  ///   The center point of the intersection.
  Intersection(Real2 center);
  
  
  
  /// Add a road to the intersection.
  /// \param[inout] road
  ///   The road to add.
  void addRoad(Road *road);
  
private:
  friend struct RoadNetwork;
  
  /// Whether or not the intersection needs to be redrawn.
  bool _dirty = true;
  
  struct _mesh {
    Texture *texture;
    Resource<Mesh> mesh;
  };
  
  /// The intersection's meshes.
  List<_mesh> _meshes { };
};

NS_CITY_BUILDER_END
