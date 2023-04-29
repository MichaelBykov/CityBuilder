# PA 9

## Team
- Michael Bykov (Lab section 1)
- Ben Swanzey (Lab section 12)



## Project Description

As we have it currently, the object of the application is to plan out a city
by defining what the underlying network of roads there are in the city and the
zoning rules for the structures around those roads.

Roads can be drawn freely, including with curves.
There is a central highway that can be connected to.
Zones can be defined for a variety of use cases, currently: residential,
commercial, and industrial.

Camera movement is through the WASD, arrow keys, scrolling, and pinch-to-zoom
(MacOS has good trackpad support).
The input system is designed such that the input keys can easily be reconfigured
if the user want's to control the program in a different way.

The 1-5 keys are quick actions for performing the following:
1. begin drawing a single-lane road.
2. begin drawing a highway.
3. begin zoning for residential buildings.
3. begin zoning for commercial buildings.
3. begin zoning for industrial buildings.

Roads can be drawn by clicking the starting point and clicking the end point.
If a curve is to be drawn, after click the the start point, the shift key can be
held to draw the direction in which the curve will go and then the end point.
Zones are set by clicking on the zone tiles next to roads.
Zones can be overwritten freely.

Drawing a road over other roads will create intersections as appropriate.

If you have any questions about any part of the project feel free to contact us
and we will be able to explain and/or show anything to you.



## Building

This project only has two direct dependencies:
BGFX (low-level wrapper around graphics APIs)
  - https://github.com/bkaradzic/bgfx
  - Used to provide a cross-platform singular interface for graphical
    operations, writing shaders, etc.
  - Needs to be built and libraries/headers installed in the standard system
    directories.
Expect (Unit testing library)
  - https://github.com/MichaelBykov/Expect
  - Used for the unit testing code.
  - Installation managed by CMake.

After BGFX and is installed, the project can simply be built using CMake with
the CMakeLists.txt at the root directory.

Note that we currently only have a program driver implemented for MacOS, and
as such it wont be able to run on or build for any other OS.


## UI

The UI is split into two categories of renderables: Primitives and Elements.

A primitive is any class of UI node that is based off the abstract class Node.
Primitives are responsible for setting basic attributes such as dimensions, position,
texture, and the representative mesh. Examples of primitivess are a rounded rectangle,
a regular rectangle, and a text character.

Elements are a higher-order class that manage UI primitives. Elements are concerned with
managing child-parent node relationships, and their visual representations (textures).

The default behavior of child nodes is to render in a linear column row. The parent node
does calculations to figure out what it's bounds are when properties such as padding and
border are present. Using the calculated bounds, it lays offsets each child's position.

There is a texture manager that takes loaded textures and a representative texture key that
can be passed to Nodes in order to draw things like icons and background images. The standard
texture used by a node depends on what type it is. For example, the rounded rectangle uses 
the rounded texture, and the rectangle takes the rectangle texture. When using the element class,
it can seamlessly switch which type of Node it's using under the hood, during runtime. If you
have a white perfect square, you could set the radius at and it handles all the texture
complexities on your behalf.



## Project Structure

- driver/...
  - The code defining the main program driver, window operations, events, and
    setting up BGFX.
    There is currently only one driver implemented, just for MacOS.
  - main.cpp : The main function which starts the program driver.
  - Driver.h : The interface for the program driver.
  - MacOS.mm : The MacOS program driver (in Objective C++ because that is what
    Mac requires).
