/**
 * @file Driver.h
 * @brief The main program driver.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Events.h>

NS_CITY_BUILDER_BEGIN
namespace Driver {

/// A platform-specific main driver.
/// \remarks
///   Call from your program's main to start the Renderer main loop.
void main();

bool loadResource(
  const char  *name     ,
  const char  *extension,
        char **contents ,
  size_t      *length
);

} // namespace Driver
NS_CITY_BUILDER_END
