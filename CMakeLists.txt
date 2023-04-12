cmake_minimum_required(VERSION 3.0.0)
project(CityBuilder VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)
find_package(OGRE REQUIRED COMPONENTS Bites CONFIG)

include_directories(${SDL2_INCLUDE_DIRS})

include_directories(include)

add_executable(CityBuilder
  "source/Storage/Check.cpp"
  "source/Storage/String.cpp"
  "source/Tools/Markup.cpp"
  "source/Building/Lane.cpp"
  "source/Building/Road.cpp"
  "Source/Camera.cpp"
  "Source/Game.cpp"
  "Source/Input.cpp"
  main.cpp
)
target_link_libraries(CityBuilder OgreBites ${SDL2_LIBRARIES})
target_precompile_headers(CityBuilder PRIVATE include/CityBuilder/Common.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
