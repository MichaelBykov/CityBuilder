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

// Include utilities to visualize SIMD types with Natvis
#define SIMD_NATVIS

#include <bgfx/bgfx.h>
#define BX_CONFIG_DEBUG 0
#include <bx/math.h>
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

/// A pack of two by two base floating-point units in the game.
typedef SIMD::Float32x2x2 Real2x2;

/// A pack of two by three base floating-point units in the game.
typedef SIMD::Float32x2x3 Real2x3;

/// A pack of two by four base floating-point units in the game.
typedef SIMD::Float32x2x4 Real2x4;

/// A pack of three by two base floating-point units in the game.
typedef SIMD::Float32x3x2 Real3x2;

/// A pack of three by three base floating-point units in the game.
typedef SIMD::Float32x3x3 Real3x3;

/// A pack of three by four base floating-point units in the game.
typedef SIMD::Float32x3x4 Real3x4;

/// A pack of four by two base floating-point units in the game.
typedef SIMD::Float32x4x2 Real4x2;

/// A pack of four by three base floating-point units in the game.
typedef SIMD::Float32x4x3 Real4x3;

/// A pack of four by four base floating-point units in the game.
typedef SIMD::Float32x4x4 Real4x4;

/// A grayscale color value.
typedef SIMD::UInt8 Color1;

/// A red-green-blue color value.
typedef SIMD::UInt8x3 Color3;

/// A red-green-blue-alpha color value.
typedef SIMD::UInt8x4 Color4;

/// A quaternion.
typedef bx::Quaternion Quaternion;

NS_CITY_BUILDER_END
