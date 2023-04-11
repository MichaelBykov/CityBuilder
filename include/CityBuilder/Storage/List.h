/**
 * @file List.h
 * @brief An array list data type.
 * @date January 28, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "Exceptions.h"
#include "Check.h"
#include <stdlib.h> // size_t, malloc, realloc, free
#include <initializer_list>
#include <new>

/// A general-purpose array list.
template<typename T>
struct List {
  /// The mutating iterator type.
  typedef T *Iterator;
  /// The constant iterator type.
  typedef const T *ConstIterator;
  
private:
  /// The data pointed to by a list.
  struct Data {
    /// The number of references to the list data.
    size_t references;
    /// The total capacity of the list data.
    size_t capacity;
    /// The total count of the list data.
    size_t count;
    
#ifdef TRACK_DOUBLE_FREE
    /// A tracker for if the list has already been freed.
    bool freed;
#endif
    
    /// The list data.
    T contents[0];
    
    /// Retain the list data.
    void retain() {
      references++;
    }
    
    /// Release the list data.
    void release() {
      if (--references == 0) {
#ifdef TRACK_DOUBLE_FREE
        if (freed)
          throw DoubleFree();
#endif
        
        for (size_t i = 0; i < count; i++)
          contents[i].~T();
#ifdef TRACK_MANAGED_OBJECT_TYPES
        removeManagedObject(typeid(List<T>));
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
  };
  
  // The list's data.
  Data *_data;
  
  /// Make the list unique for copying.
  void _makeUnique() {
    if (_data != nullptr && _data->references > 1) {
      Data *data = (Data *)malloc(sizeof(Data) + sizeof(T) * _data->capacity);
      data->references = 1;
      data->capacity = _data->capacity;
      data->count = _data->count;
#ifdef TRACK_DOUBLE_FREE
      data->freed = false;
#endif
#ifdef TRACK_MANAGED_OBJECT_TYPES
      trackManagedObject(typeid(List<T>));
#elif defined(TRACK_MANAGED_OBJECTS)
      trackManagedObject();
#endif
      
      // Transfer
      for (size_t i = 0; i < data->count; i++)
        new (&data->contents[i]) T(_data->contents[i]);
      
      _data->release();
      _data = data;
    }
  }
  
  /// Expand a list to be able to hold more elements.
  /// \param[in] additional
  ///   The number of additional elements that the list should store.
  /// \remarks
  ///   Makes the list unique.
  void _expand(size_t additional) {
    if (_data == nullptr) {
      size_t capacity = 8;
      while (capacity < additional + 1)
        capacity *= 2;
      
      _data = (Data *)malloc(sizeof(Data) + sizeof(T) * capacity);
      _data->references = 1;
      _data->capacity = capacity;
      _data->count = 0;
#ifdef TRACK_DOUBLE_FREE
      _data->freed = false;
#endif
#ifdef TRACK_MANAGED_OBJECT_TYPES
      trackManagedObject(typeid(List<T>));
#elif defined(TRACK_MANAGED_OBJECTS)
      trackManagedObject();
#endif
    } else if (_data->capacity < _data->count + additional + 1) {
      // Make unique and expand the storage
      size_t capacity = _data->capacity;
      size_t requiredCapacity =  _data->count + additional + 1;
      while (capacity < requiredCapacity)
        capacity *= 2;
      
      Data *data = (Data *)malloc(sizeof(Data) + sizeof(T) * capacity);
      data->references = 1;
      data->capacity = capacity;
      data->count = _data->count;
#ifdef TRACK_DOUBLE_FREE
      data->freed = false;
#endif
#ifdef TRACK_MANAGED_OBJECT_TYPES
      trackManagedObject(typeid(List<T>));
#elif defined(TRACK_MANAGED_OBJECTS)
      trackManagedObject();
#endif
      
      // Transfer
      for (size_t i = 0; i < data->count; i++)
        new (&data->contents[i]) T(_data->contents[i]);
      
      _data->release();
      _data = data;
    } else {
      _makeUnique();
    }
  }
  
public:
  /// Create an empty list.
  List() : _data(nullptr) { }
  
  /// Create a list from a set of elements.
  /// \param[in] elements
  ///   The elements to populate the list with.
  List(std::initializer_list<T> elements) : _data(nullptr) {
    _expand(elements.size());
    for (const T &element : elements)
      append(element);
  }
  
  List(const List &other) : _data(other._data) {
    if (_data != nullptr)
      _data->retain();
  }
  
  List &operator =(const List &other) {
    if (_data != nullptr)
      _data->release();
    
    _data = other._data;
    if (_data != nullptr)
      _data->retain();
    
    return *this;
  }
  
  List(List &&other) : _data(other._data) {
    other._data = nullptr;
  }
  
  List &operator =(List &&other) {
    if (_data != nullptr)
      _data->release();
    
    _data = other._data;
    other._data = nullptr;
    
    return *this;
  }
  
  ~List() {
    if (_data != nullptr) {
      _data->release();
      _data = nullptr;
    }
  }
  
  
  
  /// The number of elements in the list.
  size_t count() const {
    if (_data == nullptr)
      return 0;
    return _data->count;
  }
  
  /// Whether or not the list is empty.
  bool isEmpty() const {
    return _data == nullptr;
  }
  
  const T *begin() const {
    if (_data == nullptr)
      return nullptr;
    return _data->contents;
  }
  
  const T *end() const {
    if (_data == nullptr)
      return nullptr;
    return _data->contents + _data->count;
  }
  
  T *begin() {
    if (_data == nullptr)
      return nullptr;
    return _data->contents;
  }
  
  T *end() {
    if (_data == nullptr)
      return nullptr;
    return _data->contents + _data->count;
  }
  
  /// Get an element at a specific index in the list.
  /// \param[in] index
  ///   The index at which to access the element.
  /// \returns
  ///   The requested element.
  const T &operator[](size_t index) const {
    if (_data == nullptr || index >= _data->count)
      throw IndexOutOfBounds();
    return _data->contents[index];
  }
  
  /// Get an element at a specific index in the list.
  /// \param[in] index
  ///   The index at which to access the element.
  /// \returns
  ///   The requested element.
  T &operator[](size_t index) {
    if (_data == nullptr || index >= _data->count)
      throw IndexOutOfBounds();
    _makeUnique();
    return _data->contents[index];
  }
  
  /// Append an element to the list.
  /// \param[in] element
  ///   The element to append.
  List &append(const T &element) {
    _expand(1);
    new (&_data->contents[_data->count++]) T(element);
    return *this;
  }
  
  /// Insert an element into the list.
  /// \param[in] element
  ///   The element to insert.
  /// \param[in] index
  ///   The index at which to insert the element in the list.
  List &insert(const T &element, size_t index) {
    if (index > count())
      throw IndexOutOfBounds();
    _expand(1);
    for (size_t i = _data->count; i > index; i--)
      new (&_data->contents[i]) T(std::move(_data->contents[i - 1]));
    new (&_data->contents[index]) T(element);
    _data->count++;
    return *this;
  }
  
  /// Remove an element from the list.
  /// \param[in] index
  ///   The index at which to remove an element from the list.
  /// \returns
  ///   The removed element.
  T remove(size_t index) {
    if (_data == nullptr || index >= _data->count)
      throw IndexOutOfBounds();
    _makeUnique();
    T t = _data->contents[index];
    if (_data->count == 1) {
      // Empty list
      _data->release();
      _data = nullptr;
      return t;
    }
    _data->contents[index].~T();
    for (size_t i = index; i + 1 < _data->count; i++)
      new (&_data->contents[i]) T(std::move(_data->contents[i + 1]));
    _data->count--;
    return t;
  }
  
  /// Remove all elements from the list.
  void removeAll() {
    if (_data != nullptr) {
      _data->release();
      _data = nullptr;
    }
  }
};
