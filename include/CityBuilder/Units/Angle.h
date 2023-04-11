/**
 * @file Angle.h
 * @brief An angle unit type.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <math.h>

NS_CITY_BUILDER_BEGIN

/// An angle unit.
struct Angle {
  /// The angle in radians.
  Real radians;
  
  Angle() : radians(0) { }
  
  Angle(Real radians) : radians(radians) { }
  
  static constexpr const Real pi = 3.141592653589793238462643383279;
  
  static constexpr const Real pi2 = pi * 2;
  
  static constexpr const Real piHalf = pi / 2;
  
  static constexpr const Real rad2deg = 180 / pi;
  
  static constexpr const Real deg2rad = pi / 180;
  
  inline Real sin() const {
    return ::sin(radians);
  }
  
  inline Real cos() const {
    return ::cos(radians);
  }
  
  inline Real tan() const {
    return ::tan(radians);
  }
  
  inline static Real sin(Real radians) {
    return ::sin(radians);
  }
  
  inline static Real cos(Real radians) {
    return ::cos(radians);
  }
  
  inline static Real tan(Real radians) {
    return ::tan(radians);
  }
  
  inline Real2 sinCos() const {
    // TODO: Test for sincos support
    Real sin, cos;
    __sincosf(radians, &sin, &cos);
    return { sin, cos };
  }
  
  inline static Real2 sinCos(Real radians) {
    // TODO: Test for sincos support
    Real sin, cos;
    __sincosf(radians, &sin, &cos);
    return { sin, cos };
  }
  
  inline operator Real() const {
    return radians;
  }
  
  inline Angle operator+(Angle other) const {
    return fmodf(radians + other.radians, pi2);
  }
  
  inline Angle operator-(Angle other) const {
    return fmodf(radians - other.radians, pi2);
  }
  
  inline Angle operator*(Real scalar) const {
    return fmodf(radians * scalar, pi2);
  }
  
  inline Angle operator/(Real scalar) const {
    return fmodf(radians / scalar, pi2);
  }
  
  inline Angle operator-() const {
    return fmodf(-radians, pi2);
  }
  
  inline Angle &operator+=(Angle other) {
    radians = fmodf(radians + other.radians, pi2);
    return *this;
  }
  
  inline Angle &operator-=(Angle other) {
    radians = fmodf(radians - other.radians, pi2);
    return *this;
  }
  
  inline Angle &operator*=(Real scalar) {
    radians = fmodf(radians * scalar, pi2);
    return *this;
  }
  
  inline Angle &operator/=(Real scalar) {
    radians = fmodf(radians / scalar, pi2);
    return *this;
  }
  
  inline bool operator==(Angle other) const {
    return radians == other.radians;
  }
  
  inline bool operator!=(Angle other) const {
    return radians != other.radians;
  }
};

static inline constexpr Real operator""_deg(unsigned long long degrees) {
  return degrees * Angle::pi / 180;
}

static inline constexpr Real operator""_deg(long double degrees) {
  return degrees * Angle::pi / 180;
}

NS_CITY_BUILDER_END
