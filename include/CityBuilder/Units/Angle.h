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
  
  Angle(float radians) : radians(radians) { }
  
  Angle(int radians) : radians(radians) { }
  
  Angle(Real radians) : radians(radians) { }
  
  Angle(Real2 vector) : radians(atan2(vector.y, vector.x)) { }
  
  static constexpr const float pi = 3.141592653589793238462643383279;
  
  static constexpr const float pi2 = pi * 2;
  
  static constexpr const float piHalf = pi / 2;
  
  static constexpr const float rad2deg = 180 / pi;
  
  static constexpr const float deg2rad = pi / 180;
  
  inline Real degrees() const {
    return radians * Real(rad2deg);
  }
  
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
    Real2 sinCos;
    radians.sinCos(&sinCos.x, &sinCos.y);
    return sinCos;
  }
  
  inline Real2 cosSin() const {
    Real2 cosSin;
    radians.sinCos(&cosSin.y, &cosSin.x);
    return cosSin;
  }
  
  inline static Real2 sinCos(Real radians) {
    Real2 sinCos;
    radians.sinCos(&sinCos.x, &sinCos.y);
    return sinCos;
  }
  
  inline static Real2 cosSin(Real radians) {
    Real2 cosSin;
    radians.sinCos(&cosSin.y, &cosSin.x);
    return cosSin;
  }
  
  static Angle span(Angle start, Angle end) {
    Real span = 0;
    while (end.radians < start.radians) {
      span += Real(pi2);
      end.radians += Real(pi2);
    }
    return (span + (end.radians - start.radians)) % Real(pi2);
  }
  
  inline operator Real() const {
    return radians;
  }
  
  inline Angle operator+(Angle other) const {
    return (radians + other.radians) % Real(pi2);
  }
  
  inline Angle operator-(Angle other) const {
    return (radians - other.radians) % Real(pi2);
  }
  
  inline Angle operator*(Real scalar) const {
    return (radians * scalar) % Real(pi2);
  }
  
  inline Angle operator/(Real scalar) const {
    return (radians / scalar) % Real(pi2);
  }
  
  inline Angle operator-() const {
    return (radians + Real(pi)) % Real(pi2);
  }
  
  inline Angle &operator+=(Angle other) {
    radians = (radians + other.radians) % Real(pi2);
    return *this;
  }
  
  inline Angle &operator-=(Angle other) {
    radians = (radians - other.radians) % Real(pi2);
    return *this;
  }
  
  inline Angle &operator*=(Real scalar) {
    radians = (radians * scalar) % Real(pi2);
    return *this;
  }
  
  inline Angle &operator/=(Real scalar) {
    radians = (radians / scalar) % Real(pi2);
    return *this;
  }
  
  inline bool operator==(Angle other) const {
    return radians == other.radians;
  }
  
  inline bool operator!=(Angle other) const {
    return radians != other.radians;
  }
};

static inline constexpr float operator""_deg(unsigned long long degrees) {
  return degrees * Angle::pi / 180;
}

static inline constexpr float operator""_deg(long double degrees) {
  return degrees * Angle::pi / 180;
}

NS_CITY_BUILDER_END
