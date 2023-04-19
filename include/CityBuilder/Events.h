/**
 * @file Events.h
 * @brief The interface for all the main game events.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>

NS_CITY_BUILDER_BEGIN
namespace Events {

/// Startup of the program driver.
void start();

/// The main program loop.
/// \remarks
///   Called every single frame.
///   Should produce something to render.
void update();

/// Called when the program is paused.
/// \remarks.
///  Should produce a pause screen to render.
void pause();

/// Called whenever the main window is resized.
/// \param rect
///   The window's new rect.
///   Can be passed directly to the camera.
void resize(Real4 rect);

/// End point of the program driver.
void stop();

/// An input event that occurred.
struct Input {
  /// The types of events that can occur.
  enum class Type : uint8_t {
    keyboard, //< A keyboard pressed/released event.
    mouseDrag, //< Fired when the mouse moves holding a button.
    mouseMove, //< Fired when the mouse moves.
    mouseScroll, //< Fired when the mouse wheel moves.
    mousePinch, //< Fired when the trackpad is pinched in or out.
    mouseButton, //< Fired when a mouse button is pressed or released.
  };
  
  union {
    /// The data for a keyboard event.
    /// \remarks
    ///   Should only be accessed when `type` is `Type::keyboard`.
    struct {
      /// The pressed/released key's unique code.
      unsigned short keyCode;
    } keyboard;
    /// The data for a mouse button event.
    /// \remarks
    ///   Should only be accessed when `type` is `Type::mouseButton`.
    struct {
      /// The pressed/released mouse button index.
      int button;
    } mouseButton;
    /// The data for a mouse drag event.
    /// \remarks
    ///   Should only be accessed when `type` is `Type::mouseDrag`.
    struct {
      /// The current position of the mouse.
      Real2 position;
      
      /// The mouse button that was held down.
      int button;
    } mouseDrag;
    /// The current position of the mouse.
    /// \remarks
    ///   Should only be accessed when `type` is `Type::mouseMove`.
    Real2 mousePosition;
    /// The amount the mouse wheel moved.
    /// \remarks
    ///   Should only be accessed when `type` is `Type::mouseScroll`.
    Real2 mouseScroll;
    /// The amount that the trackpad was pinched in (positive) or out
    /// (negative).
    /// \remarks
    ///   Should only be accessed when `type` is `Type::mousePinch`.
    Real mousePinch;
  };
  
  /// The type of event that occurred.
  Type type;
};

/// Called whenever an input state begins.
/// \param input
///   Information about the input state.
void inputStart(Input &input);

/// Called whenever an input state changes.
/// \param input
///   Information about the input state.
void inputChange(Input &input);

/// Called whenever an input state ends.
/// \param input
///   Information about the input state.
void inputStop(Input &input);

} // namespace Events
NS_CITY_BUILDER_END
