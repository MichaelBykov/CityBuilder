/**
 * @file String.cpp
 * @brief Implement a UTF-8 string data type.
 * @date January 27, 2023
 * @copyright Copyright (c) 2023
 */

#define _CRT_SECURE_NO_WARNINGS
#include <CityBuilder/Storage/String.h>
#include <CityBuilder/Storage/Exceptions.h>
#include <CityBuilder/Storage/Check.h>
#include <stdio.h> // fgetc
#include <stdlib.h> // malloc, realloc, free
#include <string.h> // memcpy, strlen

void String::Data::retain() {
  references++;
}

void String::Data::release() {
  if (--references == 0) {
#ifdef TRACK_DOUBLE_FREE
    if (freed)
      throw DoubleFree();
#endif
    
#ifdef TRACK_MANAGED_OBJECT_TYPES
    removeManagedObject(typeid(String));
#elif defined(TRACK_MANAGED_OBJECTS)
    removeManagedObject();
#endif
    
#ifdef TRACK_DOUBLE_FREE
    freed = true;
    addZombieObject();
#else
    free(this);
#endif
  }
}

void String::_makeUnique() {
  if (_data != nullptr && _data->references > 1) {
    Data *data = (Data *)malloc(sizeof(Data) + _data->capacity);
    data->references = 1;
    data->capacity = _data->capacity;
    data->bytes = _data->bytes;
    data->length = _data->length;
    data->isAscii = _data->isAscii;
#ifdef TRACK_DOUBLE_FREE
    data->freed = false;
#endif
    memcpy(data->contents, _data->contents, _data->bytes + 1);
#ifdef TRACK_MANAGED_OBJECT_TYPES
    trackManagedObject(typeid(String));
#elif defined(TRACK_MANAGED_OBJECTS)
    trackManagedObject();
#endif
    
    _data->release();
    _data = data;
  }
}

void String::_expand(size_t additional) {
  if (_data == nullptr) {
    size_t capacity = 8;
    while (capacity < additional + 1)
      capacity *= 2;
    
    _data = (Data *)malloc(sizeof(Data) + capacity);
    _data->references = 1;
    _data->capacity = capacity;
    _data->bytes = 0;
    _data->length = 0;
#ifdef TRACK_DOUBLE_FREE
    _data->freed = false;
#endif
    _data->isAscii = true;
#ifdef TRACK_MANAGED_OBJECT_TYPES
    trackManagedObject(typeid(String));
#elif defined(TRACK_MANAGED_OBJECTS)
    trackManagedObject();
#endif
  } else if (_data->capacity < _data->bytes + additional + 1) {
    size_t requiredCapacity =  _data->bytes + additional + 1;
    if (_data->references > 1) {
      // Make unique and expand the storage
      size_t capacity = _data->capacity;
      while (capacity < requiredCapacity)
        capacity *= 2;
      
      Data *data = (Data *)malloc(sizeof(Data) + capacity);
      data->references = 1;
      data->capacity = capacity;
      data->bytes = _data->bytes;
      data->length = _data->length;
      data->isAscii = _data->isAscii;
#ifdef TRACK_DOUBLE_FREE
      data->freed = false;
#endif
      memcpy(data->contents, _data->contents, _data->bytes + 1);
#ifdef TRACK_MANAGED_OBJECT_TYPES
      trackManagedObject(typeid(String));
#elif defined(TRACK_MANAGED_OBJECTS)
      trackManagedObject();
#endif
      
      _data->release();
      _data = data;
    } else {
      // Already unique: just expand the storage
      while (_data->capacity < requiredCapacity)
        _data->capacity *= 2;
      _data = (Data *)realloc(_data, sizeof(Data) + _data->capacity);
    }
  } else {
    _makeUnique();
  }
}





String::String() : _data(nullptr) { }

