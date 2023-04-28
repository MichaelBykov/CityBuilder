/**
 * @file Ref.h
 * @brief A simple reference counter type.
 * @date January 30, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "Check.h"
#include "Exceptions.h"
#include <stddef.h> // size_t

/// A reference counter.
/// \tparam T
///   The type to reference count.
template<typename T>
struct Ref {
private:
  /// The reference-counted data.
  struct Data {
    /// The number of references.
    size_t references = 1;
    /// The value that is being counted.
    T data;
    
#ifdef TRACK_DOUBLE_FREE
    /// Double-free tracker.
    bool freed = false;
#endif
    
    Data(const T &data) : data(data) { }
    
    /// Retain the data.
    void retain() {
      references++;
    }
    
    /// Release the data.
    void release() {
      if (--references == 0) {
#ifdef TRACK_DOUBLE_FREE
        if (freed)
          throw DoubleFree();
#endif
        
#ifdef TRACK_MANAGED_OBJECT_TYPES
        removeManagedObject(typeid(Ref<T>));
#elif defined(TRACK_MANAGED_OBJECTS)
        removeManagedObject();
#endif
        
#ifdef TRACK_DOUBLE_FREE
        freed = true;
        data.~T();
        addZombieObject();
#else
        delete this;
#endif
      }
    }
  };
  
  /// The reference-counted data.
  Data *_data;
public:
  
  /// Create a null reference.
  Ref() : _data(nullptr) { }
  
  /// Create a null reference.
  Ref(nullptr_t) : _data(nullptr) { }
  
  /// Reference count a value.
  /// \param[in] data
  ///   The value to reference count.
  Ref(const T &data) : _data(new Data(data)) {
#ifdef TRACK_MANAGED_OBJECT_TYPES
    trackManagedObject(typeid(Ref<T>));
#elif defined(TRACK_MANAGED_OBJECTS)
    trackManagedObject();
#endif
  }
  
  Ref(const Ref &other) : _data(other._data) {
    _data->retain();
  }
  
  Ref &operator =(const Ref &other) {
    if (_data != nullptr)
      _data->release();
    _data = other._data;
    _data->retain();
    return *this;
  }
  
  Ref(Ref &&other) : _data(other._data) {
    other._data = nullptr;
  }
  
  Ref &operator =(Ref &&other) {
    if (_data != nullptr)
      _data->release();
    _data = other._data;
    other._data = nullptr;
    return *this;
  }
  
  ~Ref() {
    if (_data != nullptr) {
      _data->release();
      _data = nullptr;
    }
  }
  
  /// Check if the reference is a null reference.
  bool empty() const {
    return _data == nullptr;
  }
  
  /// Get the referenced value.
  T &operator *() {
    return _data->data;
  }
  
  /// Access a member of the referenced value.
  T *operator->() {
    return &_data->data;
  }
  
  /// Get the referenced value.
  const T &operator *() const {
    return _data->data;
  }
  
  /// Access a member of the referenced value.
  const T *operator->() const {
    return &_data->data;
  }

  /// Compare refs
  bool operator ==(const Ref &other) const {
    return _data == other._data;
  }
};


/// A reference counter.
/// \tparam T
///   The type to reference count.
template<typename T>
struct Ref<T &> {
private:
  template<typename>
  friend class Ref;
  
  /// The reference-counted data.
  struct Data {
    /// The number of references.
    size_t references = 1;
    /// The value that is being counted.
    T &data;
    
#ifdef TRACK_DOUBLE_FREE
    /// Double-free tracker.
    bool freed = false;
#endif
    
#ifdef TRACK_MANAGED_OBJECT_TYPES
    /// The underlying type of the value.
    std::type_index type;
    
    Data(T &data) : data(data), type(typeid(Ref<T &>)) {
      trackManagedObject(type);
    }
#elif defined(TRACK_MANAGED_OBJECTS)
    Data(T &data) : data(data) {
      trackManagedObject();
    }
#else
    Data(T &data) : data(data) { }
#endif
    
    /// Retain the data.
    void retain() {
      references++;
    }
    
    /// Release the data.
    void release() {
      if (--references == 0) {
#ifdef TRACK_DOUBLE_FREE
        if (freed)
          throw DoubleFree();
#endif
        
#ifdef TRACK_MANAGED_OBJECT_TYPES
        removeManagedObject(type);
#elif defined(TRACK_MANAGED_OBJECTS)
        removeManagedObject();
#endif
        
        delete &data;
#ifdef TRACK_DOUBLE_FREE
        freed = true;
        addZombieObject();
#else
        delete this;
#endif
      }
    }
  };
  
  /// The reference-counted data.
  Data *_data;
  
  /// Wrap a data pointer.
  Ref(Data *_data) : _data(_data) { _data->retain(); }
public:
  
  /// Create a null reference.
  Ref() : _data(nullptr) { }
  
  /// Create a null reference.
  Ref(nullptr_t) : _data(nullptr) { }
  
  /// Reference count a value.
  /// \param[in] data
  ///   The value to reference count.
  Ref(T &data) : _data(new Data(data)) { }
  
  /// Reference count a value.
  /// \param[in] data
  ///   The value to reference count.
  Ref(T *data) : _data(new Data(*data)) { }
  
  Ref(const Ref &other) : _data(other._data) {
    _data->retain();
  }
  
  Ref &operator =(const Ref &other) {
    if (_data != nullptr)
      _data->release();
    _data = other._data;
    _data->retain();
    return *this;
  }
  
  Ref(Ref &&other) : _data(other._data) {
    other._data = nullptr;
  }
  
  Ref &operator =(Ref &&other) {
    if (_data != nullptr)
      _data->release();
    _data = other._data;
    other._data = nullptr;
    return *this;
  }
  
  ~Ref() {
    if (_data != nullptr) {
      _data->release();
      _data = nullptr;
    }
  }
  
  /// Check if the reference is a null reference.
  bool empty() const {
    return _data == nullptr;
  }
  
  /// Cast this reference to another reference type.
  template<typename K>
  explicit operator Ref<K &>() {
    return Ref<K &>((typename Ref<K &>::Data *)_data);
  }
  
  /// Cast this reference to another reference type.
  template<typename K>
  explicit operator const Ref<K &>() const {
    return Ref<K &>((typename Ref<K &>::Data *)_data);
  }
  
  /// Get the referenced value.
  T &operator *() {
    return _data->data;
  }
  
  /// Access a member of the referenced value.
  T *operator->() {
    return &_data->data;
  }
  
  /// Get the referenced value.
  const T &operator *() const {
    return _data->data;
  }
  
  /// Access a member of the referenced value.
  const T *operator->() const {
    return &_data->data;
  }

  /// Compare refs
  bool operator ==(const Ref &other) const {
    return _data == other._data;
  }
};
