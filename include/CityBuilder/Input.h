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
#include <CityBuilder/Storage/Event.h>
#include <functional>

NS_CITY_BUILDER_BEGIN

/// A set of common key codes for use in with the input manager.
enum class KeyCode : int {
#if (__APPLE__ && __MACH__) // MacOS QWERTY keyboard
  
  q = 0x0C, w = 0x0D, e = 0x0E, r = 0x0F, t = 0x11, y = 0x10, u = 0x20, i = 0x22, o = 0x1F, p = 0x23,
  a = 0x00, s = 0x01, d = 0x02, f = 0x03, g = 0x05, h = 0x04, j = 0x26, k = 0x28, l = 0x25,
  z = 0x06, x = 0x07, c = 0x08, v = 0x09, b = 0x0B, n = 0x2D, m = 0x2E,
  
  _0 = 0x1D, _1 = 0x12, _2 = 0x13, _3 = 0x14, _4 = 0x15, _5 = 0x17, _6 = 0x16, _7 = 0x1A,
  _8 = 0x1C, _9 = 0x19,
  
  space = 0x31, tab = 0x30, enter = 0x24, escape = 0x35, backspace = 0x33,
  
  leftShift = 0x38, rightShift = 0x3C, leftControl = 0x3B, rightControl = 0x3E,
  leftOption = 0x3A, rightOption = 0x3D, leftCommand = 0x37, rightCommand = 0x36,
  
  left = 0x7B, right = 0x7C, up = 0x7E, down = 0x7D,
  
#endif
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
  static Real2 scrollSensitivity();
  
  /// Set the scroll sensitivity.
  static void setScrollSensitivity(Real2 sensitivity);
  
  /// The set pinch sensitivity.
  static Real pinchSensitivity();
  
  /// Set the pinch sensitivity.
  static void setPinchSensitivity(Real sensitivity);
  
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
  
  /// Get the current position of the mouse on the screen, in pixels.
  static Real2 mousePosition();
  
  /// Whether the primary mouse button is currently down.
  static bool primaryMouseDown();
  
  /// Whether the secondary mouse button is currently down.
  static bool secondaryMouseDown();
  
  /// An event fired when the primary mouse button is pressed.
  static Event<> onPrimaryMouseDown;
  
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
  static Real2 _scrollSensitivity;
  
  /// The pinch sensitivity.
  static Real _pinchSensitivity;
  
  /// The keyboard movement speed.
  static Real2 _keyMoveSpeed;
  
  /// The keyboard orbit speed.
  static Real2 _keyOrbitSpeed;
  
  /// The mouse movement speed.
  static Real2 _mouseMoveSpeed;
  
  /// The mouse orbit speed.
  static Real2 _mouseOrbitSpeed;
  
  /// Whether or not the primary mouse button is currently down.
  static bool _primaryMouseDown;
  
  /// Whether or not the secondary mouse button is currently down.
  static bool _secondaryMouseDown;
  
  /// The current mouse position, in pixels.
  static Real2 _mousePos;
  
  /// Whether or not the standard system keys (e.x. shift, alt, command, etc)
  /// are currently pressed.
  static bool _systemKeys[16];
};

NS_CITY_BUILDER_END
