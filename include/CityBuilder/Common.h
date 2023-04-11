/**
 * @file Common.h
 * @brief
 *   A set of common definitions for inclusion.
 *   Can be used as a precompiled header.
 * @version 1.0 
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once

#include <Ogre.h>
#include <OgreApplicationContext.h>



#define NS_CITY_BUILDER_BEGIN \
  namespace CityBuilder {
#define NS_CITY_BUILDER_END \
  }

#define USING_NS_CITY_BUILDER \
  using namespace CityBuilder;



NS_CITY_BUILDER_BEGIN

// Base type aliases

/// A base floating-point unit in the game.
typedef Ogre::Real Real;

/// A pack of two base floating-point units in the game.
typedef Ogre::Vector2 Real2;

/// A pack of three base floating-point units in the game.
typedef Ogre::Vector3 Real3;

/// A pack of four base floating-point units in the game.
typedef Ogre::Vector4 Real4;

NS_CITY_BUILDER_END
