cmake_minimum_required(VERSION 3.0.0)
project(CityBuilder VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)
find_package(OGRE REQUIRED COMPONENTS Bites CONFIG)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(CityBuilder main.cpp)
target_link_libraries(CityBuilder OgreBites ${SDL2_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
