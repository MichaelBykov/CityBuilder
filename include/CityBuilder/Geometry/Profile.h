/**
 * @file Profile.h
 * @brief A description of a cross-section of a body.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/List.h>
#include "CityBuilder/Geometry/Path2.h"

NS_CITY_BUILDER_BEGIN

/// A point in a side profile.
struct ProfilePoint {
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


/// A side profile mesh that can be extruded.
struct ProfileMesh {
  /// A vertex in the mesh.
  struct Vertex {
    /// The position of the vertex from the origin of the mesh.
    Real2 position;
    /// The normal vector of the vertex.
    Real2 normal;
    /// The uv coordinate of the vertex.
    Real uv;
  };
  
  /// The vertices of the mesh.
  List<Vertex> vertices;
  
  /// The triangles of the mesh.
  List<int> triangles;
  
  /// The dimensions of the mesh.
  /// \remarks
  ///   The mesh is evenly centered around the origin horizontally and never
  ///   goes below the origin vertically.
  Real2 dimensions;
  
  
  
  /// Create an empty profile mesh.
  ProfileMesh() = default;
  
  /// Create a profile mesh from a list of points.
  /// \param points
  ///   The points to create the mesh from.
  ProfileMesh(const List<ProfilePoint> &points);
};

NS_CITY_BUILDER_END
