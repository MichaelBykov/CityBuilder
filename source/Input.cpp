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

KeyCode Input::_moveKeys[4] { (KeyCode)0 };

KeyCode Input::_rotateKeys[4] { (KeyCode)0 };

bool Input::_listenForAxes = false;

bool Input::_usedKeys[256] { false };

bool Input::_keysDown[256] { false };

Real Input::_scrollSensitivity = 0.5;

Real2 Input::_keyMoveSpeed = { 3, 3 };

Real2 Input::_keyOrbitSpeed = { 1, 1 };

Real2 Input::_mouseMoveSpeed = { 0.001, 0.001 };

Real2 Input::_mouseOrbitSpeed = { 0.0033, 0.0033 };

bool Input::_leftMouseDown = false;

bool Input::_rightMouseDown = false;

bool Input::_controlDown = false;

bool Input::_optionDown = false;





/* -------------------------------------------------------------------------- *\
|                                                                              |
| Configuration                                                                |
|                                                                              |
\* -------------------------------------------------------------------------- */

void Input::setMoveKeys(
  KeyCode forward, KeyCode backward, KeyCode left, KeyCode right
) {
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
  if (_keysDown[(int)_moveKeys[2]]) x--;
  if (_keysDown[(int)_moveKeys[3]]) x++;
  return Real2(x, y);
}

Real2 Input::getOrbitAxes() {
  int x = 0, y = 0;
  if (_keysDown[(int)_rotateKeys[0]]) y++;
  if (_keysDown[(int)_rotateKeys[1]]) y--;
  if (_keysDown[(int)_rotateKeys[2]]) x--;
  if (_keysDown[(int)_rotateKeys[3]]) x++;
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

Real Input::scrollSensitivity() {
  return _scrollSensitivity;
}

void Input::setScrollSensitivity(Real sensitivity) {
  _scrollSensitivity = sensitivity;
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

bool InputDelegate::axisMoved(const OgreBites::AxisEvent &event) {
  std::cout << "Axis moved: " << event.axis << " by " << event.value << std::endl;
  return false;
}

bool InputDelegate::keyPressed(const OgreBites::KeyboardEvent &event) {
  // Normalize the key
  int key = event.keysym.sym;
  if (1073741903 <= key && key <= 1073741906)
    // Arrow keys
    key -= 1073741903 - (int)KeyCode::right;
  
  if (Input::_listenForAxes && key < 256 && Input::_usedKeys[key]) {
    // Axis
    Input::_keysDown[key] = true;
    return true;
  }
  
  switch (event.keysym.sym) {
  case 1073742048: // MacOS-specific
    Input::_controlDown = true;
    return true;
  
  case 1073742050: // MacOS-specific
    Input::_optionDown = true;
    return true;
  
  case (int)KeyCode::_1: // For testing: road 1
    
    return true;
  }
  
  std::cout << "Key pressed: " << key << std::endl;
  return false;
}

bool InputDelegate::keyReleased(const OgreBites::KeyboardEvent &event) {
  // Normalize the key
  int key = event.keysym.sym;
  if (1073741903 <= key && key <= 1073741906)
    // Arrow keys
    key -= 1073741903 - (int)KeyCode::right;
  
  if (Input::_listenForAxes && key < 256 && Input::_usedKeys[key]) {
    // Axis
    Input::_keysDown[key] = false;
    return true;
  }
  
  switch (event.keysym.sym) {
  case 1073742048: // MacOS-specific
    Input::_controlDown = false;
    return true;
  
  case 1073742050: // MacOS-specific
    Input::_optionDown = false;
    return true;
  }
  
  std::cout << "Key released: " << key << std::endl;
  return false;
}

bool InputDelegate::mouseMoved(const OgreBites::MouseMotionEvent &event) {
  if (Input::_rightMouseDown || Input::_leftMouseDown && Input::_controlDown) {
    // Orbit the camera
    Game::instance().mainCamera().rotate(
      -event.xrel * Input::_mouseOrbitSpeed.x,
        event.yrel * Input::_mouseOrbitSpeed.y
    );
    return true;
  } else if (Input::_leftMouseDown && Input::_optionDown) {
    // Pan the camera
    Camera &camera = Game::instance().mainCamera();
    camera.slide(Real2(
      -event.xrel * Input::_mouseMoveSpeed.x,
        event.yrel * Input::_mouseMoveSpeed.y
    ) * camera.distance());
    return true;
  }
  return false;
}

bool InputDelegate::mousePressed(const OgreBites::MouseButtonEvent &event) {
  if (event.button == 1) {
    Input::_leftMouseDown = true;
    return true;
  } else if (event.button == 3) {
    Input::_rightMouseDown = true;
    return true;
  }
  return false;
}

bool InputDelegate::mouseReleased(const OgreBites::MouseButtonEvent &event) {
  if (event.button == 1) {
    Input::_leftMouseDown = false;
    return true;
  } else if (event.button == 3) {
    Input::_rightMouseDown = false;
    return true;
  }
  return false;
}

bool InputDelegate::mouseWheelRolled(const OgreBites::MouseWheelEvent &event) {
  // There are two times when scrolling is used: UI and to operate the scene
  // camera
  
  // Check if the camera can be zoomed
  if (true) {
    // Zoom the camera
    Game::instance().mainCamera().zoom(-event.y * Input::_scrollSensitivity);
    return true;
  }
  return false;
}

void InputDelegate::frameRendered(const Ogre::FrameEvent &event) {
  Game::instance().update(event.timeSinceLastFrame);
}