String::String(wchar_t character) {
  _data = (Data *)malloc(sizeof(String::Data) + 8);
  _data->references = 1;
  _data->capacity = 8;
  _data->length = 1;
#ifdef TRACK_DOUBLE_FREE
  _data->freed = false;
#endif
#ifdef TRACK_MANAGED_OBJECT_TYPES
  trackManagedObject(typeid(String));
#elif defined(TRACK_MANAGED_OBJECTS)
  trackManagedObject();
#endif
  
  if (character <= 0x7F) {
    _data->bytes = 1;
    _data->isAscii = true;
    _data->contents[0] = character;
    _data->contents[1] = 0;
  } else if (character <= 0x7FF) {
    _data->bytes = 2;
    _data->isAscii = false;
    _data->contents[0] = 0b110 << 5 | character >> 6;
    _data->contents[1] = 0x80 | (character & 0b111111);
    _data->contents[2] = 0;
  } else if (character <= 0xFFFF) {
    _data->bytes = 3;
    _data->isAscii = false;
    _data->contents[0] = 0b1110 << 4 | character >> 12;
    _data->contents[1] = 0x80 | (character >> 6 & 0b111111);
    _data->contents[2] = 0x80 | (character      & 0b111111);
    _data->contents[3] = 0;
  } else if (character <= 0x10FFFF) {
    _data->bytes = 4;
    _data->isAscii = false;
    _data->contents[0] = 0b11110 << 3 | character >> 18;
    _data->contents[1] = 0x80 | (character >> 12 & 0b111111);
    _data->contents[2] = 0x80 | (character >> 6  & 0b111111);
    _data->contents[3] = 0x80 | (character       & 0b111111);
    _data->contents[4] = 0;
  } else {
    // Invalid character '�'
    _data->bytes = 3;
    _data->isAscii = false;
    _data->contents[0] = 0xEF;
    _data->contents[1] = 0xBF;
    _data->contents[2] = 0xBD;
    _data->contents[3] = 0;
  }
}

String::String(const char *string) {
  if (*string == 0) {
    // Empty string
    _data = nullptr;
    return;
  }
  
  size_t length = strlen(string);
  size_t capacity = 8;
  while (capacity < length + 1)
    capacity *= 2;
  
  _data = (Data *)malloc(sizeof(String::Data) + capacity);
  _data->references = 1;
  _data->capacity = capacity;
  _data->bytes = length;
  _data->length = length;
  _data->isAscii = true;
#ifdef TRACK_DOUBLE_FREE
  _data->freed = false;
#endif
  memcpy(_data->contents, string, length + 1);
#ifdef TRACK_MANAGED_OBJECT_TYPES
  trackManagedObject(typeid(String));
#elif defined(TRACK_MANAGED_OBJECTS)
  trackManagedObject();
#endif
}

String::String(int integer) : _data(nullptr) {
  if (integer < 0) {
    append('-');
    integer = -integer;
  } else if (integer == 0) {
    append('0');
    return;
  }
  int digits[10];
  int count = 0;
  while (integer > 0) {
    int digit = integer % 10;
    integer /= 10;
    digits[count++] = digit;
  }
  for (int i = count - 1; i >= 0; i--)
    append('0' + digits[i]);
}

String::String(size_t integer) : _data(nullptr) {
  if (integer == 0) {
    append('0');
    return;
  }
  int digits[20];
  int count = 0;
  while (integer > 0) {
    int digit = integer % 10;
    integer /= 10;
    digits[count++] = digit;
  }
  for (int i = count - 1; i >= 0; i--)
    append('0' + digits[i]);
}

String::String(const String &other) : _data(other._data) {
  if (_data != nullptr)
    _data->retain();
}

String &String::operator =(const String &other) {
  if (_data != nullptr)
    _data->release();
  
  _data = other._data;
  if (_data != nullptr)
    _data->retain();
  
  return *this;
}

String::String(String &&other) : _data(other._data) {
  other._data = nullptr;
}

String &String::operator =(String &&other) {
  if (_data != nullptr)
    _data->release();
  
  _data = other._data;
  other._data = nullptr;
  
  return *this;
}

String::~String() {
  if (_data != nullptr) {
    _data->release();
    _data = nullptr;
  }
}





size_t String::length() const {
  if (_data == nullptr)
    return 0;
  return _data->length;
}

bool String::isEmpty() const {
  return _data == nullptr;
}

