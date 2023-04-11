/**
 * @file Stack.h
 * @brief A stack data type.
 * @date January 30, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "Exceptions.h"
#include "Check.h"
#include <stdlib.h> // size_t, malloc, realloc, free
#include <initializer_list>
#include <new>

/// A dynamic stack.
template<typename T>
struct Stack {
private:
  /// The data pointed to by a stack.
  struct Data {
    /// The number of references to the stack data.
    size_t references;
    /// The total capacity of the stack data.
    size_t capacity;
    /// The total count of the stack data.
    size_t count;
    
#ifdef TRACK_DOUBLE_FREE
    /// A tracker for if the stack has already been freed.
    bool freed;
#endif
    
    /// The stack data.
    T contents[0];
    
    /// Retain the stack data.
    void retain() {
      references++;
    }
    
    /// Release the stack data.
    void release() {
      if (--references == 0) {
#ifdef TRACK_DOUBLE_FREE
        if (freed)
          throw DoubleFree();
#endif
        
        for (size_t i = 0; i < count; i++)
          contents[i].~T();
#ifdef TRACK_MANAGED_OBJECT_TYPES
        removeManagedObject(typeid(Stack<T>));
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
  
  // The stack's data.
  Data *_data;
  
  /// Make the stack unique for copying.
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
      trackManagedObject(typeid(Stack<T>));
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
  
  /// Expand a stack to be able to hold more elements.
  /// \param[in] additional
  ///   The number of additional elements that the stack should store.
  /// \remarks
  ///   Makes the stack unique.
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
      trackManagedObject(typeid(Stack<T>));
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
      trackManagedObject(typeid(Stack<T>));
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
  /// Create an empty stack.
  Stack() : _data(nullptr) { }
  
  Stack(std::initializer_list<T> elements) : _data(nullptr) {
    _expand(elements.size());
    for (const T &element : elements)
      push(element);
  }
  
  Stack(const Stack &other) : _data(other._data) {
    if (_data != nullptr)
      _data->retain();
  }
  
  Stack &operator =(const Stack &other) {
    if (_data != nullptr)
      _data->release();
    
    _data = other._data;
    if (_data != nullptr)
      _data->retain();
    
    return *this;
  }
  
  Stack(Stack &&other) : _data(other._data) {
    other._data = nullptr;
  }
  
  Stack &operator =(Stack &&other) {
    if (_data != nullptr)
      _data->release();
    
    _data = other._data;
    other._data = nullptr;
    
    return *this;
  }
  
  ~Stack() {
    if (_data != nullptr) {
      _data->release();
      _data = nullptr;
    }
  }
  
  
  
  /// The number of elements in the stack.
  size_t count() const {
    if (_data == nullptr)
      return 0;
    return _data->count;
  }
  
  /// Whether or not this stack is empty.
  bool isEmpty() const {
    return _data == nullptr;
  }
  
  /// Access an element in the stack.
  /// \param[in] index
  ///   The index of the element in the stack where 0 corresponds to the
  ///   element at the top of the stack.
  const T &operator[](size_t index) const {
    if (_data == nullptr || index >= _data->count)
      throw IndexOutOfBounds();
    return _data->contents[_data->count - 1 - index];
  }
  
  /// Access an element in the stack.
  /// \param[in] index
  ///   The index of the element in the stack where 0 corresponds to the
  ///   element at the top of the stack.
  T &operator[](size_t index) {
    if (_data == nullptr || index >= _data->count)
      throw IndexOutOfBounds();
    _makeUnique();
    return _data->contents[_data->count - 1 - index];
  }
  
  /// Push an element onto the stack.
  /// \param[in] element
  ///   The element to push.
  Stack &push(const T &element) {
    _expand(1);
    new (&_data->contents[_data->count++]) T(element);
    return *this;
  }
  
  /// Pop an element from the top of the stack.
  /// \returns
  ///   The popped element.
  T pop() {
    if (_data == nullptr)
      throw IndexOutOfBounds();
    _makeUnique();
    T t = _data->contents[--_data->count];
    _data->contents[_data->count].~T();
    if (_data->count == 0) {
      _data->release();
      _data = nullptr;
    }
    return t;
  }
  
  /// Peek at the element at the top of the stack.
  T &peek() {
    if (_data == nullptr)
      throw IndexOutOfBounds();
    return _data->contents[_data->count - 1];
  }
  
  /// Peek at the element at the top of the stack.
  const T &peek() const {
    if (_data == nullptr)
      throw IndexOutOfBounds();
    return _data->contents[_data->count - 1];
  }
  
  /// Remove all elements from the stack.
  void removeAll() {
    if (_data != nullptr) {
      _data->release();
      _data = nullptr;
    }
  }
};