- include/CityBuilder/...
  - The definition of the interface for the program.
  - Common.h : A set of common definitions, used as a precompiled header.
  - Events.h : A definition of the events delegated from the program driver.
  - Input.h : The interface for the input system.
  - Game.h : The interface for the main game/application code.
  - Storage/...
    - A variety of supporting storage types.
    - Check.h : A memory integrity validation utility.
    - Exceptions.h : A set of standard exception types.
    - String.h : A UTF-8 string class.
    - List.h : A standard array list.
    - Stack.h : An array-list stack.
    - Optional.h : An optional (nullable) type.
    - Ref.h : A generic reference counter.
    - Map.h : A hash map.
    - BSTree.h|ipp : An AVL binary search tree.
  - Tools/...
    - Another set of utilities that are not storage.
    - Markup.h|ipp : A fully statically-type-safe parser for a custom markup
      file format we use in this project.
  - Units/...
    - Supporting types that are not storage related.
    - Angle.h : A wrapper for interfacing with angles.
  - Rendering/...
    - The various types and functions used for rendering.
    - Camera.h : A simple camera for rendering a scene.
    - OrbitalCamera.h : A wrapper around the camera defining "orbital" movement.
    - Resource.h : A reference counter for resources.
    - Uniforms.h : A set of standard shared GPU uniforms used throughout the
      project.
    - Program.h : The interface for a GPU shader program.
    - Texture.h : An interface for loading and handling textures.
    - Material.h : A set of arguments for a shader.
    - Object.h : A wrapper around several types providing a simple renderable
      object (mesh + material)
    - Mesh.h : An interface for creating, loading, and managing a simple mesh.
    - ColorMesh.h : Same as Mesh.h but with vertex colors.
    - UIMesh.h : Same as Mesh.h but specialized for UI.
    - DynamicMesh.h : Same as Mesh.h but for meshes with dynamic buffers that
      can be updated frame-by-frame (whereas Mesh.h is static).
    - DistanceLight.h : An interface for a light in the far distance
      (think sun).
  - Geometry/...
    - A variety of geometric types.
    - Path2.h : A set of 2D paths (line, cubic Bezier curve).
    - Bounds2.h : A 2D bounding box.
    - RadiusPath2.h : A 2D path with a radius around it
      (think line with thickness).
    - Ray3.h : A 3D ray (used for determining where the mouse is).
    - Profile.h : A 2D cross section of an object which can be extruded along a
      path to produce a 3D mesh.
  - Roads/...
    - A set of utilities to define roads.
    - LaneDef.h : A definition of a single lane in a road.
    - RoadDef.h : A definition of a road type (collection of lanes).
    - Road.h : An actual instance of a road along a path, referencing a RoadDef.
    - Intersection.h : A definition of an intersection of roads.
    - Connection.h : A description of the connection between different
      roads/intersections
    - RoadNetwork.h : Essentially a road manager, defines the network of roads,
      handles validation and building, and renders the road appropriately.
  - Zones/...
    - Defines building zones.
    - ZoneDef.h : A description of a zone.
  - UI/...
    - A UI interface.
    - 
- source/...
  - The actual C++ source of the project.
    Very similar in structure to the interface so I will not go over everything,
    just what's different
  - Game/...
    - A set of utilities for controlling the what the user is currently doing.
      (directly included in Game.cpp)
    - RoadBuilding.ipp : Implements road building by the user.
    - Zoning.ipp : Implements zoning by the user.
  - Geometry/Intersection Table.ipp :
    (directly included in Path2.cpp)
    A set of functions for computing all the various curve-curve intersections
    there are (line-line, line-Bezier, Bezier-Bezier).
- tests
  - A set of unit tests (we were time-constrained)
  - Storage/List.cpp : Tests the very widely-used List class.
- tools
  - A set of helper tools we made
  - meta2mtl
    - A conversion tool for making a material from metadata that we have with
      some of the models we have
- textures
  - A set of textures used in the game
- media
  - Same as above but we never moved the textures after switching to BGFX from
    Ogre3D which was not a good tool for the project.
- models
  - A set of 3D models that we didn't have time to integrate into the project.
- shaders
  - The shaders used in the project.
- roads
  - The definitions for roads and lanes, designed to be easily configurable and
    expandable.
    files are in our custom markup.
- zones
  - same as above but for zones.
