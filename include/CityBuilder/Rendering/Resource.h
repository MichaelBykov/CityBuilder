/**
 * @file Resource.h
 * @brief A resource management pointer.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>

NS_CITY_BUILDER_BEGIN

template<typename T>
struct Resource {
private:
  struct Data {
    size_t references;
    T *data;
    
    void release() {
      if (--references == 0) {
        delete data;
        delete this;
      }
    }
  };
  
  Data *_data;
  
public:
  
  Resource() : _data(nullptr) { }
  
  Resource(std::nullptr_t) : _data(nullptr) { }
  
  Resource(T *data) : _data(new Data{ 1, data }) { }
  
  Resource(const Resource &other) : _data(other._data) {
    if (_data)
      _data->references++;
  }
  
  Resource &operator=(const Resource &other) {
    if (_data)
      _data->release();
    
    _data = other._data;
    if (_data)
      _data->references++;
    
    return *this;
  }
  
  Resource(Resource &&other) : _data(other._data) {
    other._data = nullptr;
  }
  
  Resource &operator=(Resource &&other) {
    if (_data)
      _data->release();
    
    _data = other._data;
    other._data = nullptr;
    
    return *this;
  }
  
  ~Resource() {
    if (_data) {
      _data->release();
      _data = nullptr;
    }
  }
  
  const T *operator ->() const {
    return _data->data;
  }
  
  T *operator ->() {
    return _data->data;
  }
  
  const T &operator *() const {
    return *_data->data;
  }
  
  T &operator *() {
    return *_data->data;
  }
  
  operator bool() const {
    return _data != nullptr;
  }
  
  const T *address() const {
    return _data->data;
  }
  
  T *address() {
    return _data->data;
  }
};

NS_CITY_BUILDER_END
