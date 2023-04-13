/**
 * @file Internal.cpp
 * @brief Implement the internal functions for geometry operations.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/Internal.h>
USING_NS_CITY_BUILDER
  
/// The global scene manager.
Ogre::SceneManager *GeometryInternal::scene;

/// An index for the unique name generator.
int GeometryInternal::uniqueNameIndex = 0;

Ogre::String GeometryInternal::uniqueName() {
  return "CITY_BUILDER_" + std::to_string(uniqueNameIndex++);
}