wchar_t String::operator[](size_t index) const {
  if (_data == nullptr || index >= _data->length)
    throw IndexOutOfBounds();
  if (_data->isAscii)
    // No encoding problems
    return _data->contents[index];
  size_t j = 0;
  for (size_t i = 0; i < index; i++) {
    if (_data->contents[j] >> 7 == 0)
      j++;
    else if (_data->contents[j] >> 5 == 0b110)
      j += 2;
    else if (_data->contents[j] >> 4 == 0b1110)
      j += 3;
    else if (_data->contents[j] >> 3 == 0b11110)
      j += 4;
    
    if (j > _data->bytes)
      throw IndexOutOfBounds();
  }
  
  if (_data->contents[j] >> 7 == 0) {
    return _data->contents[j];
  } else if (_data->contents[j] >> 5 == 0b110) {
    if (j + 1 > _data->bytes)
      throw IndexOutOfBounds();
    
    return
      ((wchar_t)_data->contents[j] & 0b11111) << 6 |
      ((wchar_t)_data->contents[j + 1] & 0b111111);
  } else if (_data->contents[j] >> 4 == 0b1110) {
    if (j + 2 > _data->bytes)
      throw IndexOutOfBounds();
    
    return
      ((wchar_t)_data->contents[j] & 0b1111) << 12 |
      ((wchar_t)_data->contents[j + 1] & 0b111111) << 6 |
      ((wchar_t)_data->contents[j + 2] & 0b111111);
  } else if (_data->contents[j] >> 3 == 0b11110) {
    if (j + 3 > _data->bytes)
      throw IndexOutOfBounds();
    
    return
      ((wchar_t)_data->contents[j] & 0b111) << 18 |
      ((wchar_t)_data->contents[j + 1] & 0b111111) << 12 |
      ((wchar_t)_data->contents[j + 2] & 0b111111) << 6 |
      ((wchar_t)_data->contents[j + 3] & 0b111111);
  } else
    return 0xFFFD; // '�'
}

wchar_t String::Iterator::operator*() {
  if (*string >> 7 == 0)
    return string[0];
  else if (*string >> 5 == 0b110)
    return
      ((wchar_t)string[0] & 0b11111 ) << 6 |
      ((wchar_t)string[1] & 0b111111)      ;
  else if (*string >> 4 == 0b1110)
    return
      ((wchar_t)string[0] & 0b1111  ) << 12 |
      ((wchar_t)string[1] & 0b111111) << 6  |
      ((wchar_t)string[2] & 0b111111)       ;
  else if (*string >> 3 == 0b11110)
    return
      ((wchar_t)string[0] & 0b111   ) << 18 |
      ((wchar_t)string[1] & 0b111111) << 12 |
      ((wchar_t)string[2] & 0b111111) << 6  |
      ((wchar_t)string[3] & 0b111111)       ;
  else
    throw InvalidIterator();
}

bool String::Iterator::operator!=(const Iterator &other) {
  return string != other.string;
}

void String::Iterator::operator++() {
  if (*string >> 7 == 0)
    string++;
  else if (*string >> 5 == 0b110)
    string += 2;
  else if (*string >> 4 == 0b1110)
    string += 3;
  else if (*string >> 3 == 0b11110)
    string += 4;
  else
    throw InvalidIterator();
}

void String::Iterator::operator++(int) {
  if (*string >> 7 == 0)
    string++;
  else if (*string >> 5 == 0b110)
    string += 2;
  else if (*string >> 4 == 0b1110)
    string += 3;
  else if (*string >> 3 == 0b11110)
    string += 4;
  else
    throw InvalidIterator();
}

void String::Iterator::operator--() {
  while (*string >> 6 == 0b10)
    string--;
}

void String::Iterator::operator--(int) {
  while (*string >> 6 == 0b10)
    string--;
}

String::Iterator String::Iterator::operator+(size_t index) {
  String::Iterator iter = *this;
  for (size_t i = 0; i < index; i++)
    iter++;
  return iter;
}

void String::Iterator::operator+=(size_t index) {
  for (size_t i = 0; i < index; i++)
    (*this)++;
}

String::Iterator String::Iterator::operator-(size_t index) {
  String::Iterator iter = *this;
  for (size_t i = 0; i < index; i++)
    iter--;
  return iter;
}

void String::Iterator::operator-=(size_t index) {
  for (size_t i = 0; i < index; i++)
    (*this)--;
}

String::Iterator String::begin() const {
  if (_data == nullptr)
    return nullptr;
  return _data->contents;
}

String::Iterator String::end() const {
  if (_data == nullptr)
    return nullptr;
  return _data->contents + _data->bytes;
}

