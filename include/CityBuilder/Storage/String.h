/**
 * @file String.h
 * @brief A UTF-8 string type.
 * @date January 27, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <stddef.h> // size_t
#include <inttypes.h>
#include "List.h"

/// A UTF-8-encoded string.
struct String {
private:
  /// The data pointed to by a string.
  struct Data {
    /// The number of references to the string data.
    size_t references;
    /// The total capacity (in bytes) of the string data.
    size_t capacity;
    /// The total length (in bytes) of the string data.
    size_t bytes;
    /// The total length (in characters) of the string data.
    size_t length;
    /// Whether or not the contents of the string are completely ASCII.
    bool isAscii;
    
#ifdef TRACK_DOUBLE_FREE
    /// A tracker for if the string has already been freed.
    bool freed;
#endif
    
    /// The string data.
    unsigned char contents[0];
    
    /// Retain the string data.
    void retain();
    /// Release the string data.
    void release();
  };
  
  // The string's data.
  Data *_data;
  
  /// Make the string unique for copying.
  void _makeUnique();
  
  /// Expand a string to be able to hold more characters.
  /// \param[in] additional
  ///   The number of additional characters that the string should store.
  /// \remarks
  ///   Makes the string unique.
  void _expand(size_t additional);
  
public:
  /// Create an empty string.
  String();
  
  /// Create a string from a single ASCII character.
  String(wchar_t character);
  
  /// Create a string from a null-terminated C string.
  String(const char *string);
  
  /// Create a string from an integer.
  /// \param[in] integer
  ///   The integer to parse.
  String(int integer);
  
  /// Create a string from an integer.
  /// \param[in] integer
  ///   The integer to parse.
  String(size_t integer);
  
  
  
  String(const String &other);
  String &operator =(const String &other);
  
  String(String &&other);
  String &operator =(String &&other);
  
  ~String();
  
  
  
  /// The length of the string in Unicode codepoints.
  size_t length() const;
  
  /// Whether or not the string is empty.
  bool isEmpty() const;
  
  /// Access a character at the given index.
  /// \param[in] index
  ///   The index of the character.
  /// \returns
  ///   The requested character.
  /// \remarks
  ///   This is a `O(n)` operation due to encoding.
  ///   It is preferred to use an iterator if iterating over the string.
  wchar_t operator[](size_t index) const;
  
  /// A string iterator.
  struct Iterator {
  private:
    friend String;
    
    /// The current pointer within the string.
    const unsigned char *string;
    
    /// Create an iterator from a string's raw UTF-8 contents.
    Iterator(const unsigned char *string)
      : string(string) { }
    
  public:
    /// Access the current character.
    wchar_t operator*();
    
    /// Move to the next character.
    void operator++();
    /// Move to the next character.
    void operator++(int);
    
    /// Move to the previous character.
    void operator--();
    /// Move to the previous character.
    void operator--(int);
    
    /// Move the iterator forward by a given amount.
    /// \param[in] index
    ///   The amount to move.
    /// \returns
    ///   An iterator with the given offset from this iterator.
    String::Iterator operator+(size_t index);
    /// Move the iterator forward a given amount.
    /// \param[in] index
    ///   The amount to move.
    void operator+=(size_t index);
    
    /// Move the iterator backward by a given amount.
    /// \param[in] index
    ///   The amount to move.
    /// \returns
    ///   An iterator with the given offset from this iterator.
    String::Iterator operator-(size_t index);
    /// Move the iterator backward a given amount.
    /// \param[in] index
    ///   The amount to move.
    void operator-=(size_t index);
    
    /// Check if two iterators are not equal.
    bool operator!=(const Iterator &other);
  };
  
  Iterator begin() const;
  Iterator end() const;
  
  /// Append a character to the string.
  /// \param[in] character
  ///   The character to append.
  String &append(wchar_t character);
  
  /// Append a C string to the string.
  /// \param[in] string
  ///   The string to append.
  String &append(const char *string);
  
  /// Append a string to the string.
  /// \param[in] string
  ///   The string to append.
  String &append(const String &string);
  
  /// Insert a character in the string.
  /// \param[in] character
  ///   The character to insert.
  /// \param[in] index
  ///   The index at which to insert the character.
  String &insert(wchar_t character, size_t index);
  
  /// Insert a C string in the string.
  /// \param[in] string
  ///   The string to insert.
  /// \param[in] index
  ///   The index at which to insert the string.
  String &insert(const char *string, size_t index);
  
  /// Insert a string in the string.
  /// \param[in] string
  ///   The string to insert.
  /// \param[in] index
  ///   The index at which to insert the string.
  String &insert(const String &string, size_t index);
  
  /// Remove the character at the given index.
  /// \param[in] index
  ///   The index at which to remove the character.
  String &remove(size_t index);
  
  /// Remove the characters in the given range.
  /// \param[in] start
  ///   The lower bound of the range from which to start removing characters
  ///   (inclusive).
  /// \param[in] start
  ///   The upper bound of the range from which to start removing characters
  ///   (exclusive).
  String &remove(size_t start, size_t end);
  
  /// Remove all characters from the string.
  String &removeAll();
  
  /// Count the number of instances of a character in the string.
  /// \param[in] character
  ///   The character to count.
  /// \returns
  ///   The number of instances of the given character within the string.
  size_t count(wchar_t character) const;
  
  /// Split a string by a character.
  /// \param[in] character
  ///   The character to split by.
  /// \returns
  ///   A list of all the substring split by the given character.
  List<String> split(wchar_t character) const;
  
  /// Get a substring of this string.
  /// \param[in] start
  ///   The lower bound of the range from which to get the substring of this
  ///   string (inclusive).
  /// \param[in] start
  ///   The upper bound of the range from which to get the substring of this
  ///   string (exclusive).
  /// \returns
  ///   The requested substring.
  String substring(size_t start, size_t end) const;
  
  /// Truncate a string to a maximum width.
  /// \param[in] width
  ///   The maximum width (length) of the string.
  /// \returns
  ///   The string truncated to be at most the given length with ellipses
  ///   inserted as necessary.
  String truncate(size_t width) const;
  
  /// Wrap a string to a set width.
  /// \param[in] width
  ///   The maximum width of the wrapped string.
  /// \param[out] outWidth
  ///   The wrapped width of the string.
  /// \param[out] outHeight
  ///   The wrapped height of the string.
  /// \returns
  ///   The string appropriately wrapped.
  String wrap(
    size_t width, size_t *outWidth, size_t *outHeight
  ) const;
  
  /// Wrap a string to a set width and height.
  /// \param[in] width
  ///   The maximum width of the wrapped string.
  /// \param[in] height
  ///   The maximum height of the wrapped string.
  /// \param[out] outWidth
  ///   The wrapped width of the string.
  /// \param[out] outHeight
  ///   The wrapped height of the string.
  /// \returns
  ///   The string appropriately wrapped with ellipses inserted as necessary.
  String wrap(
    size_t width, size_t height, size_t *outWidth, size_t *outHeight
  ) const;
  
  
  
  /// Try to parse an unsigned 8-bit integer from the string.
  /// \param[out] integer
  ///   The parsed integer, if successful.
  /// \returns
  ///   Whether or not an integer could be parsed.
  bool tryParseUInt8(uint8_t *integer) const;
  
  /// Try to parse an unsigned 16-bit integer from the string.
  /// \param[out] integer
  ///   The parsed integer, if successful.
  /// \returns
  ///   Whether or not an integer could be parsed.
  bool tryParseUInt16(uint16_t *integer) const;
  
  /// Try to parse an unsigned 32-bit integer from the string.
  /// \param[out] integer
  ///   The parsed integer, if successful.
  /// \returns
  ///   Whether or not an integer could be parsed.
  bool tryParseUInt32(uint32_t *integer) const;
  
  /// Try to parse an unsigned 64-bit integer from the string.
  /// \param[out] integer
  ///   The parsed integer, if successful.
  /// \returns
  ///   Whether or not an integer could be parsed.
  bool tryParseUInt64(uint64_t *integer) const;
  
  /// Try to parse an unsigned size integer from the string.
  /// \param[out] integer
  ///   The parsed integer, if successful.
  /// \returns
  ///   Whether or not an integer could be parsed.
  inline bool tryParseSize(size_t *integer) const {
    // The unused branch should be pruned by the compiler
    if (sizeof(size_t) == 8)
      return tryParseUInt64((uint64_t *)integer);
    else
      return tryParseUInt32((uint32_t *)integer);
  }
  
  
  
  /// [Blocking]
  /// Read a line from the given stream.
  /// \param[in] stream
  ///   A `FILE *` stream.
  /// \returns
  ///   The read line.
  static String readLine(void *stream);
  
  /// Read the contents of a UTF-8-encoded file.
  /// \param[in] file
  ///   The path to the file to read from.
  /// \param[out] success
  ///   Whether or not the operation was successful.
  /// \returns
  ///   The file contents if successful or an empty string otherwise.
  static String readFile(const char *file, bool *success);
  
  /// Write the string's contents to a UTF-8-encoded file.
  /// \param[in] file
  ///   The path to the file to write to.
  /// \returns
  ///   Whether or not the operation was successful.
  bool writeToFile(const char *file);
  
  
  
  /// Concatenate this string with a character.
  String operator +(char character) const;
  
  /// Concatenate this string with a character.
  String operator +(wchar_t character) const;
  
  /// Concatenate this string with a C string.
  String operator +(const char *other) const;
  
  /// Concatenate this string with a string.
  String operator +(const String &other) const;
  
  /// Convert this string to a C string.
  explicit operator const char *() const;
  
  /// Check if this string is equal to another string.
  bool operator==(const char *other) const;
  
  /// Check if this string is equal to another string.
  bool operator==(const String &other) const;
  
  /// Check if this string is not equal to another string.
  bool operator!=(const char *other) const;
  
  /// Check if this string is not equal to another string.
  bool operator!=(const String &other) const;
  
  /// Check if this string lexicographically comes before another string.
  bool operator< (const char *other) const;
  
  /// Check if this string lexicographically comes before another string.
  bool operator< (const String &other) const;
  
  /// Check if this string lexicographically comes before or is equal to
  /// another string.
  bool operator<=(const char *other) const;
  
  /// Check if this string lexicographically comes before or is equal to
  /// another string.
  bool operator<=(const String &other) const;
  
  /// Check if this string lexicographically comes after another string.
  bool operator> (const char *other) const;
  
  /// Check if this string lexicographically comes after another string.
  bool operator> (const String &other) const;
  
  /// Check if this string lexicographically comes after or is equal to
  /// another string.
  bool operator>=(const char *other) const;
  
  /// Check if this string lexicographically comes after or is equal to
  /// another string.
  bool operator>=(const String &other) const;
};

/// Check if this string is equal to another string.
inline bool operator==(const char *lhs, const String &rhs) {
  return rhs == lhs;
}

/// Check if this string is not equal to another string.
inline bool operator!=(const char *lhs, const String &rhs) {
  return rhs != lhs;
}

/// Check if this string lexicographically comes before another string.
inline bool operator< (const char *lhs, const String &rhs) {
  return rhs > lhs;
}

/// Check if this string lexicographically comes before or is equal to
/// another string.
inline bool operator<=(const char *lhs, const String &rhs) {
  return rhs >= lhs;
}

/// Check if this string lexicographically comes after another string.
inline bool operator> (const char *lhs, const String &rhs) {
  return rhs < lhs;
}

/// Check if this string lexicographically comes after or is equal to
/// another string.
inline bool operator>=(const char *lhs, const String &rhs) {
  return rhs <= lhs;
}

/// Concatenate a character with a string.
inline String operator +(char lhs, const String &rhs) {
  return String(lhs) + rhs;
}

/// Concatenate a character with a string.
inline String operator +(wchar_t lhs, const String &rhs) {
  return String(lhs) + rhs;
}

/// Concatenate a string with a string.
inline String operator +(const char *lhs, const String &rhs) {
  return String(lhs) + rhs;
}
