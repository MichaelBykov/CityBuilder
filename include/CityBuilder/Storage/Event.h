/**
 * @file Event.h
 * @brief An event that can be listened to.
 * @date April 18, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "List.h"
#include <functional>

/// An event that can be listened to.
/// \tparam Args
///   The types of arguments to pass to the listeners.
template<typename ...Args>
struct Event {
private:
  struct Listener {
    int id;
    std::function<void(Args...)> listener;
  };
  
  List<Listener> _listeners;
  
  int _idPool = 0;
public:
  /// Create a new event.
  Event() { }
  
  // Events shouldn't be transferred
  Event(const Event &other) = delete;
  
  
  
  /// Begin listening to the event.
  /// \param[in] listener
  ///   The function to call when the event is fired.
  /// \returns
  ///   The id of the listener.
  int operator += (void(*listener)(Args...)) {
    _listeners.append({ _idPool, listener });
    return _idPool++;
  }
  
  /// Begin listening to the event.
  /// \param[in] listener
  ///   The function to call when the event is fired.
  /// \returns
  ///   The id of the listener.
  template<typename Lambda>
  int operator += (Lambda listener) {
    _listeners.append({ _idPool, listener });
    return _idPool++;
  }
  
  /// Remove a listener from the event.
  /// \param[in] id
  ///   The id of the listener to remove.
  void operator -= (int id) {
    for (intptr_t i = _listeners.count() - 1; i >= 0; i--)
      if (_listeners[i].id == id) {
        _listeners.remove(i);
        return;
      }
  }
  
  /// Fire the event.
  /// \param[in] args
  ///   The arguments to pass to all the listeners.
  void operator()(Args... args) {
    for (auto listener : _listeners)
      listener.listener(args...);
  }
};