String &String::append(wchar_t character) {
  if (character <= 0x7F) {
    _expand(1);
    _data->contents[_data->bytes++] = character;
  } else if (character <= 0x7FF) {
    _expand(2);
    _data->isAscii = false;
    _data->contents[_data->bytes++] = 0b110 << 5 | character >> 6;
    _data->contents[_data->bytes++] = 0x80 | (character & 0b111111);
  } else if (character <= 0xFFFF) {
    _expand(3);
    _data->isAscii = false;
    _data->contents[_data->bytes++] = 0b1110 << 4 | character >> 12;
    _data->contents[_data->bytes++] = 0x80 | (character >> 6 & 0b111111);
    _data->contents[_data->bytes++] = 0x80 | (character      & 0b111111);
  } else if (character <= 0x10FFFF) {
    _expand(4);
    _data->isAscii = false;
    _data->contents[_data->bytes++] = 0b11110 << 3 | character >> 18;
    _data->contents[_data->bytes++] = 0x80 | (character >> 12 & 0b111111);
    _data->contents[_data->bytes++] = 0x80 | (character >> 6  & 0b111111);
    _data->contents[_data->bytes++] = 0x80 | (character       & 0b111111);
  } else {
    // Invalid character '�'
    _expand(3);
    _data->isAscii = false;
    _data->contents[_data->bytes++] = 0xEF;
    _data->contents[_data->bytes++] = 0xBF;
    _data->contents[_data->bytes++] = 0xBD;
  }
  _data->contents[_data->bytes] = 0;
  _data->length++;
  return *this;
}

String &String::append(const char *string) {
  size_t length = strlen(string);
  _expand(length);
  memcpy(_data->contents + _data->bytes, string, length + 1);
  _data->bytes  += length;
  _data->length += length;
  return *this;
}

String &String::append(const String &string) {
  if (string._data == nullptr)
    return *this;
  _expand(string._data->bytes);
  memcpy(
    _data->contents + _data->bytes,
    string._data->contents,
    string._data->bytes + 1
  );
  _data->bytes  += string._data->bytes ;
  _data->length += string._data->length;
  _data->isAscii &= string._data->isAscii;
  return *this;
}

String &String::insert(wchar_t character, size_t index) {
  if (index == length())
    return append(character);
  
  if (_data == nullptr || _data->length <= index)
    throw IndexOutOfBounds();
  _makeUnique();
  
  char bytes[5];
  int byte = 0;
  bool ascii;
  if (character <= 0x7F) {
    ascii = true;
    bytes[byte++] = character;
  } else if (character <= 0x7FF) {
    ascii = false;
    bytes[byte++] = 0b110 << 5 | character >> 6;
    bytes[byte++] = 0x80 | (character & 0b111111);
  } else if (character <= 0xFFFF) {
    ascii = false;
    bytes[byte++] = 0b1110 << 4 | character >> 12;
    bytes[byte++] = 0x80 | (character >> 6 & 0b111111);
    bytes[byte++] = 0x80 | (character      & 0b111111);
  } else if (character <= 0x10FFFF) {
    ascii = false;
    bytes[byte++] = 0b11110 << 3 | character >> 18;
    bytes[byte++] = 0x80 | (character >> 12 & 0b111111);
    bytes[byte++] = 0x80 | (character >> 6  & 0b111111);
    bytes[byte++] = 0x80 | (character       & 0b111111);
  } else {
    // Invalid character '�'
    ascii = false;
    bytes[byte++] = 0xEF;
    bytes[byte++] = 0xBF;
    bytes[byte++] = 0xBD;
  }
  bytes[byte] = 0;
  
  String &string = insert(bytes, index);
  string._data->isAscii &= ascii;
  return string;
}

String &String::insert(const char *string, size_t index) {
  if (index == length())
    return append(string);
  
  if (_data == nullptr || _data->length <= index)
    throw IndexOutOfBounds();
  else if (*string == 0)
    // Empty string
    return *this;
  size_t length = strlen(string);
  _expand(length);
  
  // Make space for the string
  Iterator _begin = begin() + index;
  for (
    unsigned char *ptr = (unsigned char *)end().string;
    ptr >= _begin.string;
    ptr--
  ) *(ptr + length) = *ptr;
  
  // Copy over the string
  for (size_t i = 0; i < length; i++)
    ((unsigned char *)_begin.string)[i] = string[i];
  
  _data->bytes += length;
  _data->length += length;
  
  return *this;
}

String &String::insert(const String &string, size_t index) {
  if (index == length())
    return append(string);
  
  if (_data == nullptr || _data->length <= index)
    throw IndexOutOfBounds();
  else if (string.length() == 0)
    // Empty string
    return *this;
  size_t length = string._data->bytes;
  _expand(length);
  
  // Make space for the string
  Iterator _begin = begin() + index;
  for (
    unsigned char *ptr = (unsigned char *)end().string;
    ptr >= _begin.string;
    ptr--
  ) *(ptr + length) = *ptr;
  
  // Copy over the string
  for (size_t i = 0; i < length; i++)
    ((unsigned char *)_begin.string)[i] = string._data->contents[i];
  
  _data->isAscii &= string._data->isAscii;
  _data->bytes += length;
  _data->length += string.length();
  
  return *this;
}

