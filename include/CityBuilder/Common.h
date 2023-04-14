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

#include <bgfx/bgfx.h>
#include <SIMD/SIMD++>



#define NS_CITY_BUILDER_BEGIN \
  namespace CityBuilder {
#define NS_CITY_BUILDER_END \
  }

#define USING_NS_CITY_BUILDER \
  using namespace CityBuilder;

#define NS_CITY_BUILDER \
  CityBuilder::



NS_CITY_BUILDER_BEGIN

// Base type aliases

/// A base floating-point unit in the game.
typedef SIMD::Float32 Real;

/// A pack of two base floating-point units in the game.
typedef SIMD::Float32x2 Real2;

/// A pack of three base floating-point units in the game.
typedef SIMD::Float32x3 Real3;

/// A pack of four base floating-point units in the game.
typedef SIMD::Float32x4 Real4;

NS_CITY_BUILDER_END
