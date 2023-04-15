/**
 * @file Input.h
 * @brief The central input handler
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Events.h>
#include <CityBuilder/Storage/List.h>
#include <CityBuilder/Storage/String.h>
#include <functional>

NS_CITY_BUILDER_BEGIN

/// A set of common key codes for use in with the input manager.
enum class KeyCode : int {
  a = 0x61, b = 0x62, c = 0x63, d = 0x64, e = 0x65, f = 0x66,
  g = 0x67, h = 0x68, i = 0x69, j = 0x6A, k = 0x6B, l = 0x6C,
  m = 0x6D, n = 0x6E, o = 0x6F, p = 0x70, q = 0x71, r = 0x72,
  s = 0x73, t = 0x74, u = 0x75, v = 0x76, w = 0x77, x = 0x78,
  y = 0x79, z = 0x7A,
  
  _0 = 0x30, _1 = 0x31, _2 = 0x32, _3 = 0x33, _4 = 0x34,
  _5 = 0x35, _6 = 0x36, _7 = 0x37, _8 = 0x38, _9 = 0x39,
  
  right = 0xA0, left = 0xA1, down = 0xA2, up = 0xA3,
  
  space = 0x20, escape = 0x1B, enter = 0x0D, tab = 0x09, backspace = 0x08,
};

/// The central input handler for the game.
struct Input {
  /// Set the keys used to slide the camera.
  static void setMoveKeys(
    KeyCode forward, KeyCode backward, KeyCode left, KeyCode right
  );
  
  /// Set the keys used to orbit the camera.
  static void setOrbitKeys(
    KeyCode up, KeyCode down, KeyCode left, KeyCode right
  );
  
  /// Get the current combined move axes for the frame.
  static Real2 getMoveAxes();
  
  /// Get the current combined orbit axes for the frame.
  static Real2 getOrbitAxes();
  
  /// Start listening for and handling axes keys.
  static void listenForAxes();
  
  /// Stope listening for and handling axes keys.
  static void stopListeningForAxes();
  
  
  
  /// The set scroll sensitivity.
  static Real scrollSensitivity();
  
  /// Set the scroll sensitivity.
  static void setScrollSensitivity(Real sensitivity);
  
  /// The set keyboard movement speed.
  static Real2 keyboardMoveSpeed();
  
  /// Set the keyboard movement speed.
  static void setKeyboardMoveSpeed(Real2 speed);
  
  /// The set keyboard orbit speed.
  static Real2 keyboardOrbitSpeed();
  
  /// Set the keyboard orbit speed.
  static void setKeyboardOrbitSpeed(Real2 speed);
  
  /// The set mouse movement speed.
  static Real2 mouseMoveSpeed();
  
  /// Set the mouse movement speed.
  static void setMouseMoveSpeed(Real2 speed);
  
  /// The set mouse orbit speed.
  static Real2 mouseOrbitSpeed();
  
  /// Set the mouse orbit speed.
  static void setMouseOrbitSpeed(Real2 speed);
  
private:
  friend void Events::inputStart (Events::Input &input);
  friend void Events::inputStop  (Events::Input &input);
  friend void Events::inputChange(Events::Input &input);
  
  /// The set of keys used to move the camera in the following order:
  /// forward, backward, left, right.
  static KeyCode _moveKeys[4];
  
  /// The set of keys used to orbit the camera in the following order:
  /// up, down, left, right.
  static KeyCode _rotateKeys[4];
  
  /// Whether or not axes should be handled.
  static bool _listenForAxes;
  
  /// The key codes that are used for axes inputs.
  static bool _usedKeys[256];
  
  /// The currently down key codes.
  static bool _keysDown[256];
  
  /// The scroll sensitivity.
  static Real _scrollSensitivity;
  
  /// The keyboard movement speed.
  static Real2 _keyMoveSpeed;
  
  /// The keyboard orbit speed.
  static Real2 _keyOrbitSpeed;
  
  /// The mouse movement speed.
  static Real2 _mouseMoveSpeed;
  
  /// The mouse orbit speed.
  static Real2 _mouseOrbitSpeed;
  
  /// Whether or not the left mouse button is currently down.
  static bool _leftMouseDown;
  
  /// Whether or not the right mouse button is currently down.
  static bool _rightMouseDown;
  
  /// Whether or not the control key is currently pressed.
  static bool _controlDown;
  
  /// Whether or not the option key is currently pressed.
  static bool _optionDown;
};

NS_CITY_BUILDER_END