String &String::remove(size_t index) {
  if (_data == nullptr || _data->length <= index)
    throw IndexOutOfBounds();
  else if (_data->length == 1) {
    // Make the string empty
    _data->release();
    _data = nullptr;
    return *this;
  }
  _makeUnique();
  
  Iterator _begin  = begin() + index;
  Iterator _ending = _begin + 1;
  
  unsigned char *pStart = (unsigned char *)_begin .string;
  unsigned char *pEnd   = (unsigned char *)_ending.string;
  
  // Copy over the bytes
  size_t bytes = pEnd - pStart;
  while (*pEnd != 0)
    *(pStart++) = *(pEnd++);
  *(pStart++) = *(pEnd++);
  
  _data->bytes -= bytes;
  _data->length--;
  
  return *this;
}

String &String::remove(size_t _start, size_t _end) {
  if (_start > _end)
    throw UnorderedRange();
  else if (_start == _end && _end < _data->length)
    return *this;
  else if (_data == nullptr || _data->length < _end || _start == _end)
    throw IndexOutOfBounds();
  else if (_start == 0 && _end == _data->length) {
    // Make the string empty
    _data->release();
    _data = nullptr;
    return *this;
  }
  _makeUnique();
  
  Iterator _begin  = begin() + _start;
  Iterator _ending = _begin + (_end - _start);
  
  unsigned char *pStart = (unsigned char *)_begin .string;
  unsigned char *pEnd   = (unsigned char *)_ending.string;
  
  // Copy over the bytes
  size_t bytes = pEnd - pStart;
  while (*pEnd != 0)
    *(pStart++) = *(pEnd++);
  *(pStart++) = *(pEnd++);
  
  _data->bytes -= bytes;
  _data->length -= _end - _start;
  
  return *this;
}

String &String::removeAll() {
  if (_data != nullptr) {
    _data->release();
    _data = nullptr;
  }
  return *this;
}

size_t String::count(wchar_t character) const {
  size_t count = 0;
  for (wchar_t c : *this)
    if (c == character)
      count++;
  return count;
}

List<String> String::split(wchar_t character) const {
  List<String> split { "" };
  for (wchar_t c : *this)
    if (c == character)
      split.append("");
    else
      split[split.count() - 1].append(c);
  return split;
}

String String::substring(size_t _start, size_t _end) const {
  if (_start > _end)
    throw UnorderedRange();
  else if (_start == _end &&
    (_data == nullptr && _end == 0 || _end < _data->length))
    return { };
  if (_data == nullptr || _data->length < _end || _start == _end)
    throw IndexOutOfBounds();
  
  Iterator iter = begin();
  for (size_t i = 0; i < _start && iter != end(); i++, ++iter);
  String sub { };
  for (size_t i = _start; i < _end && iter != end(); i++, ++iter)
    sub.append(*iter);
  return sub;
}

String String::truncate(size_t width) const {
  String truncated { };
  String word { };
  size_t index = 0;
  for (wchar_t c : *this) {
    if (c == ' ') {
      size_t remaining = length() - index;
      if (remaining > 4)
        remaining = 4;
      if (truncated.length() + word.length() + remaining <= width)
        truncated.append(word).append(' ');
      else
        return truncated.append("...");
      word = { };
    } else
      word.append(c);
    index++;
  }
  
  if (truncated.length() + word.length() <= width)
    truncated.append(word);
  else if (truncated.isEmpty())
    truncated.append(word.substring(0, width - 3)).append("...");
  else
    truncated.append("...");
  
  return truncated;
}

