cmake_minimum_required(VERSION 3.0.0)
project(CityBuilder VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

# Import Expect
include(FetchContent)
FetchContent_Declare(
  Expect
  GIT_REPOSITORY https://github.com/MichaelBykov/Expect.git
  GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(Expect)

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
  "source/Geometry/Path2.cpp"
  "source/Geometry/RadiusPath2.cpp"
  "source/Geometry/Profile.cpp"
  "source/Geometry/Ray3.cpp"
  "source/Tools/Markup.cpp"
  "source/Zones/ZoneDef.cpp"
  "source/Roads/LaneDef.cpp"
  "source/Roads/RoadDef.cpp"
  "source/Roads/Road.cpp"
  "source/Roads/Intersection.cpp"
  "source/Roads/RoadNetwork.cpp"
  "source/UI/System.cpp"
  "source/UI/Primitive/Node.cpp"
  "source/UI/Primitive/Rectangle.cpp"
  "source/UI/Primitive/Rounded.cpp"
  "source/UI/Primitive/Character.cpp"
  "source/UI/Element.cpp"
  "Source/Rendering/Camera.cpp"
  "Source/Rendering/OrbitalCamera.cpp"
  "Source/Rendering/Mesh.cpp"
  "Source/Rendering/ColorMesh.cpp"
  "Source/Rendering/DynamicMesh.cpp"
  "Source/Rendering/UIMesh.cpp"
  "Source/Rendering/Program.cpp"
  "Source/Rendering/Texture.cpp"
  "Source/Rendering/Uniforms.cpp"
  "Source/Rendering/Object.cpp"
  "Source/Game.cpp"
  "Source/Input.cpp"
  source/Events.cpp
)

add_executable(CityBuilderTests
  "tests/Storage/List.cpp"
)
target_link_libraries(CityBuilderTests CityBuilder AutoExpect)

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
  
  function(compile_texture output)
    cmake_parse_arguments(PARSE_ARGV 1 COMPILE_TEXTURE "" "" "")
    add_custom_command(
      OUTPUT ${output}
      COMMAND texturec
        -f "${CMAKE_CURRENT_SOURCE_DIR}/${COMPILE_TEXTURE_UNPARSED_ARGUMENTS}"
        -o ${output}
        -t RGBA8
        --as dds
        -m
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${COMPILE_TEXTURE_UNPARSED_ARGUMENTS}"
      VERBATIM
    )
  endfunction()
  
  function(compile_ui_texture output)
    cmake_parse_arguments(PARSE_ARGV 1 COMPILE_TEXTURE "" "" "")
    add_custom_command(
      OUTPUT ${output}
      COMMAND texturec
        -f "${CMAKE_CURRENT_SOURCE_DIR}/${COMPILE_TEXTURE_UNPARSED_ARGUMENTS}"
        -o ${output}
        -t RGBA8
        --as dds
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${COMPILE_TEXTURE_UNPARSED_ARGUMENTS}"
      VERBATIM
    )
  endfunction()
  
  set(CMAKE_CXX_FLAGS "-ObjC++")
  
  compile_shader(vertex.shader VERTEX shaders/vertex.sc)
  compile_shader(fragment.shader FRAGMENT shaders/fragment.sc)
  compile_shader(ui.vertex.shader VERTEX shaders/ui.vertex.sc)
  compile_shader(ui.fragment.shader FRAGMENT shaders/ui.fragment.sc)
  compile_shader(hover.vertex.shader VERTEX shaders/hover.vertex.sc)
  compile_shader(hover.fragment.shader FRAGMENT shaders/hover.fragment.sc)
  compile_shader(zone.vertex.shader VERTEX shaders/zone.vertex.sc)
  compile_shader(zone.fragment.shader FRAGMENT shaders/zone.fragment.sc)
  compile_texture(grass.texture media/grass-tmp.jpg)
  set(RESOURCE_FILES
    vertex.shader
    fragment.shader
    ui.vertex.shader
    ui.fragment.shader
    hover.vertex.shader
    hover.fragment.shader
    zone.vertex.shader
    zone.fragment.shader
    grass.texture
  )
  
  set(RESOURCES_ROADS
    "roads/roadway.lane"
    "roads/sidewalk.lane"
    "roads/single.road"
    "roads/highway.road"
  )
  set_source_files_properties(${RESOURCES_ROADS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/roads)
  
  set(RESOURCES_ZONES
    "zones/residential.zone"
    "zones/commercial.zone"
    "zones/industrial.zone"
  )
  set_source_files_properties(${RESOURCES_ZONES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/zones)
  
  compile_texture(pavement.texture     media/pavement.png)
  compile_texture(falloff.texture      media/falloff.png)
  compile_texture(lane-markers.texture media/lane-markers.png)
  compile_texture(sidewalk.texture     media/sidewalk.png)
  compile_texture(zone.texture         media/zone.png)
  set(RESOURCES_TEXTURES
    pavement.texture
    falloff.texture
    lane-markers.texture
    sidewalk.texture
    zone.texture
  )
  set_source_files_properties(${RESOURCES_TEXTURES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/textures)
  
  # UI textures
  compile_ui_texture(round.texture          textures/ui/round.png)
  compile_ui_texture(square.texture         textures/ui/square.png)
  compile_ui_texture(bulldozer-icon.texture textures/ui/bulldozer-icon.png)
  compile_ui_texture(road-icon.texture      textures/ui/road-icon.png)
  compile_ui_texture(zone-icon.texture      textures/ui/zone-icon.png)
  compile_ui_texture(font.texture           textures/ui/font.png)
  set(RESOURCES_UI_TEXTURES
    round.texture
    square.texture
    bulldozer-icon.texture
    road-icon.texture
    zone-icon.texture
    font.texture
  )
  set_source_files_properties(${RESOURCES_UI_TEXTURES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/ui)
  
  add_executable(CityBuilderDriver MACOSX_BUNDLE
    driver/MacOS.mm
    driver/main.cpp
    ${RESOURCE_FILES}
    ${RESOURCES_ROADS}
    ${RESOURCES_ZONES}
    ${RESOURCES_TEXTURES}
    ${RESOURCES_UI_TEXTURES}
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
