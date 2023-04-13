/**
 * @file Internal.h
 * @brief Internal functions for geometry operations.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>

NS_CITY_BUILDER_BEGIN
namespace GeometryInternal {

/// The global scene manager.
extern Ogre::SceneManager *scene;

/// An index for the unique name generator.
extern int uniqueNameIndex;

/// Generate a unique name to use with Ogre.
Ogre::String uniqueName();

} // namespace GeometryInternal
NS_CITY_BUILDER_END