String String::wrap(
  size_t width, size_t *outWidth, size_t *outHeight
) const {
  String wrapped { };
  size_t lineLength = 0;
  size_t line = 0;
  size_t maxWidth = 0;
  String word { };
  size_t index = 0;
  for (wchar_t c : *this) {
    if (c == ' ' || c == '\n') {
    wrap_final_word:
      if (word.isEmpty()) {
        if (c == ' ') {
          index++;
          continue;
        } else { // (c == '\n')
          wrapped.append('\n');
          lineLength = 0;
          line++;
          if (lineLength > maxWidth)
            maxWidth = lineLength;
        }
      }
      if (lineLength + 1 + word.length() <= width) {
        // Add to the line
        if (lineLength > 0) {
          wrapped.append(' ');
          lineLength++;
        }
        wrapped.append(word);
        lineLength += word.length();
      } else if (word.length() + 1 >= width) {
        // Split with the next line(s)
        size_t index = 0;
        if (lineLength > 0)
          lineLength++; // for the ' '
        if (width - lineLength >= 5) {
          index = width - 1 - lineLength;
          if (word.length() - index < 5)
            index = word.length() - 5;
          if (lineLength > 0)
            wrapped.append(' ');
          wrapped.append(word.substring(0, index) + "-\n");
          lineLength += index + 1;
        }
        line++;
        if (lineLength > maxWidth)
          maxWidth = lineLength;
        
        while (word.length() - index > width) {
          size_t start = index;
          index += width - 1;
          if (word.length() - index < 5)
            index = word.length() - 5;
          wrapped.append(word.substring(start, index) + "-\n");
          line++;
          if (index - start + 1 > maxWidth)
            maxWidth = index - start + 1;
        }
        
        wrapped.append(word.substring(index, word.length()));
        lineLength = word.length() - index;
      } else {
        // Move the word to the next line
        line++;
        if (lineLength > maxWidth)
          maxWidth = lineLength;
        wrapped.append('\n').append(word);
        lineLength = word.length();
      }
      word = { };
      if (c == '\n') {
        line++;
        if (lineLength > maxWidth)
          maxWidth = lineLength;
        wrapped.append('\n');
        lineLength = 0;
      }
    } else
      word.append(c);
    if (++index == length())
      goto wrap_final_word;
  }
  if (lineLength > maxWidth)
    maxWidth = lineLength;
  *outWidth = maxWidth;
  *outHeight = line + 1;
  return wrapped;
}

String String::wrap(
  size_t width, size_t height, size_t *outWidth, size_t *outHeight
) const {
  String wrapped { };
  size_t lineLength = 0;
  size_t line = 0;
  size_t maxWidth = 0;
  String word { };
  size_t index = 0;
  bool exited = false;
  for (wchar_t c : *this) {
    if (c == ' ' || c == '\n') {
    wrap_final_word_2:
      if (word.isEmpty()) {
        if (c == ' ')
          continue;
        else { // (c == '\n')
          if (lineLength > maxWidth)
            maxWidth = lineLength;
          if (++line == height) {
            exited = true;
            break;
          }
          wrapped.append('\n');
          lineLength = 0;
        }
      }
      if (lineLength + (lineLength == 0 ? 0 : 1) + word.length() <= width) {
        // Add to the line
        if (lineLength > 0) {
          wrapped.append(' ');
          lineLength++;
        }
        wrapped.append(word);
        lineLength += word.length();
      } else if (word.length() + 1 >= width) {
        // Split with the next line(s)
        size_t index = 0;
        if (lineLength > 0)
          lineLength++; // for the ' '
        if (width - lineLength >= 5) {
          index = width - 1 - lineLength;
          if (word.length() - index < 5)
            index = word.length() - 5;
          if (lineLength > 0)
            wrapped.append(' ');
          wrapped.append(word.substring(0, index) + '-');
          lineLength += index + 1;
        }
        if (lineLength > maxWidth)
          maxWidth = lineLength;
        if (++line == height) {
          exited = true;
          lineLength = lineLength;
          break;
        } else
          wrapped.append('\n');
        
        while (word.length() - index > width) {
          size_t start = index;
          index += width - 1;
          if (word.length() - index < 5)
            index = word.length() - 5;
          wrapped.append(word.substring(start, index) + '-');
          if (index - start + 1 > maxWidth)
            maxWidth = index - start + 1;
          if (++line == height) {
            exited = true;
            lineLength = index - start + 1;
            break;
          } else
            wrapped.append('\n');
        }
        if (line == height)
          break;
        
        wrapped.append(word.substring(index, word.length()));
        lineLength = word.length() - index;
      } else {
        // Move the word to the next line
        if (lineLength > maxWidth)
          maxWidth = lineLength;
        if (++line == height) {
          exited = true;
          break;
        }
        wrapped.append('\n').append(word);
        lineLength = word.length();
      }
      word = { };
      if (c == '\n') {
        if (lineLength > maxWidth)
          maxWidth = lineLength;
        if (++line == height) {
          exited = true;
          break;
        }
        wrapped.append('\n');
        lineLength = 0;
      }
    } else
      word.append(c);
    if (++index == length())
      goto wrap_final_word_2;
  }
  if (lineLength > maxWidth)
    maxWidth = lineLength;
  if (exited) {
    // Insert ellipses
    if (lineLength + 4 < width)
      wrapped.append(" ...");
    else
      wrapped.remove(wrapped.length() - 3, wrapped.length()).append("...");
  }
  *outWidth = maxWidth;
  *outHeight = exited ? height : line + 1;
  return wrapped;
}





