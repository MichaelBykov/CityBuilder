/**
 * @file Profile.cpp
 * @brief Implement the profile mesh.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Building/Profile.h>
USING_NS_CITY_BUILDER
using namespace Building;

ProfileMesh::ProfileMesh(const List<ProfilePoint> &points) {
  for (intptr_t i = 0; i < points.count(); i++) {
    const ProfilePoint &point = points[i];
    switch (point.type) {
      case ProfilePoint::Type::move:
        // Add a single vertex
        vertices.append({ point.position, point.normal0, point.uv0 });
        if (i + 1 < points.count()) {
          if (i == 0 ||
            points[i].type != ProfilePoint::Type::move ||
            i > 0 && points[i - 1].type == ProfilePoint::Type::move)
            // Connect with the next vertex
            triangles.append(vertices.count() - 1).append(vertices.count());
        }
        break;
      
      case ProfilePoint::Type::disjoint:
        // Add two vertices
        vertices.append({ point.position, point.normal0, point.uv0 });
        vertices.append({ point.position, point.normal1, point.uv0 });
        
        // Connect with the next vertex
        triangles.append(vertices.count() - 1).append(vertices.count());
        break;
      
      case ProfilePoint::Type::connected:
        // Add a single vertex
        vertices.append({ point.position, point.normal0, point.uv0 });
        
        // Connect with the next vertex
        triangles.append(vertices.count() - 1).append(vertices.count());
        break;
    }
  }
  
  // Normalize all the vertices
  for (Vertex &vertex : vertices)
    vertex.normal.normalise();
}

Ogre::MeshPtr ProfileMesh::extrude(Path2 &path, const Real2 offset) {
  List<Real2> points = path.points();
  
  if (points.count() < 2)
    // Nothing to extrude over
    return nullptr;
  
  // Generate a set of normals for the path
  List<Real2> normals;
  for (intptr_t i = 0; i < points.count(); i++) {
    Real2 normal;
    if (i == 0)
      normal = points[i + 1] - points[i];
    else if (i == points.count() - 1)
      normal = points[i] - points[i - 1];
    else
      normal = points[i + 1] - points[i - 1];
    normal.normalise();
    normals.append({ normal.y, -normal.x });
  }
  
  // Allocate the vertices
  size_t vertexCount = points.count() * vertices.count();
  struct {
    Real3 position;
    Real2 normal;
    Real2 uv;
  } *verts = (decltype(verts))malloc(vertexCount * sizeof(*verts));
  size_t vertexIndex = 0;
  
  // Allocate the triangle indices
  size_t indexCount = 6 * triangles.count() * (points.count() - 1);
  int *tris = (int *)malloc(indexCount * sizeof(int));
  size_t triangleIndex = 0;
  
  // Extrude the profile
  for (intptr_t i = 0; i < points.count(); i++) {
    // Get the current point
    const Real2 &point = points[i];
    const Real2 &normal = normals[i];
    
    // Add the vertices
    for (const Vertex &vertex : vertices) {
      Real2 norm = normal * vertex.normal.x;
      verts[vertexIndex].position =
        Real3(point.x, point.y, offset.y + vertex.position.y) +
        Real3(normal.x, normal.y, 0) * (vertex.position.x + offset.x);
      verts[vertexIndex].normal = { norm.x, norm.y, vertex.normal.y };
      verts[vertexIndex].uv = { vertex.uv, i / (Real)(points.count() - 1) };
      vertexIndex++;
    }
    
    if (i > 0) {
      // Connect triangles with the previous extrusion
      int prev = (i - 1) * 6 * triangles.count();
      int curr = prev + 6 * triangles.count();
      for (intptr_t j = 0; j < triangles.count(); j += 2) {
        tris[triangleIndex++] = prev + triangles[j];
        tris[triangleIndex++] = curr + triangles[j];
        tris[triangleIndex++] = prev + triangles[j + 1];
        
        tris[triangleIndex++] = curr + triangles[j];
        tris[triangleIndex++] = curr + triangles[j + 1];
        tris[triangleIndex++] = prev + triangles[j + 1];
      }
    }
  }
  
  // Create the mesh
  Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton()
    .createManual("", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  Ogre::SubMesh *sub = mesh->createSubMesh();
  
  
  
  // Create the vertex data
  sub->vertexData->vertexStart = 0;
  sub->vertexData->vertexCount = points.count() * vertices.count();
  
  // Define the vertex structure
  Ogre::VertexDeclaration *vertexDecl = sub->vertexData->vertexDeclaration;
  vertexDecl->addElement(0, 0 * 4, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
  vertexDecl->addElement(0, 3 * 4, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
  vertexDecl->addElement(0, 6 * 4, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
  
  // Allocate the vertex buffer
  Ogre::HardwareVertexBufferSharedPtr vertexBuffer =
    Ogre::HardwareBufferManager::getSingleton()
    .createVertexBuffer(
      8 * 4, // Size
      vertexCount,
      Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
    );
  sub->vertexData->vertexBufferBinding->setBinding(0, vertexBuffer);
  
  // Copy the vertex data
  vertexBuffer->writeData(0, vertexBuffer->getSizeInBytes(), verts, true);
  
  
  
  // Allocate the index buffer
  Ogre::HardwareIndexBufferSharedPtr indexBuffer =
    Ogre::HardwareBufferManager::getSingleton()
    .createIndexBuffer(
      Ogre::HardwareIndexBuffer::IT_32BIT,
      indexCount,
      Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
    );
  sub->indexData->indexBuffer = indexBuffer;
  sub->indexData->indexStart = 0;
  sub->indexData->indexCount = indexCount;
  
  // Copy the index data
  indexBuffer->writeData(0, indexBuffer->getSizeInBytes(), tris, true);
  
  
  
  // Calculate the bounding box
  Ogre::AxisAlignedBox bounds;
  for (size_t i = 0; i < vertexCount; i++)
    bounds.merge(verts[i].position);
  
  // Finalize the mesh fields
  mesh->_setBounds(bounds);
  mesh->_setBoundingSphereRadius((
    bounds.getCenter() - bounds.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM)
  ).length());
  mesh->load();
  
  // Cleanup
  free(verts);
  free(tris);
  
  return mesh;
}
