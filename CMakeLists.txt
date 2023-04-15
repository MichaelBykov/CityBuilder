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
  "Source/Rendering/Camera.cpp"
  "Source/Rendering/OrbitalCamera.cpp"
  "Source/Game.cpp"
  # "Source/Input.cpp"
  source/Events.cpp
)

if(APPLE)
  # MacOS
  
  function(compile_shader output)
    cmake_parse_arguments(PARSE_ARGV 1 COMPILE_SHADER "VERTEX;FRAGMENT" "" "")
    
    if(COMPILE_SHADER_VERTEX)
      add_custom_command(
        OUTPUT ${output}
        COMMAND shaderc
          -f "${CMAKE_CURRENT_SOURCE_DIR}/${COMPILE_SHADER_UNPARSED_ARGUMENTS}"
          -o ${output}
          --type v
          --platform osx
          --profile metal
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${COMPILE_SHADER_UNPARSED_ARGUMENTS}"
        VERBATIM
      )
    elseif(COMPILE_SHADER_FRAGMENT)
      add_custom_command(
        OUTPUT ${output}
        COMMAND shaderc
          -f "${CMAKE_CURRENT_SOURCE_DIR}/${COMPILE_SHADER_UNPARSED_ARGUMENTS}"
          -o ${output}
          --type f
          --platform osx
          --profile metal
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${COMPILE_SHADER_UNPARSED_ARGUMENTS}"
        VERBATIM
      )
    endif()
  endfunction()
  
  set(CMAKE_CXX_FLAGS "-ObjC++")
  
  compile_shader(vertex.shader VERTEX shaders/vertex.sc)
  compile_shader(fragment.shader FRAGMENT shaders/fragment.sc)
  set(RESOURCE_FILES
    vertex.shader
    fragment.shader
  )
  
  add_executable(CityBuilderDriver MACOSX_BUNDLE
    driver/MacOS.mm
    driver/main.cpp
    ${RESOURCE_FILES}
  )
  set_target_properties(CityBuilderDriver PROPERTIES
    RESOURCE "${RESOURCE_FILES}"
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
