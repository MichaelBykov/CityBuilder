/**
 * @file Road.cpp
 * @brief A road in the game.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Building/Road.h>
USING_NS_CITY_BUILDER

namespace {
  int roadID = 0;
  
  Building::ProfileMesh dividerCrossTrafficMesh = {{
    Building::ProfilePoint {
      .position = { 0, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.05,
      .type = Building::ProfilePoint::Type::move
    },
    Building::ProfilePoint {
      .position = { 0.2, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.2,
      .type = Building::ProfilePoint::Type::move
    },
  }};
  
  Building::ProfileMesh dividerCrossEdgeMesh = {{
    Building::ProfilePoint {
      .position = { 0, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.3,
      .type = Building::ProfilePoint::Type::move
    },
    Building::ProfilePoint {
      .position = { 0.2, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.45,
      .type = Building::ProfilePoint::Type::move
    },
  }};
  
  Building::ProfileMesh dividerNormalMesh = {{
    Building::ProfilePoint {
      .position = { 0, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.55,
      .type = Building::ProfilePoint::Type::move
    },
    Building::ProfilePoint {
      .position = { 0.2, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.7,
      .type = Building::ProfilePoint::Type::move
    },
  }};
  
  Building::ProfileMesh dividerEdgeMesh = {{
    Building::ProfilePoint {
      .position = { 0, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.8,
      .type = Building::ProfilePoint::Type::move
    },
    Building::ProfilePoint {
      .position = { 0.2, 0 },
      .normal0 = { 0, 1 },
      .uv0 = 0.95,
      .type = Building::ProfilePoint::Type::move
    },
  }};
  
  Building::ProfileMesh *dividerMeshes[] {
    &dividerNormalMesh,
    &dividerEdgeMesh,
    &dividerCrossTrafficMesh,
    &dividerCrossEdgeMesh,
  };
}

Road::Road(RoadDef *definition, Path2 &path, Ogre::SceneManager *scene)
  : definition(definition), path(path) {
  String name = "GENERATED_ROAD_" + String(roadID++);
  
  // Create the mesh
  mesh = Ogre::MeshManager::getSingleton().createManual(
    (const char *)name, "General");
  Ogre::AxisAlignedBox bounds;
  
  int materialID = 0;
  
  // Extrude any decorations
  Real2 half = { -definition->decorations.dimensions.x / 2, 0 };
  if (!definition->decorations.triangles.isEmpty()) {
    Ogre::SubMesh *sub = mesh->createSubMesh();
    sub->useSharedVertices = false;
    bounds.merge(definition->decorations.extrude(
      path, half, sub, 0.1));
    
    // Add the appropriate material
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
      (const char *)(name + "_" + String(materialID++)),
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    Ogre::TextureUnitState* planeTexture =
      material->getTechnique(0)->getPass(0)->
      createTextureUnitState((const char *)definition->decorationsTexture);
    planeTexture->setTextureUScale(1);
    planeTexture->setTextureVScale(1 / path.length());
    
    sub->setMaterial(material);
  }
  
  // Extrude the lanes
  for (const RoadDef::Lane &lane : definition->lanes) {
    Ogre::SubMesh *sub = mesh->createSubMesh();
    sub->useSharedVertices = false;
    bounds.merge(
      lane.definition->profile.extrude(
        path, half + lane.position, sub, 0.1)
    );
    
    // Add the appropriate material
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
      (const char *)(name + "_" + String(materialID++)),
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    Ogre::TextureUnitState* texture =
      material->getTechnique(0)->getPass(0)->
      createTextureUnitState((const char *)lane.definition->mainTexture);
    texture->setTextureUScale(1);
    texture->setTextureVScale(1 / path.length());
    
    sub->setMaterial(material);
  }
  
  // Extrude the lane markings
  if (!definition->dividers.isEmpty()) {
    half.y += 0.01;
    half.x -= 0.1;
    for (const RoadDef::Divider &divider : definition->dividers) {
      Ogre::SubMesh *sub = mesh->createSubMesh();
      sub->useSharedVertices = false;
      
      bounds.merge(
        dividerMeshes[(int)divider.type]->
        extrude(path, half + divider.position, sub, 0.1)
      );
      
      // Add the appropriate material
      Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
        (const char *)(name + "_" + String(materialID++)),
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
      
      Ogre::TextureUnitState* texture =
        material->getTechnique(0)->getPass(0)->
        createTextureUnitState("lane-markers.png");
      texture->setTextureUScale(1);
      texture->setTextureVScale(0.5 / path.length());
      
      sub->setMaterial(material);
    }
  }
  
  mesh->_setBounds(bounds);
  mesh->_setBoundingSphereRadius((bounds.getMaximum() - bounds.getCenter()).length());
  mesh->load();
  
  // Create the entity and node
  entity = scene->createEntity(mesh);
  node = scene->getRootSceneNode()->createChildSceneNode();
  node->attachObject(entity);
}

Road::~Road() {
  Ogre::SceneManager *scene = node->getCreator();
  
  // Deallocate the mesh and node
  for (Ogre::SubMesh *sub : mesh->getSubMeshes()) {
    Ogre::MaterialManager::getSingleton().remove(
      sub->getMaterial()->getName());
    sub->getMaterial()->unload();
  }
  
  Ogre::MeshManager::getSingleton().remove(mesh);
  node->getParentSceneNode()->removeAndDestroyChild(node);
  scene->destroyEntity(entity);
  
  // Deallocate everything else
  delete &path;
}
