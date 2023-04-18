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
  /// The start point of the path.
  const Real2 start;
  
  /// The end point of the path.
  const Real2 end;
  
  virtual ~Path2() { }
  
  virtual Real length() = 0;
  
  virtual Path2 *offset(Real distance) = 0;
  
  /// Project a point onto the path.
  /// \param[in] point
  ///   The point to project.
  /// \return
  ///   The projected point.
  virtual Real2 project(Real2 point) = 0;
  
  /// Generate a list of equally-spaced points that define the path along with
  /// their normals.
  inline List<Real4> pointNormals() {
    return _getPointNormals();
  }
  
protected:
  Path2() { }
  
  Path2(Real2 start, Real2 end) : start(start), end(end) { }
  
  /// The computed bounds of the path.
  Real4 _bounds;
  /// A cache of the path points.
  List<Real4> _pointCache;
  
  /// A generator for equidistant path points.
  virtual List<Real4> _pointNormals() = 0;
  
  /// Either get the path points from the cache or generate them.
  List<Real4> _getPointNormals();
};

/// A two-dimensional line.
struct Line2 : Path2 {
  Line2() { }
  
  Line2(const Real2 &start, const Real2 &end);
  
  Real length() override;
  
  Path2 *offset(Real distance) override;
  
  Real2 project(Real2 point) override;
  
protected:
  List<Real4> _pointNormals() override;
};

/// A two-dimensional arc curve.
struct Arc2 : Path2 {
  /// A control point of the curve.
  const Real2 control;
  
  Arc2() { }
  
  Arc2(const Real2 &start, const Real2 &control, const Real2 &end);
  
  Real length() override;
  
  Path2 *offset(Real distance) override;
  
  Real2 project(Real2 point) override;
  
protected:
  Real2 _center;
  Real _length;
  Real _radius;
  
  List<Real4> _pointNormals() override;
};

NS_CITY_BUILDER_END
