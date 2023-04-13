/**
 * @file SharedMesh.cpp
 * @brief Implement the shared mesh interface.
 * @date April 13, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/SharedMesh.h>
#include <CityBuilder/Geometry/Internal.h>
#include <CityBuilder/Units/Angle.h>
USING_NS_CITY_BUILDER
using namespace GeometryInternal;

SharedMesh::SharedMesh() {
  // Create the shared mesh
  _mesh = Ogre::MeshManager::getSingleton().createManual(
    uniqueName(), "General");
}

SharedMesh::~SharedMesh() {
  /*
  // Destroy all the mesh materials
  for (Ogre::SubMesh *sub : _mesh->getSubMeshes())
    Ogre::MaterialManager::getSingleton().remove(sub->getMaterialName());
  
  // Destroy the mesh itself
  Ogre::MeshManager::getSingleton().remove(_mesh->getHandle());
  */
}



SharedMesh &SharedMesh::addSubMesh(
  const List<Vertex> &vertices,
  const List<int> &indices,
  const String &textureName,
  Real2 textureScale
) {
  // Create the sub-mesh
  Ogre::SubMesh *sub = _mesh->createSubMesh();
  
  // Create the vertex data
  sub->useSharedVertices = false;
  sub->vertexData = new Ogre::VertexData();
  sub->vertexData->vertexStart = 0;
  sub->vertexData->vertexCount = vertices.count();
  
  // Create the vertex declaration
  Ogre::VertexDeclaration *decl = sub->vertexData->vertexDeclaration;
  decl->addElement(0, 0 * 4, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
  decl->addElement(0, 3 * 4, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
  decl->addElement(0, 6 * 4, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
  
  // Create the vertex buffer
  Ogre::HardwareVertexBufferSharedPtr vertexBuffer =
    Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
      decl->getVertexSize(0), sub->vertexData->vertexCount,
      Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
  sub->vertexData->vertexBufferBinding->setBinding(0, vertexBuffer);
  
  // Copy the vertex data
  vertexBuffer->writeData(0, vertexBuffer->getSizeInBytes(), &vertices[0], true);
  
  // Initialize the index data
  sub->indexData->indexStart = 0;
  sub->indexData->indexCount = indices.count();
  
  // Create the index buffer
  Ogre::HardwareIndexBufferSharedPtr indexBuffer =
    Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
      Ogre::HardwareIndexBuffer::IT_32BIT, sub->indexData->indexCount,
      Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
  sub->indexData->indexBuffer = indexBuffer;
  
  // Copy the index data
  indexBuffer->writeData(0, indexBuffer->getSizeInBytes(), &indices[0], true);
  
  // Create the material
  Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
    uniqueName(), "General");
  auto *texture = material->getTechnique(0)->getPass(0)->createTextureUnitState(
    (const char *)textureName);
  texture->setTextureScale(textureScale.x, textureScale.y);
  sub->setMaterial(material);
  
  // Add to the bounding box
  if (_mesh->getNumSubMeshes() == 1) {
    // First mesh
    _bounds.setMinimum(vertices[0].position);
    _bounds.setMaximum(vertices[0].position);
  }
  for (const Vertex &vertex : vertices)
    _bounds.merge(vertex.position);
  
  // Done
  return *this;
}

SharedMesh &SharedMesh::addRevolution(
  Revolution revolution,
  const String &textureName,
  Real2 textureScale
) {
  return addRevolutions({ revolution }, textureName, textureScale);
}

namespace {
  // 17 vectors for a semi-circle
  Real2 vectors[] {
    { 0, 1 },
    { cos(80_deg), sin(80_deg) },
    { cos(70_deg), sin(70_deg) },
    { cos(60_deg), sin(60_deg) },
    { cos(50_deg), sin(50_deg) },
    { cos(40_deg), sin(40_deg) },
    { cos(30_deg), sin(30_deg) },
    { cos(20_deg), sin(20_deg) },
    { cos(10_deg), sin(10_deg) },
    { 1, 0 },
    { cos(10_deg), -sin(10_deg) },
    { cos(20_deg), -sin(20_deg) },
    { cos(30_deg), -sin(30_deg) },
    { cos(40_deg), -sin(40_deg) },
    { cos(50_deg), -sin(50_deg) },
    { cos(60_deg), -sin(60_deg) },
    { cos(70_deg), -sin(70_deg) },
    { cos(80_deg), -sin(80_deg) },
    { 0, -1 },
  };
}

SharedMesh &SharedMesh::addRevolutions(
  List<Revolution> revolutions,
  const String &textureName,
  Real2 textureScale
) {
  List<Vertex> vertices;
  List<int> triangles;
  int indexOffset = 0;
  
  for (const Revolution &ext : revolutions) {
    // Revolve the profile
    for (int i = 0; i < 17; i++) {
      // Get the current vector
      Real2 vector = vectors[i];
      
      // Add the vertices
      for (const ProfileMesh::Vertex &vertex : ext.profile.vertices) {
        Real2 norm = vector * vertex.normal.x;
        vertices.append({
          Real3(0, (ext.offset.y + vertex.position.y) * ext.scale, 0) +
          Real3(vector.x, 0, vector.y) *
          (vertex.position.x + ext.offset.x) * ext.scale,
          { norm.x, vertex.normal.y, norm.y },
          { vertex.uv, (Real)i / 16 }
        });
      }
      
      if (i > 0) {
        // Connect triangles with the previous revolution
        int prev = indexOffset + (i - 1) * ext.profile.vertices.count();
        int curr = prev + ext.profile.vertices.count();
        for (intptr_t j = 0; j < ext.profile.triangles.count(); j += 2) {
          triangles.append(prev + ext.profile.triangles[j    ]);
          triangles.append(curr + ext.profile.triangles[j    ]);
          triangles.append(prev + ext.profile.triangles[j + 1]);
          
          triangles.append(curr + ext.profile.triangles[j    ]);
          triangles.append(curr + ext.profile.triangles[j + 1]);
          triangles.append(prev + ext.profile.triangles[j + 1]);
        }
      }
    }
    
    indexOffset = vertices.count();
  }
  
  // Add the sub-mesh
  return addSubMesh(vertices, triangles, textureName, textureScale);
}

void SharedMesh::finish() {
  // Update the bounding box
  _mesh->_setBounds(_bounds);
  _mesh->_setBoundingSphereRadius(
    (_bounds.getCenter() - _bounds.getMaximum()).length());
  
  // Update the mesh
  _mesh->load();
}



SharedMesh::Instance::Instance(const SharedMesh &mesh) {
  _entity = scene->createEntity(mesh._mesh);
  _node = scene->getRootSceneNode()->createChildSceneNode();
  _node->attachObject(_entity);
}

SharedMesh::Instance::~Instance() {
  // _node->detachObject(_entity);
  // scene->destroyEntity(_entity);
  scene->destroySceneNode(_node);
}

SharedMesh::Instance &SharedMesh::Instance::setPosition(Real3 position) {
  _node->setPosition(position);
  return *this;
}

SharedMesh::Instance &SharedMesh::Instance::setOrientation(Ogre::Quaternion orientation) {
  _node->setOrientation(orientation);
  return *this;
}

Ref<SharedMesh::Instance &> SharedMesh::instantiate() {
  return new Instance(*this);
}
