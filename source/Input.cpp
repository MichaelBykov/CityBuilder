/**
 * @file Input.cpp
 * @brief The handlers for all game input.
 * @date April 10, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Input.h>
#include <CityBuilder/Game.h>
#include <iostream>
USING_NS_CITY_BUILDER

/* -------------------------------------------------------------------------- *\
|                                                                              |
| Default values                                                               |
|                                                                              |
\* -------------------------------------------------------------------------- */

KeyCode Input::_moveKeys[4] { (KeyCode)-1 };

KeyCode Input::_rotateKeys[4] { (KeyCode)-1 };

bool Input::_listenForAxes = false;

bool Input::_usedKeys[256] { false };

bool Input::_keysDown[256] { false };

Real2 Input::_scrollSensitivity = { 0.005, 0.003 };

Real Input::_pinchSensitivity = 15;

Real2 Input::_keyMoveSpeed = { 2.5, 2.5 };

Real2 Input::_keyOrbitSpeed = { 2, 2 };

Real2 Input::_mouseMoveSpeed = { 0.001, 0.001 };

Real2 Input::_mouseOrbitSpeed = { 0.0033, 0.0033 };

bool Input::_leftMouseDown = false;

bool Input::_rightMouseDown = false;

bool Input::_systemKeys[16] { false };





/* -------------------------------------------------------------------------- *\
|                                                                              |
| Configuration                                                                |
|                                                                              |
\* -------------------------------------------------------------------------- */

void Input::setMoveKeys(
  KeyCode forward, KeyCode backward, KeyCode left, KeyCode right
) {
  if (_moveKeys[0] != (KeyCode)-1)
    for (int i = 0; i < 4; i++)
      _usedKeys[(int)_moveKeys[i]] = false;
  
  _moveKeys[0] = forward ;
  _moveKeys[1] = backward;
  _moveKeys[2] = left    ;
  _moveKeys[3] = right   ;
  
  for (int i = 0; i < 4; i++)
    _usedKeys[(int)_moveKeys[i]] = true;
}

void Input::setOrbitKeys(
  KeyCode up, KeyCode down, KeyCode left, KeyCode right
) {
  if (_rotateKeys[0] != (KeyCode)-1)
    for (int i = 0; i < 4; i++)
      _usedKeys[(int)_rotateKeys[i]] = false;
  
  _rotateKeys[0] = up   ;
  _rotateKeys[1] = down ;
  _rotateKeys[2] = left ;
  _rotateKeys[3] = right;
  
  for (int i = 0; i < 4; i++)
    _usedKeys[(int)_rotateKeys[i]] = true;
}

Real2 Input::getMoveAxes() {
  int x = 0, y = 0;
  if (_keysDown[(int)_moveKeys[0]]) y++;
  if (_keysDown[(int)_moveKeys[1]]) y--;
  if (_keysDown[(int)_moveKeys[2]]) x++;
  if (_keysDown[(int)_moveKeys[3]]) x--;
  return Real2(x, y);
}

Real2 Input::getOrbitAxes() {
  int x = 0, y = 0;
  if (_keysDown[(int)_rotateKeys[0]]) y++;
  if (_keysDown[(int)_rotateKeys[1]]) y--;
  if (_keysDown[(int)_rotateKeys[2]]) x++;
  if (_keysDown[(int)_rotateKeys[3]]) x--;
  return Real2(x, y);
}

void Input::listenForAxes() {
  _listenForAxes = true;
}

void Input::stopListeningForAxes() {
  _listenForAxes = false;
  for (int i = 0; i < 256; i++)
    _keysDown[i] = false;
}

Real2 Input::scrollSensitivity() {
  return _scrollSensitivity;
}

void Input::setScrollSensitivity(Real2 sensitivity) {
  _scrollSensitivity = sensitivity;
}

Real Input::pinchSensitivity() {
  return _pinchSensitivity;
}

void Input::setPinchSensitivity(Real sensitivity) {
  _pinchSensitivity = sensitivity;
}

Real2 Input::keyboardMoveSpeed() {
  return _keyMoveSpeed;
}

void Input::setKeyboardMoveSpeed(Real2 speed) {
  _keyMoveSpeed = speed;
}

Real2 Input::keyboardOrbitSpeed() {
  return _keyOrbitSpeed;
}

void Input::setKeyboardOrbitSpeed(Real2 speed) {
  _keyOrbitSpeed = speed;
}

Real2 Input::mouseMoveSpeed() {
  return _mouseMoveSpeed;
}

