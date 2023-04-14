cmake_minimum_required(VERSION 3.0.0)
project(CityBuilder VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

find_package(bgfx REQUIRED)

include_directories(include ${BGFX_INCLUDE_PATH})

find_library(BGFX NAMES bgfx)
find_library(BIMG NAMES bimg)
find_library(BX NAMES bx)

add_library(CityBuilder
  "source/Storage/Check.cpp"
  "source/Storage/String.cpp"
  # "source/Geometry/Path2.cpp"
  # "source/Geometry/Internal.cpp"
  # "source/Geometry/Profile.cpp"
  # "source/Geometry/Mesh.cpp"
  # "source/Geometry/SharedMesh.cpp"
  # "source/Tools/Markup.cpp"
  # "source/Building/LaneDef.cpp"
  # "source/Building/RoadDef.cpp"
  # "source/Building/Road.cpp"
  # "Source/Camera.cpp"
  # "Source/Game.cpp"
  # "Source/Input.cpp"
  source/Events.cpp
)

if(APPLE)
  # MacOS
  
  set(CMAKE_CXX_FLAGS "-ObjC++")
  
  add_executable(CityBuilderDriver MACOSX_BUNDLE
    driver/MacOS.mm
    driver/main.cpp
  )
  
  # MacOS runtimes
  target_link_libraries(CityBuilderDriver
    "-framework ServiceManagement"
    "-framework Foundation"
    "-framework Cocoa"
    "-framework Metal"
    "-framework IOKit"
    "-framework QuartzCore"
  )

  target_link_libraries(CityBuilderDriver CityBuilder ${BGFX} ${BIMG} ${BX})
  target_precompile_headers(CityBuilder PRIVATE "$<$<COMPILE_LANGUAGE:OBJCXX>:include/CityBuilder/Common.h>")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
