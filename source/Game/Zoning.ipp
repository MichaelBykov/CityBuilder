/**
 * @file Zoning.ipp
 * @brief The zoning code for the game.
 * @date April 26, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Game.h>
#include <CityBuilder/Rendering/DynamicMesh.h>
USING_NS_CITY_BUILDER

namespace {
  ProfileMesh zoningProfile = {{
    ProfilePoint {
      .position = { 0, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0,
      .type = ProfilePoint::Type::move
    },
    ProfilePoint {
      .position = { 9, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 1,
      .type = ProfilePoint::Type::move
    },
  }};
}

namespace {
  struct Zoning {
    
    /// The ID of the listener for mouse clicks.
    int clickListener;
    
    /// The ID of the listener for cancel events.
    int cancelListener;
    
    /// The zone type being zoned.
    ZoneDef *zone;
    
    /// A display of the road being built.
    Resource<DynamicMesh> display;
    
    /// Whether or not the display is visible.
    bool displayVisible = false;
    
    /// The road being zoned.
    Road *road;
    
    /// The side of the road being zoned.
    bool side;
    
    
    
    Zoning(ZoneDef *zone) : zone(zone) {
      // Create the display
      display = new DynamicMesh();
      
      // Listen for cancel events
      cancelListener = Input::onCancel += [this]() {
        // Cancel the zoning
        Game::instance().cancel();
      };
      
      // Listen for mouse clicks
      clickListener = Input::onPrimaryMouseDown += [this]() {
        if (displayVisible) {
          RoadNetwork &network = Game::instance().roads();
          network.setZone(road, side, this->zone);
          network.update();
        }
      };
    }
    
    ~Zoning() {
      Input::onPrimaryMouseDown -= clickListener;
      Input::onCancel -= cancelListener;
    }
    
    void update(Road *road, bool side) {
      display->extrude(zoningProfile, road->path.path(), Color4(zone->color, 155), {
        (side ?
           road->definition->dimensions.x :
          -road->definition->dimensions.x - Real(18)
        ) * Real(0.5),
        0.15
      }, 0.333333333333);
      display->load();
      
      this->road = road;
      this->side = side;
    }
  };
}
