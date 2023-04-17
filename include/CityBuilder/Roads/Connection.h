/**
 * @file Connection.h
 * @brief A road connection.
 * @date April 15, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>

NS_CITY_BUILDER_BEGIN

struct Road;

struct Intersection;

/// A connection between two roads and/or intersections.
struct Connection {
  /// The connection to the other object.
  union {
    /// The road.
    Road *road;
    
    /// The intersection.
    Intersection *intersection;
  } other;
  
  /// The connection type.
  enum Type {
    /// The connection is to a road.
    road,
    
    /// The connection is to an intersection.
    intersection,
    
    /// There is no connection.
    none,
  } type;
  
  
  
  Connection() : type(none) { }
  
  Connection(nullptr_t) : type(none) { }
  
  /// Create a new road connection.
  Connection(Road *road)
    : other { .road = road }, type(Type::road) { }
  
  /// Create a new intersection connection.
  Connection(Intersection *intersection)
    : other { .intersection = intersection }, type(Type::intersection) { }
};

NS_CITY_BUILDER_END