namespace {
  inline bool notDigit(wchar_t c) {
    return c < '0' || '9' < c;
  }
  
  template<typename T, int maxDigits, const char *max>
  bool convertToInt(const String &string, T *integer) {
    // Check that the string is entirely digits
    for (wchar_t c : string)
      if (notDigit(c))
        return false;
    
    // Check that it is within range
    if (string.length() > maxDigits)
      return false;
    else if (string.length() == maxDigits) {
      // Check that the number doesn't go over the maximum that can be stored
      for (int i = 0; i < 3; i++)
        if (string[i] > max[i])
          return false;
        else if (string[i] < max[i])
          break;
    }
    
    // Convert to int
    *integer = 0;
    for (wchar_t c : string) {
      *integer *= 10;
      *integer += c - '0';
    }
    
    return true;
  }
  
  const char uint8Max[] = "255";
  const char uint16Max[] = "65535";
  const char uint32Max[] = "4294967295";
  const char uint64Max[] = "18446744073709551615";
}

bool String::tryParseUInt8(uint8_t *integer) const {
  return convertToInt<uint8_t, 3, uint8Max>(*this, integer);
}

bool String::tryParseUInt16(uint16_t *integer) const {
  return convertToInt<uint16_t, 5, uint16Max>(*this, integer);
}

bool String::tryParseUInt32(uint32_t *integer) const {
  return convertToInt<uint32_t, 10, uint32Max>(*this, integer);
}

bool String::tryParseUInt64(uint64_t *integer) const {
  return convertToInt<uint64_t, 20, uint64Max>(*this, integer);
}





String String::readLine(void *stream) {
  int character;
  String line { };
  while (true) {
    character = fgetc((FILE *)stream);
    if (character == '\n' || character == EOF)
      return line;
    else
      line.append(character);
  }
}

bool String::writeToFile(const char *fileName) {
  FILE *file = fopen(fileName, "w");
  if (file == NULL)
    return false;
  if (_data != nullptr)
    fwrite(_data->contents, 1, _data->bytes, file);
  fclose(file);
  return true;
}

String String::readFile(const char *fileName, bool *success) {
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
    *success = false;
    return { };
  }
  
  String result { };
  int character;
  while ((character = fgetc(file)) != EOF) {
    if (character >> 7 == 0)
      result.append(character);
    else if (character >> 5 == 0b110) {
      int a = fgetc(file);
      if (a == EOF || a >> 6 != 0b10){
        *success = false;
        return { };
      }
      
      result.append((wchar_t)(a & 0b111111) | (wchar_t)character << 6);
    } else if (character >> 4 == 0b1110) {
      int a = fgetc(file);
      if (a == EOF || a >> 6 != 0b10){
        *success = false;
        return { };
      }
      int b = fgetc(file);
      if (b == EOF || b >> 6 != 0b10){
        *success = false;
        return { };
      }
      
      result.append(
        (wchar_t)character << 12 |
        ((wchar_t)a & 0b111111) << 6 |
        ((wchar_t)b & 0b111111)
      );
    } else if (character >> 3 == 0b11110) {
      int a = fgetc(file);
      if (a == EOF || a >> 6 != 0b10){
        *success = false;
        return { };
      }
      int b = fgetc(file);
      if (b == EOF || b >> 6 != 0b10){
        *success = false;
        return { };
      }
      int c = fgetc(file);
      if (c == EOF || c >> 6 != 0b10){
        *success = false;
        return { };
      }
      
      result.append(
        (wchar_t)character << 18 |
        ((wchar_t)a & 0b111111) << 12 |
        ((wchar_t)b & 0b111111) << 6 |
        ((wchar_t)c & 0b111111)
      );
    } else {
      *success = false;
      return { };
    }
  }
  
  fclose(file);
  *success = true;
  return result;
}





String String::operator +(char character) const {
  String compound = *this;
  compound.append(character);
  return compound;
}

