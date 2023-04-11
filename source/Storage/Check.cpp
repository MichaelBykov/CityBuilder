/**
 * @file Check.cpp
 * @brief Implement the interface for checking memory integrity.
 * @date February 10, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Storage/Check.h>
#include <typeindex>


#ifdef TRACK_MANAGED_OBJECTS

namespace {
  size_t managedObjects = 0;
}

size_t getManagedObjectCount() {
  return managedObjects;
}

#endif



#ifdef TRACK_MANAGED_OBJECT_TYPES



namespace {

struct ManagedObjectTracker {
  struct Data {
    std::type_index type = std::type_index(typeid(void));
    size_t counter = 0;
  };
  
  size_t capacity = 4;
  size_t count = 0;
  Data *data;
  
  ManagedObjectTracker()
    : data(new Data[4]) { }
  
  struct UnmanagedObject { };
  
  void add(const std::type_index &index) {
    for (size_t i = 0; i < count; i++)
      if (data[i].type == index) {
        data[i].counter++;
        return;
      }
    if (count == capacity) {
      capacity *= 2;
      Data *newData = new Data[capacity];
      for (size_t i = 0; i < count; i++)
        newData[i] = data[i];
    }
    data[count].type = index;
    data[count].counter = 1;
    count++;
  }
  
  void remove(const std::type_index &index) {
    for (size_t i = 0; i < count; i++)
      if (data[i].type == index) {
        data[i].counter--;
        return;
      }
    throw UnmanagedObject();
  }
};
ManagedObjectTracker trackers[1024] { ManagedObjectTracker() };

}



void getManagedObjects(std::function<void(const char *, size_t)> callback) {
  for (int i = 0; i < 1024; i++)
    for (size_t j = 0; j < trackers[i].count; j++)
      callback(trackers[i].data[j].type.name(), trackers[i].data[j].counter);
}

void trackManagedObject(const std::type_info &type) {
  managedObjects++;
  
  size_t hash = type.hash_code() % 1024;
  trackers[hash].add(type);
}

void trackManagedObject(const std::type_index &type) {
  managedObjects++;
  
  size_t hash = type.hash_code() % 1024;
  trackers[hash].add(type);
}

void removeManagedObject(const std::type_info &type) {
  managedObjects--;
  
  size_t hash = type.hash_code() % 1024;
  trackers[hash].remove(type);
}

void removeManagedObject(const std::type_index &type) {
  managedObjects--;
  
  size_t hash = type.hash_code() % 1024;
  trackers[hash].remove(type);
}



#elif defined(TRACK_MANAGED_OBJECTS)



void trackManagedObject() {
  managedObjects++;
}

void removeManagedObject() {
  managedObjects--;
}



#endif



#ifdef TRACK_DOUBLE_FREE

namespace {
  size_t zombies = 0;
}

void addZombieObject() {
  zombies++;
}

size_t getZombieObjects() {
  return zombies;
}

#endif
