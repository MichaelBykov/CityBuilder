/**
 * @file Path2.h
 * @brief An abstract path class and its descendants.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/List.h>

NS_CITY_BUILDER_BEGIN

/// A 2-dimensional path.
struct Path2 {
  virtual ~Path2() { }
  
  virtual Real length() = 0;
  
  /// Generate a list of equally-spaced points that define the path.
  inline List<Real2> points() {
    return _getPoints();
  }
  
protected:
  /// The computed bounds of the path.
  Real4 _bounds;
  /// A cache of the path points.
  List<Real2> _pointCache;
  
  /// A generator for equidistant path points.
  virtual List<Real2> _points() = 0;
  
  /// Either get the path points from the cache or generate them.
  List<Real2> _getPoints();
};

/// A two-dimensional line.
struct Line2 : Path2 {
  /// An endpoint of the line.
  const Real2 a, b;
  
  Line2() { }
  
  Line2(const Real2 &a, const Real2 &b);
  
  Real length() override;
  
protected:
  List<Real2> _points() override;
};

/// A two-dimensional cubic Bezier curve.
struct Cubic2 : Path2 {
  /// A control point of the curve.
  const Real2 start, control0, control1, end;
  
  Cubic2() { }
  
  Cubic2(
    const Real2 &start, const Real2 &control0,
    const Real2 &control1, const Real2 &end
  );
  
  /// Interpolate along the cubic Bezier curve.
  /// \param t
  ///   The interpolation parameter.
  /// \returns
  ///   The interpolated point.
  Real2 interpolate(Real t) const;
  
  Real length() override;
  
protected:
  Real _length;
  
  List<Real2> _points() override;
};

NS_CITY_BUILDER_END