String String::operator +(wchar_t character) const {
  String compound = *this;
  compound.append(character);
  return compound;
}

String String::operator +(const char *other) const {
  String compound = *this;
  compound.append(other);
  return compound;
}

String String::operator +(const String &other) const {
  String compound = *this;
  compound.append(other);
  return compound;
}

String::operator const char *() const {
  if (_data == nullptr)
    return "";
  return (const char *)_data->contents;
}

bool String::operator==(const char *other) const {
  if (_data == nullptr)
    return *other == 0;
  for (size_t i = 0; i <= _data->bytes; i++)
    if (_data->contents[i] != other[i])
      return false;
  return true;
}

bool String::operator==(const String &other) const {
  if ((_data == nullptr) != (other._data == nullptr))
    return false;
  if (_data == nullptr)
    return true;
  if (_data->bytes != other._data->bytes)
    return false;
  for (size_t i = 0; i < _data->bytes; i++)
    if (_data->contents[i] != other._data->contents[i])
      return false;
  return true;
}

bool String::operator!=(const char *other) const {
  if (_data == nullptr)
    return *other != 0;
  for (size_t i = 0; i <= _data->bytes; i++)
    if (_data->contents[i] != other[i])
      return true;
  return false;
}

bool String::operator!=(const String &other) const {
  if ((_data == nullptr) != (other._data == nullptr))
    return true;
  if (_data == nullptr)
    return false;
  if (_data->bytes != other._data->bytes)
    return true;
  for (size_t i = 0; i < _data->bytes; i++)
    if (_data->contents[i] != other._data->contents[i])
      return true;
  return false;
}

bool String::operator< (const char *other) const {
  if (_data == nullptr)
    return *other > 0;
  for (size_t i = 0; i <= _data->bytes; i++)
    if (_data->contents[i] != other[i])
      return _data->contents[i] < other[i];
  return _data->contents[_data->bytes] < other[_data->bytes];
}

bool String::operator< (const String &other) const {
  if (_data == nullptr)
    return other._data != nullptr;
  else if (other._data == nullptr)
    return false;
  for (size_t i = 0; i < _data->bytes; i++)
    if (_data->contents[i] != other._data->contents[i])
      return _data->contents[i] < other._data->contents[i];
  return _data->contents[_data->bytes] < other._data->contents[_data->bytes];
}

bool String::operator<=(const char *other) const {
  if (_data == nullptr)
    return true;
  for (size_t i = 0; i <= _data->bytes; i++)
    if (_data->contents[i] != other[i])
      return _data->contents[i] <= other[i];
  return _data->contents[_data->bytes] <= other[_data->bytes];
}

bool String::operator<=(const String &other) const {
  if (_data == nullptr)
    return true;
  else if (other._data == nullptr)
    return _data == nullptr;
  for (size_t i = 0; i < _data->bytes; i++)
    if (_data->contents[i] != other._data->contents[i])
      return _data->contents[i] <= other._data->contents[i];
  return _data->contents[_data->bytes] <= other._data->contents[_data->bytes];
}

bool String::operator> (const char *other) const {
  if (_data == nullptr)
    return false;
  for (size_t i = 0; i <= _data->bytes; i++)
    if (_data->contents[i] != other[i])
      return _data->contents[i] > other[i];
  return _data->contents[_data->bytes] > other[_data->bytes];
}

bool String::operator> (const String &other) const {
  if (_data == nullptr)
    return false;
  else if (other._data == nullptr)
    return _data != nullptr;
  for (size_t i = 0; i < _data->bytes; i++)
    if (_data->contents[i] != other._data->contents[i])
      return _data->contents[i] > other._data->contents[i];
  return _data->contents[_data->bytes] > other._data->contents[_data->bytes];
}

bool String::operator>=(const char *other) const {
  if (_data == nullptr)
    return *other == 0;
  for (size_t i = 0; i <= _data->bytes; i++)
    if (_data->contents[i] != other[i])
      return _data->contents[i] >= other[i];
  return _data->contents[_data->bytes] >= other[_data->bytes];
}

bool String::operator>=(const String &other) const {
  if (_data == nullptr)
    return other._data == nullptr;
  else if (other._data == nullptr)
    return true;
  for (size_t i = 0; i < _data->bytes; i++)
    if (_data->contents[i] != other._data->contents[i])
      return _data->contents[i] >= other._data->contents[i];
  return _data->contents[_data->bytes] >= other._data->contents[_data->bytes];
}