void Input::setMouseMoveSpeed(Real2 speed) {
  _mouseMoveSpeed = speed;
}

Real2 Input::mouseOrbitSpeed() {
  return _mouseOrbitSpeed;
}

void Input::setMouseOrbitSpeed(Real2 speed) {
  _mouseOrbitSpeed = speed;
}





/* -------------------------------------------------------------------------- *\
|                                                                              |
| Events                                                                       |
|                                                                              |
\* -------------------------------------------------------------------------- */

void Events::inputStart(Input &input) {
  switch (input.type) {
  case Input::Type::keyboard: {
    int key = input.keyboard.keyCode;
    
    if (NS_CITY_BUILDER Input::_listenForAxes && key < 256 &&
        NS_CITY_BUILDER Input::_usedKeys[key]) {
      // Axis
      NS_CITY_BUILDER Input::_keysDown[key] = true;
    }
    
    // Handle the system modifier keys
#if (__APPLE__ && __MACH__) // MacOS
    
    switch (key) {
    case (int)KeyCode:: leftShift  : NS_CITY_BUILDER Input::_systemKeys[0] = true; break;
    case (int)KeyCode:: leftCommand: NS_CITY_BUILDER Input::_systemKeys[1] = true; break;
    case (int)KeyCode:: leftControl: NS_CITY_BUILDER Input::_systemKeys[2] = true; break;
    case (int)KeyCode:: leftOption : NS_CITY_BUILDER Input::_systemKeys[3] = true; break;
    case (int)KeyCode::rightShift  : NS_CITY_BUILDER Input::_systemKeys[4] = true; break;
    case (int)KeyCode::rightCommand: NS_CITY_BUILDER Input::_systemKeys[5] = true; break;
    case (int)KeyCode::rightControl: NS_CITY_BUILDER Input::_systemKeys[6] = true; break;
    case (int)KeyCode::rightOption : NS_CITY_BUILDER Input::_systemKeys[7] = true; break;
    }
      
#endif
  } break;
  
  default:
    break;
  }
}

void Events::inputStop(Input &input) {
  switch (input.type) {
  case Input::Type::keyboard: {
    int key = input.keyboard.keyCode;
    
    if (NS_CITY_BUILDER Input::_listenForAxes && key < 256 &&
        NS_CITY_BUILDER Input::_usedKeys[key]) {
      // Axis
      NS_CITY_BUILDER Input::_keysDown[key] = false;
    }
    
    // Handle the system modifier keys
#if (__APPLE__ && __MACH__) // MacOS
    
    switch (key) {
    case (int)KeyCode:: leftShift  : NS_CITY_BUILDER Input::_systemKeys[0] = false; break;
    case (int)KeyCode:: leftCommand: NS_CITY_BUILDER Input::_systemKeys[1] = false; break;
    case (int)KeyCode:: leftControl: NS_CITY_BUILDER Input::_systemKeys[2] = false; break;
    case (int)KeyCode:: leftOption : NS_CITY_BUILDER Input::_systemKeys[3] = false; break;
    case (int)KeyCode::rightShift  : NS_CITY_BUILDER Input::_systemKeys[4] = false; break;
    case (int)KeyCode::rightCommand: NS_CITY_BUILDER Input::_systemKeys[5] = false; break;
    case (int)KeyCode::rightControl: NS_CITY_BUILDER Input::_systemKeys[6] = false; break;
    case (int)KeyCode::rightOption : NS_CITY_BUILDER Input::_systemKeys[7] = false; break;
    }
      
#endif
  } break;
  
  default:
    break;
  }
}

void Events::inputChange(Input &input) {
  switch (input.type) {
  case Input::Type::keyboard:
  case Input::Type::mouseDrag:
    break;
  
  case Input::Type::mouseScroll: {
    // There are two times when scrolling is used: UI and to operate the scene
    // camera
    
    // Check if the camera can be orbited
    if (true) {
      // Rotate the camera
      Game::instance().mainCamera().rotate(
        input.mouseScroll.x * NS_CITY_BUILDER Input::_scrollSensitivity.x,
        input.mouseScroll.y * NS_CITY_BUILDER Input::_scrollSensitivity.y
      );
    }
  } break;
  
  case Input::Type::mousePinch: {
    // Check if the camera can be zoomed
    if (true) {
      // Zoom the camera
      Game::instance().mainCamera().zoom(
        -input.mousePinch * NS_CITY_BUILDER Input::_pinchSensitivity
      );
    }
  } break;
  }
}
