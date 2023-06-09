/**
 * @file Markup.h
 * @brief A utility for parsing custom markup files.
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/String.h>
#include <CityBuilder/Storage/Map.h>
#include <CityBuilder/Geometry/Profile.h>

NS_CITY_BUILDER_BEGIN

template<typename T>
struct Markup;

/// Create a parser for a custom markup file.
/// \param[in] path
///   The path to the markup file.
/// \param[out] item
///   The item to parse.
/// \returns
///   A parser builder.
template<typename T>
Markup<T> &parseMarkup(const String &path, T &item);





/// A markup parser builder.
template<typename T>
struct Markup {
  /// A record parser builder.
  template<typename U>
  struct Record {
    /// Set a field in accordance with the record name.
    /// \param[in] member
    ///   The member to set.
    /// \param[in] values
    ///   A list of values to map the record name to.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .records({ "foo", "bar", "bat" }, myRecords)
    ///     .set(&MyRecord::type, {
    ///       MyRecord::Type::foo,
    ///       MyRecord::Type::bar,
    ///       MyRecord::Type::bat
    ///     })
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   foo 1 2 ...    # -> { .type = MyRecord::Type::foo, ... }
    ///   bar 1 2 ...    # -> { .type = MyRecord::Type::bar, ... }
    ///   bat 1 2 ...    # -> { .type = MyRecord::Type::bat, ... }
    ///   ```
    ///   Note that the number of `values` provided must match exactly the
    ///   number of record names there are.
    template<typename V>
    Record &set(V U::*member, const List<V> &values);
    
    /// Parse a (positive) integer value.
    /// \param[in] member
    ///   The integer member to set upon parsing.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .integer(&MyRecord::foo)
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   1   ...    # -> { .foo = 1, ... }
    ///   foo ...    # error
    ///   -1  ...    # error
    ///   ```
    Record &integer(int U::*member);
    
    /// Parse a (positive) real value.
    /// \param[in] member
    ///   The real member to set upon parsing.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .real(&MyRecord::foo)
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   1.5  ...    # -> { .foo = 1.5, ... }
    ///   foo  ...    # error
    ///   -1.5 ...    # error
    ///   ```
    Record &real(Real U::*member);
    
    /// Parse a (positive) point value.
    /// \param[in] member
    ///   The point member to set upon parsing.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .point(&MyRecord::foo)
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   1.5,3   ...    # -> { .foo = { 1.5, 3 }, ... }
    ///   foo     ...    # error
    ///   -1.5,-2 ...    # error
    ///   ```
    ///   If you want to be able to have negative values for the axes too,
    ///   use `vector` instead.
    Record &point(Real2 U::*member);
    
    /// Parse a vector value.
    /// \param[in] member
    ///   The vector member to set upon parsing.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .vector(&MyRecord::foo)
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   1.5,-3 ...    # -> { .foo = { 1.5, -3 }, ... }
    ///   foo    ...    # error
    ///   1.5 2  ...    # error
    ///   ```
    Record &vector(Real2 U::*member);
    
    /// Parse a specific identifier value.
    /// \param[in] value
    ///   The identifier value to parse.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .identifier("foo")
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   foo     ...    # OK
    ///   foo-bar ...    # error
    ///   2       ...    # error
    ///   ```
    Record &identifier(const String &value);
    
    /// Parse a comma.
    Record &comma();
    
    /// Setup a named option set.
    /// \param[in] name
    ///   The name of the option set.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .option("foo")
    ///     .integer(&MyRecord::foo)
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   foo 3   ...    # -> { .foo = 3, ... }
    ///   foo ,   ...    # error
    ///   ...            # -> { .foo = [untouched], ... }
    ///   ```
    Record &option(const String &name);
    
    /// Match a set of identifiers to a set of corresponding values.
    /// \param[in] member
    ///   The member to set with the matched value.
    /// \param[in] values
    ///   A set of identifiers and corresponding values.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .match(&MyRecord::type, {
    ///     { "foo", MyRecord::Type::foo },
    ///     { "bar", MyRecord::Type::bar },
    ///     { "bat", MyRecord::Type::bat }
    ///   })
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   foo ...    # -> { .type = MyRecord::Type::foo, ... }
    ///   bar ...    # -> { .type = MyRecord::Type::bar, ... }
    ///   bat ...    # -> { .type = MyRecord::Type::bat, ... }
    ///   ```
    template<typename V>
    Record &match(V U::*member, const Map<String, V> &values);
    
    /// Match a set of identifiers to a set of corresponding values.
    /// \param[in] member
    ///   The member to set with the matched value.
    /// \param[in] values
    ///   A set of identifiers and corresponding values.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .match(&MyRecord::type, {
    ///     { "foo", MyRecord::Type::foo },
    ///     { "bar", MyRecord::Type::bar },
    ///     { "bat", MyRecord::Type::bat }
    ///   })
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   foo ...    # -> { .type = MyRecord::Type::foo, ... }
    ///   bar ...    # -> { .type = MyRecord::Type::bar, ... }
    ///   bat ...    # -> { .type = MyRecord::Type::bat, ... }
    ///   ```
    template<typename V>
    Record &match(V *U::*member, const Map<String, V> &values);
    
    /// Match a set of strings to a set of corresponding values.
    /// \param[in] member
    ///   The member to set with the matched value.
    /// \param[in] values
    ///   A set of strings and corresponding values.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .matchString(&MyRecord::type, {
    ///     { "foo", MyRecord::Type::foo },
    ///     { "bar", MyRecord::Type::bar },
    ///     { "bat baz", MyRecord::Type::batBaz }
    ///   })
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   "foo"     ...    # -> { .type = MyRecord::Type::foo, ... }
    ///   "bar"     ...    # -> { .type = MyRecord::Type::bar, ... }
    ///   "bat baz" ...    # -> { .type = MyRecord::Type::batBaz, ... }
    ///   ```
    template<typename V>
    Record &matchString(V U::*member, const Map<String, V> &values);
    
    /// Match a set of strings to a set of corresponding values.
    /// \param[in] member
    ///   The member to set with the matched value.
    /// \param[in] values
    ///   A set of strings and corresponding values.
    /// \remarks
    ///   Usage:
    ///   ```
    ///   .matchString(&MyRecord::type, {
    ///     { "foo", MyRecord::Type::foo },
    ///     { "bar", MyRecord::Type::bar },
    ///     { "bat baz", MyRecord::Type::batBaz }
    ///   })
    ///   ```
    ///   When parsing a file with the above configuration:
    ///   ```
    ///   "foo"     ...    # -> { .type = MyRecord::Type::foo, ... }
    ///   "bar"     ...    # -> { .type = MyRecord::Type::bar, ... }
    ///   "bat baz" ...    # -> { .type = MyRecord::Type::batBaz, ... }
    ///   ```
    template<typename V>
    Record &matchString(V *U::*member, const Map<String, V> &values);
    
    /// End the current record.
    /// \remarks
    ///   Required after each record.
    Markup &end();
  
  
  
  
  
  private:
    /// The parent markup builder.
    Markup &markup;
    
    friend Markup;
    
    Record(Markup *markup) : markup(*markup) { }
  };
  
  
  
  
  
  /// Parse a named section.
  /// \param[in] name
  ///   The name of the section.
  /// \remarks
  ///   Usage:
  ///   ```
  ///   .section("foo")
  ///   ```
  ///   When parsing a file with the above configuration:
  ///   ```
  ///   [foo]
  ///   # everything under here will be matched
  ///   # with this section's configuration
  ///   ```
  Markup &section(const String &name);
  
  /// Parse a named field.
  /// \param[in] name
  ///   The name of the field.
  /// \param[in] value
  ///   The value to set upon parsing.
  /// \remarks
  ///   Usage:
  ///   ```
  ///   .section("foo")
  ///     .field("bar", myRecord.bar)
  ///   ```
  ///   When parsing a file with the above configuration:
  ///   ```
  ///   [foo]
  ///   bar "Hello, world!"    # -> { .bar = "Hello, world!", ... }
  ///   ```
  Markup &field(const String &name, String &value);
  
  /// Parse a named field.
  /// \param[in] name
  ///   The name of the field.
  /// \param[in] value
  ///   The value to set upon parsing.
  /// \param[in] values
  ///   A map of the accepted identifiers and their corresponding values.
  /// \remarks
  ///   Usage:
  ///   ```
  ///   .section("colors")
  ///     .field("primary", myRecord.primary, {
  ///       { "red",   Color::red   },
  ///       { "green", Color::green },
  ///       { "blue",  Color::blue  },
  ///     })
  ///   ```
  ///   When parsing a file with the above configuration:
  ///   ```
  ///   [colors]
  ///   primary red    # -> { .primary = Color::red, ... }
  ///   ```
  template<typename U>
  Markup &field(const String &name, U &value, const Map<String, U> &values);
  
  /// Parse a set of named records.
  /// \param[in] names
  ///   The possible names of the records.
  /// \param[in] values
  ///   The list of values to add to when parsing the given records.
  /// \remarks
  ///   Usage:
  ///   ```
  ///   .section("geometry")
  ///     .records({ "box", "sphere" }, myRecord.shapes)
  ///       .set(&MyRecord::Shape::type, {
  ///          MyRecord::Shape::Type::box,
  ///          MyRecord::Shape::Type::sphere
  ///       })
  ///       .real(&MyRecord::Shape::radius)
  ///   ```
  ///   When parsing a file with the above configuration:
  ///   ```
  ///   [geometry]
  ///   box 3         # -> shapes[0] = { .type = box, .radius = 3 }
  ///   sphere 5.5    # -> shapes[1] = { .type = sphere, .radius = 5.5 }
  ///   ```
  template<typename U>
  Record<U> &records(const List<String> &names, List<U> &values);
  
  /// Parse a set of profile points.
  /// \param[out] points
  ///   The list of points to add to when parsing.
  /// \remarks
  ///   Usage:
  ///   ```
  ///   .section("profile")
  ///     .profilePoints(myRecord.profile)
  ///   ```
  ///   When parsing a file with the above configuration:
  ///   ```
  ///   [profile]
  ///   M 0,0 uv 0 normal 0,1
  ///   D 3,1 uv 0 normal 0,1 normal 1,0
  ///   M 3,0 uv 0 normal 1,0
  ///   ```
  ///   Becomes:
  ///   ```
  ///   [0] = { .type = move, .position = { 0, 0 }, ... }
  ///   [1] = { .type = disjoint, .position = { 3, 1 }, ... }
  ///   [2] = { .type = move, .position = { 3, 0 }, ... }
  ///   ```
  Markup &profilePoints(List<ProfilePoint> &points);
  
  /// Parse the given markup file into the given item.
  /// \returns
  ///   Whether or not the parsing was successful.
  /// \remarks
  ///   Error messages will be written to `stdout`.
  operator bool();
  
  
  
  
  
private:
  /// The item to parse into.
  T &_item;
  /// The path to the markup file to parse.
  String _path;
  
  Markup(const String &path, T &item) : _path(path), _item(item) { }
   
  friend Markup &::CityBuilder::parseMarkup(const String &path, T &item);
  
  struct _section;
  
  /// The defined sections for parsing.
  List<_section> _sections;
};

NS_CITY_BUILDER_END

#include "Markup.ipp"
