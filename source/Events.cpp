/**
 * @file Events.cpp
 * @brief The main event loop event handler.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Events.h>
USING_NS_CITY_BUILDER

void Events::start() {
  
}

void Events::stop() {
  
}

void Events::pause() {
  // Overlay a paused text over the scene
  bgfx::dbgTextClear();
  bgfx::dbgTextPrintf(4, 6, 0x0f, "PAUSED");
  bgfx::setDebug(BGFX_DEBUG_TEXT);
}

void Events::update() {
  
}

void Events::resize(Real4 rect) {
  
}

void Events::inputStart(Input &input) {
  
}

void Events::inputChange(Input &input) {
  
}

void Events::inputStop(Input &input) {
  
}
