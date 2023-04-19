/**
 * @file MacOS.mm
 * @brief The MacOS program driver.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include "Driver.h"
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include <bgfx/platform.h>
USING_NS_CITY_BUILDER



// ===--- Program Driver ----------------------------------------------------===

/// A handler for window events.
@interface NotificationHandler : NSObject {
  
}

/// Invoked when the window becomes active again after it is paused.
/// \remarks
///   Ends the system program loop and restarts the renderer program loop.
- (void)becomeActive:(NSNotification *)notification;

/// Invoked whenever the window is resized.
- (void)resize:(NSNotification *)notification;

/// Invoked when the window is closed for program cleanup.
- (void)close:(NSNotification *)notification;

@end

@implementation NotificationHandler

- (void)becomeActive:(NSNotification *)notification {
  // Stop the native program loop which will give back control to our own
  // custom program loop
  [NSApp stop:nil];
}

- (void)resize:(NSNotification *)notification {
  // Update bgfx
  NSWindow *window = notification.object;
  bgfx::reset(
    window.frame.size.width  * window.backingScaleFactor,
    window.frame.size.height * window.backingScaleFactor,
    BGFX_RESET_VSYNC
  );
  bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);
  
  // Send the event to the driver
  Events::resize({
    window.frame.origin.x * window.backingScaleFactor,
    window.frame.origin.y * window.backingScaleFactor,
    window.frame.origin.x * window.backingScaleFactor + window.frame.size.width  * window.backingScaleFactor,
    window.frame.origin.y * window.backingScaleFactor + window.frame.size.height * window.backingScaleFactor
  });
}

- (void)close:(NSNotification *)notification {
  // Notify the driver that the program is about to end
  Events::stop();
  exit(0);
}

@end



namespace {
  
  void render() {
    // Clear the renderer
    bgfx::touch(0);
    
    // Let the driver do what it needs to do
    Events::update();
    
    // Submit the frame
    bgfx::frame();
  }
  
  void render_pause() {
    // Signal the pause
    Events::pause();
    bgfx::frame();
    // Flush out the back buffer
    Events::pause();
    bgfx::frame();
  }
  
  bool mousePosition(NSEvent *event, Real2 &position) {
    // Make sure we're only capturing in-window mouse movement
    if (event.window == Nil)
      return false;
    
    // Normalize the mouse position
    Real2 location = { event.locationInWindow.x, event.locationInWindow.y };
    NSRect frame = event.window.frame;
    if (location.x < 0 || location.y < 0 ||
      location.x > frame.size.width ||
      location.y > frame.size.height
    ) // Oustide the window
      return false;
    
    // Inverse the mouse y because we want 0 to be at the top
    location.y = frame.size.height - location.y;
    
    // Adjust for pixel density
    location *= Real2(event.window.backingScaleFactor);
    
    // Done
    position = location;
    return true;
  }
  
}



void Driver::main() {
  @autoreleasepool {
    // Setup the main window
    NSRect mainDisplayRect = [[NSScreen mainScreen] frame];
    NSRect windowRect = NSMakeRect(
      mainDisplayRect.origin.x + (mainDisplayRect.size.width ) * 0.25,
      mainDisplayRect.origin.y + (mainDisplayRect.size.height) * 0.25,
      mainDisplayRect.size.width  * 0.5,
      mainDisplayRect.size.height * 0.5
    );
    
    NSUInteger windowStyle = NSWindowStyleMaskTitled
                           | NSWindowStyleMaskClosable
                           | NSWindowStyleMaskMiniaturizable
                           | NSWindowStyleMaskResizable
                           | NSWindowStyleMaskFullSizeContentView
                           ;
    
    NSWindow *window = [[NSWindow alloc]
                    initWithContentRect:windowRect
                              styleMask:windowStyle
                                backing:NSBackingStoreBuffered
                                  defer:NO];
    
    window.titlebarAppearsTransparent = YES;
    [window makeKeyAndOrderFront:window];
    [NSApp activateIgnoringOtherApps:YES];
    
    
    
    // Setup bgfx
    bgfx::renderFrame();
    
    bgfx::PlatformData platform;
    platform.nwh = window;
    
    bgfx::Init init;
    init.platformData = platform;
    init.resolution.width  = windowRect.size.width  * window.backingScaleFactor;
    init.resolution.height = windowRect.size.height * window.backingScaleFactor;
    init.resolution.reset = BGFX_RESET_VSYNC;
    if (!bgfx::init(init))
      exit(1);
    
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);
    bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);
    
    
    
    // Setup the driver
    Events::start();
    Events::resize({
      windowRect.origin.x * window.backingScaleFactor,
      windowRect.origin.y * window.backingScaleFactor,
      windowRect.origin.x * window.backingScaleFactor + windowRect.size.width  * window.backingScaleFactor,
      windowRect.origin.y * window.backingScaleFactor + windowRect.size.height * window.backingScaleFactor
    });
    
    
    
    // Setup application notifications
    NotificationHandler *handler = [[NotificationHandler alloc] init];
    
    // Restart the render loop once the window is "unpaused" (the application
    // becomes active again)
    [[NSNotificationCenter defaultCenter]
      addObserver:handler
         selector:@selector(becomeActive:)
             name:NSApplicationDidBecomeActiveNotification
           object:NSApp
    ];
    // Resize the graphics device as necessary
    [[NSNotificationCenter defaultCenter]
      addObserver:handler
         selector:@selector(resize:)
             name:NSWindowDidResizeNotification
           object:window
    ];
    // "x" button
    [[NSNotificationCenter defaultCenter]
      addObserver:handler
         selector:@selector(close:)
             name:NSWindowWillCloseNotification
           object:window
    ];
    
    
    
    // Main event loop
    Events::Input input = {};
    Real2 mouse;
    for (;;) {
      @autoreleasepool {
        for (;;) {
          NSEvent* event = [NSApp
            nextEventMatchingMask:NSEventMaskAny
                        untilDate:[NSDate distantPast]
                           inMode:NSDefaultRunLoopMode
                          dequeue:YES
          ];
          if (event == nil)
            break;
          
          // Pass on relevant events to the driver
          switch (event.type) {
          case NSEventTypeKeyDown:
            input = {
              { .keyboard = { event.keyCode } },
              Events::Input::Type::keyboard
            };
            Events::inputStart(input);
            break;
          
          case NSEventTypeKeyUp:
            input = {
              { .keyboard = { event.keyCode } },
              Events::Input::Type::keyboard
            };
            Events::inputStop(input);
            break;
          
          case NSEventTypeLeftMouseDragged:
            if (mousePosition(event, mouse)) {
              input = {
                { .mouseDrag = {
                  .position = mouse,
                  .button = 0
                } },
                Events::Input::Type::mouseDrag
              };
              Events::inputChange(input);
            }
            break;
          
          case NSEventTypeRightMouseDragged:
            if (mousePosition(event, mouse)) {
              input = {
                { .mouseDrag = {
                  .position = mouse,
                  .button = 1
                } },
                Events::Input::Type::mouseDrag
              };
              Events::inputChange(input);
            }
            break;
          
          case NSEventTypeLeftMouseDown:
            input = {
              { .mouseButton = 0 },
              Events::Input::Type::mouseButton
            };
            Events::inputStart(input);
            break;
          
          case NSEventTypeRightMouseDown:
            input = {
              { .mouseButton = 1 },
              Events::Input::Type::mouseButton
            };
            Events::inputStart(input);
            break;
          
          case NSEventTypeLeftMouseUp:
            input = {
              { .mouseButton = 0 },
              Events::Input::Type::mouseButton
            };
            Events::inputStop(input);
            break;
          
          case NSEventTypeRightMouseUp:
            input = {
              { .mouseButton = 1 },
              Events::Input::Type::mouseButton
            };
            Events::inputStop(input);
            break;
          
          case NSEventTypeMouseMoved:
            if (mousePosition(event, mouse)) {
              input = {
                { .mousePosition = mouse },
                Events::Input::Type::mouseMove
              };
              Events::inputChange(input);
            }
            break;
          
          case NSEventTypeScrollWheel:
            input = {
              { .mouseScroll = Real2(event.scrollingDeltaX, event.scrollingDeltaY) },
              Events::Input::Type::mouseScroll
            };
            Events::inputChange(input);
            break;
          
          case NSEventTypeMagnify:
            input = {
              { .mousePinch = (Real)event.magnification },
              Events::Input::Type::mousePinch
            };
            Events::inputChange(input);
            break;
          
          default:
            break;
          }
          
          [NSApp sendEvent:event];
        }
      }
      
      if ([NSApp isActive])
        render();
      else {
        render_pause();
        // Make sure that we don't overflow the processesor with work when the
        // window isn't even being interacted with
        [NSApp run];
      }
    }
  }
}




// ===--- Resource Handling -------------------------------------------------===

bool Driver::loadResource(
  const char  *name     ,
  const char  *extension,
        char **contents ,
  size_t      *length
) {
  @autoreleasepool {
    NSString *_name      = [NSString stringWithCString:name
                                              encoding:NSUTF8StringEncoding];
    NSString *_extension = [NSString stringWithCString:extension
                                              encoding:NSUTF8StringEncoding];
    const char *_path = [[
        [[NSBundle mainBundle] URLForResource:_name withExtension:_extension]
        path
      ]
      cStringUsingEncoding:NSUTF8StringEncoding
    ];
    
    FILE *file = fopen(_path, "rb");
    if (file == NULL)
      return false;
    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    *contents = (char *)malloc(*length + 1);
    fread((void *)*contents, 1, *length, file);
    (*contents)[*length] = 0;
    fclose(file);
  }
  
  return true;
}
