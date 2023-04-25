/**
 * @file Path2.h
 * @brief An abstract path class and its descendants.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/List.h>
#include <CityBuilder/Storage/Ref.h>
#include "Bounds2.h"

NS_CITY_BUILDER_BEGIN

/// A 2-dimensional path.
struct Path2 {
  /// The start point of the path.
  const Real2 start;
  
  /// The end point of the path.
  const Real2 end;
  
  virtual ~Path2() { }
  
  virtual Real length() = 0;
  
  virtual Ref<Path2 &> offset(Real distance) = 0;
  
  virtual Ref<Path2 &> split(Real tStart, Real tEnd) = 0;
  
  virtual void split(Real t, Ref<Path2 &> &lhs, Ref<Path2 &> &rhs) = 0;
  
  /// Project a point onto the path.
  /// \param[in] point
  ///   The point to project.
  /// \returns
  ///   The projected point.
  virtual Real2 project(Real2 point) = 0;
  
  /// Get the point of the path at the given interpolation parameter.
  /// \param[in] t
  ///   The interpolation parameter.
  virtual Real2 point(Real t) = 0;
  
  /// Get the normal of the path at the given interpolation parameter.
  /// \param[in] t
  ///   The interpolation parameter.
  virtual Real2 normal(Real t) = 0;
  
  /// Convert a point on the path to an interpolation parameter.
  /// \param[in] point
  ///   The point to convert.
  virtual Real inverse(Real2 point) = 0;
  
  /// Find the intersections between this and another path.
  /// \param[in] other
  ///   The path to intersect with.
  /// \returns
  ///   A list of intersections.
  List<Real2> intersections(Path2 &other);
  
  /// Generate a list of equally-spaced points that define the path along with
  /// their normals.
  inline List<Real4> pointNormals() {
    return _getPointNormals();
  }
  
  /// Get the bounds of the path.
  inline Bounds2 bounds() {
    return _bounds;
  }
  
  /// A description of the path type.
  enum class Type {
    line,
    bezier
  };
  
  /// Get the path type.
  inline Type type() {
    return _type;
  }
  
protected:
  /// The computed bounds of the path.
  Bounds2 _bounds;
  /// A cache of the path points.
  List<Real4> _pointCache;
  
  /// The path type.
  Type _type;
  
  
  
  Path2(Type type) : _type(type) { }
  
  Path2(Type type, Real2 start, Real2 end)
    : _type(type), start(start), end(end) { }
  
  
  /// A generator for equidistant path points.
  virtual List<Real4> _pointNormals() = 0;
  
  /// Either get the path points from the cache or generate them.
  List<Real4> _getPointNormals();
};

/// A two-dimensional line.
struct Line2 : Path2 {
  Line2() : Path2(Type::line) { }
  
  Line2(Real2 start, Real2 end);
  
  Real length() override;
  
  Ref<Path2 &> offset(Real distance) override;
  
  Ref<Path2 &> split(Real tStart, Real tEnd) override;
  
  void split(Real t, Ref<Path2 &> &lhs, Ref<Path2 &> &rhs) override;
  
  Real2 project(Real2 point) override;
  
  Real2 point(Real t) override;
  
  Real2 normal(Real t) override;
  
  Real inverse(Real2 point) override;
  
protected:
  List<Real4> _pointNormals() override;
};

/// A two-dimensional cubic Bezier curve.
struct Bezier2 : Path2 {
  const Real2 control1, control2;
  
  Bezier2() : Path2(Type::bezier) { }
  
  Bezier2(Real2 start, Real2 control, Real2 end);
  
  Bezier2(Real2 start, Real2 control1, Real2 control2, Real2 end);
  
  Real length() override;
  
  /// Check if the Bezier curve is degenerate
  /// (all control points and end points are collinear).
  bool isDegenerate();
  
  Ref<Path2 &> offset(Real distance) override;
  
  Ref<Path2 &> split(Real tStart, Real tEnd) override;
  
  void split(Real t, Ref<Path2 &> &lhs, Ref<Path2 &> &rhs) override;
  
  Real2 project(Real2 point) override;
  
  Real2 point(Real t) override;
  
  Real2 normal(Real t) override;
  
  Real inverse(Real2 point) override;
  
protected:
  
  List<Real3> _lengths;
  
  Real _lengthLookup(Real t);
  
  List<Real4> _pointNormals() override;
};

NS_CITY_BUILDER_END
