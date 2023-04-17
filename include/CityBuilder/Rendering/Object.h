/**
 * @file Object.h
 * @brief An instance of a mesh for rendering.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include "Resource.h"
#include "Mesh.h"
#include "Material.h"

NS_CITY_BUILDER_BEGIN

/// An instance of a mesh for rendering.
struct Object {
  /// The position of the object in world space.
  Real3 position;
  
  /// The rotation of the object in world space.
  Quaternion rotation;
  
  /// The scale of the object in world space.
  Real3 scale;
  
  /// The object mesh.
  Resource<Mesh> mesh;
  
  /// The object material.
  Resource<Material> material;
  
  
  /// Create a new object.
  /// \param[in] mesh
  ///   The object mesh.
  /// \param[in] material
  ///   The object material.
  Object(Resource<Mesh> mesh, Resource<Material> material);
  
  
  
  /// Render the object.
  void draw() const;
};

NS_CITY_BUILDER_END
