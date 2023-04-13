/**
 * @file Mesh.cpp
 * @brief Implement the mesh interface.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/Mesh.h>
USING_NS_CITY_BUILDER

namespace {
  /// The global scene manager.
  Ogre::SceneManager *scene;
  
  /// An index for the unique name generator.
  int uniqueNameIndex = 0;
  
  /// Generate a unique name to use with Ogre.
  Ogre::String uniqueName() {
    return "CITY_BUILDER_" + std::to_string(uniqueNameIndex++);
  }
}

void Mesh::setSceneManager(Ogre::SceneManager *sceneManager) {
  scene = sceneManager;
}



Mesh::Mesh() {
  // Create the mesh and its associated data
  _mesh = Ogre::MeshManager::getSingleton().createManual(
    uniqueName(), "General");
  _entity = scene->createEntity(_mesh);
  _node = scene->getRootSceneNode()->createChildSceneNode();
  _node->attachObject(_entity);
}

Mesh::~Mesh() {
  // Destroy all the mesh materials
  for (Ogre::SubMesh *sub : _mesh->getSubMeshes())
    Ogre::MaterialManager::getSingleton().remove(sub->getMaterialName());
  
  // Destroy the mesh and its associated data
  _node->detachObject(_entity);
  scene->destroyEntity(_entity);
  scene->destroySceneNode(_node);
  Ogre::MeshManager::getSingleton().remove(_mesh->getHandle());
}



Mesh &Mesh::addSubMesh(
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

Mesh &Mesh::addExtrusion(
  const ProfileMesh &profile,
  Path2 &path,
  const Real2 offset,
  Real scale,
  const String &textureName,
  Real2 textureScale
) {
  // Get the path points
  List<Real4> points = path.pointNormals();
  
  if (points.count() < 2)
    // Nothing to extrude over
    return *this;
  
  // Extrude the profile
  List<Vertex> vertices;
  List<int> triangles;
  for (intptr_t i = 0; i < points.count(); ++i) {
    // Get the current point data
    const Real4 &pointNormal = points[i];
    Real2 point  = { pointNormal.x, pointNormal.y };
    Real2 normal = { pointNormal.z, pointNormal.w };
    
    // Add the vertices
    for (const ProfileMesh::Vertex &vertex : profile.vertices) {
      Real2 norm = normal * vertex.normal.x;
      vertices.append({
        Real3(point.x, (offset.y + vertex.position.y) * scale, point.y) +
        Real3(normal.x, 0, normal.y) * (vertex.position.x + offset.x) * scale,
        { norm.x, vertex.normal.y, norm.y },
        { vertex.uv, i / (Real)(points.count() - 1) }
      });
    }
    
    if (i > 0) {
      // Connect triangles with the previous extrusion
      int prev = (i - 1) * profile.vertices.count();
      int curr = prev + profile.vertices.count();
      for (intptr_t j = 0; j < profile.triangles.count(); j += 2) {
        triangles.append(prev + profile.triangles[j    ]);
        triangles.append(curr + profile.triangles[j    ]);
        triangles.append(prev + profile.triangles[j + 1]);
        
        triangles.append(curr + profile.triangles[j    ]);
        triangles.append(curr + profile.triangles[j + 1]);
        triangles.append(prev + profile.triangles[j + 1]);
      }
    }
  }
  
  // Add the sub-mesh
  return addSubMesh(vertices, triangles, textureName, textureScale);
}

void Mesh::finish() {
  // Update the bounding box
  _mesh->_setBounds(_bounds);
  _mesh->_setBoundingSphereRadius(
    (_bounds.getCenter() - _bounds.getMaximum()).length());
  
  // Update the mesh
  _mesh->load();
}