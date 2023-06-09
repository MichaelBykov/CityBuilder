/**
 * @file Map.h
 * @brief A map data type implemented through a hash table.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "Exceptions.h"
#include "Check.h"
#include "List.h"
#include "Optional.h"
#include "Hash.h"
#include <stdlib.h> // size_t, malloc, realloc, free
#include <initializer_list>
#include <new>

/// A general-purpose map.
template<typename TKey, typename TValue>
struct Map {
  /// A key-value pair.
  struct Pair {
    /// The key.
    TKey key;
    /// The value.
    TValue value;
  };
  
private:
  /// The data pointed to by a map.
  struct DataBase {
    /// The number of references to the map data.
    size_t references = 1;
    
    /// The total number of buckets in the map.
    int buckets;
    
    /// The beginning of the buckets.
    List<Pair> *begin;
    
#ifdef TRACK_DOUBLE_FREE
    /// A tracker for if the map has already been freed.
    bool freed = false;
#endif
    
    virtual ~DataBase() { }
    
    virtual DataBase *clone() = 0;
    
    virtual TValue &lookup(uint32_t hash, const TKey &key) = 0;
    
    virtual bool exists(uint32_t hash, const TKey &key) = 0;
    
    virtual void set(uint32_t hash, const TKey &key, const TValue &value) = 0;
    
    /// Retain the map data.
    void retain() {
      references++;
    }
    
    /// Release the map data.
    void release() {
      if (--references == 0) {
#ifdef TRACK_DOUBLE_FREE
        if (freed)
          throw DoubleFree();
        freed = true;
        addZombieObject();
#endif
#ifdef TRACK_MANAGED_OBJECT_TYPES
        removeManagedObject(typeid(Map<TKey, TValue>));
#elif defined(TRACK_MANAGED_OBJECTS)
        removeManagedObject();
#endif
        delete this;
      }
    }
  };
  
  template<int bucketCount>
  struct Data : DataBase {
    /// The map buckets.
    List<Pair> contents[bucketCount];
    
    Data() {
      this->buckets = bucketCount;
      this->begin = contents;
    }
    
    DataBase *clone() override {
      Data *data = new Data();
#ifdef TRACK_MANAGED_OBJECT_TYPES
      trackManagedObject(typeid(Map<TKey, TValue>));
#elif defined(TRACK_MANAGED_OBJECTS)
      trackManagedObject();
#endif
      for (int i = 0; i < bucketCount; i++)
        data->contents[i] = contents[i];
      
      return data;
    }
    
    TValue &lookup(uint32_t hash, const TKey &key) override {
      List<Pair> &bucket = contents[hash % bucketCount];
      for (Pair &pair : bucket) {
        if (pair.key == key)
          return pair.value;
      }
      throw IndexOutOfBounds();
    }
    
    bool exists(uint32_t hash, const TKey &key) override {
      List<Pair> &bucket = contents[hash % bucketCount];
      for (Pair &pair : bucket) {
        if (pair.key == key)
          return true;
      }
      return false;
    }
    
    void set(uint32_t hash, const TKey &key, const TValue &value) override {
      List<Pair> &bucket = contents[hash % bucketCount];
      for (Pair &pair : bucket) {
        if (pair.key == key) {
          pair.value = value;
          return;
        }
      }
      bucket.append({ key, value });
    }
  };
  
  // The map's data.
  DataBase *_data;
  
  /// Make the map unique for copying.
  void _makeUnique() {
    if (_data != nullptr && _data->references > 1) {
      DataBase *data = _data->clone();
      _data->release();
      _data = data;
    }
  }
  
  Map(DataBase *data) : _data(data) { }
  
public:
  /// Create an empty map.
  Map() : _data(nullptr) { }
  
  /// Create a map from a set of key-value pairs.
  /// \param[in] pairs
  ///   The pairs to populate the map with.
  Map(std::initializer_list<Pair> pairs) : _data(new Data<64>()) {
    for (const Pair &pair : pairs)
      set(pair.key, pair.value);
  }
  
  /// Create a map with a given bucket count.
  template<int buckets>
  static Map buckets() {
    return Map(new Data<buckets>());
  }
  
  /// Create a map with a given bucket count and initialize it with a set of
  /// key-value pairs.
  /// \param[in] pairs
  ///   The pairs to populate the map with.
  template<int buckets>
  static Map buckets(std::initializer_list<Pair> pairs) {
    Map map = Map(new Data<buckets>());
    for (const Pair &pair : pairs)
      map.set(pair.key, pair.value);
    return map;
  }
  
  Map(const Map &other) : _data(other._data) {
    if (_data != nullptr)
      _data->retain();
  }
  
  Map &operator =(const Map &other) {
    if (_data != nullptr)
      _data->release();
    
    _data = other._data;
    if (_data != nullptr)
      _data->retain();
    
    return *this;
  }
  
  Map(Map &&other) : _data(other._data) {
    other._data = nullptr;
  }
  
  Map &operator =(Map &&other) {
    if (_data != nullptr)
      _data->release();
    
    _data = other._data;
    other._data = nullptr;
    
    return *this;
  }
  
  ~Map() {
    if (_data != nullptr) {
      _data->release();
      _data = nullptr;
    }
  }
  
  
  
  struct Iterator {
    /// The map being iterated over.
    Map *map;
    /// The current bucket.
    int bucket;
    /// The current bucket iterator.
    typename List<Pair>::Iterator start;
    /// The end iterator.
    typename List<Pair>::Iterator end;
    
    Iterator(nullptr_t) : map(nullptr), bucket(0) { }
    
    Iterator(Map &map) : map(&map), bucket(0),
      start(map._data->begin[0].begin()),
        end(map._data->begin[0].end()) {
      if (start == end)
        while (bucket < map._data->buckets && start == end) {
          bucket++;
          start = map._data->begin[bucket].begin();
          end   = map._data->begin[bucket].end();
        }
    }
    
    Iterator &operator ++() {
      if (map == nullptr)
        return *this;
      
      ++start;
      if (start != end)
        return *this;
      bucket++;
      if (bucket < map->_data->buckets) {
        start = map->_data->begin[bucket].begin();
        end   = map->_data->begin[bucket].end();
      }
      return *this;
    }
    
    Pair &operator *() {
      return *start;
    }
    
    bool operator !=(const Iterator &other) {
      if (map == nullptr)
        return false;
      return start != end;
    }
  };
  
  List<Pair> *begin() {
    if (_data == nullptr)
      return nullptr;
    return _data->begin;
  }
  
  List<Pair> *end() {
    if (_data == nullptr)
      return nullptr;
    return _data->begin + _data->buckets;
  }
  
  
  
  /// Get the value associated with the given key.
  /// \param[in] key
  ///   The key to look up.
  /// \returns
  ///   The value associated with the given key.
  const TValue &operator[](const TKey &key) const {
    if (_data == nullptr)
      throw IndexOutOfBounds();
    uint32_t _hash = Storage::hash(key);
    return _data->lookup(_hash, key);
  }
  
  /// Get the value associated with the given key.
  /// \param[in] key
  ///   The key to look up.
  /// \returns
  ///   The value associated with the given key.
  TValue &operator[](const TKey &key) {
    if (_data == nullptr)
      throw IndexOutOfBounds();
    _makeUnique();
    uint32_t _hash = Storage::hash(key);
    return _data->lookup(_hash, key);
  }
  
  /// Associate a value with a key.
  /// \param[in] key
  ///   The key to associate the value with.
  /// \param[in] value
  ///   The value to associate with the key.
  void set(const TKey &key, const TValue &value) {
    if (_data == nullptr)
      _data = new Data<64>();
    else
      _makeUnique();
    uint32_t _hash = Storage::hash(key);
    _data->set(_hash, key, value);
  }
  
  /// Attempt to get the value associated with the given key.
  /// \param[in] key
  ///   The key to look up.
  /// \returns
  ///   The value associated with the given key, or null if no value is
  ///   associated with the key.
  Optional<TValue &> get(const TKey &key) {
    if (_data == nullptr)
      throw IndexOutOfBounds();
    uint32_t _hash = Storage::hash(key);
    if (_data->exists(_hash, key))
      return _data->lookup(_hash, key);
    else
      return nullptr;
  }
  
  /// Attempt to get the value associated with the given key.
  /// \param[in] key
  ///   The key to look up.
  /// \returns
  ///   The value associated with the given key, or null if no value is
  ///   associated with the key.
  Optional<const TValue &> get(const TKey &key) const {
    if (_data == nullptr)
      throw IndexOutOfBounds();
    uint32_t _hash = Storage::hash(key);
    if (_data->exists(_hash, key))
      return _data->lookup(_hash, key);
    else
      return nullptr;
  }
  
  /// Check if a value is associated with the given key.
  /// \param[in] key
  ///   The key to look up.
  /// \returns
  ///   Whether or not a value is associated with the given key in the map.
  bool has(const TKey &key) const {
    if (_data == nullptr)
      return false;
    uint32_t _hash = Storage::hash(key);
    return _data->exists(_hash, key);
  }
};
